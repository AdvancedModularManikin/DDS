

#include <algorithm>

#include <boost/algorithm/string.hpp>

#include "AMM/DDS_Manager.h"

extern "C" {
#include "spi_proto.h"
#include "binary_semaphore.h"
#include "spi_remote.h"
#include "spi_remote_host.h"
}
#include "spi_proto_master.h"

#include "tinyxml2.h"

using namespace std;
using namespace std::literals::string_literals;
using namespace tinyxml2;
using namespace AMM;
using namespace AMM::Capability;

// Daemonize by default
int daemonize = 1;

const string loadScenarioPrefix = "LOAD_SCENARIO:";
const string haltingString = "HALTING_ERROR";

//variables that are sent to the tiny
float operating_pressure = 0.1;
//variables that are received from the tiny
bool pressurized = false;
float total_flow = 0, last_flow_change = 0;

//TODO move these to config - variables that define peripherals on the tiny or are needed for remote
int vein_sol_gpio = 0;//TODO check which one it actually is
void send_chunk(void *, int);

#define IVC_STATUS_WAITING  0
#define IVC_STATUS_START    1
#define IVC_STATUS_PAUSE    2
#define IVC_STATUS_STOP     3
#define IVC_STATUS_RESET    4
unsigned char ivc_status = IVC_STATUS_WAITING;

bool have_pressure = 0;
bool send_status = false;
AMM::Capability::status_values current_status;

static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 1 << 23;
static uint16_t delay;

Publisher *command_publisher;
Publisher *node_publisher;

/*
 * the task has two modes: waiting and running.
 * START moves it to running from whereever.
 * PAUSE and STOP and WAITING move it to waiting
 * RESET moves it to waiting and resets the total_counts flow variable.
 */

/* IVC module has two tasks:
 * maintain pressure in vein at ~0.1 psi -- careful not to pop it
 * monitor amount of flow and send it to biogears
 */
float bleed_pressure = 0.125;
float vein_psi;
volatile int pressurization_quantum = 20; // ms

//TODO old send_ivc_spi put a pressurization into the message but it was never read. Presumably it is supposed to be used? Note that it doesn't need to be sent to the tiny anymore.
//TODO atomic or lock or something? this is modified from multiple threads
bool ivc_waiting = 1;

void
bleed_task(void);

//TODO rename
void send_ivc_spi(unsigned char status) {
    //TODO use operating_pressure to constrain control loop
    //memcpy(spi_send + 4, &operating_pressure, 4);

    //no need to actually send anymore
    switch (status) {
        case IVC_STATUS_START:
            //start task, should resume after a pause
            ivc_waiting = 0;
            break;
        case IVC_STATUS_RESET:
            //also reset flow
            //total_pulses = 0; // TODO send message over spi to reset this
            //TODO add flow sensors as a category
        case IVC_STATUS_PAUSE:
        case IVC_STATUS_STOP:
        case IVC_STATUS_WAITING:
        default:
            //stop pressurizing but do not reset flow
            ivc_waiting = 1;
            break;
    }
}

int frame = 0;

void PublishNodeData(std::string node, float dbl) {
    AMM::Physiology::Node dataInstance;
    dataInstance.nodepath(node);
    dataInstance.dbl(dbl);
    dataInstance.frame(frame);
    node_publisher->write(&dataInstance);
    frame++;
}

void ProcessConfig(const std::string configContent) {

    tinyxml2::XMLDocument doc;
    doc.Parse(configContent.c_str());

    tinyxml2::XMLHandle docHandle(&doc);

    tinyxml2::XMLElement *entry = docHandle.FirstChildElement("AMMConfiguration").ToElement();

    tinyxml2::XMLElement *entry2 = entry->FirstChildElement("scenario")->ToElement();

    tinyxml2::XMLElement *entry3 = entry2->FirstChildElement("capabilities")->ToElement();

    tinyxml2::XMLElement *entry4 = entry3->FirstChildElement("capability")->ToElement();


    //scan for capability name=ivc
    while (entry4) {
        if (!strcmp(entry4->ToElement()->Attribute("name"), "ivc")) break;

        auto v = entry4->ToElement()->NextSibling();
        if (v) {
            entry4 = v->ToElement();
        } else break;
    }
    if (!entry4) {
        perror("[IVC] cfg data didn't contain <capability name=ivc>");
        return;
    }

    //scan for data name=operating pressure
    tinyxml2::XMLElement *entry5 = entry4->FirstChildElement("configuration_data")->ToElement();

    while (entry5) {
        tinyxml2::XMLElement *entry5_1 = entry5->FirstChildElement("data")->ToElement();
        if (!strcmp(entry5_1->ToElement()->Attribute("name"), "operating_pressure")) {
            operating_pressure = entry5_1->ToElement()->FloatAttribute("value");
            have_pressure = 1;
//            send_ivc_spi(ivc_status);
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


class IVCListener : public ListenerInterface {

    void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) override {
        // We received configuration which we need to push via SPI
        if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = c.message().substr(sysPrefix.size());
            cout << " \tWe got a command: " << value << endl;
            if (value.compare("START_SIM") == 0) {
                ivc_status = IVC_STATUS_START;
                ivc_waiting = 0;
            } else if (value.compare("STOP_SIM") == 0) {
                ivc_status = IVC_STATUS_STOP;
            } else if (value.compare("PAUSE_SIM") == 0) {
                ivc_status = IVC_STATUS_PAUSE;
            } else if (value.compare("RESET_SIM") == 0) {
                frame = 0;
                ivc_status = IVC_STATUS_RESET;
            } else if (!value.compare(0, loadScenarioPrefix.size(), loadScenarioPrefix)) {
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
//                send_status = true;
                current_status = OPERATIONAL;
            }
        }
    }
};


static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n" << endl;
}

