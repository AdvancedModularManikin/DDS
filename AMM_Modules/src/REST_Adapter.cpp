#include "stdafx.h"

#include <fastrtps/Domain.h>
#include <fastrtps/participant/Participant.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/subscriber/SampleInfo.h>

#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/rtps/builtin/data/WriterProxyData.h>

#include <fastrtps/rtps/reader/ReaderListener.h>
#include <fastrtps/rtps/builtin/discovery/endpoint/EDPSimple.h>

#include <fastrtps/utils/eClock.h>

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/participant/ParticipantListener.h>
#include <fastrtps/rtps/builtin/data/ReaderProxyData.h>

#include <fastrtps/rtps/builtin/discovery/participant/PDPSimple.h>
#include <fastrtps/rtps/builtin/discovery/participant/PDPSimpleListener.h>

#include <fastrtps/rtps/builtin/BuiltinProtocols.h>
#include <fastrtps/rtps/builtin/liveliness/WLP.h>

#include <fastrtps/rtps/builtin/discovery/endpoint/EDPStatic.h>

#include <fastrtps/rtps/resources/AsyncWriterThread.h>

#include <fastrtps/rtps/writer/StatelessWriter.h>
#include <fastrtps/rtps/reader/StatelessReader.h>
#include <fastrtps/rtps/reader/WriterProxy.h>

#include <fastrtps/rtps/history/WriterHistory.h>
#include <fastrtps/rtps/history/ReaderHistory.h>

#include <fastrtps/utils/TimeConversion.h>

#include <fastrtps/rtps/participant/RTPSParticipant.h>

#include <chrono>

#include "AMM/BaseLogger.h"

#include "AMM/DataTypes.h"
#include "AMM/DDS_Manager.h"

#include <pistache/http.h>
#include <pistache/http_header.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include "rapidjson/writer.h"
#include "rapidjson/document.h"

#include <Net/UdpDiscoveryServer.h>

#include "boost/filesystem.hpp"

#include "tinyxml2.h"

#include <thirdparty/sqlite_modern_cpp.h>

using namespace std;
using namespace std::chrono;
using namespace boost::filesystem;
using namespace rapidjson;
using namespace Pistache;
using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace sqlite;
using namespace tinyxml2;
using namespace AMM;

// UDP discovery port
short discoveryPort = 8889;

// REST adapter port
int portNumber = 9080;

// REST threads
int thr = 2;

// Daemonize by default
int daemonize = 1;
int discovery = 1;

char hostname[HOST_NAME_MAX];

std::string action_path = "Actions/";
std::string state_path = "./states/";
std::string patient_path = "./patients/";
std::string dataKey = "name";

DDS_Manager *mgr;

struct logEntry {
    GUID_t source;
    std::string topic;
    int64_t tick = 0;
    std::chrono::milliseconds timestamp;
    std::string data = "";
};

std::map<std::string, double> nodeDataStorage;
std::vector<logEntry> eventLog;
std::vector<std::string> actions;

std::map<std::string, std::string> statusStorage = {
        {"STATUS",       "NOT RUNNING"},
        {"LAST_COMMAND", ""},
        {"TICK",         "0"},
        {"TIME",         "0"}
};

bool m_runThread = false;
int64_t lastTick = 0;

Participant *mp_participant;
boost::asio::io_service io_service;
database db("amm.db");

std::string extractPhysiologyModificationName(const std::string& payload) {
    XMLDocument doc(false);
    doc.Parse(payload.c_str());
    tinyxml2::XMLNode *root = doc.FirstChildElement("Scenario");
    std::string modName = root->FirstChildElement("Name")->ToElement()->GetText();
    std::string modDesc = root->FirstChildElement("Description")->ToElement()->GetText();
    std::ostringstream fullModName;
    fullModName << "[" << modName << "] " << modDesc;
    return fullModName.str();
}

class AMMListener : public ListenerInterface {
    void onNewTickData(AMM::Simulation::Tick t, SampleInfo_t* info) {
        if (statusStorage["STATUS"].compare("NOT RUNNING") == 0 && t.frame() > lastTick) {
            statusStorage["STATUS"] = "RUNNING";
        }
        lastTick = t.frame();
        statusStorage["TICK"] = to_string(t.frame());
        statusStorage["TIME"] = to_string(t.time());
    }

    void onNewScenarioData(AMM::Capability::Scenario sc, SampleInfo_t* info) {};

