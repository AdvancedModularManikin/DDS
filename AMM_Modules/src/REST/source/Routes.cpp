#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <typeinfo>

#include "stdafx.h"

#include <fastrtps/fastrtps_fwd.h>

#include <fastrtps/Domain.h>

#include <fastrtps/participant/Participant.h>
#include <fastrtps/participant/ParticipantListener.h>

#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/subscriber/SubscriberListener.h>

#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/rtps/builtin/BuiltinProtocols.h>
#include <fastrtps/rtps/builtin/data/ReaderProxyData.h>
#include <fastrtps/rtps/builtin/data/WriterProxyData.h>
#include <fastrtps/rtps/builtin/discovery/endpoint/EDPSimple.h>
#include <fastrtps/rtps/builtin/discovery/endpoint/EDPStatic.h>
#include <fastrtps/rtps/builtin/discovery/participant/PDPSimple.h>
#include <fastrtps/rtps/builtin/discovery/participant/PDPSimpleListener.h>
#include <fastrtps/rtps/builtin/liveliness/WLP.h>
#include <fastrtps/rtps/history/ReaderHistory.h>
#include <fastrtps/rtps/history/WriterHistory.h>
#include <fastrtps/rtps/participant/RTPSParticipant.h>
#include <fastrtps/rtps/reader/ReaderListener.h>
#include <fastrtps/rtps/reader/StatelessReader.h>
#include <fastrtps/rtps/reader/WriterProxy.h>
#include <fastrtps/rtps/resources/AsyncWriterThread.h>
#include <fastrtps/rtps/writer/StatelessWriter.h>

#include <fastrtps/utils/TimeConversion.h>
#include <fastrtps/utils/eClock.h>

#include "tinyxml2.h"
#include <Net/UdpDiscoveryServer.h>
#include <thirdparty/sqlite_modern_cpp.h>

#include "AMM/BaseLogger.h"
#include "AMM/DDS_Log_Appender.h"
#include "AMM/DDS_Manager.h"
#include "AMM/Utility.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

// #include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/host_name.hpp>

#include <experimental/filesystem>
#include <filesystem>

#include "REST/headers/HttpRequest.hxx"
#include "REST/headers/Routes.hxx"

// using namespace std;
// using namespace std::chrono;
// using namespace boost::filesystem;
// using namespace rapidjson;
// using namespace eprosima;
// using namespace eprosima::fastrtps;
// using namespace sqlite;
// using namespace tinyxml2;
// using namespace AMM;
// using namespace AMM::Capability;


// UDP discovery port
// short discoveryPort = 8889;

// REST threads
// int thr = 2;

// Daemonize by default
// int daemonize = 1;
// int discovery = 1;

#define HOST_NAME_MAX 64
char hostname[HOST_NAME_MAX];

std::string action_path = "Actions/";
std::string state_path = "./states/";
std::string patient_path = "./patients/";

std::map<std::string, double> nodeDataStorage;

std::map<std::string, std::string> statusStorage = {{"STATUS", "NOT RUNNING"}, {"TICK", "0"},
    {"TIME", "0"}, {"SCENARIO", ""}, {"STATE", ""}, {"CLEAR_SUPPLY", ""}, {"BLOOD_SUPPLY", ""},
    {"FLUIDICS_STATE", ""}, {"IVARM_STATE", ""}};

bool m_runThread = false;
// int64_t lastTick = 0;


// DDS_Manager *mgr;
// Participant *mp_participant;
// boost::asio::io_service io_service;
sqlite::database db("amm.db");


/// All the following route-binded functions mutate ostringstream that then get written out to the
/// client.

void test(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    std::string param;
    int err = ParseURLParam(request.url, urlTemplate, param);
    if (err != 0)
    {
        /// Handle error here.
    }

    std::string body;
    body += "Test successful!\n";
    // body += "Method: ";
    // body += request.method;
    // body += "\n";
    // body += "URL: ";
    // body += request.url;
    // body += "\n";
    // body += "URL param: ";
    // body += param;

    // std::cout << "\n" << body << std::endl;

    oss << "HTTP/1.1 200 OK\r\n";
    // oss << "Access-Control-Allow-Origin: *\r\n";
    // oss << "Cache-Control: no-cache, private\r\n";
    oss << "Content-Type: text/plain\r\n";
    oss << "Content-Length: ";
    oss << body.length();
    oss << "\r\n\r\n";
    oss << body;
}

