#include "stdafx.h"

#include <algorithm>
#include <fstream>
#include <map>

#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <Net/Client.h>
#include <Net/Server.h>
#include <Net/UdpDiscoveryServer.h>

#include "AMM/DDS/AMMPubSubTypes.h"
#include "AMM/DDS_Manager.h"

#include "AMM/BaseLogger.h"
#include "AMM/DDS_Log_Appender.h"

#include "AMM/Utility.h"

#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;
using namespace AMM;
using namespace AMM::Capability;

Server *s;

short discoveryPort = 8888;
int bridgePort = 9015;

// Daemonize by default
int daemonize = 1;
int discovery = 1;

std::map <std::string, std::string> globalInboundBuffer;

const string capabilityPrefix = "CAPABILITY=";
const string settingsPrefix = "SETTINGS=";
const string statusPrefix = "STATUS=";
const string configPrefix = "CONFIG=";
const string modulePrefix = "MODULE_NAME=";
const string registerPrefix = "REGISTER=";
const string requestPrefix = "REQUEST=";
const string keepHistoryPrefix = "KEEP_HISTORY=";
const string actionPrefix = "ACT=";
std::string genericTopicPrefix = "[";
const string keepAlivePrefix = "[KEEPALIVE]";
const string loadScenarioPrefix = "LOAD_SCENARIO:";
const string haltingString = "HALTING_ERROR";

string encodedConfig = "";

bool closed = false;

DDS_Manager *mgr;

std::map <std::string, std::vector<std::string>> subscribedTopics;
std::map <std::string, std::vector<std::string>> publishedTopics;

std::map <std::string, std::map<std::string, double>> labNodes;
std::map <std::string, std::map<std::string, std::string>> equipmentSettings;
std::map <std::string, std::string> clientMap;
std::map <std::string, std::string> clientTypeMap;