int main(int argc, char *argv[]) {
    host_remote_init(&remote);
    std::thread remote_thread(remote_task);
    std::thread bleed_thread(bleed_task);

    cout << "=== [IVC_Module] Ready ..." << endl;
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

    const char *nodeName = "AMM_IVC";
    std::string nodeString(nodeName);
    auto *mgr = new DDS_Manager(nodeName);

    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();

    IVCListener ivcl;
    command_sub_listener->SetUpstream(&ivcl);
    config_sub_listener->SetUpstream(&ivcl);

    auto *pub_listener = new DDS_Listeners::PubListener();

    mgr->InitializeReliableSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), command_sub_listener);
    mgr->InitializeReliableSubscriber(AMM::DataTypes::configurationTopic, AMM::DataTypes::getConfigurationType(),
                                      config_sub_listener);

    Publisher *command_publisher = mgr->InitializeReliablePublisher(AMM::DataTypes::commandTopic,
                                                         AMM::DataTypes::getCommandType(), pub_listener);

    Publisher *node_publisher = mgr->InitializeReliablePublisher(AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(),
                                                      pub_listener);

    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration

    mgr->PublishModuleConfiguration(
            mgr->module_id,
            nodeString,
            "CREST",
            "ivc_module",
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/ivc_module_capabilities.xml")
    );

    while (1) {
        if (send_status) {
            cout << "[IVC] Setting status to " << current_status << endl;
            send_status = false;
            mgr->SetStatus(mgr->module_id, nodeString, current_status);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    cout << "=== [IVC] Simulation stopped." << endl;

    return 0;

}

//functions implemented for port
void
delay_ms(unsigned int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void
bleed_task(void) {
    uint8_t vein_sol = 7; //struct solenoid::solenoid &vein_sol = solenoids[7];

    //enable 24V rail
    int rail_24V = 15;
    remote_set_gpio(rail_24V, 1);


    //module logic:
    //wait for start message
    //begin pressurizing
    //when pressurized, stop pressurizing and send the "I'm sealed" message to SoM code
    for (;;) {
        //wait for start message
        //TODO use semaphore here?
        while (ivc_waiting)
            delay_ms(100);


        uint32_t adcRead = remote_get_adc(3); //uint32_t adcRead = carrier_sensors[0].raw_pressure;
        vein_psi = ((float) adcRead) * (3.0 / 10280.0 * 16.0) - 15.0 / 8.0;
        if (vein_psi < bleed_pressure) {
            remote_set_gpio(vein_sol_gpio, 1); // TODO this is a solenoid, open might be 0?
            do {
                //this is to support pausing.
                if (ivc_waiting) {
                    remote_set_gpio(vein_sol_gpio, 0);
                    //TODO use semaphore here?
                    while (ivc_waiting) delay_ms(50); //this is triggered elsewhere and not by weird k66 stuff
                    remote_set_gpio(vein_sol_gpio, 1);
                }
                delay_ms(pressurization_quantum); //TODO modify to account for comms delay?
                adcRead = remote_get_adc(0); //adcRead = carrier_sensors[0].raw_pressure;
                vein_psi = ((float) adcRead) * (3.0 / 10280.0 * 16.0) - 15.0 / 8.0;
            } while (vein_psi < bleed_pressure);
            remote_set_gpio(vein_sol_gpio, 0);
        }

        pressurized = 1;
        ivc_waiting = 1;
        delay_ms(50);
    }
}

/*
TODO flow processing
{
  float total_flow_new;
  memcpy(&total_flow_new, &pack.msg[4], 4);
  last_flow_change = total_flow_new - total_flow;
  total_flow = total_flow_new;
  PublishNodeData("IVC_TOTAL_BLOOD_LOSS", total_flow);
}
*/
