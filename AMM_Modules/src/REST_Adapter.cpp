#include "stdafx.h"

#include <mutex>
#include <chrono>
#include <thread>
#include <iostream>
#include <functional>
#include <condition_variable>

#include "AMM/BaseLogger.h"
#include "AMM/DDS_Log_Appender.h"

#include "AMM/DDS_Manager.h"

#include "AMM/Utility.h"

#include <pistache/endpoint.h>
#include <pistache/router.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include <Net/UdpDiscoveryServer.h>

#include "boost/filesystem.hpp"
#include <boost/algorithm/string/join.hpp>

#include "tinyxml2.h"

#include <thirdparty/sqlite_modern_cpp.h>

using namespace std;
using namespace std::chrono;
using namespace boost::filesystem;
using namespace rapidjson;
using namespace Pistache;
using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace sqlite;
using namespace tinyxml2;
using namespace AMM;
using namespace AMM::Capability;

// UDP discovery port
short discoveryPort = 8889;

// REST adapter port
int portNumber = 9080;

// REST threads
int thr = 2;

// Daemonize by default
int daemonize = 1;
int discovery = 1;

char hostname[HOST_NAME_MAX];

std::string action_path = "Actions/";
std::string state_path = "./states/";
std::string patient_path = "./patients/";

std::map<std::string, double> nodeDataStorage;

std::map<std::string, std::string> statusStorage = {{"STATUS",         "NOT RUNNING"},
                                                    {"TICK",           "0"},
                                                    {"TIME",           "0"},
                                                    {"SCENARIO",       ""},
                                                    {"STATE",          ""},
                                                    {"CLEAR_SUPPLY",   ""},
                                                    {"BLOOD_SUPPLY",   ""},
                                                    {"FLUIDICS_STATE", ""},
                                                    {"IVARM_STATE",    ""}};
std::vector<std::string> labsStorage;

bool m_runThread = false;
int64_t lastTick = 0;


DDS_Manager *mgr;
Participant *mp_participant;
boost::asio::io_service io_service;
database db("amm.db");


