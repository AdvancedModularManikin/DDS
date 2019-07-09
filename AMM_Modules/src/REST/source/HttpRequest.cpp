
#include <string>
#include <iostream>
#include "REST/headers/HttpRequest.hxx"

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

   std::cout << up.beforeParam << std::endl;
   std::cout << up.param << std::endl;
   std::cout << up.afterParam << std::endl;
   std::cout << up.paramStartPos << std::endl;
   std::cout << up.paramEndPos << std::endl;
}
