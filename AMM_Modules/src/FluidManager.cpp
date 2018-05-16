#include "stdafx.h"

#include "AMM/DDS_Manager.h"

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <fcntl.h>    /* For O_RDWR */

using namespace std;

// Daemonize by default
int daemonize = 1;


const string loadScenarioPrefix = "LOAD_SCENARIO:";
const string haltingString = "HALTING_ERROR";

const string tourniquet_action = "PROPER_TOURNIQUET";
const string hemorrhage_action = "LEG_HEMORRHAGE";
float heartrate = 40.0;
float breathrate = 12.0;
bool tourniquet = false;
bool hemorrhage = false;
bool closed = false;

static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 1 << 23;
static uint16_t delay;

int spi_transfer(int fd, const unsigned char *tx_buf, const unsigned char *rx_buf, __u32 buflen) {
    int ret;
    struct spi_ioc_transfer tr = {tx_buf : (unsigned long) tx_buf, rx_buf : (unsigned long) rx_buf, len : buflen, speed_hz : speed,
            delay_usecs : delay, bits_per_word : bits,};

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        perror("can't send spi message");
    return ret;
}


class FluidListener : public ListenerInterface {
    void onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t *info) override {
        // rip out the capabilities string and send it on to the TCP client
        /**
          <?xml version="1.0" encoding="utf-8"?>
<AMMConfiguration>
  <scenario id="m1s2" name="Mule 1 / Scene 2 / ER">
    <versions>
      <data name="scenario" value="1.0.0" />
      <data name="amm_core" value="0.0.1" />
      <data name="amm_specification" value="0.0.1" />
    </versions>
    <capabilities>
      <capability name="fluidics">
        <configuration_data>
          <data name="operating_pressure" value="5.0" />
        </configuration_data>
      </capability>
    </capabilities>
  </scenario>
</AMMConfiguration>
         */

        // Will receive the above xml - need to pass it via SPI and set status as shown below
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) override {
        if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = c.message().substr(sysPrefix.size());
            if (!value.compare(0, loadScenarioPrefix.size(), loadScenarioPrefix)) {
                std::string scene = value.substr(loadScenarioPrefix.size());
                ostringstream static_filename;
                static_filename << "mule1/module_configuration_static/" << scene << "_fluid_manager.xml";
                std::ifstream ifs(static_filename.str());
                std::string configContent((std::istreambuf_iterator<char>(ifs)),
                                          (std::istreambuf_iterator<char>()));
                ifs.close();
                /**
                 <configuration_data>
                    <data name="operating_pressure" value="5.0" />
                </configuration_data>
                 */
                // send the config content downstream via SPI?
                // or just send little pieces
            }
        }
    }
};


static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n" << endl;
}

int main(int argc, char *argv[]) {
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

    int spi_fd = open(device, O_RDWR);

    const char* nodeName = "AMM_FluidManager";
    std::string nodeString(nodeName);
    auto *mgr = new DDS_Manager(nodeName);
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();
    FluidListener fl;
    command_sub_listener->SetUpstream(&fl);
    config_sub_listener->SetUpstream(&fl);

    auto *pub_listener = new DDS_Listeners::PubListener();

    mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), command_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::configurationTopic, AMM::DataTypes::getConfigurationType(), config_sub_listener);

    Publisher *command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), pub_listener);


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

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus( nodeString, OPERATIONAL);


    int count = 0;
    while (!closed) {

        unsigned char spi_send[4];
        spi_send[0] = heartrate;
        spi_send[1] = breathrate;
        spi_send[2] = static_cast<unsigned char>(tourniquet);
        spi_send[3] = static_cast<unsigned char>(hemorrhage);
        unsigned char spi_rcvd[4];

        //do SPI communication
        int spi_tr_res = spi_transfer(spi_fd, spi_send, spi_rcvd, 4);

        //std::cout << "spi_msg " << std::hex << std::setw(2)
        //	<< std::setfill('0') << (unsigned int) spi_msg << std::endl;
        //std::cout << "spi_rcvd " << std::hex << std::setw(2)
        //	<< std::setfill('0') << (unsigned int) spi_rcvd << std::endl;
        //send press messages based on received SPI
        //the buttons send 1 when they are up and 0 when they are pressed
        if (spi_rcvd[1] != 0u) {
            //button 2 was pressed
            //send hemorrhage action
            cout << "=== [FluidManager] Sending a command:" << hemorrhage_action << endl;
            AMM::PatientAction::BioGears::Command cmdInstance;
            cmdInstance.message(hemorrhage_action);
            command_publisher->write(&cmdInstance);
        }


        ++count;
    }

    cout << "=== [FluidManager] Simulation stopped." << endl;


    return 0;

}

