#include <fstream>
#include <istream>
#include <sstream>
#include <iostream>
#include <typeinfo>

// #include "stdafx.h"

// #include <fastrtps/fastrtps_fwd.h>

// #include <fastrtps/Domain.h>

// #include <fastrtps/participant/Participant.h>
// #include <fastrtps/participant/ParticipantListener.h>

// #include <fastrtps/publisher/Publisher.h>
// #include <fastrtps/publisher/PublisherListener.h>
// #include <fastrtps/subscriber/SampleInfo.h>
// #include <fastrtps/subscriber/Subscriber.h>
// #include <fastrtps/subscriber/SubscriberListener.h>

// #include <fastrtps/rtps/RTPSDomain.h>
// #include <fastrtps/rtps/builtin/data/WriterProxyData.h>
// #include <fastrtps/rtps/builtin/discovery/endpoint/EDPSimple.h>
// #include <fastrtps/rtps/builtin/data/ReaderProxyData.h>
// #include <fastrtps/rtps/builtin/discovery/participant/PDPSimple.h>
// #include <fastrtps/rtps/builtin/discovery/participant/PDPSimpleListener.h>
// #include <fastrtps/rtps/builtin/BuiltinProtocols.h>
// #include <fastrtps/rtps/builtin/liveliness/WLP.h>
// #include <fastrtps/rtps/builtin/discovery/endpoint/EDPStatic.h>
// #include <fastrtps/rtps/resources/AsyncWriterThread.h>
// #include <fastrtps/rtps/writer/StatelessWriter.h>
// #include <fastrtps/rtps/reader/StatelessReader.h>
// #include <fastrtps/rtps/reader/ReaderListener.h>
// #include <fastrtps/rtps/reader/WriterProxy.h>
// #include <fastrtps/rtps/history/ReaderHistory.h>
// #include <fastrtps/rtps/history/WriterHistory.h>
// #include <fastrtps/rtps/participant/RTPSParticipant.h>

// #include <fastrtps/utils/eClock.h>
// #include <fastrtps/utils/TimeConversion.h>

// #include <Net/UdpDiscoveryServer.h>
// #include "tinyxml2.h"
// #include <thirdparty/sqlite_modern_cpp.h>

// #include "AMM/BaseLogger.h"
// #include "AMM/DDS_Log_Appender.h"
// #include "AMM/DDS_Manager.h"
// #include "AMM/Utility.h"

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

std::map<std::string, std::string> statusStorage = {
   {"STATUS",         "NOT RUNNING"},
   {"TICK",           "0"},
   {"TIME",           "0"},
   {"SCENARIO",       ""},
   {"STATE",          ""},
   {"CLEAR_SUPPLY",   ""},
   {"BLOOD_SUPPLY",   ""},
   {"FLUIDICS_STATE", ""},
   {"IVARM_STATE",    ""}
};

// bool m_runThread = false;
// int64_t lastTick = 0;


// DDS_Manager *mgr;
// Participant *mp_participant;
// boost::asio::io_service io_service;
// sqlite::database db("amm.db");


/// All the following route-binded functions mutate ostringstream that then get written out to the client.

void test (std::ostringstream& oss, HttpRequest &request, std::string urlTemplate) {

   UrlParam up;
   {
      int err = ParseURLParam(request.url, urlTemplate, up);
      if (err != 0) {
         /// Handle error here.
      }
   }

   std::string body;
   body += "Test successful!\n";
   body += "URL param: " + up.param;

   oss << "HTTP/1.1 200 OK\r\n";
   oss << "Access-Control-Allow-Origin: *\r\n";
   oss << "Cache-Control: no-cache, private\r\n";
   oss << "Content-Type: text/plain\r\n";
   oss << "Content-Length: ";
   oss << body.length();
   oss << "\r\n";
   oss << "\r\n";
   oss << body;
}

void getInstance (std::ostringstream& oss, HttpRequest &request, std::string urlTemplate) {

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

void getStates(std::ostringstream& oss, HttpRequest &request, std::string urlTemplate) {

   std::cout << "Enter getStates." << std::endl;

   using namespace rapidjson;
   // using namespace boost::filesystem;
   using namespace std::experimental::filesystem::v1;

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

               /// TODO:
               /// - Fix cast error on `filename` going into `writer.String()`
               /// - Fix `<<` operator error on `writeTime`

               // writer.String(filename);
               writer.Key("description");
               std::stringstream writeTime;
               // writeTime << last_write_time(dir_itr->path());
               writer.String(writeTime.str().c_str());
               writer.EndObject();
            }
         }
      }
   }

   /// ORIGINAL CODE
   // if (exists(state_path) && is_directory(state_path)) {
   //    path p(state_path);
   //    if (is_directory(p)) {
   //       directory_iterator end_iter;
   //       for (directory_iterator dir_itr(p); dir_itr != end_iter; ++dir_itr) {
   //          if (is_regular_file(dir_itr->status())) {
   //             writer.StartObject();
   //             writer.Key("name");
   //             // std::cout << dir_itr->path().filename().c_str() << std::endl;
   //             // writer.String(dir_itr->path().filename().c_str());
   //             writer.Key("description");
   //             stringstream writeTime;
   //             writeTime << last_write_time(dir_itr->path());
   //             writer.String(writeTime.str().c_str());
   //             writer.EndObject();
   //          }
   //       }
   //    }
   // }

   writer.EndArray();

   // response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
   // response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));

   std::cout << "Writing response." << std::endl;

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

// void deleteStates (std::ostringstream* oss) {
//
//    auto name =
//
//    std::string str = s.GetString();
//    *oss << "HTTP/1.1 200 Ok\r\n";
//    *oss << "Access-Control-Allow-Origin: *\r\n";
//    *oss << "Cache-Control: no-cache, private\r\n";
//    *oss << "Content-Type: application/json\r\n";
//    *oss << "Content-Length: ";
//    *oss << str.length();
//    *oss << "\r\n\r\n";
//    *oss << s.GetString();
// }


void getNodes (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

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

void getNodeByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate) {

   UrlParam up;
   int err = ParseURLParam(request.url, urlTemplate, up);
   {
      if (err != 0) {
         /// Handle error here.
      }
   }

   using namespace rapidjson;

   std::string name = up.param;
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
   }
}
