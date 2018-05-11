#include "stdafx.h"

#include <map>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include <fstream>

#include <Net/Server.h>
#include <Net/UdpDiscoveryServer.h>

#include "AMMPubSubTypes.h"

#include "AMM/DDS_Manager.h"

#include "tinyxml2.h"
#include <fstream>
#include <string>
#include <iostream>


using namespace std;

Server *s;

short discoveryPort = 8888;
int bridgePort = 9015;

// Daemonize by default
int daemonize = 1;
int discovery = 1;

const string capabilityPrefix = "CAPABILITY=";
const string statusPrefix = "STATUS=";
const string configPrefix = "CONFIG=";
const string modulePrefix = "MODULE_NAME=";
const string registerPrefix = "REGISTER=";
const string requestPrefix = "REQUEST=";
const string keepHistoryPrefix = "KEEP_HISTORY=";
const string actionPrefix = "ACT=";
const string keepAlivePrefix = "[KEEPALIVE]";
const string loadScenarioPrefix = "LOAD_SCENARIO:";

string encodedConfig = "";

bool closed = false;

Publisher *command_publisher;

std::vector<std::string> publishNodes = {
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
        "Respiratory_LeftPleuralCavity_Volume",
        "Respiratory_LeftLung_Volume",
        "Respiratory_RightPleuralCavity_Volume",
        "Respiratory_RightLung_Volume"
};

std::map<std::string, double> labNodes;

std::string decode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
    return boost::algorithm::trim_right_copy_if(std::string(It(std::begin(val)), It(std::end(val))), [](char c) {
        return c == '\0';
    });
}

std::string encode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
    return tmp.append((3 - val.size() % 3) % 3, '=');
}

void InitializeLabNodes() {
    labNodes["Substance_Sodium"] = 0.0f;
    labNodes["MetabolicPanel_CarbonDioxide"] = 0.0f;
    labNodes["Substance_Glucose_Concentration"] = 0.0f;
    labNodes["BloodChemistry_BloodUreaNitrogen_Concentration"] = 0.0f;
    labNodes["Substance_Creatinine_Concentration"] = 0.0f;
    labNodes["BloodChemistry_WhiteBloodCell_Count"] = 0.0f;
    labNodes["BloodChemistry_RedBloodCell_Count"] = 0.0f;
    labNodes["Substance_Hemoglobin_Concentration"] = 0.0f;
    labNodes["BloodChemistry_Hemaocrit"] = 0.0f;
    labNodes["CompleteBloodCount_Platelet"] = 0.0f;
    labNodes["BloodChemistry_BloodPH"] = 0.0f;
    labNodes["BloodChemistry_Arterial_CarbonDioxide_Pressure"] = 0.0f;
    labNodes["BloodChemistry_Arterial_Oxygen_Pressure"] = 0.0f;
    labNodes["Substance_Bicarbonate"] = 0.0f;
    labNodes["Substance_BaseExcess"] = 0.0f;
}

void sendConfig(Client *c, std::string clientType) {
    cout << "Reading from current scenario file..." << endl;
    std::ifstream t("mule1/current_scenario.txt");
    std::string scenario((std::istreambuf_iterator<char>(t)),
                         std::istreambuf_iterator<char>());

    scenario.erase(std::remove(scenario.begin(), scenario.end(), '\n'), scenario.end());
    
    cout << "We got: " << scenario<< endl;
    t.close();

    ostringstream static_filename;
    static_filename << "mule1/module_configuration_static/" << scenario << "_" << clientType << "_configuration.xml";
    cout << "Loading static filename " << static_filename.str() << endl;
    std::ifstream ifs(static_filename.str());
    std::string configContent((std::istreambuf_iterator<char>(ifs)),
                              (std::istreambuf_iterator<char>()));
    std::string encodedConfigContent = encode64(configContent);
    encodedConfig = configPrefix + encodedConfigContent + "\n";

    cout << "The unencoded config looks like: " << configContent << endl;
    cout << "The encoded config looks like: " << encodedConfig << endl;

    Server::SendToClient(c, encodedConfig);
}

