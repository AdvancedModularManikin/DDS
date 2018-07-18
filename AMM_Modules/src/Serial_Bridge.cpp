#include "stdafx.h"

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <vector>
#include <queue>
#include <stack>
#include <thread>
#include <fstream>
#include <string>
#include <iostream>

#include "AMMPubSubTypes.h"

#include "AMM/BaseLogger.h"
#include "AMM/DDS_Manager.h"

#include "AMM/SerialPort.h"

#include "tinyxml2.h"

using namespace std;
using namespace ::boost::asio;
using namespace tinyxml2;
using namespace AMM;

#define PORT_LINUX "/dev/tty96B0"
// #define PORT_LINUX "/dev/ttyACM0"
// #define PORT_LINUX "/dev/serial/by-id/usb-Adafruit_Feather_32u4-if00"
#define PORT_WINDOW "COM7"

int daemonize = 1;
bool closed = false;
bool first_message = true;

std::string requestPrefix = "[REQUEST]";
std::string reportPrefix = "[REPORT]";
std::string actionPrefix = "[AMM_Command]";
std::string xmlPrefix = "<?xml";

std::string haltingString = "HALTING_ERROR";

std::map <std::string, std::string> subscribedTopics;
std::map <std::string, std::string> publishedTopics;

std::vector <std::string> publishNodes = {
        "EXIT",
        "SIM_TIME",
        "Cardiovascular_HeartRate",
        "Cardiovascular_Arterial_Systolic_Pressure",
        "Cardiovascular_Arterial_Diastolic_Pressure",
        "Cardiovascular_Arterial_Mean_Pressure",
        "Cardiovascular_CentralVenous_Mean_Pressure",
        "MetabolicPanel_CarbonDioxide",
        "BloodChemistry_Oxygen_Saturation",
        "Respiratory_Respiration_Rate",
        "Energy_Core_Temperature",
        "Cardiovascular_Arterial_Pressure",
        "Respiratory_CarbonDioxide_Exhaled",
        "ECG",
        "Respiratory_Tidal_Volume",
        "Respiratory_LeftPleuralCavity_Volume",
        "Respiratory_LeftLung_Volume",
        "Respiratory_RightPleuralCavity_Volume",
        "Respiratory_RightLung_Volume"
};

Publisher *command_publisher;
std::queue <std::string> transmitQ;

// Set up DDS
const char *nodeName = "AMM_Serial_Bridge";
DDS_Manager *mgr;

std::vector <std::string> explode(const std::string &delimiter, const std::string &str) {
    std::vector <std::string> arr;

    int strleng = str.length();
    int delleng = delimiter.length();
    if (delleng == 0)
        return arr;//no change

    int i = 0;
    int k = 0;
    while (i < strleng) {
        int j = 0;
        while (i + j < strleng && j < delleng && str[i + j] == delimiter[j])
            j++;
        if (j == delleng)//found delimiter
        {
            arr.push_back(str.substr(k, i - k));
            i += delleng;
            k = i;
        } else {
            i++;
        }
    }
    arr.push_back(str.substr(k, i - k));
    return arr;
};

void sendConfigInfo(std::string scene) {
    std::ostringstream static_filename;
    static_filename << "mule1/module_configuration_static/" << scene << "_liquid_sensor.txt";
    std::ifstream ifs(static_filename.str());
    std::string configContent((std::istreambuf_iterator<char>(ifs)),
                              (std::istreambuf_iterator<char>()));
    ifs.close();
    std::vector <std::string> v = explode("\n", configContent);
    for (int i = 0; i < v.size(); i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::string rsp = v[i];
        transmitQ.push(rsp);

    }
};

