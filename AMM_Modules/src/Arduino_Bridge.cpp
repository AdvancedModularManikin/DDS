#include "stdafx.h"

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <vector>
#include <queue>
#include <stack>
#include <thread>
#include <fstream>
#include <string>
#include <iostream>

#include "AMM/Utility.h"

extern "C" {
#include "Serial/arduino-serial-lib.h"
}

#include "AMM/DDS/AMMPubSubTypes.h"
#include "AMM/BaseLogger.h"
#include "AMM/DDS_Log_Appender.h"

#include "AMM/DDS_Manager.h"


#include "tinyxml2.h"

#define PORT_LINUX "/dev/tty96B0"
#define BAUD 115200

using namespace AMM;
using namespace AMM::Capability;
using namespace std;
using namespace tinyxml2;

bool first_message = true;
bool closed = false;
bool initializing = true;

std::string globalInboundBuffer;
std::string requestPrefix = "[REQUEST]";
std::string reportPrefix = "[REPORT]";
std::string actionPrefix = "[AMM_Command]";
std::string genericTopicPrefix = "[";
std::string xmlPrefix = "<?xml";

std::vector<std::string> subscribedTopics;
std::vector<std::string> publishedTopics;
std::map<std::string, std::string> subMaps;
std::map<std::string, std::map<std::string, std::string>> equipmentSettings;

std::queue<std::string> transmitQ;

// Set up DDS
const char *nodeName = "AMM_Serial_Bridge";

DDS_Manager *mgr;

int fd = -1;
int rc;

void sendConfigInfo(std::string scene, std::string module) {
    std::ostringstream static_filename;
    static_filename << "static/module_configuration_static/" << scene << "_" << module << ".txt";
    LOG_DEBUG << "Loading config from filename: " << static_filename.str();
    std::ifstream ifs(static_filename.str());
    std::string configContent((std::istreambuf_iterator<char>(ifs)),
                              (std::istreambuf_iterator<char>()));
    ifs.close();
    if (configContent.empty()) {
        LOG_ERROR << "Configuration empty.";
        return;
    }
    std::vector<std::string> v = Utility::explode("\n", configContent);
    for (int i = 0; i < v.size(); i++) {
        std::string rsp = v[i] + "\n";
        transmitQ.push(rsp);
    }
};

void PublishSettings(std::string const &equipmentType) {
    std::ostringstream payload;
    LOG_INFO << "Publishing equipment " << equipmentType << " settings";
    for (auto &inner_map_pair : equipmentSettings[equipmentType]) {
        payload << inner_map_pair.first << "=" << inner_map_pair.second
                << std::endl;
        LOG_DEBUG << "\t" << inner_map_pair.first << ": " << inner_map_pair.second;
    }

    AMM::InstrumentData i;
    i.instrument(equipmentType);
    i.payload(payload.str());
    mgr->PublishInstrumentData(i);
}

