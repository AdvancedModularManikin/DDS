#pragma once

#include <sstream>

#include "REST/headers/HttpRequest.hxx"
#include "AMM/BaseLogger.h"
#include "AMM/DDS_Log_Appender.h"
#include "AMM/DDS_Manager.h"
#include "AMM/Utility.h"

/// Global data that needs random access.
class AMMData {
public:
   static AMM::DDS_Manager* mgr;
};


/// HANDLERS

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

void handleGetLabs (std::ostringstream& oss, HttpRequest& request, std::string urlTemplate);


/// CORE FUNCTIONS

void SendPhysiologyModification (
   const std::string &type,
   const std::string &location,
   const std::string &practitioner,
   const std::string &payload
);

void SendRenderModification (
   const std::string &type,
   const std::string &location,
   const std::string &practitioner,
   const std::string &payload
);

void SendPerformanceAssessment (
   const std::string &assessment_type,
   const std::string &location,
   const std::string &practitioner,
   const std::string &assessment_info,
   const std::string &step,
   const std::string &comment
);

void SendCommand(const std::string &command);