    void onNewRenderModificationData(AMM::Render::Modification rm, SampleInfo_t* info) {
        milliseconds timestamp = duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()
        );
        GUID_t changeGuid;
        iHandle2GUID(changeGuid, info->iHandle);

        std::ostringstream logmessage;
        logmessage << rm.payload();

        logEntry newLogEntry{
                changeGuid,
                "AMM::Render::Modification",
                lastTick,
                timestamp,
                logmessage.str()
        };
        eventLog.push_back(newLogEntry);

    };

    void onNewPhysiologyModificationData(AMM::Physiology::Modification pm, SampleInfo_t* info) {
        milliseconds timestamp = duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()
        );
        GUID_t changeGuid;
        iHandle2GUID(changeGuid, info->iHandle);

        std::string physModName = extractPhysiologyModificationName(pm.payload());

        std::ostringstream logmessage;
        logmessage << physModName;

        logEntry newLogEntry{
                changeGuid,
                "AMM::Physiology::Modification",
                lastTick,
                timestamp,
                logmessage.str()
        };
        eventLog.push_back(newLogEntry);
    };

    void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) {
        milliseconds timestamp = duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()
        );
        GUID_t changeGuid;
        iHandle2GUID(changeGuid, info->iHandle);

        logEntry newLogEntry{
                changeGuid,
                "AMM::Command",
                lastTick,
                timestamp,
                c.message()
        };
        eventLog.push_back(newLogEntry);

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

    void onNewNodeData(AMM::Physiology::Node n, SampleInfo_t* info) {
        LOG_TRACE << "Got new node data for " << n.nodepath() << " = " << n.dbl();
        nodeDataStorage[n.nodepath()] = n.dbl();
    }

};

void SendPhysiologyModification(const std::string &command) {
    std::string physModName = extractPhysiologyModificationName(command);
    LOG_INFO << "Publishing a phys mod: " << physModName;
    AMM::Physiology::Modification modInstance;
    modInstance.payload(command);
    mgr->PublishPhysiologyModification(modInstance);
}

void SendRenderModification(const std::string &command) {
    LOG_INFO << "Publishing a render mod: " << command;
    AMM::Render::Modification modInstance;
    modInstance.payload(command);
    mgr->PublishRenderModification(modInstance);
}

