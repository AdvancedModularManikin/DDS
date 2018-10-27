#include "stdafx.h"

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>

#include <fstream>
#include <queue>
#include <stack>
#include <thread>

#include "AMMPubSubTypes.h"

#include "AMM/BaseLogger.h"
#include "AMM/DDS_Manager.h"

#include "AMM/SerialPort.h"

#include "AMM/Utility.hpp"

#include "tinyxml2.h"

using namespace std;
using namespace ::boost::asio;
using namespace boost;
using namespace tinyxml2;
using namespace AMM;

#define PORT_LINUX "/dev/tty96B0"
// #define PORT_LINUX "/dev/ttyACM0"
// #define PORT_LINUX "/dev/serial/by-id/usb-Adafruit_Feather_32u4-if00"
#define PORT_WINDOW "COM7"

int daemonize = 1;
bool closed = false;
bool first_message = true;

std::string globalInboundBuffer;
std::string requestPrefix = "[REQUEST]";
std::string reportPrefix = "[REPORT]";
std::string actionPrefix = "[AMM_Command]";
std::string genericTopicPrefix = "[";
std::string xmlPrefix = "<?xml";

std::vector<std::string> subscribedTopics;
std::vector<std::string> publishedTopics;

std::queue<std::string> transmitQ;

// Set up DDS
const char *nodeName = "AMM_Serial_Bridge";
DDS_Manager *mgr;

void sendConfigInfo(std::string scene) {
    std::ostringstream static_filename;
    static_filename << "mule1/module_configuration_static/" << scene
                    << "_liquid_sensor.txt";
    std::ifstream ifs(static_filename.str());
    std::string configContent((std::istreambuf_iterator<char>(ifs)),
                              (std::istreambuf_iterator<char>()));
    ifs.close();
    std::vector<std::string> v = explode("\n", configContent);
    for (int i = 0; i < v.size(); i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::string rsp = v[i] + "\n";
        transmitQ.push(rsp);
    }
};

