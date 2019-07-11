
#include <string>
#include <iostream>
#include <istream>
#include <sstream>
#include <vector>
#include <iterator>
#include "REST/headers/HttpRequest.hxx"



int ParseRequest (const char* msg, int length, HttpRequest& request) {

   request.contentLength = length;

   std::vector<std::string> httprequest;

   std::istringstream iss(msg);

   std::string newline;
   for (int i = 0; std::getline(iss, newline); ) {
      httprequest.push_back(newline);
   }

   {
      /// DEBUG OUT
      // for (int i = 0; i < httprequest.size(); i++) {
      //    std::cout << i << ":  " << httprequest[i] << std::endl;
      // }

      int requestBodyIndex;

      for (int i = 0; i < httprequest.size(); i++) {
         if (httprequest[i].length() == 1) {
            requestBodyIndex = i + 1;
            break;
         }
      }

      request.body = httprequest[requestBodyIndex];
   }

   {
      std::vector<std::string> words;
      int wordstop1 = 0;
      int wordstop2 = 0;
      for (int i = 0; i < httprequest[0].length(); i++) {
         if (httprequest[0][i] == ' ') {
            wordstop1 = wordstop2;
            wordstop2 = i;
            std::string newword;
            for (int j = wordstop1; j < wordstop2; j++) {
               if (httprequest[0][j] == ' ') {
                  /// Fix to prevent spaces from appearing in front of
                  /// every other word after the first.
                  continue;
               }
               newword += httprequest[0][j];
            }
            words.push_back(newword);
         }
      }

      if (words[0] == "GET") { request.method = Method::GET; }
      else if (words[0] == "POST") { request.method = Method::POST; }
      else { return 1; }

      request.url = words[1];
   }

   PrintHttpRequest(request);


   return 0;
}

int ParseMethodFromText (std::string strMethod, Method& method) {
   if (strMethod == "GET")  { method = Method::GET;  return 0; }
   if (strMethod == "POST") { method = Method::POST; return 0; }

   return 1;
}

int ParseURLParam (std::string url, std::string endpointMatch, UrlParam& up) {

   /// Assumes URL contains a single param.
   /// If multiple, returns the first occurrence.

   up.paramStartPos = 0;
   up.paramEndPos = 0;

   for (int i = 0; i < endpointMatch.length(); i++) {
      if (endpointMatch[i] == ':') {
         up.paramStartPos = i;
         break;
      }
   }

   if (up.paramStartPos == 0) {return 1;}


   for (int i = up.paramStartPos; i < url.length(); i++) {
      if (url[i] == '/') {
         up.paramEndPos = i;
         break;
      }
   }

   if (up.paramEndPos == 0) {up.paramEndPos = url.length();}

   for (int i = 0; i < up.paramStartPos; i++) {
      up.beforeParam += url[i];
   }

   for (int i = up.paramStartPos; i < up.paramEndPos; i++) {
      up.param += url[i];
   }

   for (int i = up.paramEndPos; i < url.length(); i++) {
      up.afterParam += url[i];
   }

   return 0;
}

int CompareURLWithEndPointMatch (std::string url, std::string endpointMatch) {

   /// NOTE:
   /// Assumes URL contians a single

   UrlParam up;
   int err = ParseURLParam(url, endpointMatch, up);
   if (err != 0) {
      /// Catch and handle errors here.
   }

   UrlParam ep;
   err = ParseURLParam(endpointMatch, endpointMatch, ep);
   if (err != 0) {
      /// Catch and handle errors here.
   }

   if ( (up.beforeParam + up.afterParam) != (ep.beforeParam + ep.afterParam) ) {
      return 1;
   }

   // if (up.param.length() == 0) {
   //    return 1;
   // }

   return 0;
}


void PrintUrlParam (UrlParam& up) {
   std::cout << "UrlParam:" << std::endl;
   std::cout << up.beforeParam << std::endl;
   std::cout << up.param << std::endl;
   std::cout << up.afterParam << std::endl;
   std::cout << up.paramStartPos << std::endl;
   std::cout << up.paramEndPos << std::endl;
}

void PrintHttpRequest (HttpRequest& r) {
   std::cout << "HttpRequest:" << std::endl;
   std::cout << r.method << std::endl;
   std::cout << r.contentLength << std::endl;
   std::cout << r.url << std::endl;
   std::cout << r.body << std::endl;
}