void readHandler() {
    /*std::copy(buffer, sizeof(buffer), std::back_inserter(globalInboundBuffer));
    if (!boost::algorithm::ends_with(globalInboundBuffer, "\n")) {
        return;
    }*/
    std::vector<std::string> v = Utility::explode("\n", globalInboundBuffer);
    globalInboundBuffer.clear();
    for (int i = 0; i < v.size(); i++) {
        std::string rsp = v[i];
        if (!rsp.compare(0, reportPrefix.size(), reportPrefix)) {
            std::string value = rsp.substr(reportPrefix.size());
            LOG_DEBUG << "Received report via serial: " << value;
        } else if (!rsp.compare(0, actionPrefix.size(), actionPrefix)) {
            std::string value = rsp.substr(actionPrefix.size());
            LOG_INFO << "Received command via serial, publishing to AMM: " << value;
            AMM::PatientAction::BioGears::Command cmdInstance;
            boost::trim_right(value);
            cmdInstance.message(value);
            mgr->PublishCommand(cmdInstance);
        } else if (!rsp.compare(0, xmlPrefix.size(), xmlPrefix)) {
            std::string value = rsp;
            LOG_INFO << "Received XML via serial";
            LOG_DEBUG << "\tXML: " << value;
            tinyxml2::XMLDocument doc(false);
            doc.Parse(value.c_str());
            tinyxml2::XMLNode *root = doc.FirstChildElement("AMMModuleConfiguration");

            if (root) {
                tinyxml2::XMLNode *mod = root->FirstChildElement("module");
                tinyxml2::XMLElement *module = mod->ToElement();

                if (initializing) {
                    LOG_INFO << "Module is initializing, so we'll publish the configuration.";

                    std::string module_name = module->Attribute("name");
                    std::string manufacturer = module->Attribute("manufacturer");
                    std::string model = module->Attribute("model");
                    std::string serial_number = module->Attribute("serial_number");
                    std::string module_version = module->Attribute("module_version");

                    mgr->module_name = module_name;
                    mgr->PublishModuleConfiguration(
                            mgr->module_id,
                            module_name,
                            manufacturer,
                            model,
                            serial_number,
                            module_version,
                            value
                    );
                    initializing = false;
                }

                subscribedTopics.clear();
                publishedTopics.clear();
                tinyxml2::XMLNode *caps = mod->FirstChildElement("capabilities");

                if (caps) {
                    for (tinyxml2::XMLNode *node = caps->FirstChildElement(
                            "capability"); node; node = node->NextSibling()) {
                        tinyxml2::XMLElement *cap = node->ToElement();
                        std::string capabilityName = cap->Attribute("name");

                        tinyxml2::XMLElement *starting_settings = cap->FirstChildElement(
                                "starting_settings");
                        if (starting_settings) {
                            LOG_DEBUG << "Received starting settings";
                            for (tinyxml2::XMLNode *settingNode = starting_settings->FirstChildElement(
                                    "setting"); settingNode; settingNode = settingNode->NextSibling()) {
                                tinyxml2::XMLElement *setting = settingNode->ToElement();
                                std::string settingName = setting->Attribute("name");
                                std::string settingValue = setting->Attribute("value");
                                LOG_DEBUG << "[" << settingName << "] = " << settingValue;
                            }
                        }

                        tinyxml2::XMLElement *configEl =
                                cap->FirstChildElement("configuration");
                        if (configEl) {
                            for (tinyxml2::XMLNode *settingNode =
                                    configEl->FirstChildElement("setting");
                                 settingNode; settingNode = settingNode->NextSibling()) {
                                tinyxml2::XMLElement *setting = settingNode->ToElement();
                                std::string settingName = setting->Attribute("name");
                                std::string settingValue = setting->Attribute("value");
                                equipmentSettings[capabilityName][settingName] =
                                        settingValue;
                            }
                            PublishSettings(capabilityName);
                        }

                        // Store subscribed topics for this capability
                        tinyxml2::XMLNode *subs = node->FirstChildElement("subscribed_topics");
                        if (subs) {
                            for (tinyxml2::XMLNode *sub = subs->FirstChildElement(
                                    "topic"); sub; sub = sub->NextSibling()) {
                                tinyxml2::XMLElement *s = sub->ToElement();
                                std::string subTopicName = s->Attribute("name");


                                if (s->Attribute("nodepath")) {
                                    std::string subNodePath = s->Attribute("nodepath");
                                    if (subTopicName == "AMM_HighFrequencyNode_Data") {
                                        subTopicName = "HF_" + subNodePath;
                                    } else {
                                        subTopicName = subNodePath;
                                    }
                                }

                                if (s->Attribute("map_name")) {
                                    std::string subMapName = s->Attribute("map_name");
                                    subMaps[subTopicName] = subMapName;
                                }

                                Utility::add_once(subscribedTopics, subTopicName);
                                LOG_DEBUG << "[" << capabilityName << "][SUBSCRIBE]" << subTopicName;
                            }
                        }

                        // Store published topics for this capability
                        tinyxml2::XMLNode *pubs = node->FirstChildElement("published_topics");
                        if (pubs) {
                            for (tinyxml2::XMLNode *pub = pubs->FirstChildElement(
                                    "topic"); pub; pub = pub->NextSibling()) {
                                tinyxml2::XMLElement *p = pub->ToElement();
                                std::string pubTopicName = p->Attribute("name");
                                Utility::add_once(publishedTopics, pubTopicName);
                                LOG_DEBUG << "[" << capabilityName << "][PUBLISH]" << pubTopicName;
                            }
                        }
                    }
                }
            } else {
                tinyxml2::XMLNode *root = doc.FirstChildElement("AMMModuleStatus");
                tinyxml2::XMLElement *module = root->FirstChildElement("module")->ToElement();
                const char *name = module->Attribute("name");
                std::string nodeName(name);

                tinyxml2::XMLElement *caps = module->FirstChildElement("capabilities");
                if (caps) {
                    for (tinyxml2::XMLNode *node = caps->FirstChildElement(
                            "capability"); node; node = node->NextSibling()) {
                        tinyxml2::XMLElement *cap = node->ToElement();
                        std::string capabilityName = cap->Attribute("name");
                        std::string statusVal = cap->Attribute("status");

                        if (statusVal == "OPERATIONAL") {
                            mgr->SetStatus(mgr->module_id, nodeName, capabilityName, OPERATIONAL);
                        } else if (statusVal == "HALTING_ERROR") {
                            if (cap->Attribute("message")) {
                                std::string errorMessage = cap->Attribute("message");
                                std::vector<std::string> errorMessages = {errorMessage};
                                mgr->SetStatus(mgr->module_id, nodeName, capabilityName, HALTING_ERROR, errorMessages);
                            } else {
                                mgr->SetStatus(mgr->module_id, nodeName, capabilityName, HALTING_ERROR);
                            }
                        } else if (statusVal == "IMPENDING_ERROR") {
                            if (cap->Attribute("message")) {
                                std::string errorMessage = cap->Attribute("message");
                                std::vector<std::string> errorMessages = {errorMessage};
                                mgr->SetStatus(mgr->module_id, nodeName, capabilityName, IMPENDING_ERROR,
                                               errorMessages);
                            } else {
                                mgr->SetStatus(mgr->module_id, nodeName, capabilityName, IMPENDING_ERROR);
                            }
                        } else {
                            LOG_ERROR << "Invalid status value " << statusVal << " for capability " << capabilityName;
                        }
                    }
                }
            }
        } else if (!rsp.compare(0, genericTopicPrefix.size(), genericTopicPrefix)) {
            std::string topic, message, modType, modLocation, modPayload, modInfo;
            unsigned first = rsp.find("[");
            unsigned last = rsp.find("]");
            topic = rsp.substr(first + 1, last - first - 1);
            message = rsp.substr(last + 1);

            std::list<std::string> tokenList;
            split(tokenList, message, boost::algorithm::is_any_of(";"), boost::token_compress_on);
            std::map<std::string, std::string> kvp;

            BOOST_FOREACH(std::string token, tokenList) {
                            size_t sep_pos = token.find_first_of("=");
                            std::string key = token.substr(0, sep_pos);
                            std::string value = (sep_pos == std::string::npos ? "" : token.substr(sep_pos + 1,
                                                                                                  std::string::npos));
                            kvp[key] = value;
                            if (key == "type") {
                                modType = kvp[key];
                            } else if (key == "location") {
                                modLocation = kvp[key];
                            } else if (key == "info") {
                                modInfo = kvp[key];
                            } else if (key == "payload") {
                                modPayload = kvp[key];
                            }

                        }

            if (topic == "AMM_Render_Modification") {
                AMM::Render::Modification renderMod;
                renderMod.type(modType);
                renderMod.payload(modPayload);
                renderMod.location().description(modLocation);
                mgr->PublishRenderModification(renderMod);
            } else if (topic == "AMM_Physiology_Modification") {
                AMM::Physiology::Modification physMod;
                physMod.type(modType);
                physMod.payload(modPayload);
                physMod.location().description(modLocation);
                mgr->PublishPhysiologyModification(physMod);
            } else if (topic == "AMM_Performance_Assessment") {
                AMM::Performance::Assessment assessment;
                assessment.assessment_info(modInfo);
                assessment.assessment_type(modType);
                mgr->PublishPerformanceData(assessment);
            } else if (topic == "AMM_Diagnostics_Log_Record") {
                if (modType == "info") {
                    LOG_INFO << modPayload;
                } else if (modType == "warning") {
                    LOG_WARNING << modPayload;
                } else if (modType == "error") {
                    LOG_ERROR << modPayload;
                } else {
                    LOG_DEBUG << modPayload;
                }
            } else {
                LOG_DEBUG << "Unknown topic: " << topic;
            }
        } else {
            if (!rsp.empty() && rsp != "\r") {
                LOG_DEBUG << "Serial debug: " << rsp;
            }
        }
    }
}


