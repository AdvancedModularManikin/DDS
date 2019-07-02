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
   VIEW,
   NONE
};

struct HttpRequest {
   Method method;
   std::string url;
};


Method ParseMethodFromText (std::string method);