void InitializeLabNodes() {
    //
    labNodes["ALL"]["Substance_Sodium"] = 0.0f;
    labNodes["ALL"]["MetabolicPanel_CarbonDioxide"] = 0.0f;
    labNodes["ALL"]["Substance_Glucose_Concentration"] = 0.0f;
    labNodes["ALL"]["BloodChemistry_BloodUreaNitrogen_Concentration"] = 0.0f;
    labNodes["ALL"]["Substance_Creatinine_Concentration"] = 0.0f;
    labNodes["ALL"]["BloodChemistry_WhiteBloodCell_Count"] = 0.0f;
    labNodes["ALL"]["BloodChemistry_RedBloodCell_Count"] = 0.0f;
    labNodes["ALL"]["Substance_Hemoglobin_Concentration"] = 0.0f;
    labNodes["ALL"]["BloodChemistry_Hemaocrit"] = 0.0f;
    labNodes["ALL"]["CompleteBloodCount_Platelet"] = 0.0f;
    labNodes["ALL"]["BloodChemistry_BloodPH"] = 0.0f;
    labNodes["ALL"]["BloodChemistry_Arterial_CarbonDioxide_Pressure"] = 0.0f;
    labNodes["ALL"]["BloodChemistry_Arterial_Oxygen_Pressure"] = 0.0f;
    labNodes["ALL"]["Substance_Bicarbonate"] = 0.0f;
    labNodes["ALL"]["Substance_BaseExcess"] = 0.0f;
    labNodes["ALL"]["Substance_Lactate_Concentration_mmol"] = 0.0f;

    labNodes["POCT"]["Substance_Sodium"] = 0.0f;
    labNodes["POCT"]["MetabolicPanel_Potassium"] = 0.0f;
    labNodes["POCT"]["MetabolicPanel_Chloride"] = 0.0f;
    labNodes["POCT"]["MetabolicPanel_CarbonDioxide"] = 0.0f;
    labNodes["POCT"]["Substance_Glucose_Concentration"] = 0.0f;
    labNodes["POCT"]["BloodChemistry_BloodUreaNitrogen_Concentration"] = 0.0f;
    labNodes["POCT"]["Substance_Creatinine_Concentration"] = 0.0f;

    labNodes["Hematology"]["BloodChemistry_Hemaocrit"] = 0.0f;
    labNodes["Hematology"]["Substance_Hemoglobin_Concentration"] = 0.0f;

    labNodes["ABG"]["BloodChemistry_BloodPH"] = 0.0f;
    labNodes["ABG"]["BloodChemistry_Arterial_CarbonDioxide_Pressure"] = 0.0f;
    labNodes["ABG"]["BloodChemistry_Arterial_Oxygen_Pressure"] = 0.0f;
    labNodes["ABG"]["MetabolicPanel_CarbonDioxide"] = 0.0f;
    labNodes["ABG"]["Substance_Bicarbonate"] = 0.0f;
    labNodes["ABG"]["Substance_BaseExcess"] = 0.0f;
    labNodes["ABG"]["BloodChemistry_Oxygen_Saturation"] = 0.0f;
    labNodes["ABG"]["Substance_Lactate_Concentration_mmol"] = 0.0f;

    labNodes["VBG"]["Substance_Lactate_Concentration_mmol"] = 0.0f;
    labNodes["VBG"]["BloodChemistry_BloodPH"] = 0.0f;
    labNodes["VBG"]["BloodChemistry_VenousCarbonDioxidePressure"] = 0.0f;
    labNodes["VBG"]["MetabolicPanel_CarbonDioxide"] = 0.0f;
    labNodes["VBG"]["Substance_Bicarbonate"] = 0.0f;
    labNodes["VBG"]["Substance_BaseExcess"] = 0.0f;

    labNodes["BMP"]["Substance_Sodium"] = 0.0f;
    labNodes["BMP"]["MetabolicPanel_Potassium"] = 0.0f;
    labNodes["BMP"]["MetabolicPanel_Chloride"] = 0.0f;
    labNodes["BMP"]["MetabolicPanel_CarbonDioxide"] = 0.0f;
    labNodes["BMP"]["Substance_Glucose_Concentration"] = 0.0f;
    labNodes["BMP"]["BloodChemistry_BloodUreaNitrogen_Concentration"] = 0.0f;
    labNodes["BMP"]["Substance_Creatinine_Concentration"] = 0.0f;

    labNodes["CBC"]["BloodChemistry_WhiteBloodCell_Count"] = 0.0f;
    labNodes["CBC"]["BloodChemistry_RedBloodCell_Count"] = 0.0f;
    labNodes["CBC"]["Substance_Hemoglobin_Concentration"] = 0.0f;
    labNodes["CBC"]["BloodChemistry_Hemaocrit"] = 0.0f;
    labNodes["CBC"]["CompleteBloodCount_Platelet"] = 0.0f;

    labNodes["CMP"]["Substance_Albumin_Concentration"] = 0.0f;
    labNodes["CMP"]["BloodChemistry_BloodUreaNitrogen_Concentration"] = 0.0f;
    labNodes["CMP"]["Substance_Calcium_Concentration"] = 0.0f;
    labNodes["CMP"]["MetabolicPanel_Chloride"] = 0.0f;
    labNodes["CMP"]["MetabolicPanel_CarbonDioxide"] = 0.0f;
    labNodes["CMP"]["Substance_Creatinine_Concentration"] = 0.0f;
    labNodes["CMP"]["Substance_Glucose_Concentration"] = 0.0f;
    labNodes["CMP"]["MetabolicPanel_Potassium"] = 0.0f;
    labNodes["CMP"]["Substance_Sodium"] = 0.0f;
    labNodes["CMP"]["MetabolicPanel_Bilirubin"] = 0.0f;
    labNodes["CMP"]["MetabolicPanel_Protein"] = 0.0f;
}