void sendConfigToAll(string scene) {
    ostringstream static_filename;
    static_filename << "mule1/module_configuration_static/" << scene << "_virtual_patient_configuration.xml";
    cout << "Loading static filename " << static_filename.str() << endl;
    std::ifstream ifs(static_filename.str());
    std::string configContent((std::istreambuf_iterator<char>(ifs)),
                              (std::istreambuf_iterator<char>()));
    std::string encodedConfigContent = encode64(configContent);
    encodedConfig = configPrefix + encodedConfigContent + "\n";

    cout << "The unencoded config looks like: " << configContent << endl;
    cout << "The encoded config looks like: " << encodedConfig << endl;
    std::string loadScenarioPrefix = "LOAD_SCENARIO:";
    s->SendToAll(encodedConfig);
}

/**
 * FastRTPS/DDS Listener for subscriptions
 */
class TCPBridgeListener : public ListenerInterface {
public:
    void onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t *info) override {
        // rip out the capabilities string and send it on to the TCP client
    }

    void onNewNodeData(AMM::Physiology::Node n, SampleInfo_t *info) override {
        if (n.nodepath() == "EXIT") {
            cout << "Shutting down simulation based on shutdown node-data from physiology engine." << endl;
            closed = true;
        }

        if (labNodes.find(n.nodepath()) != labNodes.end()) {
            labNodes[n.nodepath()] = n.dbl();
        }

        if (std::find(publishNodes.begin(), publishNodes.end(), n.nodepath()) != publishNodes.end()) {
            std::ostringstream messageOut;
            messageOut << n.nodepath() << "=" << n.dbl() << "|";
            string stringOut = messageOut.str();
            s->SendToAll(messageOut.str());
        }
    }

  void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) override {
    cout << "We got command data!   It is: " << c.message() << endl;
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
      } else if (!value.compare(0, loadScenarioPrefix.size(), loadScenarioPrefix)) {
	std::string scene = value.substr(loadScenarioPrefix.size());
	sendConfigToAll(scene);
      } 
    } else {
      std::ostringstream messageOut;
      messageOut << "ACT" << "=" << c.message() << "|";
      s->SendToAll(messageOut.str());
    }
  }
};