class AMMListener : public ListenerInterface {
public:
    void onNewHighFrequencyNodeData(AMM::Physiology::HighFrequencyNode n, SampleInfo_t *info) override {
        std::string hfname = "HF_" + n.nodepath();
        if (std::find(subscribedTopics.begin(), subscribedTopics.end(), hfname) != subscribedTopics.end()) {
            std::ostringstream messageOut;
            map<string, string>::iterator i = subMaps.find(hfname);
            if (i == subMaps.end()) {
                messageOut << "[AMM_Node_Data]" << n.nodepath() << "=" << n.dbl() << std::endl;
            } else {
                messageOut << "[" << i->first << "]" << n.dbl() << std::endl;
            }

            rc = serialport_write(fd, messageOut.str().c_str());
            if (rc == -1) {
                LOG_ERROR << " Error writing to serial port";
            }
        }
    }

    void onNewNodeData(AMM::Physiology::Node n, SampleInfo_t *info) override {
        // Publish values that are supposed to go out on every change
        if (std::find(subscribedTopics.begin(), subscribedTopics.end(), n.nodepath()) != subscribedTopics.end()) {
            std::ostringstream messageOut;
            map<string, string>::iterator i = subMaps.find(n.nodepath());
            if (i == subMaps.end()) {
                messageOut << "[AMM_Node_Data]" << n.nodepath() << "=" << n.dbl() << std::endl;
            } else {
                messageOut << "[" << i->first << "]" << n.dbl() << std::endl;
            }

            rc = serialport_write(fd, messageOut.str().c_str());
            if (rc == -1) {
                LOG_ERROR << " Error writing to serial port";
            }
        }
    }