void readHandler(boost::array<char, SerialPort::k_readBufferSize> const &buffer,
                 std::size_t bytesTransferred) {
    using namespace AMM::Capability;
    std::copy(buffer.begin(), buffer.begin() + bytesTransferred,
              std::back_inserter(globalInboundBuffer));
    if (!boost::algorithm::ends_with(globalInboundBuffer, "\n")) {
        return;
    }
    vector<string> v = explode("\n", globalInboundBuffer);
    globalInboundBuffer.clear();
    for (int i = 0; i < v.size(); i++) {
        std::string rsp = v[i];
        if (!rsp.compare(0, reportPrefix.size(), reportPrefix)) {
            std::string value = rsp.substr(reportPrefix.size());
            LOG_TRACE << "Received report via serial: " << value;
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
            if (first_message) {
                first_message = false;
                LOG_TRACE << "\tCAPABILITY XML:  " << value;

                XMLDocument doc(false);
                doc.Parse(value.c_str());
                tinyxml2::XMLNode *root =
                        doc.FirstChildElement("AMMModuleConfiguration");
                tinyxml2::XMLNode *mod = root->FirstChildElement("module");

                tinyxml2::XMLElement *module = mod->ToElement();
                std::string module_name = module->Attribute("name");
                std::string manufacturer = module->Attribute("manufacturer");
                std::string model = module->Attribute("model");
                std::string serial_number = module->Attribute("serial_number");
                std::string module_version = module->Attribute("module_version");

                mgr->PublishModuleConfiguration(mgr->module_id, module_name,
                                                manufacturer, model, serial_number,
                                                module_version, value);

                subscribedTopics.clear();
                publishedTopics.clear();
                tinyxml2::XMLNode *caps = mod->FirstChildElement("capabilities");

                if (caps) {
                    for (tinyxml2::XMLNode *node = caps->FirstChildElement("capability");
                         node; node = node->NextSibling()) {
                        tinyxml2::XMLElement *cap = node->ToElement();
                        std::string capabilityName = cap->Attribute("name");

                        tinyxml2::XMLElement *starting_settings =
                                cap->FirstChildElement("starting_settings");
                        if (starting_settings) {
                            LOG_TRACE << "Received starting settings";
                            for (tinyxml2::XMLNode *settingNode =
                                    starting_settings->FirstChildElement("setting");
                                 settingNode; settingNode = settingNode->NextSibling()) {
                                tinyxml2::XMLElement *setting = settingNode->ToElement();
                                std::string settingName = setting->Attribute("name");
                                std::string settingValue = setting->Attribute("value");
                                LOG_TRACE << "[" << settingName << "] = " << settingValue;
                            }
                        }

                        // Store subscribed topics for this capability
                        tinyxml2::XMLNode *subs =
                                node->FirstChildElement("subscribed_topics");
                        if (subs) {
                            for (tinyxml2::XMLNode *sub = subs->FirstChildElement("topic");
                                 sub; sub = sub->NextSibling()) {
                                tinyxml2::XMLElement *s = sub->ToElement();
                                std::string subTopicName = s->Attribute("name");

                                if (s->Attribute("nodepath")) {
                                    subTopicName = s->Attribute("nodepath");
                                } else if (s->Attribute("type")) {
                                    subTopicName = s->Attribute("type");
                                }
                                add_once(subscribedTopics, subTopicName);
                                LOG_TRACE << "[" << capabilityName << "][SUBSCRIBE]"
                                          << subTopicName;
                            }
                        }

                        // Store published topics for this capability
                        tinyxml2::XMLNode *pubs =
                                node->FirstChildElement("published_topics");
                        if (pubs) {
                            for (tinyxml2::XMLNode *pub = pubs->FirstChildElement("topic");
                                 pub; pub = pub->NextSibling()) {
                                tinyxml2::XMLElement *p = pub->ToElement();
                                std::string pubTopicName = p->Attribute("name");
                                add_once(publishedTopics, pubTopicName);
                                LOG_TRACE << "[" << capabilityName << "][PUBLISH]"
                                          << pubTopicName;
                            }
                        }
                    }
                }
            } else {
                LOG_TRACE << "\tSTATUS XML: " << value;
                XMLDocument doc(false);
                doc.Parse(value.c_str());
                tinyxml2::XMLNode *root = doc.FirstChildElement("AMMModuleStatus");
                tinyxml2::XMLElement *module =
                        root->FirstChildElement("module")->ToElement();
                const char *name = module->Attribute("name");
                std::string nodeName(name);

                tinyxml2::XMLElement *caps = module->FirstChildElement("capabilities");
                if (caps) {
                    for (tinyxml2::XMLNode *node = caps->FirstChildElement("capability");
                         node; node = node->NextSibling()) {
                        tinyxml2::XMLElement *cap = node->ToElement();
                        std::string capabilityName = cap->Attribute("name");
                        std::string statusVal = cap->Attribute("status");

                        if (statusVal == "OPERATIONAL") {
                            mgr->SetStatus(mgr->module_id, nodeName, capabilityName,
                                           OPERATIONAL);
                        } else if (statusVal == "HALTING_ERROR") {
                            std::string errorMessage = cap->Attribute("message");
                            std::vector<std::string> errorMessages = {errorMessage};
                            mgr->SetStatus(mgr->module_id, nodeName, capabilityName,
                                           HALTING_ERROR, errorMessages);
                        } else if (statusVal == "IMPENDING_ERROR") {
                            std::string errorMessage = cap->Attribute("message");
                            std::vector<std::string> errorMessages = {errorMessage};
                            mgr->SetStatus(mgr->module_id, nodeName, capabilityName,
                                           IMPENDING_ERROR, errorMessages);
                        } else {
                            LOG_ERROR << "Invalid status value " << statusVal
                                      << " for capability " << capabilityName;
                        }
                    }
                }
            }
        } else if (!rsp.compare(0, genericTopicPrefix.size(), genericTopicPrefix)) {
            std::string topic, message, modType, modLocation, modPayload;
            unsigned first = rsp.find("[");
            unsigned last = rsp.find("]");
            topic = rsp.substr(first + 1, last - first - 1);
            message = rsp.substr(last + 1);
            LOG_INFO << "Received a message for topic " << topic
                     << " with a payload of: " << message;

            list<string> tokenList;
            split(tokenList, message, is_any_of(";"), token_compress_on);
            map<string, string> kvp;

            BOOST_FOREACH (string token, tokenList) {
                            size_t sep_pos = token.find_first_of("=");
                            string key = token.substr(0, sep_pos);
                            string value =
                                    (sep_pos == string::npos ? ""
                                                             : token.substr(sep_pos + 1, string::npos));
                            kvp[key] = value;
                            LOG_TRACE << "\t" << key << " => " << kvp[key];
                        }

            auto type = kvp.find("type");
            if (type != kvp.end()) {
                modType = type->second;
            }

            auto location = kvp.find("location");
            if (location != kvp.end()) {
                modLocation = type->second;
            }

            auto payload = kvp.find("payload");
            if (payload != kvp.end()) {
                modPayload = type->second;
            }

            if (topic == "AMM_Render_Modification") {
                AMM::Render::Modification renderMod;
                renderMod.type(modType);
                renderMod.payload(modPayload);
                mgr->PublishRenderModification(renderMod);
            } else if (topic == "AMM_Physiology_Modification") {
                AMM::Physiology::Modification physMod;
                physMod.type(modType);
                physMod.payload(modPayload);
                mgr->PublishPhysiologyModification(physMod);
            } else {
                LOG_TRACE << "Unknown topic: " << topic;
            }
        } else {
            if (!rsp.empty() && rsp != "\r") {
                LOG_TRACE << "Unknown message: " << rsp;
            }
        }
    }
}