void sendConfig(Client *c, std::string scene, std::string clientType) {
    ostringstream static_filename;
    static_filename << "static/module_configuration_static/" << scene << "_"
                    << clientType << "_configuration.xml";
    LOG_DEBUG << "Sending " << static_filename.str() << " to " << c->id;
    std::ifstream ifs(static_filename.str());
    std::string configContent((std::istreambuf_iterator<char>(ifs)),
                              (std::istreambuf_iterator<char>()));
    std::string encodedConfigContent = Utility::encode64(configContent);
    encodedConfig = configPrefix + encodedConfigContent + "\n";

    Server::SendToClient(c, encodedConfig);
}

void sendConfigToAll(std::string scene) {
    auto it = clientMap.begin();
    while (it != clientMap.end()) {
        std::string cid = it->first;
        std::string clientType = clientTypeMap[it->first];
        Client *c = Server::GetClientByIndex(cid);
        if (c) {
            sendConfig(c, scene, clientType);
        }
        ++it;
    }
}

/**
 * FastRTPS/DDS Listener for subscriptions
 */
class TCPBridgeListener : public ListenerInterface {
public:
    void onNewHighFrequencyNodeData(AMM::Physiology::HighFrequencyNode n, SampleInfo_t *info) override {
        std::string hfname = "HF_" + n.nodepath();
        auto it = clientMap.begin();
        while (it != clientMap.end()) {
            std::string cid = it->first;
            std::vector <std::string> subV = subscribedTopics[cid];
            if (std::find(subV.begin(), subV.end(), hfname) != subV.end()) {
                Client *c = Server::GetClientByIndex(cid);
                if (c) {
                    std::ostringstream messageOut;
                    messageOut << n.nodepath() << "=" << n.dbl() << "|" << std::endl;
                    string stringOut = messageOut.str();
                    Server::SendToClient(c, messageOut.str());
                }
            }
            ++it;
        }
    }

    void onNewNodeData(AMM::Physiology::Node n, SampleInfo_t *info) override {
        // Drop values into the lab sheets
        for (auto &outer_map_pair : labNodes) {
            if (labNodes[outer_map_pair.first].find(n.nodepath()) !=
                labNodes[outer_map_pair.first].end()) {
                labNodes[outer_map_pair.first][n.nodepath()] = n.dbl();
            }
        }

        auto it = clientMap.begin();
        while (it != clientMap.end()) {
            std::string cid = it->first;
            std::vector <std::string> subV = subscribedTopics[cid];

            if (std::find(subV.begin(), subV.end(), n.nodepath()) != subV.end()) {
                Client *c = Server::GetClientByIndex(cid);
                if (c) {
                    std::ostringstream messageOut;
                    messageOut << n.nodepath() << "=" << n.dbl() << "|" << std::endl;
                    string stringOut = messageOut.str();
                    Server::SendToClient(c, messageOut.str());
                }
            }
            ++it;
        }
    }

    void onNewPhysiologyModificationData(AMM::Physiology::Modification pm,
                                         SampleInfo_t *info) override {
        // Publish values that are supposed to go out on every change
        std::ostringstream messageOut;
        messageOut << "[AMM_Physiology_Modification]"
                   << "type=" << pm.type() << ";"
                   << "location=" << pm.location().description() << ";"
                   << "learner_id=" << pm.practitioner() << ";"
                   << "payload=" << pm.payload()
                   << std::endl;
        string stringOut = messageOut.str();

        LOG_DEBUG << "Received a phys mod: " << stringOut;

        auto it = clientMap.begin();
        while (it != clientMap.end()) {
            std::string cid = it->first;
            std::vector <std::string> subV = subscribedTopics[cid];

            if (std::find(subV.begin(), subV.end(), pm.type()) != subV.end() ||
                std::find(subV.begin(), subV.end(), "AMM_Physiology_Modification") !=
                subV.end()) {
                Client *c = Server::GetClientByIndex(cid);
                if (c) {
                    Server::SendToClient(c, stringOut);
                }
            }
            ++it;
        }
    }