void AppendLabRow() {
    std::ostringstream labRow;

    labRow << statusStorage["TIME"] << ",";

// POCT
    labRow << nodeDataStorage["Substance_Sodium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Potassium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Chloride"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << ","; // Anion Gap
    labRow << ","; // Ionized Calcium (iCa)
    labRow << nodeDataStorage["Substance_Glucose_Concentration"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodUreaNitrogen_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Creatinine_Concentration"] << ",";

// Hematology
    labRow << nodeDataStorage["BloodChemistry_Hemaocrit"] << ",";
    labRow << nodeDataStorage["Substance_Hemoglobin_Concentration"] << ",";

//ABG
    labRow << nodeDataStorage["Substance_Lactate_Concentration_mmol"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodPH"] << ",";
    labRow << nodeDataStorage["BloodChemistry_Arterial_CarbonDioxide_Pressure"] << ",";
    labRow << nodeDataStorage["BloodChemistry_Arterial_Oxygen_Pressure"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << nodeDataStorage["Substance_Bicarbonate"] << ",";
    labRow << nodeDataStorage["Substance_BaseExcess"] << ",";
    labRow << nodeDataStorage["BloodChemistry_Oxygen_Saturation"] << ",";
    labRow << nodeDataStorage["Substance_Carboxyhemoglobin_Concentration"] << ",";

// VBG
    labRow << nodeDataStorage["Substance_Lactate_Concentration_mmol"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodPH"] << ",";
    labRow << nodeDataStorage["BloodChemistry_VenousCarbonDioxidePressure"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << nodeDataStorage["Substance_Bicarbonate"] << ",";
    labRow << nodeDataStorage["Substance_BaseExcess"] << ",";
    labRow << nodeDataStorage["Substance_Carboxyhemoglobin_Concentration"] << ",";


    // BMP
    labRow << nodeDataStorage["Substance_Sodium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Potassium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Chloride"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << ","; // Anion Gap
    labRow << ","; // Ionized Calcium (iCa)
    labRow << nodeDataStorage["Substance_Glucose_Concentration"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodUreaNitrogen_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Creatinine_Concentration"] << ",";


    // CBC
    labRow << nodeDataStorage["BloodChemistry_WhiteBloodCell_Count"] << ",";
    labRow << nodeDataStorage["BloodChemistry_RedBloodCell_Count"] << ",";
    labRow << nodeDataStorage["Substance_Hemoglobin_Concentration"] << ",";
    labRow << nodeDataStorage["BloodChemistry_Hemaocrit"] << ",";
    labRow << nodeDataStorage["CompleteBloodCount_Platelet"] << ",";

// CMP
    labRow << nodeDataStorage["Substance_Albumin_Concentration"] << ",";
    labRow << ","; // ALP
    labRow << ","; // ALT
    labRow << ","; // AST
    labRow << nodeDataStorage["BloodChemistry_BloodUreaNitrogen_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Calcium_Concentration"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Chloride"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << nodeDataStorage["Substance_Creatinine_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Creatinine_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Glucose_Concentration"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Potassium"] << ",";
    labRow << nodeDataStorage["Substance_Sodium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Bilirubin"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Protein"] << ",";
    labsStorage.push_back(labRow.str());
}

class AMMListener : public ListenerInterface {
    void onNewStatusData(AMM::Capability::Status st,
                         SampleInfo_t *info) override {
        ostringstream statusValue;
        statusValue << st.status_value();

        LOG_DEBUG << "[" << st.module_id() << "][" << st.module_name() << "]["
                  << st.capability() << "] Status = " << statusValue.str();

        if (st.module_name() == "AMM_FluidManager" && st.capability() == "") {
            statusStorage["FLUIDICS_STATE"] = statusValue.str();
        }

        if (st.module_name() == "AMM_FluidManager" && st.capability() == "clear_supply") {
            statusStorage["CLEAR_SUPPLY"] = statusValue.str();
        }

        if (st.module_name() == "AMM_FluidManager" && st.capability() == "blood_supply") {
            statusStorage["BLOOD_SUPPLY"] = statusValue.str();
        }

        if (st.capability() == "iv_detection") {
            statusStorage["IVARM_STATE"] = statusValue.str();
        }
    }

    void onNewTickData(AMM::Simulation::Tick t, SampleInfo_t *info) override {
        if (statusStorage["STATUS"].compare("NOT RUNNING") == 0 &&
            t.frame() > lastTick) {
            statusStorage["STATUS"] = "RUNNING";
        }
        lastTick = t.frame();
        statusStorage["TICK"] = to_string(t.frame());
        statusStorage["TIME"] = to_string(t.time());
    }

    void onNewScenarioData(AMM::Capability::Scenario sc,
                           SampleInfo_t *info) override {};


    void onNewCommandData(AMM::PatientAction::BioGears::Command c,
                          SampleInfo_t *info) override {
        if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = c.message().substr(sysPrefix.size());
            if (value.compare("START_SIM") == 0) {
                statusStorage["STATUS"] = "RUNNING";
            } else if (value.compare("STOP_SIM") == 0) {
                statusStorage["STATUS"] = "STOPPED";
            } else if (value.compare("PAUSE_SIM") == 0) {
                statusStorage["STATUS"] = "PAUSED";
            } else if (value.compare("RESET_SIM") == 0) {
                statusStorage["STATUS"] = "NOT RUNNING";
                statusStorage["TICK"] = "0";
                statusStorage["TIME"] = "0";
                nodeDataStorage.clear();
                labsStorage.clear();
            } else if (value.compare("APPEND_LABS") == 0) {
                AppendLabRow();
            } else if (value.compare("CLEAR_LOG") == 0) {

            } else if (!value.compare(0, loadPrefix.size(), loadPrefix)) {
                statusStorage["STATE"] = value.substr(loadPrefix.size());
            } else if (!value.compare(0, loadScenarioPrefix.size(),
                                      loadScenarioPrefix)) {
                statusStorage["SCENARIO"] = value.substr(loadScenarioPrefix.size());
            }
        }
    }

    void onNewNodeData(AMM::Physiology::Node n, SampleInfo_t *info) override {
        if (!isnan(n.dbl())) {
            nodeDataStorage[n.nodepath()] = n.dbl();
        }
    }
};

void SendPhysiologyModification(const std::string &type,
                                const std::string &location,
                                const std::string &practitioner,
                                const std::string &payload) {
    LOG_DEBUG << "Publishing a phys mod: " << type;
    AMM::Physiology::Modification modInstance;
    modInstance.type(type);
    //  modInstance.location.description(location);
    modInstance.practitioner(practitioner);
    modInstance.payload(payload);
    mgr->PublishPhysiologyModification(modInstance);
}

void SendRenderModification(const std::string &type,
                            const std::string &location,
                            const std::string &practitioner,
                            const std::string &payload) {
    LOG_DEBUG << "Publishing a render mod: " << type;
    AMM::Render::Modification modInstance;
    modInstance.type(type);
    FMA_Location fma_location;
    fma_location.description(location);
    modInstance.location(fma_location);
    modInstance.practitioner(practitioner);
    modInstance.payload(payload);
    mgr->PublishRenderModification(modInstance);
}

void SendPerformanceAssessment(const std::string &assessment_type,
                               const std::string &location,
                               const std::string &practitioner,
                               const std::string &assessment_info,
                               const std::string &step,
                               const std::string &comment) {
    LOG_INFO << "Publishing an assessment: " << assessment_type;
    AMM::Performance::Assessment assessInstance;
    assessInstance.assessment_type(assessment_type);
    // location
    assessInstance.learner_id(practitioner);
    assessInstance.assessment_info(assessment_info);
    assessInstance.step(step);
    assessInstance.comment(comment);
    mgr->PublishPerformanceData(assessInstance);
}

void SendCommand(const std::string &command) {
    LOG_INFO << "Publishing a command:" << command;
    AMM::PatientAction::BioGears::Command cmdInstance;
    cmdInstance.message(command);
    mgr->PublishCommand(cmdInstance);
}

void printCookies(const Http::Request &req) {
    auto cookies = req.cookies();
    std::cout << "Cookies: [" << std::endl;
    const std::string indent(4, ' ');
    for (const auto &c : cookies) {
        std::cout << indent << c.name << " = " << c.value << std::endl;
    }
    std::cout << "]" << std::endl;
}

namespace Generic {

    void handleReady(const Rest::Request &request, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "1");
    }
}

class DDSEndpoint {

public:
    explicit DDSEndpoint(Address addr)
            : httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {}

    void init(int thr = 2) {
        auto opts = Http::Endpoint::options()
                .threads(thr)
                .flags(Tcp::Options::ReuseAddr)
                .maxPayload(65536);
        httpEndpoint->init(opts);
        setupRoutes();
    }

    void start() {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serveThreaded();
    }

    void shutdown() { httpEndpoint->shutdown(); }

private:
    void setupRoutes() {
        using namespace Rest;

        Routes::Get(router, "/instance",
                    Routes::bind(&DDSEndpoint::getInstance, this));
        Routes::Get(router, "/node/:name",
                    Routes::bind(&DDSEndpoint::getNode, this));
        Routes::Get(router, "/nodes", Routes::bind(&DDSEndpoint::getNodes, this));
        Routes::Get(router, "/command/:name",
                    Routes::bind(&DDSEndpoint::issueCommand, this));
        Routes::Get(router, "/ready", Routes::bind(&Generic::handleReady));
        Routes::Get(router, "/debug", Routes::bind(&DDSEndpoint::doDebug, this));

        Routes::Get(router, "/labs", Routes::bind(&DDSEndpoint::getLabsReport, this));

        Routes::Get(router, "/events",
                    Routes::bind(&DDSEndpoint::getEventLog, this));

        Routes::Get(router, "/logs",
                    Routes::bind(&DDSEndpoint::getDiagnosticLog, this));

        Routes::Get(router, "/modules/count",
                    Routes::bind(&DDSEndpoint::getModuleCount, this));
        Routes::Get(router, "/modules",
                    Routes::bind(&DDSEndpoint::getModules, this));
        Routes::Get(router, "/module/id/:id",
                    Routes::bind(&DDSEndpoint::getModuleById, this));
        Routes::Get(router, "/module/guid/:guid",
                    Routes::bind(&DDSEndpoint::getModuleByGuid, this));

        Routes::Get(router, "/shutdown",
                    Routes::bind(&DDSEndpoint::doShutdown, this));

        Routes::Get(router, "/actions",
                    Routes::bind(&DDSEndpoint::getActions, this));
        Routes::Get(router, "/action/:name",
                    Routes::bind(&DDSEndpoint::getAction, this));
        Routes::Post(router, "/action",
                     Routes::bind(&DDSEndpoint::createAction, this));
        Routes::Put(router, "/action/:name",
                    Routes::bind(&DDSEndpoint::updateAction, this));
        Routes::Delete(router, "/action/:name",
                       Routes::bind(&DDSEndpoint::deleteAction, this));

        Routes::Post(router, "/execute",
                     Routes::bind(&DDSEndpoint::executeCommand, this));
        Routes::Options(router, "/execute",
                        Routes::bind(&DDSEndpoint::executeOptions, this));

        Routes::Post(
                router, "/topic/physiology_modification",
                Routes::bind(&DDSEndpoint::executePhysiologyModification, this));
        Routes::Post(router, "/topic/render_modification",
                     Routes::bind(&DDSEndpoint::executeRenderModification, this));
        Routes::Post(
                router, "/topic/performance_assessment",
                Routes::bind(&DDSEndpoint::executePerformanceAssessment, this));
        Routes::Options(router, "/topic/:mod_type",
                        Routes::bind(&DDSEndpoint::executeOptions, this));

        Routes::Get(router, "/patients",
                    Routes::bind(&DDSEndpoint::getPatients, this));

        Routes::Get(router, "/states", Routes::bind(&DDSEndpoint::getStates, this));
        Routes::Get(router, "/states/:name/delete",
                    Routes::bind(&DDSEndpoint::deleteState, this));
    }

    void getInstance(const Rest::Request &request,
                     Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);

        std::ifstream t("static/current_scenario.txt");
        std::string scenario((std::istreambuf_iterator<char>(t)),
                             std::istreambuf_iterator<char>());
        t.close();

        writer.StartObject();
        writer.Key("name");
        writer.String(hostname);
        writer.Key("scenario");
        writer.String(scenario.c_str());
        writer.EndObject();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getStates(const Rest::Request &request, Http::ResponseWriter response) {

        StringBuffer s;
        Writer<StringBuffer> writer(s);

        writer.StartArray();
        if (exists(state_path) && is_directory(state_path)) {
            path p(state_path);
            if (is_directory(p)) {
                directory_iterator end_iter;
                for (directory_iterator dir_itr(p); dir_itr != end_iter; ++dir_itr) {
                    if (is_regular_file(dir_itr->status())) {
                        writer.StartObject();
                        writer.Key("name");
                        writer.String(dir_itr->path().filename().c_str());
                        writer.Key("description");
                        stringstream writeTime;
                        writeTime << last_write_time(dir_itr->path());
                        writer.String(writeTime.str().c_str());
                        writer.EndObject();
                    }
                }
            }
        }
        writer.EndArray();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void deleteState(const Rest::Request &request,
                     Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        if (name != "StandardMale@0s.xml") {
            std::ostringstream deleteFile;
            deleteFile << state_path << "/" << name;
            path deletePath(deleteFile.str().c_str());
            if (exists(deletePath) && is_regular_file(deletePath)) {
                LOG_INFO << "Deleting " << deletePath;
                boost::filesystem::remove(deletePath);
                response.send(Pistache::Http::Code::Ok, "Deleted",
                              MIME(Application, Json));
            } else {
                response.send(Pistache::Http::Code::Forbidden,
                              "Unable to delete state file", MIME(Application, Json));
            }
        } else {
            response.send(Pistache::Http::Code::Forbidden,
                          "Can not delete default state file",
                          MIME(Application, Json));
        }
    }

    void getPatients(const Rest::Request &request,
                     Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);

        writer.StartArray();
        if (exists(patient_path) && is_directory(patient_path)) {
            path p(patient_path);
            if (is_directory(p)) {
                directory_iterator end_iter;
                for (directory_iterator dir_itr(p); dir_itr != end_iter; ++dir_itr) {
                    if (is_regular_file(dir_itr->status())) {
                        writer.StartObject();
                        writer.Key("name");
                        writer.String(dir_itr->path().filename().c_str());
                        writer.Key("description");
                        stringstream writeTime;
                        writeTime << last_write_time(dir_itr->path());
                        writer.String(writeTime.str().c_str());
                        writer.EndObject();
                    }
                }
            }
        }
        writer.EndArray();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Pistache::Http::Code::Ok, s.GetString(),
                      MIME(Application, Json));
    }

    void getActions(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);

        writer.StartArray();
        if (exists(action_path) && is_directory(action_path)) {
            path p(action_path);
            if (is_directory(p)) {
                directory_iterator end_iter;
                for (directory_iterator dir_itr(p); dir_itr != end_iter; ++dir_itr) {
                    if (is_regular_file(dir_itr->status())) {
                        writer.StartObject();
                        writer.Key("name");
                        writer.String(dir_itr->path().filename().c_str());
                        writer.Key("description");
                        stringstream writeTime;
                        writeTime << last_write_time(dir_itr->path());
                        writer.String(writeTime.str().c_str());
                        writer.EndObject();
                    }
                }
            }
        }
        writer.EndArray();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Pistache::Http::Code::Ok, s.GetString(),
                      MIME(Application, Json));
    }

    void executeCommand(const Rest::Request &request,
                        Http::ResponseWriter response) {
        Document document;
        document.Parse(request.body().c_str());
        std::string payload = document["payload"].GetString();
        SendPhysiologyModification(payload, "", "", "");
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Pistache::Http::Code::Ok,
                      "{\"message\":\"Command executed\"}");
    }

    void executePhysiologyModification(const Rest::Request &request,
                                       Http::ResponseWriter response) {
        std::string type, location, practitioner, payload;
        Document document;
        document.Parse(request.body().c_str());
        if (document.HasMember("type")) {
            type = document["type"].GetString();
        }
        if (document.HasMember("location")) {
            location = document["location"].GetString();
        }
        if (document.HasMember("practitioner")) {
            practitioner = document["practitioner"].GetString();
        }
        if (document.HasMember("payload")) {
            payload = document["payload"].GetString();
        }
        SendPhysiologyModification(type, location, practitioner, payload);
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Pistache::Http::Code::Ok,
                      "{\"message\":\"Physiology modification published\"}");
    }

    void executeRenderModification(const Rest::Request &request,
                                   Http::ResponseWriter response) {
        std::string type, location, practitioner, payload;
        Document document;
        document.Parse(request.body().c_str());
        if (document.HasMember("type")) {
            type = document["type"].GetString();
        }
        if (document.HasMember("location")) {
            location = document["location"].GetString();
        }
        if (document.HasMember("practitioner")) {
            practitioner = document["practitioner"].GetString();
        }
        if (document.HasMember("payload")) {
            payload = document["payload"].GetString();
        }
        SendRenderModification(type, location, practitioner, payload);
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Pistache::Http::Code::Ok,
                      "{\"message\":\"Render modification published\"}");
    }

    void executePerformanceAssessment(const Rest::Request &request,
                                      Http::ResponseWriter response) {
        std::string type, location, practitioner, payload, info, step, comment;
        Document document;
        document.Parse(request.body().c_str());
        if (document.HasMember("type")) {
            type = document["type"].GetString();
        }
        if (document.HasMember("location")) {
            location = document["location"].GetString();
        }
        if (document.HasMember("practitioner")) {
            practitioner = document["practitioner"].GetString();
        }
        if (document.HasMember("info")) {
            info = document["info"].GetString();
        }
        if (document.HasMember("step")) {
            step = document["step"].GetString();
        }
        if (document.HasMember("comment")) {
            comment = document["comment"].GetString();
        }
        SendPerformanceAssessment(type, location, practitioner, info, step,
                                  comment);
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Pistache::Http::Code::Ok,
                      "{\"message\":\"Performance assessment published\"}");
    }

    void executeOptions(const Rest::Request &request,
                        Http::ResponseWriter response) {
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Pistache::Http::Code::Ok, "{\"message\":\"success\"}");
    }

    void createAction(const Rest::Request &request,
                      Http::ResponseWriter response) {}

    void deleteAction(const Rest::Request &request,
                      Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
    }

    void updateAction(const Rest::Request &request,
                      Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
    }

    void getAction(const Rest::Request &request, Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
    }

    void issueCommand(const Rest::Request &request,
                      Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
        SendCommand(name);
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartObject();
        writer.Key("Sent command");
        writer.String(name.c_str());
        writer.EndObject();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString());
    }

    void getModuleById(const Rest::Request &request,
                       Http::ResponseWriter response) {
        auto id = request.param(":id").as<std::string>();
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        db << "SELECT "
              "module_id AS module_id,"
              "module_guid as module_guid,"
              "module_name AS module_name,"
              "capabilities as capabilities,"
              "manufacturer as manufacturer,"
              "model as model "
              " FROM "
              " module_capabilities "
              " WHERE module_id = ?"
           << id >>
           [&](string module_id, string module_guid, string module_name,
               string capabilities, string manufacturer, string model) {
               writer.StartObject();

               writer.Key("Module_ID");
               writer.String(module_id.c_str());

               writer.Key("Module_GUID");
               writer.String(module_guid.c_str());

               writer.Key("Module_Name");
               writer.String(module_name.c_str());

               writer.Key("Manufacturer");
               writer.String(manufacturer.c_str());

               writer.Key("Model");
               writer.String(model.c_str());

               writer.Key("Module_Capabilities");
               writer.String(capabilities.c_str());

               writer.EndObject();
           };

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getModuleByGuid(const Rest::Request &request,
                         Http::ResponseWriter response) {
        auto guid = request.param(":guid").as<std::string>();
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        db << "SELECT "
              "module_id AS module_id,"
              "module_guid as module_guid,"
              "module_name AS module_name,"
              "capabilities as capabilities,"
              "manufacturer as manufacturer,"
              "model as model "
              " FROM "
              " module_capabilities "
              " WHERE module_guid = ?"
           << guid >>
           [&](string module_id, string module_guid, string module_name,
               string capabilities, string manufacturer, string model) {
               writer.StartObject();

               writer.Key("Module_ID");
               writer.String(module_id.c_str());

               writer.Key("Module_GUID");
               writer.String(module_guid.c_str());

               writer.Key("Module_Name");
               writer.String(module_name.c_str());

               writer.Key("Manufacturer");
               writer.String(manufacturer.c_str());

               writer.Key("Model");
               writer.String(model.c_str());

               writer.Key("Module_Capabilities");
               writer.String(capabilities.c_str());

               writer.EndObject();
           };

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getModuleCount(const Rest::Request &request,
                        Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);

        int count = 0;
        db << "SELECT COUNT(DISTINCT module_name) FROM module_capabilities" >> count;
        writer.StartObject();

        writer.Key("module_count");
        writer.Int(count);

        writer.EndObject();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getModules(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();

        db << "SELECT "
              "module_capabilities.module_id AS module_id,"
              "module_capabilities.module_guid as module_guid,"
              "module_capabilities.module_name AS module_name,"
              "module_capabilities.capabilities as capabilities,"
              "module_capabilities.manufacturer as manufacturer,"
              "module_capabilities.model as model "
              " FROM "
              " module_capabilities; " >>
           [&](string module_id, string module_guid, string module_name,
               string capabilities,
               string manufacturer, string model) {
               writer.StartObject();

               writer.Key("Module_ID");
               writer.String(module_id.c_str());

               writer.Key("Module_GUID");
               writer.String(module_guid.c_str());

               writer.Key("Module_Name");
               writer.String(module_name.c_str());

               writer.Key("Manufacturer");
               writer.String(manufacturer.c_str());

               writer.Key("Model");
               writer.String(model.c_str());

               writer.Key("Module_Capabilities");
               writer.String(capabilities.c_str());

               writer.EndObject();
           };

        writer.EndArray();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getEventLog(const Rest::Request &request,
                     Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();

        db << "SELECT "
              "module_capabilities.module_name,"
              "events.source,"
              "events.topic,"
              "events.tick,"
              "events.timestamp,"
              "events.data "
              "FROM "
              "events "
              "LEFT JOIN module_capabilities "
              "ON "
              "events.source = module_capabilities.module_guid" >>
           [&](string module_name, string source, string topic, int64_t tick, int64_t timestamp,
               string data) {

               writer.StartObject();
               writer.Key("source");
               writer.String(module_name.c_str());
               writer.Key("module_guid");
               writer.String(source.c_str());
               writer.Key("tick");
               writer.Uint64(tick);
               writer.Key("timestamp");
               writer.Uint64(timestamp);
               writer.Key("topic");
               writer.String(topic.c_str());
               writer.Key("message");
               writer.String(data.c_str());
               writer.EndObject();
           };

        writer.EndArray();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getDiagnosticLog(const Rest::Request &request,
                          Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();

        db << "SELECT "
              "logs.module_name, "
              "logs.module_guid, "
              "logs.module_id, "
              "logs.message,"
              "logs.log_level,"
              "logs.timestamp "
              "FROM "
              "logs " >>
           [&](string module_name, string module_guid, string module_id, string message, string log_level,
               int64_t timestamp) {

               writer.StartObject();
               writer.Key("source");
               writer.String(module_name.c_str());
               writer.Key("module_guid");
               writer.String(module_guid.c_str());
               writer.Key("module_id");
               writer.String(module_id.c_str());
               writer.Key("timestamp");
               writer.Uint64(timestamp);
               writer.Key("log_level");
               writer.String(log_level.c_str());
               writer.Key("message");
               writer.String(message.c_str());
               writer.EndObject();
           };

        writer.EndArray();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getNodes(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();

        auto nit = nodeDataStorage.begin();
        while (nit != nodeDataStorage.end()) {
            writer.StartObject();
            writer.Key(nit->first.c_str());
            writer.Double(nit->second);
            writer.EndObject();
            ++nit;
        }

        auto sit = statusStorage.begin();
        while (sit != statusStorage.end()) {
            writer.StartObject();
            writer.Key(sit->first.c_str());
            writer.String(sit->second.c_str());
            writer.EndObject();
            ++sit;
        }

        writer.EndArray();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getLabsReport(const Rest::Request &request, Http::ResponseWriter response) {
        std::string labReport = boost::algorithm::join(labsStorage, "\n");
        auto mime = Http::Mime::MediaType::fromString("text/csv");
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, labReport, mime);
    }

    void getNode(const Rest::Request &request, Http::ResponseWriter response) {

        auto name = request.param(":name").as<std::string>();
        auto it = nodeDataStorage.find(name);
        if (it != nodeDataStorage.end()) {
            StringBuffer s;
            Writer<StringBuffer> writer(s);
            writer.StartObject();
            writer.Key(it->first.c_str());
            writer.Double(it->second);
            writer.EndObject();
            response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
            response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
        } else {
            response.send(Http::Code::Not_Found, "Node data does not exist");
        }
    }

    void doDebug(const Rest::Request &request, Http::ResponseWriter response) {
        printCookies(request);
        response.cookies().add(Http::Cookie("lang", "en-US"));
        response.send(Http::Code::Ok);
    }

    void doShutdown(const Rest::Request &request, Http::ResponseWriter response) {
        m_runThread = false;
        response.cookies().add(Http::Cookie("lang", "en-US"));
        response.send(Http::Code::Ok);
    }

    typedef std::mutex Lock;
    typedef std::lock_guard<Lock> Guard;
    Lock commandLock;

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
};

void UdpDiscoveryThread() {
    if (discovery) {
        UdpDiscoveryServer udps(io_service, discoveryPort);
        LOG_INFO << "UDP Discovery listening on port " << discoveryPort;
        io_service.run();
    } else {
        LOG_INFO << "UDP discovery service not started due to command line option.";
    }
}

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>"
         << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n"
         << endl;
}

Port port(static_cast<uint16_t>(portNumber));
Address addr(Ipv4::any(), port);
DDSEndpoint server(addr);

void signalHandler(int signum) {
    LOG_WARNING << "Interrupt signal (" << signum << ") received.";

    if (signum == 15 || signum == 9) {
        server.shutdown();
        LOG_TRACE << "Stopped REST listener.";

        io_service.stop();
        LOG_TRACE << "Stopping IO services";
    }

    exit(signum);
}

int main(int argc, char *argv[]) {

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }

        if (arg == "-d") {
            daemonize = 1;
        }

        if (arg == "-nodiscovery") {
            discovery = 0;
        }
    }

    string action;

    const char *nodeName = "AMM_REST_Adapter";
    std::string nodeString(nodeName);
    mgr = new DDS_Manager(nodeName);
    mp_participant = mgr->GetParticipant();

    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    static plog::DDS_Log_Appender<plog::TxtFormatter> DDSAppender(mgr);
    plog::init(plog::verbose, &consoleAppender).addAppender(&DDSAppender);

    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *tick_sub_listener = new DDS_Listeners::TickSubListener();
    auto *status_sub_listener = new DDS_Listeners::StatusSubListener();

    AMMListener rl;
    node_sub_listener->SetUpstream(&rl);
    command_sub_listener->SetUpstream(&rl);
    tick_sub_listener->SetUpstream(&rl);
    status_sub_listener->SetUpstream(&rl);

    mgr->InitializeSubscriber(AMM::DataTypes::nodeTopic, &mgr->NodeType, node_sub_listener);
    mgr->InitializeReliableSubscriber(AMM::DataTypes::commandTopic, &mgr->CommandType, command_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::tickTopic, &mgr->TickType, tick_sub_listener);
    mgr->InitializeReliableSubscriber(AMM::DataTypes::statusTopic, &mgr->StatusType, status_sub_listener);

    std::thread udpD(UdpDiscoveryThread);

    gethostname(hostname, HOST_NAME_MAX);

    server.init(thr);
    LOG_INFO << "Listening on *:" << portNumber;

    m_runThread = true;

    server.start();

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    mgr->PublishModuleConfiguration(
            mgr->module_id,
            nodeString,
            "Vcom3D",
            "REST_Adapter",
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("static/module_capabilities/rest_adapter_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);

    LOG_INFO << "Ready.";

    while (m_runThread) {
        getline(cin, action);
        transform(action.begin(), action.end(), action.begin(), ::toupper);
        if (action == "EXIT") {
            m_runThread = false;
            LOG_INFO << "Shutting down from command-line.";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        cout.flush();
    }

    server.shutdown();
    io_service.stop();
    udpD.join();

    LOG_INFO << "Shutdown complete";

    return 0;
}
