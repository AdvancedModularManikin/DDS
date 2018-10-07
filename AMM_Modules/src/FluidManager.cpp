

#include <algorithm>

#include <boost/algorithm/string.hpp>

#include "AMM/DDS_Manager.h"

#include "tinyxml2.h"

extern "C" {
#include "spi_proto.h"
#include "binary_semaphore.h"
#include "spi_remote.h"
#include "spi_remote_host.h"
}
#include "spi_proto_master.h"


using namespace std;
using namespace std::literals::string_literals;
using namespace tinyxml2;
using namespace AMM;

// Daemonize by default
int daemonize = 1;

const string loadScenarioPrefix = "LOAD_SCENARIO:";
const string haltingString = "HALTING_ERROR";

float operating_pressure = 15.0;
float purge_pressure = 15.0;
bool have_pressure = 0;
bool send_status = false;
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
    void onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t *info) override {
        // Will receive the above xml - need to pass it via SPI and set status as shown below

    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) override {
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
            mgr->module_id,
            nodeString,
            "Entropic",
            "fluid_manager",
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/fluid_manager_capabilities.xml")
    );

    bool closed = 0;
    while (!closed) {
        if (send_status) {
            cout << "[FluidManager] Setting status to " << current_status << endl;
            send_status = false;
            mgr->SetStatus(mgr->module_id, nodeString, current_status);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
  int solenoid_A = solenoid_0 + 6;
  int solenoid_B = solenoid_0 + 7;
  int solenoid_C = solenoid_0 + 5;
  int solenoid_AC = solenoid_0 + 0;
  int solenoid_AD = solenoid_0 + 1;
  remote_set_gpio(solenoid_B, 1); // TODO turn off to vent, another control output
  remote_set_gpio(solenoid_A, 0); //solenoid A TODO to purge lines A off B on
  remote_set_gpio(solenoid_C, 0);
  int motor_enable = 16;//B1
  remote_set_gpio(motor_enable, 1);
  //in order to purge: Turn B off, Turn A on, Turn AC & AD on
  //P1 pressure will slowly drop to atmospheric
  //p4 pressure should stay above 1 bar until the lines are clear of liquid
  //p1, p2 & p3 should remain close to each other until the reservoirs are empty
  //when purging control loop should work off of Pressure4, but pressure1 otherwise
#if 0
  //temp. purge code. leave control loop where it is, need air to purge
  remote_set_gpio(solenoid_AC, 1);
  //remote_set_gpio(solenoid_AD, 1);
  remote_set_gpio(solenoid_B, 0);
  remote_set_gpio(solenoid_A, 1);
#endif
  //adcs
  int P1 = 0, P2 = 1, P3 = 2, P4 = 3;

  pid.p = 24;
  pid.i = 1.0/1024;
  pid.d = 1.0/16;
  pid.isum = 0;

  uint16_t dacVal;
  int rail_24V = 15;
  remote_set_gpio(rail_24V, 1); //should_24v_be_on = 1;
  bool should_motor_run = 1;

  state_startup:
  remote_set_gpio(rail_24V, 1);
  remote_set_gpio(motor_enable, 1);
  remote_set_gpio(solenoid_B, 1);
  remote_set_gpio(solenoid_A, 0);
  remote_set_gpio(solenoid_C, 0);
  int not_pressurized = 1;
  puts("entering startup!");

  //pressurize, when done goto enter_state_operational;
  //TODO need to determine if pressure is really completed.
  while(not_pressurized) {
    pid.target = operating_pressure;
    float hold_isum = pid.isum;
    uint32_t adcRead = remote_get_adc(0);
    float psi = ((float)adcRead)*(3.0/10280.0*16.0) - 15.0/8.0;
    ret = pi_supply(&pid, psi);

    //convert back to 0-2^12 range for DAC
    val = (uint32_t) (ret*1000.0);
    should_motor_run = stall_val < val;
    if (!should_motor_run) {
      pid.isum = hold_isum;
    }
    dacVal = val > 0xfff ? 0xfff : val;
    remote_set_dac(motor_dac, dacVal);

    //float psiP2 = ((float)remote_get_adc(P2))*(3.0/10280.0*16.0) - 15.0/8.0;
    //float psiP3 = ((float)remote_get_adc(P3))*(3.0/10280.0*16.0) - 15.0/8.0;
    float psiP4 = ((float)remote_get_adc(P4))*(3.0/10280.0*16.0) - 15.0/8.0;
    if(psiP4 > 4.9) {
        puts("pressurization complete!");
        current_status = OPERATIONAL;
        send_status = true;
        goto state_operational;
    }
  }

  state_operational:
  puts("entering operational state!");
  remote_set_gpio(rail_24V, 1);
  remote_set_gpio(motor_enable, 1);
  remote_set_gpio(solenoid_B, 1);
  remote_set_gpio(solenoid_A, 0);
  remote_set_gpio(solenoid_C, 0);

  int stay_operational = 1; //TODO change in response to DDS commands
  while (stay_operational) {
    pid.target = operating_pressure;
    float hold_isum = pid.isum;
    uint32_t adcRead = remote_get_adc(0);
    float psi = ((float)adcRead)*(3.0/10280.0*16.0) - 15.0/8.0;
    ret = pi_supply(&pid, psi);

    //convert back to 0-2^12 range for DAC
    val = (uint32_t) (ret*1000.0);
    should_motor_run = stall_val < val;
    if (!should_motor_run) {
      pid.isum = hold_isum;
    }
    dacVal = val > 0xfff ? 0xfff : val;
    remote_set_dac(motor_dac, dacVal);
    //TODO this thread waits on other threads in remote_ calls so it does not actually need this delay here
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    //TODO no predicate for leaving this, but leave in response to a message.
    //TODO also leave after 20s for testing purposes
    //goto state_purge;
    printf("pressurizing psi to %f\n", psi);
  }

  state_purge:
  //TODO purge does not quite complete, issues detecting if the lines are clear
  //the five lines following purge both (although having both open causes an issue similar to blowing your nose)
  remote_set_gpio(solenoid_C, 0);
  remote_set_gpio(solenoid_A, 1);
  remote_set_gpio(solenoid_B, 1);
  //remote_set_gpio(solenoid_AD, 1);
  //remote_set_gpio(solenoid_AC, 1);
  //std::this_thread::sleep_for(std::chrono::milliseconds(60*1000));

  //control loop off of P4, for AC then AD, a purge is done when P1 hits 0.01psi above atmo
  const int purge_states = 2;
  for (int purge_ix = 0; purge_ix < purge_states; purge_ix++) {
    switch(purge_ix) {
    case 0:
      remote_set_gpio(solenoid_AC, 1);
      remote_set_gpio(solenoid_AD, 0);
      puts("Purging AC!");
      break;
    case 1:
      remote_set_gpio(solenoid_AC, 0);
      remote_set_gpio(solenoid_AD, 1);
      puts("Purging AD!");
      break;
    default:
      printf("unhandled case in purge_ix: %d\n", purge_ix);
    }
    bool purge_not_complete = 1;
    while (purge_not_complete) {
      pid.target = purge_pressure;
      float hold_isum = pid.isum;
      uint32_t adcRead = remote_get_adc(P3);
      float psi = ((float)adcRead)*(3.0/10280.0*16.0) - 15.0/8.0;
      ret = pi_supply(&pid, psi);

      //convert back to 0-2^12 range for DAC
      val = (uint32_t) (ret*1000.0);
      should_motor_run = stall_val < val;
      if (!should_motor_run) {
        pid.isum = hold_isum;
      }
      dacVal = val > 0xfff  ? 0xfff : val;
      remote_set_dac(motor_dac, dacVal);

      int adcP1 = remote_get_adc(P1);
      int adcP2 = remote_get_adc(P2);
      int adcP3 = remote_get_adc(P3);
      int adcP4 = remote_get_adc(P4);
      float psi1 = ((float)adcP1)*(3.0/10280.0*16.0) - 15.0/8.0;
      float psi2 = ((float)adcP2)*(3.0/10280.0*16.0) - 15.0/8.0;
      float psi3 = ((float)adcP3)*(3.0/10280.0*16.0) - 15.0/8.0;
      float psi4 = ((float)adcP4)*(3.0/10280.0*16.0) - 15.0/8.0;
      printf("psi1: %f\tpsi2: %f\tpsi3: %f\tpsi4: %f\n", psi1, psi2, psi3, psi4);
      if (purge_ix == 0) {
        purge_not_complete = psi2 > 0.22;
      } else {
        purge_not_complete = psi1 > 0.22;
      }
    }
  }
  remote_set_gpio(solenoid_AC, 0);
  remote_set_gpio(solenoid_AD, 0);
  goto state_error;

  state_error:
  //TODO turn off motor, close all solenoids, turn off 24V rail
  remote_set_gpio(motor_enable, 0);
  remote_set_gpio(rail_24V, 0);
  for (int i = 0; i < SOLENOID_NUM;i++) remote_set_gpio(solenoid_0+i, 0);
  current_status = HALTING_ERROR;
  send_status = true;

  while(1) {
    puts("in error state!");
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  }
}