    void onNewRenderModificationData(AMM::Render::Modification rm,
                                     SampleInfo_t *info) override {
        // Publish values that are supposed to go out on every change
        std::ostringstream messageOut;
        messageOut << "[AMM_Render_Modification]"
                   << "type=" << rm.type() << ";"
                   << "location=" << rm.location().description() << ";"
                   << "learner_id=" << rm.practitioner() << ";"
                   << "payload=" << rm.payload()
                   << std::endl;
        string stringOut = messageOut.str();

        LOG_DEBUG << "Received a render mod: " << stringOut;

        auto it = clientMap.begin();
        while (it != clientMap.end()) {
            std::string cid = it->first;
            std::vector <std::string> subV = subscribedTopics[cid];
            if (std::find(subV.begin(), subV.end(), rm.type()) != subV.end() ||
                std::find(subV.begin(), subV.end(), "AMM_Render_Modification") !=
                subV.end()) {
                Client *c = Server::GetClientByIndex(cid);
                if (c) {
                    Server::SendToClient(c, stringOut);
                }
            }
            ++it;
        }
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c,
                          SampleInfo_t *info) override {
        // LOG_DEBUG << "We got command data!   It is: " << c.message();
        if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = c.message().substr(sysPrefix.size());
            if (value.compare("START_SIM") == 0) {
                std::string tmsg = "ACT=START_SIM";
                s->SendToAll(tmsg);
            } else if (value.compare("STOP_SIM") == 0) {
                std::string tmsg = "ACT=STOP_SIM";
                s->SendToAll(tmsg);
            } else if (value.compare("PAUSE_SIM") == 0) {
                std::string tmsg = "ACT=PAUSE_SIM";
                s->SendToAll(tmsg);
            } else if (value.compare("RESET_SIM") == 0) {
                std::string tmsg = "ACT=RESET_SIM";
                s->SendToAll(tmsg);
                InitializeLabNodes();
            } else if (!value.compare(0, loadScenarioPrefix.size(),
                                      loadScenarioPrefix)) {
                std::string scene = value.substr(loadScenarioPrefix.size());
                sendConfigToAll(scene);
                std::ostringstream messageOut;
                messageOut << "ACT" << "=" << c.message();
                s->SendToAll(messageOut.str());
            } else {
                std::ostringstream messageOut;
                messageOut << "ACT" << "=" << c.message();
                s->SendToAll(messageOut.str());
            }
        } else {
            std::ostringstream messageOut;
            messageOut << "ACT"
                       << "=" << c.message() << "|";
            s->SendToAll(messageOut.str());
        }
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

void HandleSettings(Client *c, std::string const &settingsVal) {
    XMLDocument doc(false);
    doc.Parse(settingsVal.c_str());
    tinyxml2::XMLNode *root =
            doc.FirstChildElement("AMMModuleConfiguration");
    tinyxml2::XMLElement *module = root->FirstChildElement("module");
    tinyxml2::XMLElement *caps =
            module->FirstChildElement("capabilities");
    if (caps) {
        for (tinyxml2::XMLNode *node =
                caps->FirstChildElement("capability");
             node; node = node->NextSibling()) {
            tinyxml2::XMLElement *cap = node->ToElement();
            std::string capabilityName = cap->Attribute("name");
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
            }
            PublishSettings(capabilityName);
        }
    }
}