void readHandler(boost::array<char, SerialPort::k_readBufferSize> buffer, std::size_t bytesTransferred) {
    std::string inboundBuffer;
    std::copy(buffer.begin(), buffer.begin() + bytesTransferred, std::back_inserter(inboundBuffer));
    vector <string> v = explode("\n", inboundBuffer);
    for (int i = 0; i < v.size(); i++) {
        std::string rsp = v[i];
        if (!rsp.compare(0, reportPrefix.size(), reportPrefix)) {
            std::string value = rsp.substr(reportPrefix.size());
            cout << "=== [SERIAL] Making REPORT: " << value << endl;
        } else if (!rsp.compare(0, actionPrefix.size(), actionPrefix)) {
            std::string value = rsp.substr(actionPrefix.size());
            cout << "=== [SERIAL] Sending a command: " << value << endl;
            AMM::PatientAction::BioGears::Command cmdInstance;
            boost::trim_right(value);
            cmdInstance.message(value);
            command_publisher->write(&cmdInstance);
        } else if (!rsp.compare(0, xmlPrefix.size(), xmlPrefix)) {
            std::string value = rsp;
            cout << "=== [SERIAL] Recieved an XML snippet:" << endl;
            if (first_message) {
                first_message = false;
                cout << "\t[CAPABILITY_XML] " << value << endl;
                XMLDocument doc(false);
                doc.Parse(value.c_str());
                tinyxml2::XMLNode *root = doc.FirstChildElement("AMMModuleConfiguration");
                tinyxml2::XMLElement *module = root->FirstChildElement("module")->ToElement();
                const char *name = module->Attribute("name");
                std::string nodeName(name);
                mgr->PublishModuleConfiguration(
                        mgr->module_id,
                        nodeName,
                        "Vcom3D",
                        nodeName,
                        "00001",
                        "0.0.1",
                        value
                );

                tinyxml2::XMLElement *caps = module->FirstChildElement("capabilities");
                if (caps) {
                    for (tinyxml2::XMLNode *node = caps->FirstChildElement(
                            "capability"); node; node = node->NextSibling()) {
                        tinyxml2::XMLElement *cap = node->ToElement();
                        std::string capabilityName = cap->Attribute("name");

                        subscribedTopics[capabilityName].clear();
                        publishedTopics[capabilityName].clear();

                        // Store subscribed topics for this capability
                        tinyxml2::XMLElement *subs = node->FirstChildElement("subscribed_topics");
                        for (tinyxml2::XMLNode *sub = subs->FirstChildElement("topic"); sub; sub = sub->NextSibling()) {
                            tinyxml2::XMLElement *s = sub->ToElement();
                            std::string subTopicName = s->Attribute("name");
                            std::string subTopicNodePath = s->Attribute("nodepath");
                            if (!subTopicNodePath.empty()) {
                                LOG_TRACE << "Adding " << subTopicNodePath << " to subscribed topics";
                                subscribedTopics[capabilityName] = subTopicNodePath;
                            } else {
                                LOG_TRACE << "Adding " << subTopicName << " to subscribed topics";
                                subscribedTopics[capabilityName] = subTopicName;
                            }
                        }

                        // Store published topics for this capability
                        tinyxml2::XMLElement *pubs = node->FirstChildElement("published_topics");
                        for (tinyxml2::XMLNode *pub = pubs->FirstChildElement("topic"); pub; pub = pub->NextSibling()) {
                            tinyxml2::XMLElement *p = pub->ToElement();
                            std::string pubTopicName = p->Attribute("name");
                            std::string pubTopicNodePath = p->Attribute("nodepath");
                            if (!pubTopicNodePath.empty()) {
                                LOG_TRACE << "Adding " << pubTopicNodePath << " to published topics";
                                publishedTopics[capabilityName] = pubTopicNodePath;
                            } else {
                                LOG_TRACE << "Adding " << pubTopicName << " to published topics";
                                publishedTopics[capabilityName] = pubTopicName;
                            }
                        }
                    }
                }
            } else {
                cout << "\t[STATUS_XML] " << value << endl;
                XMLDocument doc(false);
                doc.Parse(value.c_str());
                tinyxml2::XMLNode *root = doc.FirstChildElement("AMMModuleConfiguration");
                tinyxml2::XMLElement *module = root->FirstChildElement("module")->ToElement();
                const char *name = module->Attribute("name");
                std::string nodeName(name);

                std::size_t found = value.find(haltingString);
                if (found != std::string::npos) {
                    mgr->SetStatus(mgr->module_id, nodeName, HALTING_ERROR);
                } else {
                    mgr->SetStatus(mgr->module_id, nodeName, OPERATIONAL);
                }
            }
        } else {
            if (!rsp.empty() && rsp != "\r") {
                cout << "=== [SERIAL][DEBUG] " << rsp << endl;
            }
        }
    }
}

class GenericSerialListener : public ListenerInterface {
public:
    void onNewNodeData(AMM::Physiology::Node n) override {
        if (n.nodepath() == "EXIT") {
            cout << "Shutting down simulation based on shutdown node-data from physiology engine." << endl;
            closed = true;
            return;
        }

        // Publish values that are supposed to go out on every change
        if (std::find(publishNodes.begin(), publishNodes.end(), n.nodepath()) != publishNodes.end()) {
            std::ostringstream messageOut;
            messageOut << "[AMM_Node_Data]" << n.nodepath() << "=" << n.dbl();
            string stringOut = messageOut.str();
            transmitQ.push(messageOut.str());
        }
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c) override {
        cout << "We got some command data!  " << c.message() << endl;
        if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = c.message().substr(sysPrefix.size());
            if (value.compare("START_SIM") == 0) {

            } else if (value.compare("STOP_SIM") == 0) {

            } else if (value.compare("PAUSE_SIM") == 0) {

            } else if (value.compare("RESET_SIM") == 0) {

            } else if (!value.compare(0, loadScenarioPrefix.size(), loadScenarioPrefix)) {

                std::string scene = value.substr(loadScenarioPrefix.size());
                cout << "Time to load scene " << scene << endl;
                sendConfigInfo(scene);
            }
        } else {
            ostringstream cmdMessage;
            cmdMessage << "[AMM_Command]" << c.message() << endl;
            transmitQ.push(cmdMessage.str());
        }
    }
};

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n" << "\t-h,--help\t\tShow this help message\n" << endl;
}

int main(int argc, char *argv[]) {
    cout << "=== [AMM - Serial Module Bridge] ===" << endl;

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
    auto *pub_listener = new DDS_Listeners::PubListener();

    GenericSerialListener al;
    node_sub_listener->SetUpstream(&al);
    command_sub_listener->SetUpstream(&al);

    mgr->InitializeSubscriber(AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(), node_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), command_sub_listener);

    command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(),
                                                 pub_listener);

    // Set up serial
    io_service io;
    SerialPort serialPort(io, 115200, PORT_LINUX);
    serialPort.DataRead.connect(&readHandler);
    boost::thread
    t(
            boost::bind(
                    &boost::asio::io_service::run,
                    &io));

    if (serialPort.Initialize()) {
        std::cerr << "Initialization failed!" << std::endl;
        return 1;
    }


    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            mgr->module_id,
            nodeString,
            "Vcom3D",
            nodeName,
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/serial_bridge_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);

    cout << "=== [Serial_Bridge] Ready ..." << endl;

    while (!closed) {
        while (!transmitQ.empty()) {
            serialPort.Write(transmitQ.front());
            transmitQ.pop();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        cout.flush();
    }

    cout << "=== [Serial_Bridge] Simulation stopped." << endl;

    return 0;

}