void SendPerformanceAssessment(const std::string &command) {
    LOG_INFO << "Publishing a phys mod: " << command;
    AMM::Performance::Assessment assessInstance;
    assessInstance.assessment_info(command);
    mgr->PublishPerformanceData(assessInstance);
}
void SendCommand(const std::string &command) {
    LOG_INFO << "Publishing a command:" << command;
    AMM::PatientAction::BioGears::Command cmdInstance;
    cmdInstance.message(command);
    mgr->PublishCommand(cmdInstance);
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
        auto opts = Http::Endpoint::options().threads(thr).flags(Tcp::Options::InstallSignalHandler).maxPayload(65536);
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

        Routes::Get(router, "/instance", Routes::bind(&DDSEndpoint::getInstance, this));
        Routes::Get(router, "/node/:name", Routes::bind(&DDSEndpoint::getNode, this));
        Routes::Get(router, "/nodes", Routes::bind(&DDSEndpoint::getNodes, this));
        Routes::Get(router, "/command/:name", Routes::bind(&DDSEndpoint::issueCommand, this));
        Routes::Get(router, "/ready", Routes::bind(&Generic::handleReady));
        Routes::Get(router, "/debug", Routes::bind(&DDSEndpoint::doDebug, this));

        Routes::Get(router, "/events", Routes::bind(&DDSEndpoint::getEventLog, this));

        Routes::Get(router, "/modules", Routes::bind(&DDSEndpoint::getModules, this));

        Routes::Get(router, "/shutdown", Routes::bind(&DDSEndpoint::doShutdown, this));

        Routes::Get(router, "/actions", Routes::bind(&DDSEndpoint::getActions, this));
        Routes::Get(router, "/action/:name", Routes::bind(&DDSEndpoint::getAction, this));
        Routes::Post(router, "/action", Routes::bind(&DDSEndpoint::createAction, this));
        Routes::Put(router, "/action/:name", Routes::bind(&DDSEndpoint::updateAction, this));
        Routes::Delete(router, "/action/:name", Routes::bind(&DDSEndpoint::deleteAction, this));

        Routes::Post(router, "/execute", Routes::bind(&DDSEndpoint::executeCommand, this));
        Routes::Options(router, "/execute", Routes::bind(&DDSEndpoint::executeOptions, this));

        Routes::Post(router, "/topic/physiology_modification", Routes::bind(&DDSEndpoint::executePhysiologyModification, this));
        Routes::Post(router, "/topic/render_modification", Routes::bind(&DDSEndpoint::executeRenderModification, this));
        Routes::Post(router, "/topic/performance_assessment", Routes::bind(&DDSEndpoint::executePerformanceAssessment, this));
        Routes::Options(router, "/topic/:mod_type", Routes::bind(&DDSEndpoint::executeOptions, this));

        Routes::Get(router, "/patients", Routes::bind(&DDSEndpoint::getPatients, this));

        Routes::Get(router, "/states", Routes::bind(&DDSEndpoint::getStates, this));
        Routes::Get(router, "/states/:name/delete", Routes::bind(&DDSEndpoint::deleteState, this));

    }

    void getInstance(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);

        std::ifstream t("mule1/current_scenario.txt");
        std::string scenario((std::istreambuf_iterator<char>(t)),
                             std::istreambuf_iterator<char>());
        t.close();


        writer.StartObject();
        writer.Key("name");
        writer.String(hostname);
        writer.Key("scenario");
        writer.String(scenario.c_str());
        writer.EndObject();


        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
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
                        writer.StartObject();
                        writer.Key("name");
                        writer.String(dir_itr->path().filename().c_str());
                        writer.Key("description");
                        stringstream writeTime;
                        writeTime << last_write_time(dir_itr->path());
                        writer.String(writeTime.str().c_str());
                        writer.EndObject();
                    }
                }
            }
        }
        writer.EndArray();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void deleteState(const Rest::Request &request, Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        if (name != "StandardMale@0s.xml") {
            std::ostringstream deleteFile;
            deleteFile << state_path << "/" << name;
            path deletePath(deleteFile.str().c_str());
            if (exists(deletePath) && is_regular_file(deletePath)) {
                LOG_INFO << "Deleting " << deletePath;
                boost::filesystem::remove(deletePath);
                response.send(Http::Code::Ok, "Deleted", MIME(Application, Json));
            } else {
                response.send(Http::Code::Forbidden, "Unable to delete state file", MIME(Application, Json));
            }
        } else {
            response.send(Http::Code::Forbidden, "Can not delete default state file", MIME(Application, Json));
        }
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
                        writer.StartObject();
                        writer.Key("name");
                        writer.String(dir_itr->path().filename().c_str());
                        writer.Key("description");
                        stringstream writeTime;
                        writeTime << last_write_time(dir_itr->path());
                        writer.String(writeTime.str().c_str());
                        writer.EndObject();
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
                        writer.StartObject();
                        writer.Key("name");
                        writer.String(dir_itr->path().filename().c_str());
                        writer.Key("description");
                        stringstream writeTime;
                        writeTime << last_write_time(dir_itr->path());
                        writer.String(writeTime.str().c_str());
                        writer.EndObject();
                    }
                }
            }
        }
        writer.EndArray();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void executeCommand(const Rest::Request &request, Http::ResponseWriter response) {
        Document document;
        document.Parse(request.body().c_str());
        std::string payload = document["payload"].GetString();
        SendPhysiologyModification(payload);
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Http::Code::Ok, "Command executed");
    }

    void executePhysiologyModification(const Rest::Request &request, Http::ResponseWriter response) {
        Document document;
        document.Parse(request.body().c_str());
        std::string payload = document["payload"].GetString();
        SendPhysiologyModification(payload);
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Http::Code::Ok, "Physiology modification published");
    }

    void executeRenderModification(const Rest::Request &request, Http::ResponseWriter response) {
        Document document;
        document.Parse(request.body().c_str());
        std::string payload = document["payload"].GetString();
        SendRenderModification(payload);
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Http::Code::Ok, "Render modification published");
    }

    void executePerformanceAssessment(const Rest::Request &request, Http::ResponseWriter response) {
        Document document;
        document.Parse(request.body().c_str());
        std::string payload = document["payload"].GetString();
        SendPerformanceAssessment(payload);
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Http::Code::Ok, "Performance assessment published");
    }

    void executeOptions(const Rest::Request &request, Http::ResponseWriter response) {
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Pistache::Http::Code::Ok, "{\"message\":\"success\"}");
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


    void getModules(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();

        db << "SELECT "
              "module_capabilities.module_id AS module_id,"
              "module_capabilities.module_name AS module_name,"
              "module_capabilities.capabilities as capabilities,"
              "module_status.capability as capability,"
              "module_status.status as capability_status,"
              "module_capabilities.manufacturer as manufacturer,"
              "module_capabilities.model as model "
              " FROM "
              " module_capabilities "
              " LEFT JOIN module_status ON module_capabilities.module_name = module_status.module_name;"
           >> [&](string module_id, string module_name, string capabilities, string capability,
                  string capability_status,
                  string manufacturer, string model) {
               writer.StartObject();

               writer.Key("Module_ID");
               writer.String(module_id.c_str());

               writer.Key("Module_Name");
               writer.String(module_name.c_str());

               writer.Key("Manufacturer");
               writer.String(manufacturer.c_str());

               writer.Key("Model");
               writer.String(model.c_str());

               writer.Key("Module_Capabilities");
               writer.String(capabilities.c_str());

               writer.Key("Capability_Status");
               writer.String(capability.c_str());

               writer.Key("Status");
               writer.String(capability_status.c_str());


               writer.EndObject();
           };


        writer.EndArray();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getEventLog(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();

        auto eventit = eventLog.begin();
        while (eventit != eventLog.end()) {
            std::ostringstream entityIdstring;
            entityIdstring << (*eventit).source.entityId;
            writer.StartObject();
            writer.Key("source");
            writer.String(entityIdstring.str().c_str());
            writer.Key("tick");
            writer.Double((*eventit).tick);
            writer.Key("timestamp");
            writer.Uint((*eventit).timestamp.count());
            writer.Key("message");
            writer.String((*eventit).data.c_str());
            writer.EndObject();
            ++eventit;
        }

        writer.EndArray();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getNodes(const Rest::Request &request, Http::ResponseWriter response) {
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
    if (discovery) {
        UdpDiscoveryServer udps(io_service, discoveryPort);
        LOG_INFO << "UDP Discovery listening on port " << discoveryPort;
        io_service.run();
    } else {
        LOG_INFO << "UDP discovery service not started due to command line option.";
    }
}

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n" << "\t-h,--help\t\tShow this help message\n"
         << endl;
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }

        if (arg == "-d") {
            daemonize = 1;
        }

        if (arg == "-nodiscovery") {
            discovery = 0;
        }
    }

    string action;

    const char *nodeName = "AMM_REST_Adapter";
    std::string nodeString(nodeName);
    mgr = new DDS_Manager(nodeName);
    mp_participant = mgr->GetParticipant();

    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *tick_sub_listener = new DDS_Listeners::TickSubListener();
    auto *physmod_sub_listener = new DDS_Listeners::PhysiologyModificationListener();
    auto *rendermod_sub_listener = new DDS_Listeners::RenderModificationListener();

    AMMListener rl;
    node_sub_listener->SetUpstream(&rl);
    command_sub_listener->SetUpstream(&rl);
    tick_sub_listener->SetUpstream(&rl);
    physmod_sub_listener->SetUpstream(&rl);
    rendermod_sub_listener->SetUpstream(&rl);

    mgr->InitializeSubscriber(AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(), node_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), command_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::tickTopic, AMM::DataTypes::getTickType(), tick_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::renderModTopic, AMM::DataTypes::getRenderModificationType(), rendermod_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::physModTopic, AMM::DataTypes::getPhysiologyModificationType(), physmod_sub_listener);

    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            mgr->module_id,
            nodeString,
            "Vcom3D",
            "REST_Adapter",
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/rest_adapter_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);

    std::thread udpD(UdpDiscoveryThread);


    gethostname(hostname, HOST_NAME_MAX);

    Port port(static_cast<uint16_t>(portNumber));
    Address addr(Ipv4::any(), port);
    DDSEndpoint server(addr);
    LOG_INFO << "REST_Adapter Listening on *:" << portNumber;
    LOG_INFO << "\tCores =\t" << hardware_concurrency();
    LOG_INFO << "\tThreads =\t" << thr;
    server.init(thr);

    m_runThread = true;

    server.start();

    milliseconds timestamp = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
    );

    while (m_runThread) {
        getline(cin, action);
        transform(action.begin(), action.end(), action.begin(), ::toupper);
        if (action == "EXIT") {
            m_runThread = false;
            LOG_INFO << "Shutting down.";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        cout.flush();
    }

    server.shutdown();
    LOG_INFO << "Stopped REST listener.";

    io_service.stop();
    udpD.join();
    LOG_INFO << "Stopped UDP discovery.";

    return 0;
}