void HandleCapabilities(Client *c, std::string const &capabilityVal) {
    XMLDocument doc(false);
    doc.Parse(capabilityVal.c_str());

    tinyxml2::XMLNode *root = doc.FirstChildElement("AMMModuleConfiguration");
    tinyxml2::XMLElement *module = root->FirstChildElement("module")->ToElement();
    const char *name = module->Attribute("name");
    const char *manufacturer = module->Attribute("manufacturer");
    const char *model = module->Attribute("model");
    const char *serial = module->Attribute("serial_number");
    const char *module_version = module->Attribute("module_version");

    std::string nodeName(name);
    std::string nodeManufacturer(manufacturer);
    std::string nodeModel(model);
    std::string serialNumber(serial);
    std::string moduleVersion(module_version);

    // Set the client's type
    ServerThread::LockMutex(c->id);
    c->SetClientType(nodeName);
    clientTypeMap[c->id] = nodeName;
    ServerThread::UnlockMutex(c->id);

    // Publish configuration
    mgr->PublishModuleConfiguration(c->id, nodeName, nodeManufacturer,
                                    nodeModel, serialNumber,
                                    moduleVersion, capabilityVal);

    subscribedTopics[c->id].clear();
    publishedTopics[c->id].clear();

    tinyxml2::XMLElement *caps =
            module->FirstChildElement("capabilities");
    if (caps) {
        for (tinyxml2::XMLNode *node = caps->FirstChildElement("capability"); node; node = node->NextSibling()) {
            tinyxml2::XMLElement *cap = node->ToElement();
            std::string capabilityName = cap->Attribute("name");
            tinyxml2::XMLElement *starting_settings =
                    cap->FirstChildElement("starting_settings");
            if (starting_settings) {
                for (tinyxml2::XMLNode *settingNode =
                        starting_settings->FirstChildElement("setting");
                     settingNode; settingNode = settingNode->NextSibling()) {
                    tinyxml2::XMLElement *setting = settingNode->ToElement();
                    std::string settingName = setting->Attribute("name");
                    std::string settingValue = setting->Attribute("value");
                    equipmentSettings[capabilityName][settingName] =
                            settingValue;
                }
                PublishSettings(capabilityName);
            }

            tinyxml2::XMLNode *subs =
                    node->FirstChildElement("subscribed_topics");
            if (subs) {
                for (tinyxml2::XMLNode *sub =
                        subs->FirstChildElement("topic");
                     sub; sub = sub->NextSibling()) {
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
                    Utility::add_once(subscribedTopics[c->id], subTopicName);
                    LOG_DEBUG << "[" << capabilityName << "][" << c->id
                              << "] Subscribing to " << subTopicName;
                }
            }

            // Store published topics for this capability
            tinyxml2::XMLNode *pubs =
                    node->FirstChildElement("published_topics");
            if (pubs) {
                for (tinyxml2::XMLNode *pub =
                        pubs->FirstChildElement("topic");
                     pub; pub = pub->NextSibling()) {
                    tinyxml2::XMLElement *p = pub->ToElement();
                    std::string pubTopicName = p->Attribute("name");
                    Utility::add_once(publishedTopics[c->id], pubTopicName);
                    LOG_DEBUG << "[" << capabilityName << "][" << c->id
                              << "] Publishing " << pubTopicName;
                }
            }
        }
    }
}

void HandleStatus(Client *c, std::string const &statusVal) {
    XMLDocument doc(false);
    doc.Parse(statusVal.c_str());

    tinyxml2::XMLNode *root = doc.FirstChildElement("AMMModuleStatus");
    tinyxml2::XMLElement *module =
            root->FirstChildElement("module")->ToElement();
    const char *name = module->Attribute("name");
    std::string nodeName(name);

    std::size_t found = statusVal.find(haltingString);
    if (found != std::string::npos) {
        mgr->SetStatus(c->id, nodeName, HALTING_ERROR);
    } else {
        mgr->SetStatus(c->id, nodeName, OPERATIONAL);
    }
}

void DispatchRequest(Client *c, std::string const &request) {
    if (boost::starts_with(request, "LABS")) {
        LOG_DEBUG << "LABS request: " << request;
        const auto equals_idx = request.find_first_of(';');
        if (std::string::npos != equals_idx) {
            auto str = request.substr(equals_idx + 1);
            LOG_DEBUG << "Return lab values for " << str;
            auto it = labNodes[str].begin();
            while (it != labNodes[str].end()) {
                std::ostringstream messageOut;
                messageOut << it->first << "=" << it->second << ":" << str << "|";
                Server::SendToClient(c, messageOut.str());
                ++it;
            }
        } else {
            LOG_DEBUG << "No specific labs requested, return all values.";
            auto it = labNodes["ALL"].begin();
            while (it != labNodes["ALL"].end()) {
                std::ostringstream messageOut;
                messageOut << it->first << "=" << it->second << "|";
                Server::SendToClient(c, messageOut.str());
                ++it;
            }
        }
    }
}