// Override client handler code from Net Server
void *Server::HandleClient(void *args) {
    auto *c = (Client *) args;
    char buffer[4096 - 25];
    int index;
    ssize_t n;

    //Add client in Static clients <vector> (Critical section!)
    ServerThread::LockMutex(c->name);
    //Before adding the new client, calculate its id. (Now we have the lock)
    c->SetId(Server::clients.size());
    string defaultName = "Client #" + c->id;
    c->SetName(defaultName);
    cout << "Adding client with id: " << c->id << endl;
    Server::clients.push_back(*c);
    ServerThread::UnlockMutex(c->name);

    while (true) {
        memset(buffer, 0, sizeof buffer);
        n = recv(c->sock, buffer, sizeof buffer, 0);

        //Client disconnected?
        if (n == 0) {
            cout << "Client " << c->name << " disconnected" << endl;
            close(c->sock);

            //Remove client in Static clients <vector> (Critical section!)
            ServerThread::LockMutex(c->name);

            index = Server::FindClientIndex(c);
            cout << "Erasing user in position " << index << " whose name id is: "
                 << Server::clients[index].id << endl;
            Server::clients.erase(Server::clients.begin() + index);

            ServerThread::UnlockMutex(c->name);

            break;
        } else if (n < 0) {
            cerr << "Error while receiving message from client: " << c->name << endl;
        } else {
            vector<string> strings;
            boost::split(strings, buffer, boost::is_any_of("\n"));

            for (auto str : strings) {
                boost::trim_right(str);
                if (!str.empty()) {
                    //                    cout << "We got a message from " << c->name << ": " << str << endl;
                    if (str.substr(0, modulePrefix.size()) == modulePrefix) {
                        string moduleName = str.substr(modulePrefix.size());
                        c->SetName(moduleName);
                        cout << "[CLIENT][" << moduleName << "] module connected" << endl;
                    } else if (str.substr(0, registerPrefix.size()) == registerPrefix) {
                        // Registering for data
                        std::string registerVal = str.substr(registerPrefix.size());
                        cout << "[CLIENT][REGISTER] Client registered for " << registerVal << endl;
                    } else if (str.substr(0, statusPrefix.size()) == statusPrefix) {
                        // Client set their status (OPERATIONAL, etc)
                        std::string statusVal = decode64(str.substr(statusPrefix.size()));
                        cout << "[CLIENT][STATUS] Client sent status of: " << statusVal << endl;
                        /*XMLDocument doc (false);
                        doc.Parse (statusVal);*/
                    } else if (str.substr(0, capabilityPrefix.size()) == capabilityPrefix) {
                        // Client sent their capabilities / announced
                        std::string capabilityVal = decode64(str.substr(capabilityPrefix.size()));
                        cout << "[CLIENT][CAPABILITY] Client sent capabilities of " << capabilityVal << endl;
                        /*XMLDocument doc (false);
                        doc.Parse (capabilityVal);*/
			/**
                        cout << "[CLIENT][CONFIG] Sending configuration file" << endl;
                        cout << "[CLIENT] Client name is " << c->name << endl;
                        sendConfig(c, "virtual_patient");
                        cout << "[CLIENT][CONFIG] Sent!" << endl;
			**/
                    } else if (str.substr(0, keepHistoryPrefix.size()) == keepHistoryPrefix) {
                        // Setting the KEEP_HISTORY flag
                        std::string keepHistory = str.substr(keepHistoryPrefix.size());
                        if (keepHistory == "TRUE") {
                            cout << "[CLIENT] Client wants to keep history." << endl;
                            c->SetKeepHistory(true);
                        } else {
                            cout << "[CLIENT] Client does not want to keep history." << endl;
                            c->SetKeepHistory(false);
                        }
                    } else if (str.substr(0, requestPrefix.size()) == requestPrefix) {
                        std::string request = str.substr(requestPrefix.size());
                        if (request == "LABS") {
                            auto it = labNodes.begin();
                            while (it != labNodes.end()) {
                                std::ostringstream messageOut;
                                messageOut << it->first << "=" << it->second << "|";
                                Server::SendToClient(c, messageOut.str());
                                ++it;
                            }
                        }
                    } else if (str.substr(0, actionPrefix.size()) == actionPrefix) {
                        // Sending action
                        std::string action = str.substr(actionPrefix.size());
                        cout << "[CLIENT] Posting action to AMM: " << action << endl;
                        AMM::PatientAction::BioGears::Command cmdInstance;
                        cmdInstance.message(action);
                        command_publisher->write(&cmdInstance);
                    } else if (str.substr(0, keepAlivePrefix.size()) == keepAlivePrefix) {
                        // keepalive, ignore it
                    } else {
                        cout << "[CLIENT] Unknown message:" << str << endl;
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
        cout << "\tUDP Discovery listening on port " << discoveryPort << endl;
        io_service.run();
    } else {
        cout << "\tUDP discovery service not started due to command line option." << endl;
    }
}

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n" << "\t-h,--help\t\tShow this help message\n" << endl;
}


int main(int argc, const char *argv[]) {
    cout << "=== [AMM - Network Bridge] ===" << endl;

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
    auto *mgr = new DDS_Manager(nodeName.c_str());
    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();

    TCPBridgeListener tl;
    node_sub_listener->SetUpstream(&tl);
    command_sub_listener->SetUpstream(&tl);
    config_sub_listener->SetUpstream(&tl);

    Subscriber *node_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(),
                                                            node_sub_listener);
    Subscriber *command_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::commandTopic,
                                                               AMM::DataTypes::getCommandType(), command_sub_listener);

    auto *pub_listener = new DDS_Listeners::PubListener();
    command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(),
                                                 pub_listener);

    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            "Vcom3D",
            nodeName,
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/tcp_bridge_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(OPERATIONAL);

    cout << "=== [Network_Bridge] Ready ..." << endl;

    std::thread t1(UdpDiscoveryThread);
    s = new Server(bridgePort);
    s->AcceptAndDispatch();

    t1.join();


    cout << "=== [Network_Bridge] Simulation stopped." << endl;

}
