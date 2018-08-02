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

std::string globalInboundBuffer;
std::string requestPrefix = "[REQUEST]";
std::string reportPrefix = "[REPORT]";
std::string actionPrefix = "[AMM_Command]";
std::string xmlPrefix = "<?xml";

std::vector <std::string> subscribedTopics;
std::vector <std::string> publishedTopics;

Publisher *command_publisher;
std::queue <std::string> transmitQ;

// Set up DDS
const char *nodeName = "AMM_Serial_Bridge";
DDS_Manager *mgr;

void add_once(std::vector<std::string>& vec, const std::string& element) {
  std::remove(vec.begin(), vec.end(), element);
  vec.push_back(element);
}

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
        std::string rsp = v[i] + "\n";
        transmitQ.push(rsp);

    }
};

void readHandler(boost::array<char, SerialPort::k_readBufferSize> const &buffer, std::size_t bytesTransferred) {
  std::copy(buffer.begin(), buffer.begin() + bytesTransferred, std::back_inserter(globalInboundBuffer));
  if (!boost::algorithm::ends_with(globalInboundBuffer, "\n")) {
    return;
  }
  vector <string> v = explode("\n", globalInboundBuffer);
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
            command_publisher->write(&cmdInstance);
        } else if (!rsp.compare(0, xmlPrefix.size(), xmlPrefix)) {
            std::string value = rsp;
	    LOG_INFO << "Received XML via serial";
            if (first_message) {
	      first_message = false;
	      LOG_TRACE << "\tCAPABILITY XML:  " << value;

	      XMLDocument doc(false);
	      doc.Parse(value.c_str());
	      tinyxml2::XMLNode *root = doc.FirstChildElement("AMMModuleConfiguration");
	      tinyxml2::XMLNode *mod = root->FirstChildElement("module");
	      
	      tinyxml2::XMLElement *module = mod->ToElement();
	      std::string module_name = module->Attribute("name");
	      std::string manufacturer = module->Attribute("manufacturer");
	      std::string model = module->Attribute("model");
	      std::string serial_number = module->Attribute("serial_number");
	      std::string module_version = module->Attribute("module_version");
	      
	      mgr->PublishModuleConfiguration(
					      mgr->module_id,
					      module_name,
					      manufacturer,
					      model,
					      serial_number,
					      module_version,
					      value
					      );
	      
	      subscribedTopics.clear();
	      publishedTopics.clear();
	      tinyxml2::XMLNode *caps = mod->FirstChildElement("capabilities");
	      
	      if (caps) {
		for (tinyxml2::XMLNode *node = caps->FirstChildElement("capability"); node; node = node->NextSibling()) {
		  tinyxml2::XMLElement *cap = node->ToElement();
		  std::string capabilityName = cap->Attribute("name");
		  
		  // Store subscribed topics for this capability
		  tinyxml2::XMLNode *subs = node->FirstChildElement("subscribed_topics");
		  if (subs) {
		    for (tinyxml2::XMLNode *sub = subs->FirstChildElement("topic"); sub; sub = sub->NextSibling()) {
		      tinyxml2::XMLElement *s = sub->ToElement();
		      std::string subTopicName = s->Attribute("name");
		      
		      if (s->Attribute("nodepath")) {
			subTopicName = s->Attribute("nodepath");
		      }
		      add_once(subscribedTopics,subTopicName);
		      LOG_TRACE << "[" << capabilityName << "] Subscribed to " << subTopicName;
		    }
		  }
		  
		  // Store published topics for this capability
		  tinyxml2::XMLNode *pubs = node->FirstChildElement("published_topics");
		  if (pubs) {
		    for (tinyxml2::XMLNode *pub = pubs->FirstChildElement("topic"); pub; pub = pub->NextSibling()) {
		      tinyxml2::XMLElement *p = pub->ToElement();
		      std::string pubTopicName = p->Attribute("name");
		      add_once(publishedTopics,pubTopicName);
		      LOG_TRACE << "[" << capabilityName << "] Publishing to " << pubTopicName;
		    }
		  }
		}
	      }
            } else {
	      LOG_TRACE << "\tSTATUS XML: " << value;
	      XMLDocument doc(false);
	      doc.Parse(value.c_str());
	      tinyxml2::XMLNode *root = doc.FirstChildElement("AMMModuleStatus");
	      tinyxml2::XMLElement *module = root->FirstChildElement("module")->ToElement();
	      const char *name = module->Attribute("name");
	      std::string nodeName(name);
	      
	      tinyxml2::XMLElement *caps = module->FirstChildElement("capabilities");
	      if (caps) {
		for (tinyxml2::XMLNode *node = caps->FirstChildElement("capability"); node; node = node->NextSibling()) {
		  tinyxml2::XMLElement *cap = node->ToElement();
		  std::string capabilityName = cap->Attribute("name");
		  std::string statusVal = cap->Attribute("status");
		  
		  if (statusVal == "OPERATIONAL") {
		    mgr->SetStatus(mgr->module_id, nodeName, capabilityName, OPERATIONAL);
		  } else if (statusVal == "HALTING_ERROR") {
		    std::string errorMessage = cap->Attribute("message");
		    std::vector<std::string> errorMessages = { errorMessage };
		    mgr->SetStatus(mgr->module_id, nodeName, capabilityName, HALTING_ERROR, errorMessages);
		  } else if (statusVal == "IMPENDING_ERROR") {
		    std::string errorMessage = cap->Attribute("message");
		    std::vector<std::string> errorMessages = { errorMessage };
		    mgr->SetStatus(mgr->module_id, nodeName, capabilityName, IMPENDING_ERROR, errorMessages);
		  } else {
		    LOG_ERROR << "Invalid status value " << statusVal << " for capability " << capabilityName;
		  }
		}
	      }
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
  void onNewNodeData(AMM::Physiology::Node n) override {
    if (n.nodepath() == "EXIT") {
      LOG_INFO << "Shutting down simulation based on shutdown node-data from physiology engine.";
      closed = true;
      return;
    }
    
    // Publish values that are supposed to go out on every change
    if (std::find(subscribedTopics.begin(), subscribedTopics.end(), n.nodepath()) != subscribedTopics.end()) {
      std::ostringstream messageOut;
      messageOut << "[AMM_Node_Data]" << n.nodepath() << "=" << n.dbl();
      transmitQ.push(messageOut.str());
    }
  }
  
  void onNewCommandData(AMM::PatientAction::BioGears::Command c) override {
    LOG_TRACE << "Command received from AMM: " << c.message();
    if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
      std::string value = c.message().substr(sysPrefix.size());
      if (value.compare("START_SIM") == 0) {
	
      } else if (value.compare("STOP_SIM") == 0) {
	
      } else if (value.compare("PAUSE_SIM") == 0) {
	
      } else if (value.compare("RESET_SIM") == 0) {
	
      } else if (!value.compare(0, loadScenarioPrefix.size(), loadScenarioPrefix)) {
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
    cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n" << "\t-h,--help\t\tShow this help message\n" << endl;
}

int main(int argc, char *argv[]) {
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
  auto *pub_listener = new DDS_Listeners::PubListener();
  
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
  boost::thread t(boost::bind(&boost::asio::io_service::run, &io));
  
  if (serialPort.Initialize()) {
    LOG_ERROR << "Initialization failed!";
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
  
  mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);
  
  LOG_INFO << "Serial_Bridge ready";
  
  while (!closed) {
    while (!transmitQ.empty()) {
      serialPort.Write(transmitQ.front());
      transmitQ.pop();
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    cout.flush();
  }
  
  LOG_INFO << "Serial_Bridge simulation stopped.";
  return 0;
}