// Override client handler code from Net Server
void *Server::HandleClient(void *args) {
    using namespace AMM::Capability;
    auto *c = (Client *) args;
    char buffer[8192 - 25];
    int index;
    ssize_t n;

    std::string uuid = mgr->GenerateID();

    ServerThread::LockMutex(uuid);
    c->SetId(uuid);
    string defaultName = "Client " + c->id;
    c->SetName(defaultName);
    Server::clients.push_back(*c);
    clientMap[c->id] = uuid;
    LOG_DEBUG << "Adding client with id: " << c->id;
    ServerThread::UnlockMutex(uuid);

    while (true) {
        memset(buffer, 0, sizeof buffer);
        n = recv(c->sock, buffer, sizeof buffer, 0);

        // Client disconnected?
        if (n == 0) {
            LOG_INFO << c->name << " disconnected";
            close(c->sock);

            // Remove client in Static clients <vector>
            ServerThread::LockMutex(uuid);
            index = Server::FindClientIndex(c);
            LOG_DEBUG << "Erasing user in position " << index
                      << " whose name id is: " << Server::clients[index].id;
            Server::clients.erase(Server::clients.begin() + index);

            // Remove from our client/UUID map
            auto it = clientMap.find(c->id);
            mgr->PublishModuleConfiguration(it->first, "disconnect", "", "", "", "", "");
            clientMap.erase(it);
            ServerThread::UnlockMutex(uuid);
            break;
        } else if (n < 0) {
            LOG_ERROR << "Error while receiving message from client: " << c->name;
        } else {
            std::string tempBuffer(buffer);
            globalInboundBuffer[c->id] += tempBuffer;
            if (!boost::algorithm::ends_with(globalInboundBuffer[c->id], "\n")) {
                continue;
            }
            vector <string> strings = Utility::explode("\n", globalInboundBuffer[c->id]);
            globalInboundBuffer[c->id].clear();

            for (auto str : strings) {
                boost::trim_right(str);
                if (!str.empty()) {
                    if (str.substr(0, modulePrefix.size()) == modulePrefix) {
                        std::string moduleName = str.substr(modulePrefix.size());

                        // Add the modules name to the static Client vector
                        ServerThread::LockMutex(uuid);
                        c->SetName(moduleName);
                        ServerThread::UnlockMutex(uuid);
                        LOG_DEBUG << "Client " << c->id
                                  << " module connected: " << moduleName;
                    } else if (str.substr(0, registerPrefix.size()) == registerPrefix) {
                        // Registering for data
                        std::string registerVal = str.substr(registerPrefix.size());
                        LOG_INFO << "Client " << c->id
                                 << " registered for: " << registerVal;
                    } else if (str.substr(0, statusPrefix.size()) == statusPrefix) {
                        // Client set their status (OPERATIONAL, etc)
                        std::string statusVal;
                        try {
                            statusVal = Utility::decode64(str.substr(statusPrefix.size()));
                        } catch (exception &e) {
                            LOG_ERROR << "Error decoding base64 string: " << e.what();
                            break;
                        }

                        LOG_DEBUG << "Client " << c->id << " sent status: " << statusVal;
                        HandleStatus(c, statusVal);
                    } else if (str.substr(0, capabilityPrefix.size()) ==
                               capabilityPrefix) {
                        // Client sent their capabilities / announced
                        std::string capabilityVal;
                        try {
                            capabilityVal = Utility::decode64(str.substr(capabilityPrefix.size()));
                        } catch (exception &e) {
                            LOG_ERROR << "Error decoding base64 string: " << e.what();
                            break;
                        }
                        LOG_INFO << "Client " << c->id
                                 << " sent capabilities: " << capabilityVal;
                        HandleCapabilities(c, capabilityVal);
                    } else if (str.substr(0, settingsPrefix.size()) == settingsPrefix) {
                        std::string settingsVal;
                        try {
                            settingsVal = Utility::decode64(str.substr(settingsPrefix.size()));
                        } catch (exception &e) {
                            LOG_ERROR << "Error decoding base64 string: " << e.what();
                            break;
                        }
                        LOG_INFO << "Client " << c->id << " sent settings: " << settingsVal;
                        HandleSettings(c, settingsVal);
                    } else if (str.substr(0, keepHistoryPrefix.size()) ==
                               keepHistoryPrefix) {
                        // Setting the KEEP_HISTORY flag
                        std::string keepHistory = str.substr(keepHistoryPrefix.size());
                        if (keepHistory == "TRUE") {
                            LOG_DEBUG << "Client " << c->id << " wants to keep history.";
                            c->SetKeepHistory(true);
                        } else {
                            LOG_DEBUG << "Client " << c->id
                                      << " does not want to keep history.";
                            c->SetKeepHistory(false);
                        }
                    } else if (str.substr(0, requestPrefix.size()) == requestPrefix) {
                        std::string request = str.substr(requestPrefix.size());
                        DispatchRequest(c, request);
                    } else if (str.substr(0, actionPrefix.size()) == actionPrefix) {
                        // Sending action
                        std::string action = str.substr(actionPrefix.size());
                        LOG_INFO << "Client " << c->id
                                 << " posting action to AMM: " << action;
                        AMM::PatientAction::BioGears::Command cmdInstance;
                        cmdInstance.message(action);
                        mgr->PublishCommand(cmdInstance);
                    } else if (!str.compare(0, genericTopicPrefix.size(), genericTopicPrefix)) {
                        std::string topic, message, modType, modLocation, modPayload, modLearner, modInfo;
                        unsigned first = str.find("[");
                        unsigned last = str.find("]");
                        topic = str.substr(first + 1, last - first - 1);
                        message = str.substr(last + 1);

                        if (topic == "KEEPALIVE") {
                            continue;
                        }

                        LOG_INFO << "Received a message for topic " << topic << " with a payload of: " << message;

                        std::list <std::string> tokenList;
                        split(tokenList, message, boost::algorithm::is_any_of(";"), boost::token_compress_on);
                        std::map <std::string, std::string> kvp;

                        BOOST_FOREACH(std::string
                        token, tokenList) {
                            size_t sep_pos = token.find_first_of("=");
                            std::string key = token.substr(0, sep_pos);
                            std::string value = (sep_pos == std::string::npos ? "" : token.substr(
                                    sep_pos + 1,
                                    std::string::npos));
                            kvp[key] = value;
                            LOG_DEBUG << "\t" << key << " => " << kvp[key];
                        }

                        auto type = kvp.find("type");
                        if (type != kvp.end()) {
                            modType = type->second;
                        }

                        auto location = kvp.find("location");
                        if (location != kvp.end()) {
                            modLocation = location->second;
                        }

                        auto learner_id = kvp.find("learner_id");
                        if (learner_id != kvp.end()) {
                            modLearner = learner_id->second;
                        }

                        auto payload = kvp.find("payload");
                        if (payload != kvp.end()) {
                            modPayload = payload->second;
                        }

                        auto info = kvp.find("info");
                        if (info != kvp.end()) {
                            modInfo = info->second;
                        }

                        if (topic == "AMM_Render_Modification") {
                            AMM::Render::Modification renderMod;
                            renderMod.type(modType);
                            renderMod.payload(modPayload);
                            renderMod.practitioner(modLearner);
                            mgr->PublishRenderModification(renderMod);
                        } else if (topic == "AMM_Physiology_Modification") {
                            AMM::Physiology::Modification physMod;
                            physMod.type(modType);
                            physMod.payload(modPayload);
                            physMod.practitioner(modLearner);
                            mgr->PublishPhysiologyModification(physMod);
                        } else if (topic == "AMM_Performance_Assessment") {
                            AMM::Performance::Assessment assessment;
                            assessment.assessment_info(modInfo);
                            assessment.assessment_type(modType);
                            mgr->PublishPerformanceData(assessment);
                        } else if (topic == "AMM_Command") {
                            AMM::PatientAction::BioGears::Command cmdInstance;
                            cmdInstance.message(message);
                            mgr->PublishCommand(cmdInstance);
                        } else {
                            LOG_DEBUG << "Unknown topic: " << topic;
                        }
                    } else if (str.substr(0, keepAlivePrefix.size()) == keepAlivePrefix) {
                        // keepalive, ignore it
                    } else {
                        if (!boost::algorithm::ends_with(str, "Connected")) {
                            LOG_ERROR << "Client " << c->id << " unknown message:" << str;
                        }
                    }
                }
            }
        }
    }

    return nullptr;
}