class GenericSerialListener : public ListenerInterface {
public:
    void onNewNodeData(AMM::Physiology::Node n, SampleInfo_t *info) override {
        if (n.nodepath() == "EXIT") {
            LOG_INFO << "Shutting down simulation based on shutdown node-data from "
                        "physiology engine.";
            closed = true;
            return;
        }

        // Publish values that are supposed to go out on every change
        if (std::find(subscribedTopics.begin(), subscribedTopics.end(),
                      n.nodepath()) != subscribedTopics.end()) {
            std::ostringstream messageOut;
            messageOut << "[AMM_Node_Data]" << n.nodepath() << "=" << n.dbl()
                       << std::endl;
            transmitQ.push(messageOut.str());
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
                   << "payload=" << pm.payload() << std::endl;
        string stringOut = messageOut.str();

        if (std::find(subscribedTopics.begin(), subscribedTopics.end(),
                      pm.type()) != subscribedTopics.end() ||
            std::find(subscribedTopics.begin(), subscribedTopics.end(),
                      "AMM_Physiology_Modification") != subscribedTopics.end()) {
            transmitQ.push(messageOut.str());
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
                   << "payload=" << rm.payload() << std::endl;
        string stringOut = messageOut.str();

        LOG_TRACE << "Render modification being sent: " << stringOut;

        if (std::find(subscribedTopics.begin(), subscribedTopics.end(),
                      rm.type()) != subscribedTopics.end() ||
            std::find(subscribedTopics.begin(), subscribedTopics.end(),
                      "AMM_Render_Modification") != subscribedTopics.end()) {
            transmitQ.push(messageOut.str());
        }
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c,
                          SampleInfo_t *info) override {
        LOG_TRACE << "Command received from AMM: " << c.message();
        if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = c.message().substr(sysPrefix.size());
            if (value.compare("START_SIM") == 0) {

            } else if (value.compare("STOP_SIM") == 0) {

            } else if (value.compare("PAUSE_SIM") == 0) {

            } else if (value.compare("RESET_SIM") == 0) {

            } else if (!value.compare(0, loadScenarioPrefix.size(),
                                      loadScenarioPrefix)) {
                std::string scene = value.substr(loadScenarioPrefix.size());
                LOG_TRACE << "Time to load scene " << scene;
                sendConfigInfo(scene);
            }
        } else {
            ostringstream cmdMessage;
            cmdMessage << "[AMM_Command]" << c.message();
            transmitQ.push(cmdMessage.str());
        }
    }
};

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>"
         << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n"
         << endl;
}

int main(int argc, char *argv[]) {
    using namespace AMM::Capability;
    LOG_INFO << "Serial_Bridge starting up";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }

        if (arg == "-d") {
            daemonize = 1;
        }
    }
    std::string nodeString(nodeName);
    mgr = new DDS_Manager(nodeName);

    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();
    auto *render_mod_listener = new DDS_Listeners::RenderModificationListener();
    auto *phys_mod_listener = new DDS_Listeners::PhysiologyModificationListener();

    GenericSerialListener al;
    node_sub_listener->SetUpstream(&al);
    command_sub_listener->SetUpstream(&al);
    config_sub_listener->SetUpstream(&al);
    render_mod_listener->SetUpstream(&al);
    phys_mod_listener->SetUpstream(&al);

    mgr->InitializeSubscriber(AMM::DataTypes::nodeTopic,
                              AMM::DataTypes::getNodeType(), node_sub_listener);
    mgr->InitializeReliableSubscriber(AMM::DataTypes::commandTopic,
                                      AMM::DataTypes::getCommandType(),
                                      command_sub_listener);

    mgr->InitializeReliableSubscriber(AMM::DataTypes::renderModTopic,
                                      AMM::DataTypes::getRenderModificationType(),
                                      render_mod_listener);
    mgr->InitializeReliableSubscriber(
            AMM::DataTypes::physModTopic,
            AMM::DataTypes::getPhysiologyModificationType(), phys_mod_listener);

    // Set up serial
    io_service io;
    SerialPort serialPort(io, 115200, PORT_LINUX);
    serialPort.DataRead.connect(&readHandler);
    boost::thread t(boost::bind(&boost::asio::io_service::run, &io));

    if (serialPort.Initialize()) {
        LOG_ERROR << "Initialization failed!";
        return 1;
    }

    // Publish bridge module configuration once we've set all our publishers and
    // listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            mgr->module_id, nodeString, "Vcom3D", nodeName, "00001", "0.0.1",
            mgr->GetCapabilitiesAsString(
                    "mule1/module_capabilities/serial_bridge_capabilities.xml"));

    mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);

    LOG_INFO << "Serial_Bridge ready";

    while (!closed) {
        while (!transmitQ.empty()) {
            serialPort.Write(transmitQ.front());
            transmitQ.pop();
        }

        //        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        //        cout.flush();
    }

    io.stop();
    io.reset();

    LOG_INFO << "Serial_Bridge simulation stopped.";
    return 0;
}
