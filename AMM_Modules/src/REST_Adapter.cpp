#include "stdafx.h"

#include "AMM/DDS_Manager.h"

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include "rapidjson/writer.h"

using namespace std;
using namespace rapidjson;
using namespace Pistache;

std::map<std::string, double> nodeDataStorage;

std::thread m_thread;
std::mutex m_mutex;
bool m_runThread = false;

Publisher *command_publisher;

class RESTListener : public ListenerInterface {

    void onNewTickData(AMM::Simulation::Tick t) override {

    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c) override {

    }

    void onNewNodeData(AMM::Physiology::Node n) override {
        nodeDataStorage[n.nodepath()] = n.dbl();
    }
};

void SendCommand(const std::string &command) {
    cout << "=== [REST_Adapter] Sending a command:" << command << endl;
    AMM::PatientAction::BioGears::Command cmdInstance;
    cmdInstance.message(command);
    command_publisher->write(&cmdInstance);
}


void DataLoop() {
    while (m_runThread) {
        // Data processing is handled in the listener, but we could do something here...
    }
}

void printCookies(const Http::Request &req) {
    auto cookies = req.cookies();
    std::cout << "Cookies: [" << std::endl;
    const std::string indent(4, ' ');
    for (const auto &c : cookies) {
        std::cout << indent << c.name << " = " << c.value << std::endl;
    }
    std::cout << "]" << std::endl;
}


namespace Generic {

    void handleReady(const Rest::Request & request, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "1");
    }

}

class DDSEndpoint {
public:

    explicit DDSEndpoint(Address addr) :
            httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {
    }

    void init(int thr = 2) {
        auto opts = Http::Endpoint::options().threads(thr).flags(Tcp::Options::InstallSignalHandler);
        httpEndpoint->init(opts);
        setupRoutes();
    }

    void start() {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serveThreaded();
    }

    void shutdown() {
        httpEndpoint->shutdown();
    }

private:
    void setupRoutes() {
        using namespace Rest;

        Routes::Get(router, "/node/:name", Routes::bind(&DDSEndpoint::doGetNode, this));
        Routes::Get(router, "/nodes", Routes::bind(&DDSEndpoint::doGetAllNodes, this));
        Routes::Get(router, "/command/:name", Routes::bind(&DDSEndpoint::doIssueCommand, this));
        Routes::Get(router, "/ready", Routes::bind(&Generic::handleReady));
        Routes::Get(router, "/debug", Routes::bind(&DDSEndpoint::doDebug, this));
        Routes::Get(router, "/shutdown", Routes::bind(&DDSEndpoint::doShutdown, this));

    }

    void doIssueCommand(const Rest::Request &request, Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
        SendCommand(name);
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartObject();
        writer.Key("Sent command");
        writer.String(name.c_str());
        writer.EndObject();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString());
    }

    void doGetAllNodes(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();
        auto it = nodeDataStorage.begin();
        while (it != nodeDataStorage.end()) {
            writer.StartObject();
            writer.Key(it->first.c_str());
            std::ostringstream str;
            str << it->second;
            writer.String(str.str().c_str());
            writer.EndObject();
            ++it;
        }
        writer.EndArray();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void doGetNode(const Rest::Request &request, Http::ResponseWriter response) {

        auto name = request.param(":name").as<std::string>();
        auto it = nodeDataStorage.find(name);
        if (it != nodeDataStorage.end()) {
            StringBuffer s;
            Writer<StringBuffer> writer(s);
            writer.StartObject();
            writer.Key(it->first.c_str());
            std::ostringstream ns;
            ns << it->second;
            writer.String(ns.str().c_str());
            writer.EndObject();
            response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
            response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
        } else {
            response.send(Http::Code::Not_Found, "Node data does not exist");
        }
    }

    void doDebug(const Rest::Request &request, Http::ResponseWriter response) {
        printCookies(request);
        response.cookies().add(Http::Cookie("lang", "en-US"));
        response.send(Http::Code::Ok);
    }

    void doShutdown(const Rest::Request &request, Http::ResponseWriter response) {
        m_runThread = false;
        response.cookies().add(Http::Cookie("lang", "en-US"));
        response.send(Http::Code::Ok);
    }

    typedef std::mutex Lock;
    typedef std::lock_guard<Lock> Guard;
    Lock commandLock;

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
};

int main(int argc, char *argv[]) {
    cout << "=== [AMM - REST Adapter] ===" << endl;

    int portNumber = 9080;
    int thr = 2;
    string action;

    auto *mgr = new DDS_Manager();
    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();

    RESTListener rl;
    node_sub_listener->SetUpstream(&rl);
    Subscriber *node_subscriber = mgr->InitializeNodeSubscriber(node_sub_listener);

    auto *pub_listener = new DDS_Listeners::PubListener();
    command_publisher = mgr->InitializeCommandPublisher(pub_listener);

    // start data thread
    m_runThread = true;
    m_thread = std::thread(DataLoop);

    Port port(static_cast<uint16_t>(portNumber));
    Address addr(Ipv4::any(), port);
    DDSEndpoint server(addr);
    cout << "=== [REST_Adapter] Ready ..." << endl;
    cout << "  = [REST_Adapter] Listening on *:" << portNumber << endl;
    cout << "  = [REST_Adapter] Cores = " << hardware_concurrency() << endl;
    cout << "  = [REST_Adapter] Using " << thr << " threads" << endl;
    cout << "  = Type EXIT and hit enter to shutdown" << endl;
    server.init(thr);

    server.start();

    while (m_runThread) {
        getline(cin, action);
        transform(action.begin(), action.end(), action.begin(), ::toupper);
        if (action == "EXIT") {
            m_runThread = false;
        }
        sleep(1);
    }


    cout << "=== [REST_Adapter] Simulation stopped." << endl;

    server.shutdown();
    m_thread.join();

    return 0;
}