void UdpDiscoveryThread() {
    if (discovery) {
        boost::asio::io_service io_service;
        UdpDiscoveryServer udps(io_service, discoveryPort);
        LOG_INFO << "UDP Discovery listening on port " << discoveryPort;
        io_service.run();
    } else {
        LOG_INFO << "UDP discovery service not started due to command line option.";
    }
}

static void show_usage(const std::string &name) {
    using namespace AMM::Capability;
    std::cerr << "Usage: " << name << " <option(s)>"
              << "\nOptions:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << std::endl;
}

int main(int argc, const char *argv[]) {

    using namespace AMM::Capability;
    LOG_INFO << "=== [AMM - TCP Bridge] ===";

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

    InitializeLabNodes();

    const std::string nodeName = "AMM_TCP_Bridge";
    std::string nodeString(nodeName);
    mgr = new DDS_Manager(nodeName.c_str());

    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    static plog::DDS_Log_Appender<plog::TxtFormatter> DDSAppender(mgr);
    plog::init(plog::verbose, &consoleAppender).addAppender(&DDSAppender);

    LOG_INFO << "Log to console and DDS";

    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *hf_node_sub_listener = new DDS_Listeners::HighFrequencyNodeSubListener();
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();
    auto *render_mod_listener = new DDS_Listeners::RenderModificationListener();
    auto *phys_mod_listener = new DDS_Listeners::PhysiologyModificationListener();

    TCPBridgeListener tl;
    node_sub_listener->SetUpstream(&tl);
    hf_node_sub_listener->SetUpstream(&tl);
    command_sub_listener->SetUpstream(&tl);
    config_sub_listener->SetUpstream(&tl);
    render_mod_listener->SetUpstream(&tl);
    phys_mod_listener->SetUpstream(&tl);

    mgr->InitializeSubscriber(AMM::DataTypes::nodeTopic,
                              &mgr->NodeType, node_sub_listener);

    mgr->InitializeSubscriber(AMM::DataTypes::highFrequencyNodeTopic,
                              &mgr->HighFrequencyNodeType, hf_node_sub_listener);

    mgr->InitializeReliableSubscriber(AMM::DataTypes::commandTopic,
                                      &mgr->CommandType,
                                      command_sub_listener);

    mgr->InitializeReliableSubscriber(AMM::DataTypes::renderModTopic,
                                      &mgr->RenderModificationType,
                                      render_mod_listener);
    mgr->InitializeReliableSubscriber(
            AMM::DataTypes::physModTopic,
            &mgr->PhysiologyModificationType, phys_mod_listener);

    mgr->PublishModuleConfiguration(
            mgr->module_id, nodeString, "Vcom3D", nodeName, "00001", "0.0.1",
            mgr->GetCapabilitiesAsString(
                    "mule1/module_capabilities/tcp_bridge_capabilities.xml"));

    LOG_INFO << "TCP Bridge ready.";

    std::thread t1(UdpDiscoveryThread);
    s = new Server(bridgePort);
    std::string action;

    s->AcceptAndDispatch();

    t1.join();

    LOG_INFO << "TCP Bridge shutdown.";
}
