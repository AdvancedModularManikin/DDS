#include "stdafx.h"

#include "AMM/DDS_Manager.h"

#include "AMM/SerialPort.h"

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

using namespace ::boost::asio;
using namespace std;

#define PORT_LINUX "/dev/tty96B0"
// #define PORT_LINUX "/dev/ttyACM0"
// #define PORT_LINUX "/dev/serial/by-id/usb-Adafruit_Feather_32u4-if00"
#define PORT_WINDOW "COM7"

int daemonize = 1;
bool closed = false;
bool transmit = false;
bool first_message = true;
std::string reportPrefix = "[REPORT]";
std::string actionPrefix = "[AMM_Command]";
std::string xmlPrefix = "<?xml";

std::string msg1 = "[Scenario]m1s1=mule1_scene1\n";
std::string msg2 = "[Capability]monitor_level=true\n";
std::string msg3 = "[Config_Data]sound_alarm=false\n";
std::string msg4 = "[Capability]detect_button_press=true\n";
std::string msg5 = "[Config_Data]button_message=some_action_name\n";
std::string haltingString = "HALTING_ERROR";

Publisher *command_publisher;
queue<string> transmitQ;

// Set up DDS
const char* nodeName = "AMM_Serial_Bridge";
DDS_Manager *mgr;

vector<string> explode( const string &delimiter, const string &str)
{
  vector<string> arr;
  
  int strleng = str.length();
  int delleng = delimiter.length();
  if (delleng==0)
    return arr;//no change
  
  int i=0;
  int k=0;
  while( i<strleng )
    {
      int j=0;
      while (i+j<strleng && j<delleng && str[i+j]==delimiter[j])
	j++;
      if (j==delleng)//found delimiter
	{
	  arr.push_back(  str.substr(k, i-k) );
	  i+=delleng;
	  k=i;
	}
      else
	{
	  i++;
	}
    }
  arr.push_back(  str.substr(k, i-k) );
  return arr;
}

void sendConfigInfo(std::string scene) {
    ostringstream static_filename;
    static_filename << "mule1/module_configuration_static/" << scene << "_liquid_sensor.txt";
    std::ifstream ifs(static_filename.str());
    std::string configContent((std::istreambuf_iterator<char>(ifs)),
                              (std::istreambuf_iterator<char>()));
    ifs.close();
     vector<string> v = explode("\n", configContent);
    for(int i=0; i<v.size(); i++) {
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
      std::string rsp = v[i];
      transmitQ.push(rsp);

      }

}

void readHandler(boost::array<char,SerialPort::k_readBufferSize> const& buffer, size_t bytesTransferred) {
    std::string inboundBuffer;
    std::copy(buffer.begin(), buffer.begin()+bytesTransferred, std::back_inserter(inboundBuffer));

    vector<string> v = explode("\n", inboundBuffer);
    for(int i=0; i<v.size(); i++) {
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
      }  else if (!rsp.compare(0, xmlPrefix.size(), xmlPrefix)) {
        std::string value = rsp;
	
        cout << "=== [SERIAL] Recieved an XML snippet:" << endl;
	
	if (first_message) {
	  cout << "\t[CAPABILITY_XML] " << value << endl;
	  first_message = false;
        mgr->PublishModuleConfiguration(
                "liquid_reservoir",
                "Vcom3D",
                "liquid_reservoir",
                "00001",
                "0.0.1",
                value
        );
	} else {
	  cout << "\t[STATUS_XML] " << value << endl;
	  std::size_t found = value.find(haltingString);
	  if (found!=std::string::npos) {
	    mgr->SetStatus("liquid_reservoir", HALTING_ERROR);
	  } else {
	    mgr->SetStatus("liquid_reservoir", OPERATIONAL);
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
    void onNewNodeData(AMM::Physiology::Node n, SampleInfo_t *info) override {
      if (n.nodepath() == "EXIT") {
	  cout << "Shutting down simulation based on shutdown node-data from physiology engine."  << endl;
	  closed = true;
	  return;
        }
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) override {
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

    auto * node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto * command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto * pub_listener = new DDS_Listeners::PubListener();

    GenericSerialListener al;
    node_sub_listener->SetUpstream(&al);
    command_sub_listener->SetUpstream(&al);

    mgr->InitializeSubscriber(AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(), node_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), command_sub_listener);

    command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), pub_listener);

    // Set up serial
    io_service io;
    SerialPort serialPort(io, 115200, PORT_LINUX);
    serialPort.DataRead.connect(&readHandler);
    boost::thread t(
            boost::bind(
                    &boost::asio::io_service::run,
                    &io));

    if(serialPort.Initialize())
    {
        std::cerr << "Initialization failed!" << std::endl;
        return 1;
    }


    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            nodeString,
            "Vcom3D",
            nodeName,
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/serial_bridge_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus( nodeString,OPERATIONAL);

    cout << "=== [Serial_Bridge] Ready ..." << endl;

    while (!closed) {
        while(!transmitQ.empty()) {
	  cout << "[SERIAL][SEND_QUEUE] " << transmitQ.front();
	  serialPort.Write(transmitQ.front());
	  transmitQ.pop();
        }

	std::this_thread::sleep_for (std::chrono::milliseconds(250));
        cout.flush();
    }

    cout << "=== [Serial_Bridge] Simulation stopped." << endl;

    return 0;

}

