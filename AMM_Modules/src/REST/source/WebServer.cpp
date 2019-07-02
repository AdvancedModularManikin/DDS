#include <string>
#include <istream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <vector>
#include <iterator>

#include "REST/headers/WebServer.hxx"
#include "REST/headers/HttpRequest.hxx"
#include "REST/headers/Routes.hxx"

void WebServer::MethodNotAllowedResp (std::ostringstream* oss) {
   *oss << "HTTP/1.1 404 Not Found\r\n";
   *oss << "Cache-Control: no-cache, private\r\n";
   *oss << "Content-Type: text/plain\r\n";
   *oss << "Content-Length: 25\r\n";
   *oss << "\r\n";
   *oss << "405 -- Method Not Allowed";
}


void WebServer::OnMessageReceived (int clientSocket, const char* msg, int length) {

   /// GET /index.html HTTP/1.1

   std::istringstream iss(msg);
   std::vector<std::string> parsed((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

   /// Request outputs
   // for (int i = 0; i < parsed.size(); i++) {
   //    std::cout << "Element: " + parsed[i] + "\n" << std::endl;
   // }

   HttpRequest request {
      ParseMethodFromText(parsed[0]),
      parsed[1],
   };


   /// MAGIC HAPPENS HERE!!

   /// Init routes (by comparing the string literal of the url to call a function.)

   std::ostringstream oss;
   if (request.url == "/test") {
      oss << "HTTP/1.1 200 OK\r\n";
      oss << "Cache-Control: no-cache, private\r\n";
      oss << "Content-Type: text/plain\r\n";
      oss << "Content-Length: 16\r\n";
      oss << "\r\n";
      oss << "Test successful!";

   } else if (request.url == "/instance") {

      getInstance(&oss);


   } else if (request.url == "/nodes") {


      /// TODO: Need variable binding for name.
   } else if (request.url == "/node/:name") {


   } else if (request.url == "/ready") {


   } else if (request.url == "/debug") {


   } else if (request.url == "/events") {


   } else if (request.url == "/logs") {


   } else if (request.url == "/shutdown") {


   } else if (request.url == "/modules") {


   } else if (request.url == "/actions") {


   } else if (request.url == "/action") {

      switch (request.method) {
         case Method::POST: {

            break;
         }

         default: {
            WebServer::MethodNotAllowedResp(&oss);
         }
      }

   /// TODO: Need variable binding for id.
   } else if (request.url == "/action/:name") {

      switch (request.method) {
         case Method::PUT: {

            break;
         }

         case Method::REMOVE: {

            break;
         }

         default: {
            WebServer::MethodNotAllowedResp(&oss);
         }
      }


   } else if (request.url == "/execute") {

      switch (request.method) {
         case Method::POST: {

            break;
         }

         case Method::OPTIONS: {

            break;
         }

         default: {
            WebServer::MethodNotAllowedResp(&oss);
         }
      }


   } else if (request.url == "/patients") {


   } else if (request.url == "/states") {

      // getStates

   } else if (request.url == "/states/:name/delete") {

      switch (request.method) {
         case Method::REMOVE: {

            break;
         }

         default: {
            WebServer::MethodNotAllowedResp(&oss);
         }
      }


   /// TODO: Need variable binding for id.
   } else if (request.url == "/execute/:name/delete") {


   } else if (request.url == "/topic/physiology_modification") {

      switch (request.method) {
         case Method::POST: {

            break;
         }

         default: {
            WebServer::MethodNotAllowedResp(&oss);
         }
      }


   } else if (request.url == "/topic/render_modification") {

      switch (request.method) {
         case Method::POST: {

            break;
         }

         default: {
            WebServer::MethodNotAllowedResp(&oss);
         }
      }


   } else if (request.url == "/topic/performance_modification") {

      switch (request.method) {
         case Method::POST: {

            break;
         }

         default: {
            WebServer::MethodNotAllowedResp(&oss);
         }
      }


   /// TODO: Need variable binding for mod_type.
   } else if (request.url == "/topic/:mod_type") {

      switch (request.method) {
         case Method::OPTIONS: {

            break;
         }

         default: {
            WebServer::MethodNotAllowedResp(&oss);
         }
      }


   } else if (request.url == "/modules/count") {


   /// TODO: Need variable binding for id.
   } else if (request.url == "/module/id/:id") {


   /// TODO: Need variable binding for name.
   } else if (request.url == "/command/:name") {


   /// TODO: Need variable binding for id.
   } else if (request.url == "/command/id/:id") {


   /// TODO: Need variable binding for name.
   } else if (request.url == "/command/:name") {

   } else {
      oss << "HTTP/1.1 404 Not Found\r\n";
      oss << "Cache-Control: no-cache, private\r\n";
      oss << "Content-Type: text/plain\r\n";
      oss << "Content-Length: 16\r\n";
      oss << "\r\n";
      oss << "404 -- Not Found";
   }

   /// =====

   std::string output = oss.str();
   int size = output.size() + 1;
   SendToClient(clientSocket, output.c_str(), size);

}



void WebServer::OnClientConnected (int clientSocket) {

}

void WebServer::OnClientDisconnected (int clientSocket) {

}
