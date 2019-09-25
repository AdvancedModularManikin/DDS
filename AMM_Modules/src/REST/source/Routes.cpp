#include <fstream>
#include <istream>
#include <sstream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <functional>
#include <condition_variable>

#include "AMM/BaseLogger.h"
#include "AMM/DDS_Log_Appender.h"
#include "AMM/DDS_Manager.h"
#include "AMM/Utility.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include <Net/UdpDiscoveryServer.h>


#include "tinyxml2.h"

#include <thirdparty/sqlite_modern_cpp.h>

// #include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/algorithm/string/join.hpp>

#include <experimental/filesystem>
#include <filesystem>

#include "REST/headers/HttpRequest.hxx"
#include "REST/headers/Routes.hxx"


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
std::vector<std::string> labsStorage;

std::map<std::string, std::string> statusStorage = {{"STATUS", "NOT RUNNING"}, {"TICK", "0"},
    {"TIME", "0"}, {"SCENARIO", ""}, {"STATE", ""}, {"CLEAR_SUPPLY", ""}, {"BLOOD_SUPPLY", ""},
    {"FLUIDICS_STATE", ""}, {"IVARM_STATE", ""}};

bool m_runThread = false;
int64_t lastTick = 0;



// boost::asio::io_service io_service;
sqlite::database db("amm.db");


/// All the following route-binded functions mutate ostringstream that then get written out to the
/// client.

