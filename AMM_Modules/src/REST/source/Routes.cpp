#include <fstream>
#include <istream>
#include <sstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

/// All the following route-binded functions mutate ostringstream that then get written out to the client.

void getInstance (std::ostringstream* oss) {

   rapidjson::StringBuffer s;
   rapidjson::Writer<rapidjson::StringBuffer> writer(s);

   std::ifstream t("static/current_scenario.txt");
   std::string scenario((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
   t.close();

   writer.StartObject();
   writer.Key("name");
   writer.String("TEST_HOST");
   writer.Key("scenario");
   writer.String(scenario.c_str());
   writer.EndObject();

   // response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
   // response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
   std::string str = s.GetString();
   *oss << "HTTP/1.1 200 Ok\r\n";
   *oss << "Cache-Control: no-cache, private\r\n";
   *oss << "Content-Type: application/json\r\n";
   *oss << "Content-Length: ";
   *oss << str.length();
   *oss << "\r\n\r\n";
   *oss << s.GetString();
}

// void getStates (std::ostringstream* oss) {
//
//    using namespace rapidjson
//
//    StringBuffer s;
//    Writer<StringBuffer> writer(s);
//
//    writer.StartArray();
//    if (exists(state_path) && is_directory(state_path)) {
//       path p(state_path);
//       if (is_directory(p)) {
//          directory_iterator end_iter;
//          for (directory_iterator dir_itr(p); dir_itr != end_iter; ++dir_itr) {
//             if (is_regular_file(dir_itr->status())) {
//                writer.StartObject();
//                writer.Key("name");
//                writer.String(dir_itr->path().filename().c_str());
//                writer.Key("description");
//                stringstream writeTime;
//                writeTime << last_write_time(dir_itr->path());
//                writer.String(writeTime.str().c_str());
//                writer.EndObject();
//             }
//          }
//       }
//    }
//    writer.EndArray();
//
//    // response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
//    // response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
//
//    *oss << "HTTP/1.1 200 Ok\r\n";
//    *oss << "Cache-Control: no-cache, private\r\n";
//    *oss << "Content-Type: application/json\r\n";
//    *oss << "Content-Length: 25\r\n";
//    *oss << "\r\n";
//    *oss << s.GetString();
// }
