#pragma once

#include <sstream>
#include "REST/headers/HttpRequest.hxx"

void test           (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void getInstance    (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void getStates      (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void getNodes       (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void getNodesByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
