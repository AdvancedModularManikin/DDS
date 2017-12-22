#include "stdafx.h"

#include "AMM/DDS_Manager.h"

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include "rapidjson/writer.h"

#include <Net/UdpDiscoveryServer.h>

#include "boost/filesystem.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace rapidjson;
using namespace Pistache;

// UDP discovery port
short discoveryPort = 8889;

// REST adapter port
int portNumber = 9080;

// REST threads
int thr = 2;

// Daemonize by default
int daemonize = 1;

std::string action_path = "Actions/";
std::string state_path = "states/";
std::string patient_path = "patients/";

std::vector <std::string> actions;
std::map<std::string, double> nodeDataStorage;
std::map<std::string, std::string> statusStorage = {
        {"STATUS",       "NOT RUNNING"},
        {"LAST_COMMAND", ""},
        {"TICK",         "0"},
        {"TIME",         "0"}
};

bool m_runThread = false;
int64_t lastTick = 0;

Publisher *command_publisher;

class RESTListener : public ListenerInterface {
public:
    void onNewTickData(AMM::Simulation::Tick t) override {
        if (statusStorage["STATUS"].compare("NOT RUNNING") == 0 && t.frame() > lastTick) {
            statusStorage["STATUS"] = "RUNNING";
        }
        lastTick = t.frame();
        statusStorage["TICK"] = to_string(t.frame());
        statusStorage["TIME"] = to_string(t.time());
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c) override {
        if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = c.message().substr(sysPrefix.size());
            if (value.compare("START_SIM") == 0) {
                statusStorage["STATUS"] = "RUNNING";
            } else if (value.compare("STOP_SIM") == 0) {
                statusStorage["STATUS"] = "STOPPED";
            } else if (value.compare("PAUSE_SIM") == 0) {
                statusStorage["STATUS"] = "PAUSED";
            } else if (value.compare("RESET_SIM") == 0) {
                statusStorage["STATUS"] = "NOT RUNNING";
                statusStorage["TICK"] = "0";
                statusStorage["TIME"] = "0";
                nodeDataStorage.clear();
            }
        }

        statusStorage["LAST_COMMAND"] = c.message();
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

    void handleReady(const Rest::Request &request, Http::ResponseWriter response) {
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

        	Routes::Get(router, "/node/:name", Routes::bind(&DDSEndpoint::getNode, this));
	      Routes::Get(router, "/nodes", Routes::bind(&DDSEndpoint::getAllNodes, this));
        	Routes::Get(router, "/command/:name", Routes::bind(&DDSEndpoint::issueCommand, this));
        	Routes::Get(router, "/ready", Routes::bind(&Generic::handleReady));
        	Routes::Get(router, "/debug", Routes::bind(&DDSEndpoint::doDebug, this));

        	Routes::Get(router, "/shutdown", Routes::bind(&DDSEndpoint::doShutdown, this));

        	Routes::Get(router, "/actions", Routes::bind(&DDSEndpoint::getActions, this));
        	Routes::Get(router, "/action/:name", Routes::bind(&DDSEndpoint::getAction, this));
        	Routes::Post(router, "/action", Routes::bind(&DDSEndpoint::createAction, this));
        	Routes::Put(router, "/action/:name", Routes::bind(&DDSEndpoint::updateAction, this));
        	Routes::Delete(router, "/action/:name", Routes::bind(&DDSEndpoint::deleteAction, this));
        
			Routes::Get(router, "/patients", Routes::bind(&DDSEndpoint::getPatients, this));

			Routes::Get(router, "/states", Routes::bind(&DDSEndpoint::getStates, this));        
        

    }

	void getStates(const Rest::Request &request, Http::ResponseWriter response) {
			StringBuffer s;
        	Writer<StringBuffer> writer(s);
        	
        	writer.StartArray();
			if (exists(state_path) && is_directory(state_path)) {				
				path p(state_path);        
				if (is_directory(p)) {   
					directory_iterator end_iter;
    				for (directory_iterator dir_itr(p); dir_itr != end_iter; ++dir_itr) {
						if (is_regular_file(dir_itr->status())) {
							stringstream ss;
							ss << dir_itr->path().filename();
							writer.String(ss.str().c_str());
      				}      
    				}    
  				}				               	            	            	            
        	}
			writer.EndArray();
			
        	response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        	response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }
    
