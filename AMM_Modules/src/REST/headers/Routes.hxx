#pragma once

#include <sstream>
#include "REST/headers/HttpRequest.hxx"

void test                 (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handleGetInstance    (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);

void handleGetNodes       (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handleGetNodeByName  (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);

void handleGetReady       (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handleGetDebug       (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handleGetEvents      (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handleGetLogs        (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);

void handleGetModulesCount (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handleGetModules     (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handleGetModuleById  (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handleGetModuleByGuid (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);

void handleGetShutdown    (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);

void handleGetActions     (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handleGetActionByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handlePostAction     (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handlePutActionByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handleDeleteActionByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);

void handlePostExecute     (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handleOptionsExecute  (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);

void handlePostPhysMod (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handlePostRendMod (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handlePostPerfAss (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handlePostModType (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);

void handleGetPatients (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);

void handleGetStates   (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
void handleGetDeletedStateByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);

void handleGetCommandByName (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);
