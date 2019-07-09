#pragma once

#include <string>

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

void PrintUrlParam (UrlParam& up);

/// Sets `method` and returns 0 if a valid method was parsed.
int ParseMethodFromText (std::string strMethod, Method& method);

/// Sets `up` and returns 0 if there is a valid param in the url.
int ParseURLParam (std::string url, std::string endpointMatch, UrlParam& up);

/// Returns 0 if the url with param matches the endpoint template.
/// Only use when url endpoint contains a param that needs to be parsed.
int CompareURLWithEndPointMatch (std::string url, std::string endpointMatch);
