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
Subscriber *node_subscriber;

class RESTListener : public ListenerInterface {
    void onNewNodeData(AMM::Physiology::Node n) {
        nodeDataStorage[n.nodepath()] = n.dbl();
    }
};

void InitializeDDS() {
    auto *mgr = new DDS_Manager();
    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    RESTListener rl;
    node_sub_listener->SetUpstream(&rl);
    command_publisher = mgr->InitializeCommandPublisher();
    node_subscriber = mgr->InitializeNodeSubscriber(node_sub_listener);
}

void SendCommand(const std::string &command) {
    AMM::PatientAction::BioGears::Command cmdInstance;
    cmdInstance.message(command);
    cout << "=== [CommandExecutor] Sending a command containing:" << endl;
    cout << "    Command : \"" << cmdInstance.message() << "\"" << endl;
    command_publisher->write(&cmdInstance);
}


void DataLoop() {
    while (m_runThread) {
        // Data processing is handled in the listener
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

    void handleReady(const Rest::Request &, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "1");
    }

}

class DDSEndpoint {
public:

    DDSEndpoint(Address addr) :
            httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {
    }

    void init(size_t thr = 2) {
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
        response.headers().addRaw(Http::Header::Raw("Access-Control-Allow-Origin", "*"));
        response.send(Http::Code::Ok, s.GetString());
    }

    void doGetAllNodes(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();
        std::map<std::string, double>::iterator it = nodeDataStorage.begin();
        while (it != nodeDataStorage.end()) {
            writer.StartObject();
            writer.Key(it->first.c_str());
            std::ostringstream s;
            s << it->second;
            writer.String(s.str().c_str());
            writer.EndObject();
            it++;
        }
        writer.EndArray();
        // response.headers().add(<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
        response.headers().addRaw(Http::Header::Raw("Access-Control-Allow-Origin", "*"));
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void doGetNode(const Rest::Request &request, Http::ResponseWriter response) {

        auto name = request.param(":name").as<std::string>();
        string responseString = "";
        std::map<std::string, double>::iterator it = nodeDataStorage.find(name);
        if (it != nodeDataStorage.end()) {
            StringBuffer s;
            Writer<StringBuffer> writer(s);
            writer.StartObject();
            writer.Key(it->first.c_str());
            std::ostringstream ns;
            ns << it->second;
            writer.String(ns.str().c_str());
            writer.EndObject();
            response.headers().addRaw(Http::Header::Raw("Access-Control-Allow-Origin", "*"));
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
    int portNumber = 9080;
    int thr = 2;
    string action = "";

    InitializeDDS();

    // start data thread
    m_runThread = true;
    m_thread = std::thread(DataLoop);

    Port port(portNumber);
    Address addr(Ipv4::any(), port);
    DDSEndpoint server(addr);
    cout << "=== [REST_Adapter] Ready ..." << endl;
    cout << "  = [REST_Adapter] Listening on *:" << portNumber << endl;
    cout << "  = [REST_Adapter] Cores = " << hardware_concurrency() << endl;
    cout << "  = [REST_Adapter] Using " << thr << " threads" << endl;
    cout << "  = Type EXIT and hit enter to shutdown" << endl;
    server.init(thr);

    server.start();

    while(m_runThread) {
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
