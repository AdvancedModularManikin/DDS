#include "stdafx.h"

#include <string>
#include <iostream>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include <thirdparty/tixml2cx.h>

#include "AMM/DDS_Manager.h"
extern "C" {
#include "spi_proto/spi_proto.h"
#include "spi_proto/binary_semaphore.h"
#include "spi_proto/spi_remote.h"
#include "spi_proto/spi_remote_host.h"
}
#include "spi_proto/spi_proto_master.h"


using namespace std;
using namespace std::literals::string_literals;

// Daemonize by default
int daemonize = 1;

const string loadScenarioPrefix = "LOAD_SCENARIO:";
const string haltingString = "HALTING_ERROR";

float operating_pressure = 5.0;
bool have_pressure = 0;
AMM::Capability::status_values current_status;

void ProcessConfig(const std::string configContent) {

    tinyxml2::XMLDocument doc;
    doc.Parse(configContent.c_str());

    tinyxml2::XMLHandle docHandle(&doc);

    tinyxml2::XMLElement *entry = docHandle.FirstChildElement("AMMConfiguration").ToElement();

    tinyxml2::XMLElement *entry2 = entry->FirstChildElement("scenario")->ToElement();

    tinyxml2::XMLElement *entry3 = entry2->FirstChildElement("capabilities")->ToElement();

    tinyxml2::XMLElement *entry4 = entry3->FirstChildElement("capability")->ToElement();


    //scan for capability name=fluidics
    while (entry4) {
        if (!strcmp(entry4->ToElement()->Attribute("name"), "fluidics")) break;

        auto v = entry4->ToElement()->NextSibling();
        if (v) {
            entry4 = v->ToElement();
        } else break;
    }
    if (!entry4) {
        perror("[FLUIDMGR] cfg data didn't contain <capability name=fluidics>");
        return;
    }

    //scan for data name=operating pressure
    tinyxml2::XMLElement *entry5 = entry4->FirstChildElement("configuration_data")->ToElement();

    while (entry5) {
        tinyxml2::XMLElement *entry5_1 = entry5->FirstChildElement("data")->ToElement();
        if (!strcmp(entry5_1->ToElement()->Attribute("name"), "operating_pressure")) {
            operating_pressure = entry5_1->ToElement()->FloatAttribute("value");
            have_pressure = 1;
            //TODO used to send the pressure as a message here. Ensure it's getting where it needs to go in the local state
            break;
        }
        auto v = entry5->ToElement()->NextSibling();
        if (v) {
            entry5 = v->ToElement();
        } else break;
    }

    if (!entry5) {
        perror("[FLUIDMGR] cfg data didn't contain <data name=operating_pressure>");
    }

}


class FluidListener : public ListenerInterface {
    void onNewConfigData(AMM::Capability::Configuration cfg) override {
        // Will receive the above xml - need to pass it via SPI and set status as shown below

    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c) override {
        // We received configuration which we need to push via SPI
        if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = c.message().substr(sysPrefix.size());
            if (!value.compare(0, loadScenarioPrefix.size(), loadScenarioPrefix)) {
                std::string scene = value.substr(loadScenarioPrefix.size());
                boost::algorithm::to_lower(scene);
                ostringstream static_filename;
                static_filename << "mule1/module_configuration_static/" << scene << "_fluid_manager.xml";
                std::ifstream ifs(static_filename.str());
                std::string configContent((std::istreambuf_iterator<char>(ifs)),
                                          (std::istreambuf_iterator<char>()));

                ifs.close();

                ProcessConfig(configContent);

                // These should be sent when a status change is received via spi.
                // We'll force them for now.
                //TODO confirm nothing else needs to happen //send_status = true;
                current_status = OPERATIONAL;
            }
        }
    }
};


static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n" << endl;
}
void air_reservoir_control_task(void);

int main(int argc, char *argv[]) {
    host_remote_init(&remote);
    std::thread remote_thread(remote_task);
    std::thread air_tank_thread(air_reservoir_control_task);
    cout << "=== [FluidManager] Ready ..." << endl;
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


    const char *nodeName = "AMM_FluidManager";
    std::string nodeString(nodeName);
    auto *mgr = new DDS_Manager(nodeName);

    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();

    FluidListener fl;
    command_sub_listener->SetUpstream(&fl);
    config_sub_listener->SetUpstream(&fl);

    auto *pub_listener = new DDS_Listeners::PubListener();

    mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), command_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::configurationTopic, AMM::DataTypes::getConfigurationType(),
                              config_sub_listener);

    Publisher *command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic,
                                                            AMM::DataTypes::getCommandType(), pub_listener);


    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration

    mgr->PublishModuleConfiguration(
            nodeString,
            "Entropic",
            "fluid_manager",
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/fluid_manager_capabilities.xml")
    );

    int count = 0;
    bool closed = 0;
    while (!closed) {

        //TODO move status somewhere else
#if 0
        if (send_status) {
            cout << "[FluidManager] Setting status to " << current_status << endl;
            send_status = false;
            mgr->SetStatus(nodeString, current_status);
        }
#endif

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        ++count;
    }
    remote_thread.join();
    air_tank_thread.join();
    cout << "=== [FluidManager] Simulation stopped." << endl;

    return 0;

}

//code from air_tank.cpp initially copied here
struct pid_ctl {
  float p;
  float i;
  float d;
  float target;

  float isum; // current value
  float last;
  float last_diff;
};

float
pi_supply(struct pid_ctl *p, float reading)
{
  float diff = reading - p->last;
  p->last = reading;
  p->last_diff = diff;
  float oset = p->target - reading;

  p->isum += oset * p->i;

  return p->isum + p->p*oset + p->d*diff;
}

struct pid_ctl pid;

uint32_t stall_val = 0x100;
//PSI (atmospheric is 0)
//float operating_pressure = 5.0;

bool should_pid_run = true;
float ret;
uint32_t val;
void
air_reservoir_control_task(void)
{
  int solenoid_0 = 7, motor_dac = 0;
  remote_set_gpio(solenoid_0 + 0, 0); //solenoid::off(solenoids[0]);
  remote_set_gpio(solenoid_0 + 1, 1); //solenoid::on(solenoids[1]);
  int motor_enable = 16;//GPIO_SetPinsOutput(GPIOB, 1U<<1U);
  remote_set_gpio(motor_enable, 1);

  pid.p = 24;
  pid.i = 1.0/1024;
  pid.d = 1.0/16;
  pid.isum = 0;

  uint16_t dacVal;
  int rail_24V = 15; // TODO confirm
  remote_set_gpio(rail_24V, 1); //should_24v_be_on = 1;
  bool should_motor_run = 1;

  for (;;) {
    pid.target = operating_pressure;
    if (should_pid_run) {
      //don't update if motor isn't running as it will run too far off
      //TODO also don't update if solenoid 2 is open
      float hold_isum = pid.isum;
      uint32_t adcRead = remote_get_adc(0); //uint32_t adcRead = carrier_sensors[0].raw_pressure;
      float psi = ((float)adcRead)*(3.0/10280.0*16.0) - 15.0/8.0;
      printf("adc: %d\t\t psi: %f\n", adcRead, psi);

      ret = pi_supply(&pid, psi);

      //convert back to 0-2^12 range for DAC
      val = (uint32_t) (ret*1000.0);
      should_motor_run = stall_val < val;
      if (!should_motor_run) {
        pid.isum = hold_isum;
      }
      dacVal = val > 0xfff ? 0xfff : val;
      remote_set_dac(motor_dac, dacVal);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}
