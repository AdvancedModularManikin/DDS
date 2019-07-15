#pragma once

#include <string>
#include <sstream>

enum Method {
   GET = 0,
   POST,
   PUT,
   PATCH,
   REMOVE, /// DELETE,
   COPY,
   HEAD,
   OPTIONS,
   LINK,
   UNLINK,
   PURGE,
   LOCK,
   UNLOCK,
   PROPFIND,
   VIEW
};

struct HttpRequest {
   Method method;
   int contentLength;
   std::string url;
   std::string body;
};

struct UrlParam {
   std::string beforeParam;
   std::string param;
   std::string afterParam;
   int paramStartPos;
   int paramEndPos;
};

int ParseRequest (const char* msg, int length, HttpRequest& request);

void PrintHttpRequest (HttpRequest& r);

/// Sets `up` and returns 0 if there is a valid param in the url.
int ParseURLParam (std::string url, std::string endpointMatch, std::string& param);

/// Returns 0 if the url with param matches the endpoint template.
/// Only use when url endpoint contains a param that needs to be parsed.
int CompareURLWithEndPointMatch (std::string url, std::string endpointMatch);

void MethodNotAllowedResp (std::ostringstream& oss);

void MethodNotImplementedResp (std::ostringstream& oss);


void WriteResponse (std::ostringstream& oss, std::string body, std::string code, std::string mime);
