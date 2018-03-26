#include "stdafx.h"

#include <map>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>

#include <Net/Server.h>
#include <Net/UdpDiscoveryServer.h>

#include "AMMPubSubTypes.h"

#include "AMM/DDS_Manager.h"

using namespace std;

Server *s;

short discoveryPort = 8888;
int bridgePort = 9015;

// Daemonize by default
int daemonize = 1;

Publisher *command_publisher;
Subscriber *command_subscriber;
Subscriber *node_subscriber;

const string modulePrefix = "MODULE_NAME=";
const string registerPrefix = "REGISTER=";
const string requestPrefix = "REQUEST=";
const string keepHistoryPrefix = "KEEP_HISTORY=";
const string actionPrefix = "ACT=";
const string keepAlivePrefix = "[KEEPALIVE]";

bool closed = false;

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

/**
 * FastRTPS/DDS Listener for subscriptions
 */
class TCPBridgeListener : public ListenerInterface {
public:
    void onNewNodeData(AMM::Physiology::Node n) override {
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

    void onNewCommandData(AMM::PatientAction::BioGears::Command c) override {
    	
		if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = c.message().substr(sysPrefix.size());
            if (value.compare("START_SIM") == 0) {
                
            } else if (value.compare("STOP_SIM") == 0) {
                
            } else if (value.compare("PAUSE_SIM") == 0) {
                
            } else if (value.compare("RESET_SIM") == 0) {
					InitializeLabNodes();
            }
        }    	
    	
    	
        std::ostringstream messageOut;
        messageOut << "ACT" << "=" << c.message() << "|";
        s->SendToAll(messageOut.str());
    }
};

// Override client handler code from Net Server
void *Server::HandleClient(void *args) {
    auto *c = (Client *) args;
    char buffer[256 - 25];
    int index;
    ssize_t n;

    //Add client in Static clients <vector> (Critical section!)
    ServerThread::LockMutex(c->name);
    //Before adding the new client, calculate its id. (Now we have the lock)
    c->SetId(Server::clients.size());
    string defaultName = "Client n.%d" + c->id;
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
                    // cout << "We got a message from " << c->name << ": " << str << endl;
                    if (str.substr(0, modulePrefix.size()) == modulePrefix) {
                        string moduleName = str.substr(modulePrefix.size());
                        c->SetName(moduleName);
                        cout << "[CLIENT][" << moduleName << "] module connected" << endl;
                    } else if (str.substr(0, registerPrefix.size()) == registerPrefix) {
                        // Registering for data
                        std::string registerVal = str.substr(registerPrefix.size());
                        // cout << "[CLIENT][REGISTER] Client registered for " << registerVal << endl;
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

void UdpDiscoveryThread()
{
    boost::asio::io_service io_service;
    UdpDiscoveryServer udps(io_service, discoveryPort);
    cout << "\tUDP Discovery listening on port " << discoveryPort << endl;
    io_service.run();
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
    }

    InitializeLabNodes();

    auto *mgr = new DDS_Manager();
    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *pub_listener = new DDS_Listeners::PubListener();

    TCPBridgeListener tl;
    node_sub_listener->SetUpstream(&tl);
    command_sub_listener->SetUpstream(&tl);

    mgr->InitializeSubscriber(AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(), node_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), command_sub_listener);

    command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), pub_listener);

    cout << "=== [Network_Bridge] Ready ..." << endl;

    std::thread t1(UdpDiscoveryThread);
    s = new Server(bridgePort);
    s->AcceptAndDispatch();

    t1.join();


    cout << "=== [Network_Bridge] Simulation stopped." << endl;

}