    void onNewPhysiologyModificationData(AMM::Physiology::Modification pm, SampleInfo_t *info) override {
        // Publish values that are supposed to go out on every change
        std::ostringstream messageOut;
        messageOut << "[AMM_Physiology_Modification]" << "type=" << pm.type() << ";" << "location="
                   << pm.location().description() << ";" << "learner_id=" << pm.practitioner() << ";" << "payload="
                   << pm.payload() << std::endl;
        std::string stringOut = messageOut.str();
        LOG_DEBUG << "Physiology modification received from AMM: " << stringOut;

        if (std::find(subscribedTopics.begin(), subscribedTopics.end(), pm.type()) != subscribedTopics.end() ||
            std::find(subscribedTopics.begin(), subscribedTopics.end(), "AMM_Physiology_Modification") !=
            subscribedTopics.end()
                ) {
            transmitQ.push(messageOut.str());
        }
    }

    void onNewRenderModificationData(AMM::Render::Modification rm, SampleInfo_t *info) override {
        // Publish values that are supposed to go out on every change
        std::ostringstream messageOut;
        messageOut << "[AMM_Render_Modification]" << "type=" << rm.type() << ";" << "location="
                   << rm.location().description() << ";" << "learner_id=" << rm.practitioner() << ";" << "payload="
                   << rm.payload() << std::endl;
        std::string stringOut = messageOut.str();

        LOG_DEBUG << "Render modification received from AMM: " << stringOut;

        if (std::find(subscribedTopics.begin(), subscribedTopics.end(), rm.type()) != subscribedTopics.end() ||
            std::find(subscribedTopics.begin(), subscribedTopics.end(), "AMM_Render_Modification") !=
            subscribedTopics.end()
                ) {
            transmitQ.push(messageOut.str());
        }

    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) override {
        LOG_DEBUG << "Command received from AMM: " << c.message();
        if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = c.message().substr(sysPrefix.size());
            if (value.compare("START_SIM") == 0) {

            } else if (value.compare("STOP_SIM") == 0) {

            } else if (value.compare("PAUSE_SIM") == 0) {

            } else if (value.compare("RESET_SIM") == 0) {

            } else if (!value.compare(0, loadScenarioPrefix.size(), loadScenarioPrefix)) {
                std::string scene = value.substr(loadScenarioPrefix.size());
                LOG_DEBUG << "Time to load scene " << scene;
                //@TODO Grab the actual module name
                sendConfigInfo(scene, "liquid_sensor");
            }
            // Send it on through the bridge
            std::ostringstream cmdMessage;
            cmdMessage << "[AMM_Command]" << value << "\n";
            transmitQ.push(cmdMessage.str());
        } else {
            std::ostringstream cmdMessage;
            cmdMessage << "[AMM_Command]" << c.message() << "\n";
            transmitQ.push(cmdMessage.str());
        }
    }
};


