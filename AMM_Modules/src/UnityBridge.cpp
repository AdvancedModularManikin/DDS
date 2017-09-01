#include "stdafx.h"

#include <map>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>

#include "AMM/DDS_Manager.h"
#include "AMM/TCPServer.h"

using namespace std;

bool closed = false;

Publisher *command_publisher;
Subscriber *command_subscriber;
Subscriber *node_subscriber;
TCPServer tcp;

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
        "ECG"
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




void PublishLabs() {
  cout << "Publishing the lab nodes!" << endl;
  std::map<std::string, double>::iterator it = labNodes.begin();
  while(it != labNodes.end())
    {
      std::ostringstream messageOut;
      messageOut << it->first << "=" << it->second << "|";
      tcp.Send(messageOut.str());
      it++;
    }
  tcp.clean();
}

void *tcpClientLoop(void *m) {
  pthread_detach(pthread_self());
  std::string actionPrefix = "ACT=";
  std::string requestPrefix = "REQUEST=";
  
  while (!closed) {
    string str = tcp.getMessage();
    if (str != "") {
      boost::trim_right(str);
      cout << "[CLIENT] Message:" << str << endl;

      // It's a request
      if(str.substr(0, requestPrefix.size()) == requestPrefix) {
	std::string request = str.substr(requestPrefix.size());
	if (request.compare("LABS") == 0) {
	  PublishLabs();
	}
      }
      
      // It's an action
      if(str.substr(0, actionPrefix.size()) == actionPrefix) {
	std::string action = str.substr(actionPrefix.size());
	cout << "[CLIENT] Posting action to AMM: " << action << endl;
	AMM::PatientAction::BioGears::Command cmdInstance;
	cmdInstance.message(action);
	command_publisher->write(&cmdInstance);
      }
      //      tcp.clean();
    }
    usleep(1000);
  }
  tcp.detach();
}

class GenericListener : public ListenerInterface {
public:
    void onNewNodeData(AMM::Physiology::Node n) {
        if (n.nodepath() == "EXIT") {
            cout << "Shutting down simulation based on shutdown node-data from physiology engine." << endl;
            closed = true;
        }

        if(labNodes.find(n.nodepath()) != labNodes.end()) {
	  labNodes[n.nodepath()] = n.dbl();
        }

        if (std::find(publishNodes.begin(), publishNodes.end(), n.nodepath()) != publishNodes.end()) {
            std::ostringstream messageOut;
            messageOut << n.nodepath() << "=" << n.dbl() << "|";
            tcp.Send(messageOut.str());
        }
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c) {
        std::ostringstream messageOut;
        messageOut << "ACT" << "=" << c.message() << "|";
        tcp.Send(messageOut.str());
    }
};


int main(int argc, char *argv[]) {
    int count = 0;
    cout << "=== [AMM - Unity Bridge] ===" << endl;

    InitializeLabNodes();

    auto *mgr = new DDS_Manager();

    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *pub_listener = new DDS_Listeners::PubListener();

    GenericListener al;
    node_sub_listener->SetUpstream(&al);
    command_sub_listener->SetUpstream(&al);

    node_subscriber = mgr->InitializeNodeSubscriber(node_sub_listener);
    command_subscriber = mgr->InitializeCommandSubscriber(command_sub_listener);
    command_publisher = mgr->InitializeCommandPublisher(pub_listener);

    cout << "=== [UnityBridge] Ready ..." << endl;

    pthread_t msg;
    tcp.setup(9015);
    if (pthread_create(&msg, NULL, tcpClientLoop, (void *) 0) == 0) {
        tcp.receive();
    }

    cout << "=== [UnityBridge] Simulation stopped." << endl;

    return 0;

}

