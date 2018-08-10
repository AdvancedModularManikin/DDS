#include "stdafx.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <chrono>

#include <boost/algorithm/string.hpp>

#include <thirdparty/tixml2cx.h>

#include "AMM/DDS_Manager.h"

using namespace std;
using namespace std::literals::string_literals;

class SyringeListener;

// Daemonize by default
int daemonize = 1;


bool have_pressure = 0;
bool send_status = false;
AMM::Capability::status_values current_status;

Publisher* command_publisher;
Publisher* node_publisher;



int frame = 0;

void PublishNodeData(std::string node, float dbl) {
  AMM::Physiology::Node dataInstance;
  dataInstance.nodepath(node);
  dataInstance.dbl(dbl);
  dataInstance.frame(frame);
  node_publisher->write(&dataInstance);
  frame++;
}



class SyringeListener : public ListenerInterface {
  
  void onNewCommandData(AMM::PatientAction::BioGears::Command c) override {
    // We received configuration which we need to push via SPI
    if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
      std::string value = c.message().substr(sysPrefix.size());
      cout << " \tWe got a command: " << value << endl;
      if (value.compare("START_SIM") == 0) {
      } else if (value.compare("STOP_SIM") == 0) {
      } else if (value.compare("PAUSE_SIM") == 0) {
      } else if (value.compare("RESET_SIM") == 0) {
        frame = 0;
      } /*else if (!value.compare(0, loadScenarioPrefix.size(), loadScenarioPrefix)) {
        std::string scene = value.substr(loadScenarioPrefix.size());
        boost::algorithm::to_lower(scene);
        ostringstream static_filename;
        static_filename << "mule1/module_configuration_static/" << scene << "_ivc_module.xml";
        std::ifstream ifs(static_filename.str());
        std::string configContent((std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>()));
        
        ifs.close();
        
        ProcessConfig(configContent);
        
        // These should be sent when a status change is received via spi.
        // We'll force them for now.
        send_status = true;
        current_status = OPERATIONAL;
      }*/
    }
  }
};


static void show_usage(const std::string &name) {
  cerr << "Usage: " << name << "\nOptions:\n"
       << "\t-h,--help\t\tShow this help message\n" << endl;
}

int main(int argc, char *argv[]) {
  cout << "=== [SyringeMock] Ready ..." << endl;
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
  
  //TODO change nodename
  const char *nodeName = "EE_Syringe";
  std::string nodeString(nodeName);
  auto *mgr = new DDS_Manager(nodeName);
  
  auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
  auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();
  
  SyringeListener ivcl;
  command_sub_listener->SetUpstream(&ivcl);
  config_sub_listener->SetUpstream(&ivcl);
  
  auto *pub_listener = new DDS_Listeners::PubListener();
  
  mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), command_sub_listener);
  mgr->InitializeSubscriber(AMM::DataTypes::configurationTopic, AMM::DataTypes::getConfigurationType(),
    config_sub_listener);
  
  command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic,
    AMM::DataTypes::getCommandType(), pub_listener);
  
  node_publisher = mgr->InitializePublisher(AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(), pub_listener);
  
  // Publish module configuration once we've set all our publishers and listeners
  // This announces that we're available for configuration
  
  mgr->PublishModuleConfiguration(
          nodeString,
          "EntropicEngineering",
          "Syringe", // mock must pretend to be a syringe still
          "00001",
          "0.0.1",
          mgr->GetCapabilitiesAsString("mule1/module_capabilities/syringe_mock_module_capabilities.xml")
          );
  
  //TODO mock syringe should send amounts over random periods (up to a minute total duration) totalling at some set volume
  //TODO replace this with syringe stuff and a random timeout
  float total_dur_s = 30.0;
  float total_amount = 100;//mL
  float delays[5] = {0.2*total_dur_s, 0.2*total_dur_s, 0.2*total_dur_s, 0.2*total_dur_s, 0.2*total_dur_s};
  
  for (int i = 0; i < 5; i++) {
    //std::this_thread::sleep_for(std::chrono::milliseconds(delays[i]));
    std::this_thread::sleep_for(std::chrono::milliseconds(6000));
    PublishNodeData("Syringe_InjectedAmount", total_amount/5);
  }
  
  cout << "=== [SyringeMock] Simulation stopped." << endl;
  
  return 0;
  
}