void checkForExit() {
    std::string action;
    while (!closed) {
        getline(std::cin, action);
        std::transform(action.begin(), action.end(), action.begin(), ::toupper);
        if (action == "EXIT") {
            closed = true;
            LOG_INFO << "Shutting down.";
        }
    }
}


void signalHandler(int signum) {
    LOG_WARNING << "Interrupt signal (" << signum << ") received.";

    if (signum == 15) {
        serialport_close(fd);
        LOG_INFO << "Shutdown complete";
    }

    exit(signum);
}


int main(int argc, char *argv[]) {
    const int buf_max = 8192;
    char serialport[40];
    char eolchar = '\n';
    int timeout = 500;
    char buf[buf_max];
    strcpy(serialport, PORT_LINUX);

    std::string nodeString(nodeName);
    mgr = new DDS_Manager(nodeName);

    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    static plog::DDS_Log_Appender<plog::TxtFormatter> DDSAppender(mgr);
    plog::init(plog::verbose, &consoleAppender).addAppender(&DDSAppender);

    LOG_INFO << "Linux Arduino_Bridge starting up";

    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *hf_node_sub_listener = new DDS_Listeners::HighFrequencyNodeSubListener();
    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();
    auto *render_mod_listener = new DDS_Listeners::RenderModificationListener();
    auto *phys_mod_listener = new DDS_Listeners::PhysiologyModificationListener();

    AMMListener al;

    command_sub_listener->SetUpstream(&al);
    node_sub_listener->SetUpstream(&al);
    hf_node_sub_listener->SetUpstream(&al);
    config_sub_listener->SetUpstream(&al);
    render_mod_listener->SetUpstream(&al);
    phys_mod_listener->SetUpstream(&al);

    mgr->InitializeSubscriber(AMM::DataTypes::nodeTopic, &mgr->NodeType, node_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::highFrequencyNodeTopic, &mgr->HighFrequencyNodeType,
                              hf_node_sub_listener);
    mgr->InitializeReliableSubscriber(AMM::DataTypes::commandTopic, &mgr->CommandType, command_sub_listener);
    mgr->InitializeReliableSubscriber(AMM::DataTypes::renderModTopic, &mgr->RenderModificationType,
                                      render_mod_listener);
    mgr->InitializeReliableSubscriber(AMM::DataTypes::physModTopic, &mgr->PhysiologyModificationType,
                                      phys_mod_listener);

    /*mgr->PublishModuleConfiguration(
            mgr->module_id,
            nodeString,
            "Vcom3D",
            nodeName,
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/serial_bridge_capabilities.xml")
    );*/

    std::thread ec(checkForExit);

    fd = serialport_init(serialport, BAUD);
    if (fd == -1) {
        LOG_ERROR << "Unable to open serial port " << serialport;
        exit(EXIT_FAILURE);
    }

    LOG_INFO << "Opened port " << serialport;
//    serialport_flush(fd);

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    LOG_INFO << "Serial_Bridge ready";

    while (!closed) {
        memset(buf, 0, buf_max);  //
        serialport_read_until(fd, buf, eolchar, buf_max, timeout);
        //        LOG_DEBUG << "Read in string: " << buf;
        globalInboundBuffer += buf;
        readHandler();

        while (!transmitQ.empty()) {
            std::string sendStr = transmitQ.front();
            // LOG_DEBUG << "Writing from transmitQ: " << sendStr;
            rc = serialport_write(fd, sendStr.c_str());
            if (rc == -1) {
                LOG_ERROR << " Error writing to serial port";
            }
            transmitQ.pop();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    serialport_close(fd);

    ec.join();

    exit(EXIT_SUCCESS);
}