void test (std::ostringstream& oss, HttpRequest &request, std::string urlTemplate) {

   std::string param;
   int err = ParseURLParam(request.url, urlTemplate, param);
   if (err != 0) {
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

void handleGetInstance (std::ostringstream& oss, HttpRequest &request, std::string urlTemplate) {

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

void handleGetNodes (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace rapidjson;

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

void handleGetNodeByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   std::string param;
   int err = ParseURLParam(request.url, urlTemplate, param);
   if (err != 0) {
      /// Handle error here.
   }

   using namespace rapidjson;

   std::string name = param;
   // auto name = request.param(":name").as<std::string>();
   auto it = nodeDataStorage.find(name);
   if (it != nodeDataStorage.end()) {
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
   } else {
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

void handleGetReady (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

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

void handleGetDebug (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace std;

   string body;
   body += "TODO: Implement cookie parsing";
   WriteResponse(oss, body, "200 OK", "text/plain");

}

void handleGetEvents (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

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
      [&](string module_name, string source, string topic, int64_t tick, int64_t timestamp, string data) {
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

void handleGetLogs (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

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
           [&](string module_name, string module_guid, string module_id, string message, string log_level,
               int64_t timestamp) {

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

void handleGetModulesCount (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

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

void handleGetModules (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

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
           [&](string module_id, string module_guid, string module_name,
               string capabilities,
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

void handleGetModuleById (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace std;
   using namespace rapidjson;

   string id;
   int err = ParseURLParam(request.url, urlTemplate, id);
   if (err != 0) {
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
           [&](string module_id, string module_guid, string module_name,
               string capabilities, string manufacturer, string model) {
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

void handleGetModuleByGuid (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace std;
   using namespace rapidjson;

   string guid;
   int err = ParseURLParam(request.url, urlTemplate, guid);
   if (err != 0) {
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
           [&](string module_id, string module_guid, string module_name,
               string capabilities, string manufacturer, string model) {
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

void handleGetShutdown (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {
   m_runThread = false;
   std::string body;
   body += "TODO: implement cookie parsing.";
   WriteResponse(oss, body, "200 OK", "text/plain");
}

void handleGetActions (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace std;
   using namespace std::chrono;
   using namespace rapidjson;
   using namespace std::experimental::filesystem::v1;

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
                       writer.String(dir_itr->path().filename().string().c_str());
                       writer.Key("description");
                       stringstream writeTime;

                       /// TODO:
                       /// Fix cast error on return of last_write_time.
                       // writeTime << last_write_time(dir_itr->path());

                       writer.String(writeTime.str().c_str());
                       writer.EndObject();
                   }
               }
           }
      }
      writer.EndArray();

      WriteResponse(oss, s.GetString(), "200 OK", "application/json");
}

void handleGetActionByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {
   MethodNotImplementedResp(oss);
}

void handlePostAction (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {
   MethodNotImplementedResp(oss);
}

void handlePutActionByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {
   MethodNotImplementedResp(oss);
}

void handleDeleteActionByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {
   MethodNotImplementedResp(oss);
}

void handlePostExecute (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace std;
   using namespace rapidjson;


   Document document;
        document.Parse(request.body.c_str());
        std::string payload = document["payload"].GetString();
        SendPhysiologyModification(payload, "", "", "");
        WriteResponse(oss, "{\"message\":\"Command executed\"}", "200 OK", "text/plain");
}

void handleOptionsExecute (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {
   MethodNotImplementedResp(oss);
}

void handlePostPhysMod (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace std;
   using namespace rapidjson;

   string type, location, practitioner, payload;
        Document document;
        document.Parse(request.body.c_str());
        if (document.HasMember("type")) {
            type = document["type"].GetString();
        }
        if (document.HasMember("location")) {
            location = document["location"].GetString();
        }
        if (document.HasMember("practitioner")) {
            practitioner = document["practitioner"].GetString();
        }
        if (document.HasMember("payload")) {
            payload = document["payload"].GetString();
        }
        SendPhysiologyModification(type, location, practitioner, payload);

        WriteResponse(oss, "{\"message\":\"Physiology modification published\"}", "200 OK", "text/plain");

}

void handlePostRendMod (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace std;
   using namespace rapidjson;

   string type, location, practitioner, payload;
        Document document;
        document.Parse(request.body.c_str());
        if (document.HasMember("type")) {
            type = document["type"].GetString();
        }
        if (document.HasMember("location")) {
            location = document["location"].GetString();
        }
        if (document.HasMember("practitioner")) {
            practitioner = document["practitioner"].GetString();
        }
        if (document.HasMember("payload")) {
            payload = document["payload"].GetString();
        }
        SendRenderModification(type, location, practitioner, payload);
        WriteResponse(oss, "{\"message\":\"Render modification published\"}", "200 OK", "text/plain");
}

void handlePostPerfAss (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace std;
   using namespace rapidjson;

   string type, location, practitioner, payload, info, step, comment;
        Document document;
        document.Parse(request.body.c_str());
        if (document.HasMember("type")) {
            type = document["type"].GetString();
        }
        if (document.HasMember("location")) {
            location = document["location"].GetString();
        }
        if (document.HasMember("practitioner")) {
            practitioner = document["practitioner"].GetString();
        }
        if (document.HasMember("info")) {
            info = document["info"].GetString();
        }
        if (document.HasMember("step")) {
            step = document["step"].GetString();
        }
        if (document.HasMember("comment")) {
            comment = document["comment"].GetString();
        }
        SendPerformanceAssessment(type, location, practitioner, info, step, comment);

        WriteResponse(oss, "{\"message\":\"Performance assessment published\"}", "200 OK", "text/plain");
}

void handlePostModType (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {
   MethodNotImplementedResp(oss);
}

void handleGetPatients (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace std;
   using namespace rapidjson;
   using namespace std::experimental::filesystem::v1;

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
                        writer.String(dir_itr->path().filename().string().c_str());
                        writer.Key("description");
                        stringstream writeTime;
                        // writeTime << last_write_time(dir_itr->path());
                        writer.String(writeTime.str().c_str());
                        writer.EndObject();
                    }
                }
            }
        }
        writer.EndArray();

        WriteResponse(oss, s.GetString(), "200 OK", "application/json");
}

void handleGetDeletedStateByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace std;
   using namespace rapidjson;
   using namespace std::experimental::filesystem::v1;

   string name;
   int err = ParseURLParam(request.url, urlTemplate, name);
   if (err != 0) {
      /// Handle error here.
   }

   /// BUG:
   /// Something here is causing an error in record.h.

      // if (name != "StandardMale@0s.xml") {
      //      ostringstream deleteFile;
      //      deleteFile << state_path << "/" << name;
      //      path deletePath(deleteFile.str().c_str());
      //      if (exists(deletePath) && is_regular_file(deletePath)) {
      //          LOG_INFO << "Deleting " << deletePath;
      //          remove(deletePath);
      //          WriteResponse(oss, "Deleted", "200 OK", "text/plain");
      //      } else {
      //        WriteResponse(oss, "Unable to delete state file", "403 Forbidden", "text/plain");
      //      }
      // } else {
      //    WriteResponse(oss, "Can not delete default state file", "403 Forbidden", "text/plain");
      // }
}

void handleGetCommandByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace std;
   using namespace rapidjson;

   string name;
   int err = ParseURLParam(request.url, urlTemplate, name);
   if (err != 0) {
      /// Handle error here.
   }

        SendCommand(name);
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartObject();
        writer.Key("Sent command");
        writer.String(name.c_str());
        writer.EndObject();

        WriteResponse(oss, s.GetString(), "200 OK", "text/plain");

}

void handleGetLabs (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   using namespace std;
   using namespace rapidjson;

   string labReport = boost::algorithm::join(labsStorage, "\n");
   WriteResponse(oss, labReport, "200 OK", "text/csv");
}

/// CORE FUNCTIONS

void SendPhysiologyModification(const std::string &type,
                                const std::string &location,
                                const std::string &practitioner,
                                const std::string &payload) {
   using namespace AMM;
   using namespace AMM::Capability;
    LOG_DEBUG << "Publishing a phys mod: " << type;
    AMM::Physiology::Modification modInstance;
    modInstance.type(type);
    //  modInstance.location.description(location);
    modInstance.practitioner(practitioner);
    modInstance.payload(payload);
    AMMData::mgr->PublishPhysiologyModification(modInstance);
}

void SendRenderModification(const std::string &type,
                            const std::string &location,
                            const std::string &practitioner,
                            const std::string &payload) {
   using namespace AMM;
   using namespace AMM::Capability;
    LOG_DEBUG << "Publishing a render mod: " << type;
    AMM::Render::Modification modInstance;
    modInstance.type(type);
    FMA_Location fma_location;
    fma_location.description(location);
    modInstance.location(fma_location);
    modInstance.practitioner(practitioner);
    modInstance.payload(payload);
    AMMData::mgr->PublishRenderModification(modInstance);
}

void SendPerformanceAssessment(const std::string &assessment_type,
                               const std::string &location,
                               const std::string &practitioner,
                               const std::string &assessment_info,
                               const std::string &step,
                               const std::string &comment) {
    LOG_INFO << "Publishing an assessment: " << assessment_type;
    AMM::Performance::Assessment assessInstance;
    assessInstance.assessment_type(assessment_type);
    // location
    assessInstance.learner_id(practitioner);
    assessInstance.assessment_info(assessment_info);
    assessInstance.step(step);
    assessInstance.comment(comment);
    AMMData::mgr->PublishPerformanceData(assessInstance);
}

void SendCommand(const std::string &command) {
    LOG_INFO << "Publishing a command:" << command;
    AMM::PatientAction::BioGears::Command cmdInstance;
    cmdInstance.message(command);
    AMMData::mgr->PublishCommand(cmdInstance);
}

void ResetLabs() {
    labsStorage.clear();
    std::ostringstream labRow;

    labRow << "Time,";

// POCT
    labRow << "POCT,";
    labRow <<  "Sodium (Na),";
    labRow <<  "Potassium (K),";
    labRow <<  "Chloride (Cl),";
    labRow << "TCO2,";
    labRow << "Anion Gap,"; // Anion Gap
    labRow << "Ionized Calcium (iCa),"; // Ionized Calcium (iCa)
    labRow << "Glucose (Glu),";
    labRow << "Urea Nitrogen (BUN)/Urea,";
    labRow << "Creatinine (Crea),";

// Hematology
    labRow << "Hematology,";
    labRow << "Hematocrit (Hct),";
    labRow << "Hemoglobin (Hgb),";

//ABG
    labRow << "ABG,";
    labRow << "Lactate,";
    labRow << "pH,";
    labRow << "PCO2,";
    labRow << "PO2,";
    labRow << "TCO2,";
    labRow << "HCO3,";
    labRow << "Base Excess (BE),";
    labRow << "SpO2,";
    labRow << "COHb,";

// VBG
    labRow << "VBG,";
    labRow << "Lactate,";
    labRow << "pH,";
    labRow << "PCO2,";
    labRow << "TCO2,";
    labRow << "HCO3,";
    labRow << "Base Excess (BE),";
    labRow << "COHb,";


    // BMP
    labRow << "BMP,";
    labRow << "Sodium (Na),";
    labRow << "Potassium (K),";
    labRow << "Chloride (Cl),";
    labRow << "TCO2,";
    labRow << "Anion Gap,"; // Anion Gap
    labRow << "Ionized Calcium (iCa),"; // Ionized Calcium (iCa)
    labRow << "Glucose (Glu),";
    labRow << "Urea Nitrogen (BUN)/Urea,";
    labRow << "Creatinine (Crea),";


    // CBC
    labRow << "CBC,";
    labRow << "WBC,";
    labRow << "RBC,";
    labRow << "Hgb,";
    labRow << "Hct,";
    labRow << "Plt,";

// CMP
    labRow << "CMP,";
    labRow << "Albumin,";
    labRow << "ALP,"; // ALP
    labRow << "ALT,"; // ALT
    labRow << "AST,"; // AST
    labRow << "BUN,";
    labRow << "Calcium,";
    labRow << "Chloride,";
    labRow << "CO2,";
    labRow << "Creatinine (men),";
    labRow << "Creatinine (women),";
    labRow << "Glucose,";
    labRow << "Potassium,";
    labRow << "Sodium,";
    labRow << "Total bilirubin,";
    labRow << "Total protein";
    labsStorage.push_back(labRow.str());
}

void AppendLabRow() {
    std::ostringstream labRow;

    labRow << nodeDataStorage["SIM_TIME"] << ",";

// POCT
    labRow << "POCT,";
    labRow << nodeDataStorage["Substance_Sodium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Potassium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Chloride"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << ","; // Anion Gap
    labRow << ","; // Ionized Calcium (iCa)
    labRow << nodeDataStorage["Substance_Glucose_Concentration"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodUreaNitrogen_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Creatinine_Concentration"] << ",";

// Hematology
    labRow << "Hematology,";
    labRow << nodeDataStorage["BloodChemistry_Hemaocrit"] << ",";
    labRow << nodeDataStorage["Substance_Hemoglobin_Concentration"] << ",";

//ABG
    labRow << "ABG,";
    labRow << nodeDataStorage["Substance_Lactate_Concentration_mmol"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodPH"] << ",";
    labRow << nodeDataStorage["BloodChemistry_Arterial_CarbonDioxide_Pressure"] << ",";
    labRow << nodeDataStorage["BloodChemistry_Arterial_Oxygen_Pressure"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << nodeDataStorage["Substance_Bicarbonate"] << ",";
    labRow << nodeDataStorage["Substance_BaseExcess"] << ",";
    labRow << nodeDataStorage["BloodChemistry_Oxygen_Saturation"] << ",";
    labRow << nodeDataStorage["Substance_Carboxyhemoglobin_Concentration"] << ",";

// VBG
    labRow << "VBG,";
    labRow << nodeDataStorage["Substance_Lactate_Concentration_mmol"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodPH"] << ",";
    labRow << nodeDataStorage["BloodChemistry_VenousCarbonDioxidePressure"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << nodeDataStorage["Substance_Bicarbonate"] << ",";
    labRow << nodeDataStorage["Substance_BaseExcess"] << ",";
    labRow << nodeDataStorage["Substance_Carboxyhemoglobin_Concentration"] << ",";


    // BMP
    labRow << "BMP,";
    labRow << nodeDataStorage["Substance_Sodium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Potassium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Chloride"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << ","; // Anion Gap
    labRow << ","; // Ionized Calcium (iCa)
    labRow << nodeDataStorage["Substance_Glucose_Concentration"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodUreaNitrogen_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Creatinine_Concentration"] << ",";


    // CBC
    labRow << "CBC,";
    labRow << nodeDataStorage["BloodChemistry_WhiteBloodCell_Count"] << ",";
    labRow << nodeDataStorage["BloodChemistry_RedBloodCell_Count"] << ",";
    labRow << nodeDataStorage["Substance_Hemoglobin_Concentration"] << ",";
    labRow << nodeDataStorage["BloodChemistry_Hemaocrit"] << ",";
    labRow << nodeDataStorage["CompleteBloodCount_Platelet"] << ",";

// CMP
    labRow << "CMP,";
    labRow << nodeDataStorage["Substance_Albumin_Concentration"] << ",";
    labRow << ","; // ALP
    labRow << ","; // ALT
    labRow << ","; // AST
    labRow << nodeDataStorage["BloodChemistry_BloodUreaNitrogen_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Calcium_Concentration"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Chloride"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << nodeDataStorage["Substance_Creatinine_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Creatinine_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Glucose_Concentration"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Potassium"] << ",";
    labRow << nodeDataStorage["Substance_Sodium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Bilirubin"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Protein"];
    labsStorage.push_back(labRow.str());
}