void handleGetInstance(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    using namespace rapidjson;

    StringBuffer s;
    Writer<StringBuffer> writer(s);

    std::ifstream t("static/current_scenario.txt");
    std::string scenario((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();

    writer.StartObject();
    writer.Key("name");
    writer.String(boost::asio::ip::host_name().c_str());
    writer.Key("scenario");
    writer.String(scenario.c_str());
    writer.EndObject();

    std::string str = s.GetString();
    oss << "HTTP/1.1 200 Ok\r\n";
    oss << "Access-Control-Allow-Origin: *\r\n";
    oss << "Cache-Control: no-cache, private\r\n";
    oss << "Content-Type: application/json\r\n";
    oss << "Content-Length: ";
    oss << str.length();
    oss << "\r\n\r\n";
    oss << s.GetString();
}

void handleGetStates(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    using namespace rapidjson;
    using namespace std::experimental::filesystem::v1;

    StringBuffer s;
    Writer<StringBuffer> writer(s);

    writer.StartArray();

    if (exists(state_path) && is_directory(state_path))
    {        
        for (const auto& di : recursive_directory_iterator(state_path))
        {            
            if (is_regular_file(di.path()))
            {
                
                writer.StartObject();
                writer.Key("name");
                writer.String(di.path().string().c_str());
                writer.Key("description");
                auto ftime = std::experimental::filesystem::last_write_time(di.path());
                std::time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
                std::stringstream writeTime;
                writeTime << std::asctime(std::localtime(&cftime));
                writer.String(writeTime.str().c_str());
                writer.EndObject();
            }
        }
    }    
    writer.EndArray();
    
    std::string str = s.GetString();
    oss << "HTTP/1.1 200 Ok\r\n";
    oss << "Access-Control-Allow-Origin: *\r\n";
    oss << "Cache-Control: no-cache, private\r\n";
    oss << "Content-Type: application/json\r\n";
    oss << "Content-Length: ";
    oss << str.length();
    oss << "\r\n\r\n";
    oss << s.GetString();
}

void handleGetNodes(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    using namespace rapidjson;

    StringBuffer s;
    Writer<StringBuffer> writer(s);
    writer.StartArray();

    auto nit = nodeDataStorage.begin();
    while (nit != nodeDataStorage.end())
    {
        writer.StartObject();
        writer.Key(nit->first.c_str());
        writer.Double(nit->second);
        writer.EndObject();
        ++nit;
    }

    auto sit = statusStorage.begin();
    while (sit != statusStorage.end())
    {
        writer.StartObject();
        writer.Key(sit->first.c_str());
        writer.String(sit->second.c_str());
        writer.EndObject();
        ++sit;
    }

    writer.EndArray();

    std::string str = s.GetString();
    oss << "HTTP/1.1 200 Ok\r\n";
    oss << "Access-Control-Allow-Origin: *\r\n";
    oss << "Cache-Control: no-cache, private\r\n";
    oss << "Content-Type: application/json\r\n";
    oss << "Content-Length: ";
    oss << str.length();
    oss << "\r\n\r\n";
    oss << s.GetString();
}

void handleGetNodeByName(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    std::string param;
    int err = ParseURLParam(request.url, urlTemplate, param);
    if (err != 0)
    {
        /// Handle error here.
    }

    using namespace rapidjson;

    std::string name = param;
    // auto name = request.param(":name").as<std::string>();
    auto it = nodeDataStorage.find(name);
    if (it != nodeDataStorage.end())
    {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartObject();
        writer.Key(it->first.c_str());
        writer.Double(it->second);
        writer.EndObject();

        std::string str = s.GetString();
        oss << "HTTP/1.1 200 Ok\r\n";
        oss << "Access-Control-Allow-Origin: *\r\n";
        oss << "Cache-Control: no-cache, private\r\n";
        oss << "Content-Type: application/json\r\n";
        oss << "Content-Length: ";
        oss << str.length();
        oss << "\r\n\r\n";
        oss << s.GetString();
    }
    else
    {
        std::string str = "Node data does not exist";
        oss << "HTTP/1.1 404 Not found\r\n";
        oss << "Access-Control-Allow-Origin: *\r\n";
        oss << "Cache-Control: no-cache, private\r\n";
        oss << "Content-Type: application/json\r\n";
        oss << "Content-Length: ";
        oss << str.length();
        oss << "\r\n\r\n";
        oss << str;
    }
}

void handleGetReady(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    std::string str = "1";
    oss << "HTTP/1.1 200 Ok\r\n";
    oss << "Access-Control-Allow-Origin: *\r\n";
    oss << "Cache-Control: no-cache, private\r\n";
    oss << "Content-Type: text/plain\r\n";
    oss << "Content-Length: ";
    oss << str.length();
    oss << "\r\n\r\n";
    oss << str;
}

void handleGetDebug(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    using namespace std;

    string body;
    body += "TODO: Implement cookie parsing";
    WriteResponse(oss, body, "200 OK", "text/plain");
}

void handleGetEvents(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    using namespace std;
    using namespace rapidjson;

    StringBuffer s;
    Writer<StringBuffer> writer(s);
    writer.StartArray();

    db << "SELECT "
          "module_capabilities.module_name,"
          "events.source,"
          "events.topic,"
          "events.tick,"
          "events.timestamp,"
          "events.data "
          "FROM "
          "events "
          "LEFT JOIN module_capabilities "
          "ON "
          "events.source = module_capabilities.module_guid" >>
        [&](string module_name, string source, string topic, int64_t tick, int64_t timestamp,
            string data) {
            writer.StartObject();
            writer.Key("source");
            writer.String(module_name.c_str());
            writer.Key("module_guid");
            writer.String(source.c_str());
            writer.Key("tick");
            writer.Uint64(tick);
            writer.Key("timestamp");
            writer.Uint64(timestamp);
            writer.Key("topic");
            writer.String(topic.c_str());
            writer.Key("message");
            writer.String(data.c_str());
            writer.EndObject();
        };

    writer.EndArray();

    WriteResponse(oss, s.GetString(), "200 OK", "application/json");
}

void handleGetLogs(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    using namespace std;
    using namespace rapidjson;

    StringBuffer s;
    Writer<StringBuffer> writer(s);
    writer.StartArray();

    db << "SELECT "
          "logs.module_name, "
          "logs.module_guid, "
          "logs.module_id, "
          "logs.message,"
          "logs.log_level,"
          "logs.timestamp "
          "FROM "
          "logs " >>
        [&](string module_name, string module_guid, string module_id, string message,
            string log_level, int64_t timestamp) {
            writer.StartObject();
            writer.Key("source");
            writer.String(module_name.c_str());
            writer.Key("module_guid");
            writer.String(module_guid.c_str());
            writer.Key("module_id");
            writer.String(module_id.c_str());
            writer.Key("timestamp");
            writer.Uint64(timestamp);
            writer.Key("log_level");
            writer.String(log_level.c_str());
            writer.Key("message");
            writer.String(message.c_str());
            writer.EndObject();
        };

    writer.EndArray();

    WriteResponse(oss, s.GetString(), "200 OK", "application/json");
}

void handleGetModulesCount(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    using namespace std;
    using namespace rapidjson;

    StringBuffer s;
    Writer<StringBuffer> writer(s);

    int count = 0;
    db << "SELECT COUNT(DISTINCT module_name) FROM module_capabilities" >> count;
    writer.StartObject();

    writer.Key("module_count");
    writer.Int(count);

    writer.EndObject();

    WriteResponse(oss, s.GetString(), "200 OK", "application/json");
}

void handleGetModules(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    using namespace std;
    using namespace rapidjson;

    StringBuffer s;
    Writer<StringBuffer> writer(s);
    writer.StartArray();

    db << "SELECT "
          "module_capabilities.module_id AS module_id,"
          "module_capabilities.module_guid as module_guid,"
          "module_capabilities.module_name AS module_name,"
          "module_capabilities.capabilities as capabilities,"
          "module_capabilities.manufacturer as manufacturer,"
          "module_capabilities.model as model "
          " FROM "
          " module_capabilities; " >>
        [&](string module_id, string module_guid, string module_name, string capabilities,
            string manufacturer, string model) {
            writer.StartObject();

            writer.Key("Module_ID");
            writer.String(module_id.c_str());

            writer.Key("Module_GUID");
            writer.String(module_guid.c_str());

            writer.Key("Module_Name");
            writer.String(module_name.c_str());

            writer.Key("Manufacturer");
            writer.String(manufacturer.c_str());

            writer.Key("Model");
            writer.String(model.c_str());

            writer.Key("Module_Capabilities");
            writer.String(capabilities.c_str());

            writer.EndObject();
        };

    writer.EndArray();

    WriteResponse(oss, s.GetString(), "200 OK", "application/json");
}

void handleGetModuleById(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    using namespace std;
    using namespace rapidjson;

    string id;
    int err = ParseURLParam(request.url, urlTemplate, id);
    if (err != 0)
    {
        /// Handle error here.
    }

    // auto id = request.param(":id").as<std::string>();
    StringBuffer s;
    Writer<StringBuffer> writer(s);
    db << "SELECT "
          "module_id AS module_id,"
          "module_guid as module_guid,"
          "module_name AS module_name,"
          "capabilities as capabilities,"
          "manufacturer as manufacturer,"
          "model as model "
          " FROM "
          " module_capabilities "
          " WHERE module_id = ?"
       << id >>
        [&](string module_id, string module_guid, string module_name, string capabilities,
            string manufacturer, string model) {
            writer.StartObject();

            writer.Key("Module_ID");
            writer.String(module_id.c_str());

            writer.Key("Module_GUID");
            writer.String(module_guid.c_str());

            writer.Key("Module_Name");
            writer.String(module_name.c_str());

            writer.Key("Manufacturer");
            writer.String(manufacturer.c_str());

            writer.Key("Model");
            writer.String(model.c_str());

            writer.Key("Module_Capabilities");
            writer.String(capabilities.c_str());

            writer.EndObject();
        };

    WriteResponse(oss, s.GetString(), "200 OK", "application/json");
}

void handleGetModuleByGuid(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{

    using namespace std;
    using namespace rapidjson;

    string guid;
    int err = ParseURLParam(request.url, urlTemplate, guid);
    if (err != 0)
    {
        /// Handle error here.
    }

    // auto guid = request.param(":guid").as<std::string>();
    StringBuffer s;
    Writer<StringBuffer> writer(s);
    db << "SELECT "
          "module_id AS module_id,"
          "module_guid as module_guid,"
          "module_name AS module_name,"
          "capabilities as capabilities,"
          "manufacturer as manufacturer,"
          "model as model "
          " FROM "
          " module_capabilities "
          " WHERE module_guid = ?"
       << guid >>
        [&](string module_id, string module_guid, string module_name, string capabilities,
            string manufacturer, string model) {
            writer.StartObject();

            writer.Key("Module_ID");
            writer.String(module_id.c_str());

            writer.Key("Module_GUID");
            writer.String(module_guid.c_str());

            writer.Key("Module_Name");
            writer.String(module_name.c_str());

            writer.Key("Manufacturer");
            writer.String(manufacturer.c_str());

            writer.Key("Model");
            writer.String(model.c_str());

            writer.Key("Module_Capabilities");
            writer.String(capabilities.c_str());

            writer.EndObject();
        };

    WriteResponse(oss, s.GetString(), "200 OK", "application/json");
}

void handleGetShutdown(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    m_runThread = false;
    std::string body;
    body += "TODO: implement cookie parsing.";
    WriteResponse(oss, body, "200 OK", "text/plain");
}

void handleGetActions(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handleGetActionByName(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handlePostAction(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handlePutActionByName(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handleDeleteActionByName(
    std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handlePostExecute(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handleOptionsExecute(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handlePostPhysMod(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handlePostRendMod(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handlePostPerfAss(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handlePostModType(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handleGetPatients(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handleGetDeletedStateByName(
    std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}

void handleGetCommandByName(std::ostringstream& oss, HttpRequest& request, std::string urlTemplate)
{
    test(oss, request, urlTemplate);
}