    void getPatients(const Rest::Request &request, Http::ResponseWriter response) {
			StringBuffer s;
        	Writer<StringBuffer> writer(s);
        	
        	writer.StartArray();
			if (exists(patient_path) && is_directory(patient_path)) {				
				path p(patient_path);        
				if (is_directory(p)) {   
					directory_iterator end_iter;
    				for (directory_iterator dir_itr(p); dir_itr != end_iter; ++dir_itr) {
						if (is_regular_file(dir_itr->status())) {
							stringstream ss;
							ss << dir_itr->path().filename();
							writer.String(ss.str().c_str());
      				}      
    				}    
  				}				               	            	            	            
        	}
			writer.EndArray();
			
        	response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        	response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getActions(const Rest::Request &request, Http::ResponseWriter response) {
			StringBuffer s;
        	Writer<StringBuffer> writer(s);
        	
        	writer.StartArray();
			if (exists(action_path) && is_directory(action_path)) {				
				path p(action_path);        
				if (is_directory(p)) {   
					directory_iterator end_iter;
    				for (directory_iterator dir_itr(p); dir_itr != end_iter; ++dir_itr) {
						if (is_regular_file(dir_itr->status())) {
							stringstream ss;
							ss << dir_itr->path().filename();
							writer.String(ss.str().c_str());							
      				}      
    				}    
  				}				               	            	            	            
        	}
			writer.EndArray();
			
        	response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        	response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void createAction(const Rest::Request &request, Http::ResponseWriter response) {

    }

    void deleteAction(const Rest::Request &request, Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
    }

    void updateAction(const Rest::Request &request, Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
    }

    void getAction(const Rest::Request &request, Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
    }

    void issueCommand(const Rest::Request &request, Http::ResponseWriter response) {
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

    void getAllNodes(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();

        auto nit = nodeDataStorage.begin();
        while (nit != nodeDataStorage.end()) {
            writer.StartObject();
            writer.Key(nit->first.c_str());
            writer.Double(nit->second);
            writer.EndObject();
            ++nit;
        }


        auto sit = statusStorage.begin();
        while (sit != statusStorage.end()) {
            writer.StartObject();
            writer.Key(sit->first.c_str());
            writer.String(sit->second.c_str());
            writer.EndObject();
            ++sit;
        }

        writer.EndArray();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getNode(const Rest::Request &request, Http::ResponseWriter response) {

        auto name = request.param(":name").as<std::string>();
        auto it = nodeDataStorage.find(name);
        if (it != nodeDataStorage.end()) {
            StringBuffer s;
            Writer<StringBuffer> writer(s);
            writer.StartObject();
            writer.Key(it->first.c_str());
            writer.Double(it->second);
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

void UdpDiscoveryThread() {
    boost::asio::io_service io_service;
    UdpDiscoveryServer udps(io_service, discoveryPort);
    cout << "\tUDP Discovery listening on port " << discoveryPort << "\n" << endl;
    io_service.run();
}

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n" << "\t-h,--help\t\tShow this help message\n" << endl;
}

int main(int argc, char *argv[]) {
    cout << "=== [AMM - REST Adapter] ===" << endl;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }

        if (arg == "-d") {
            daemonize = 1;
        }
    }

    string action;

    auto *mgr = new DDS_Manager();
    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *tick_sub_listener = new DDS_Listeners::TickSubListener();



    RESTListener rl;
    node_sub_listener->SetUpstream(&rl);
    command_sub_listener->SetUpstream(&rl);
    tick_sub_listener->SetUpstream(&rl);

    mgr->InitializeNodeSubscriber(node_sub_listener);
    mgr->InitializeCommandSubscriber(command_sub_listener);
    mgr->InitializeTickSubscriber(tick_sub_listener);

    auto *pub_listener = new DDS_Listeners::PubListener();
    command_publisher = mgr->InitializeCommandPublisher(pub_listener);

    std::thread udpD(UdpDiscoveryThread);

    Port port(static_cast<uint16_t>(portNumber));
    Address addr(Ipv4::any(), port);
    DDSEndpoint server(addr);
    cout << "=== [REST_Adapter] Ready ..." << endl;
    cout << "  = [REST_Adapter] Listening on *:" << portNumber << endl;
    cout << "  = [REST_Adapter] Cores = " << hardware_concurrency() << endl;
    cout << "  = [REST_Adapter] Using " << thr << " threads" << endl;
    cout << "  = Type EXIT and hit enter to shutdown" << endl;
    server.init(thr);

    m_runThread = true;

    server.start();

    while (m_runThread) {
        getline(cin, action);
        transform(action.begin(), action.end(), action.begin(), ::toupper);
        if (action == "EXIT") {
            m_runThread = false;
            cout << "=== [REST_Adapter] Shutting down." << endl;
        }
        sleep(1);
    }

    server.shutdown();
    cout << "=== [REST_Adapter] Stopped REST listener." << endl;

    udpD.join();
    cout << "=== [REST_Adapter] Stopped UDP." << endl;

    return 0;
}