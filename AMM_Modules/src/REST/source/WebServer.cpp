#include <string>
// #include <istream>
// #include <sstream>
// #include <fstream>
#include <iostream>
// #include <streambuf>
// #include <vector>
// #include <iterator>

#include "REST/headers/WebServer.hxx"
#include "REST/headers/HttpRequest.hxx"
#include "REST/headers/Routes.hxx"




void WebServer::OnMessageReceived (int clientSocket, const char* msg, int length) {

   /// Error code for error handling.
   /// 0 = no error;
   int err;

   HttpRequest request;
   err = ParseRequest(msg, length, request);
   if (err != 0) {
      /// Handle error here.
      std::cout << "Something happened when parsing the request." << std::endl;
      return;
   }


   /// Response object where all headers and body is written to.
   std::ostringstream oss;

   if (CompareURLWithEndPointMatch(request.url, "/test/:var") == 0) {
      test(oss, request, "/test/:var");

   } else if (request.url == "/instance") {

      if (request.method == Method::GET) {
         handleGetInstance(oss, request, "/instance");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/nodes") {

      if (request.method == Method::GET) {
         handleGetNodes(oss, request, "/nodes");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (CompareURLWithEndPointMatch(request.url, "/node/:name") == 0) {

      if (request.method == Method::GET) {
         handleGetNodeByName(oss, request, "/node/:name");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/ready") {

      if (request.method == Method::GET) {
         handleGetReady(oss, request, "/ready");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/debug") {

      if (request.method == Method::GET) {
         handleGetDebug(oss, request, "/debug");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/events") {

      if (request.method == Method::GET) {
         handleGetEvents(oss, request, "/events");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/logs") {

      if (request.method == Method::GET) {
         handleGetLogs(oss, request, "/logs");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/shutdown") {

      if (request.method == Method::GET) {
         handleGetShutdown(oss, request, "/shutdown");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/modules") {

      if (request.method == Method::GET) {
         handleGetModules(oss, request, "/modules");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/modules/count") {

      if (request.method == Method::GET) {
         handleGetModulesCount(oss, request, "/modules/count");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (CompareURLWithEndPointMatch(request.url, "/module/id/:id") == 0) {

      if (request.method == Method::GET) {
         handleGetModuleById(oss, request, "/module/id/:id");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (CompareURLWithEndPointMatch(request.url, "/module/guid/:guid") == 0) {

      if (request.method == Method::GET) {
         handleGetModuleByGuid(oss, request, "/module/guid/:guid");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/actions") {
      /// BUG:
      /// subscript out of range when this handler is called.
      if (request.method == Method::GET) {
         handleGetActions(oss, request, "/actions");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/action") {
      /// BUG:
      /// subscript out of range when this handler is called.
      if (request.method == Method::POST) {
         handlePostAction(oss, request, "/action");
      } else {
         MethodNotAllowedResp(oss);
      }


   } else if (CompareURLWithEndPointMatch(request.url, "/action/:name") == 0) {

      if (request.method == Method::GET) {
         handleGetActionByName(oss, request, "/action/:name");
      } else if (request.method == Method::PUT) {
         handlePutActionByName(oss, request, "/action/:name");
      } else if (request.method == Method::REMOVE) {
         handleDeleteActionByName(oss, request, "/action/:name");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/execute") {

      if (request.method == Method::POST) {
         handlePostExecute(oss, request, "/execute");
      } else if (request.method == Method::OPTIONS) {
         handleOptionsExecute(oss, request, "/execute");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/patients") {

      if (request.method == Method::GET) {
         handleGetPatients(oss, request, "/patients");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/states") {

      if (request.method == Method::GET) {
         handleGetStates(oss, request, "/states");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (CompareURLWithEndPointMatch(request.url, "/states/:name/delete") == 0) {

      if (request.method == Method::GET) {
         handleGetDeletedStateByName(oss, request, "/states/:name/delete");
      } else {
         MethodNotAllowedResp(oss);
      }


   } else if (request.url == "/topic/physiology_modification") {

      if (request.method == Method::POST) {
         handlePostPhysMod(oss, request, "/topic/physiology_modification");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/topic/render_modification") {

      if (request.method == Method::POST) {
         handlePostRendMod(oss, request, "/topic/render_modification");
      } else {
         MethodNotAllowedResp(oss);
      }


   } else if (request.url == "/topic/performance_modification") {

      if (request.method == Method::POST) {
         handlePostPhysMod(oss, request, "/topic/performance_modification");
      } else {
         MethodNotAllowedResp(oss);
      }


   } else if (CompareURLWithEndPointMatch(request.url, "/topic/:mod_type") == 0) {

      if (request.method == Method::POST) {
         handlePostModType(oss, request, "/topic/:mod_type");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (request.url == "/modules/count") {

      if (request.method == Method::GET) {
         handleGetModulesCount(oss, request, "/modules/count");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (CompareURLWithEndPointMatch(request.url, "/module/id/:id") == 0) {

      if (request.method == Method::GET) {
         handleGetModuleById(oss, request, "/module/id/:id");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else if (CompareURLWithEndPointMatch(request.url, "/command/:name") == 0) {

      if (request.method == Method::GET) {
         handleGetCommandByName(oss, request, "/command/:name");
      } else {
         MethodNotAllowedResp(oss);
      }

   } else {
      std::string resp;
      resp += "HTTP/1.1 404 Not Found\r\n";
      resp += "Access-Control-Allow-Origin: *\r\n";
      resp += "Cache-Control: no-cache, private\r\n";
      resp += "Content-Type: text/plain\r\n";
      resp += "Content-Length: 16\r\n";
      resp += "\r\n";
      resp += "404 -- Not Found";
      std::cout << "\n" << resp << std::endl;
      oss << resp;
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
