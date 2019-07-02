
#include <string>
#include "REST/headers/HttpRequest.hxx"

Method ParseMethodFromText (std::string method) {
   if (method == "GET")  return Method::GET;
   if (method == "POST") return Method::POST;
   return Method::NONE;
}
