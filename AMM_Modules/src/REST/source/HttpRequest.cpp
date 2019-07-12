
#include <string>
#include <iostream>
#include <istream>
#include <sstream>
#include <vector>
#include <iterator>

// #include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>

#include "REST/headers/HttpRequest.hxx"



int ParseRequest (const char* msg, int length, HttpRequest& request) {

   /// Not body content length;
   request.contentLength = length;

   std::vector<std::string> httprequest;

   std::istringstream iss(msg);

   std::string newline;
   for (int i = 0; std::getline(iss, newline); ) {
      httprequest.push_back(newline);
   }

   for (int i = 0; i < httprequest.size(); i++) {
      std::cout << i << ":  " << httprequest[i] << std::endl;
   }

   /// Determines if there is body content to parse.
   bool isThereBody = false;
   {
      for (int i = 0; i < httprequest.size(); i++) {
         std::string substr = httprequest[i].substr(0, 13);
         if (substr == "Content-Length") {
            isThereBody = true;
         }
      }

   }

   {
      /// NOTE:
      /// Determine where the content body is in the request, by looking
      /// for a blank line. If there is no body after the blank line,
      /// request.body should be "";

      /// Determines where teh body content is if any.
      int requestBodyIndex = 0;

      for (int i = 0; i < httprequest.size(); i++) {
         if (httprequest[i].length() == 1) {
            requestBodyIndex = i + 1;
            break;
         }
      }

      if (isThereBody) {
         request.body = httprequest[requestBodyIndex];
      } else {
         request.body = "";
      }
   }

   {
      /// Gets method info and url from the first header.

      using namespace boost;

      std::vector<std::string> words;
      split(words, httprequest[0], is_any_of(" "));

      if      (words[0] == "GET")      { request.method = Method::GET; }
      else if (words[0] == "POST")     { request.method = Method::POST; }
      else if (words[0] == "PUT")      { request.method = Method::PUT; }
      else if (words[0] == "PATCH")    { request.method = Method::PATCH; }
      else if (words[0] == "DELETE")   { request.method = Method::REMOVE; }
      else if (words[0] == "COPY")     { request.method = Method::HEAD; }
      else if (words[0] == "OPTIONS")  { request.method = Method::OPTIONS; }
      else if (words[0] == "LINK")     { request.method = Method::LINK; }
      else if (words[0] == "UNLINK")   { request.method = Method::UNLINK; }
      else if (words[0] == "PURGE")    { request.method = Method::PURGE; }
      else if (words[0] == "LOCK")     { request.method = Method::LOCK; }
      else if (words[0] == "UNLOCK")   { request.method = Method::UNLOCK; }
      else if (words[0] == "PROPFIND") { request.method = Method::PROPFIND; }
      else if (words[0] == "VIEW")     { request.method = Method::VIEW; }
      else    { return 1; }

      request.url = words[1];
   }


   return 0;
}

int ParseURLParam (std::string url, std::string endpointMatch, std::string& param) {

   /// Assumes URL contains a single param.
   /// If multiple, returns the last occurrence.

   using namespace std;
   using namespace boost;

   vector<string> urldirs;
   split(urldirs, url, is_any_of("/"));

   vector<string> epdirs;
   split(epdirs, endpointMatch, is_any_of("/"));

   int paramPos = 0;

   if (urldirs.size() != epdirs.size()) {
      /// URL length doesn't match, so URL doesn't match.
      return 1;
   } else {
      for (int i = 0; i < urldirs.size(); i++) {
         if (epdirs[i][0] == ':') {
            /// Set position of this param and skip th enext check.
            paramPos = i;
            continue;
         }
         if (urldirs[i] != epdirs[i]) {
            /// URLs don't match.
            return 1;
         }
      }
   }

   /// If paramPos is 0, this means the template does not require a param,
   /// so an empty string will be given.
   if (paramPos != 0) {
      param = urldirs[paramPos];
   } else {
      param = "";
   }

   return 0;
}

int CompareURLWithEndPointMatch (std::string url, std::string endpointMatch) {

   /// NOTE:
   /// Assumes URL contians a single

   using namespace std;
   using namespace boost;

   vector<string> urldirs;
   split(urldirs, url, is_any_of("/"));

   vector<string> epdirs;
   split(epdirs, endpointMatch, is_any_of("/"));


   if (urldirs.size() != epdirs.size()) {
      return 1;
   } else {
      for (int i = 0; i < urldirs.size(); i++) {
         if (epdirs[i][0] == ':') {
            continue;
         }
         if (urldirs[i] != epdirs[i]) {
            return 1;
         }
      }
   }

   return 0;
}

void PrintHttpRequest (HttpRequest& r) {

   using namespace std;

   cout << "HttpRequest:"  << endl;
   cout << r.method        << endl;
   cout << r.contentLength << endl;
   cout << r.url           << endl;
   cout << r.body          << endl;
}

void MethodNotAllowedResp (std::ostringstream& oss) {

   std::string resp;

   resp += "HTTP/1.1 405 Method Not Allowed\r\n";
   resp += "Cache-Control: no-cache, private\r\n";
   resp += "Content-Type: text/plain\r\n";
   resp += "Content-Length: 25\r\n";
   resp += "\r\n";
   resp += "405 -- Method Not Allowed";

   std::cout << "\n" << resp << std::endl;
   oss << resp;
}
