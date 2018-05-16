#include "stdafx.h"

#include "tinyxml2.h"

#include "AMM/DDS_Manager.h"
#include "spi_proto.h"

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <fcntl.h>    /* For O_RDWR */

using namespace std;

// Daemonize by default
int daemonize = 1;


const string loadScenarioPrefix = "LOAD_SCENARIO:";
const string haltingString = "HALTING_ERROR";

float operating_pressure;
bool have_pressure = 0;

static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 1 << 23;
static uint16_t delay;

//TODO centralize
#define TRANSFER_SIZE 36

int spi_transfer(int fd, const unsigned char *tx_buf, unsigned char *rx_buf, __u32 buflen) {
    int ret;
    struct spi_ioc_transfer tr = {
		tx_buf : (unsigned long) tx_buf,
		rx_buf : (unsigned long) rx_buf,
		len : TRANSFER_SIZE, speed_hz : speed,
		delay_usecs : delay, bits_per_word : bits,
		};

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        perror("can't send spi message");
    return ret;
}

struct spi_state spi_state;

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
		
		tinyxml2::XMLDocument doc;
	    doc.Parse(cfg.capabilities().c_str());
		tinyxml2::XMLHandle docHandle(&doc);
	
	    tinyxml2::XMLElement *entry = docHandle.FirstChildElement("AMMConfiguration").ToElement();
		tinyxml2::XMLElement *entry2 = entry->FirstChildElement("scenario")->ToElement();
		tinyxml2::XMLElement *entry3 = entry2->FirstChildElement("capabilities")->ToElement();
		tinyxml2::XMLElement *entry4 = entry2->FirstChildElement("capability")->ToElement();
		//scan for capability name=fluidics
		while(entry4) {
			if (!strcmp(entry4->ToElement()->Attribute("name"), "fluidics")) break;
			
			entry4 = entry4->ToElement()->NextSibling()->ToElement();
		}
		if (!entry4) {
			perror("[FLUIDMGR] cfg data didn't contain <capability name=fluidics>");
			return;
		}
		
		//scan for data name=operating pressure
		tinyxml2::XMLElement *entry5 = entry4->FirstChildElement("configuration_data")->ToElement();
		while (entry5) {
			tinyxml2::XMLElement *entry5_1 = entry5->FirstChildElement("data")->ToElement();
			if (!strcmp(entry5_1->ToElement()->Attribute("name"),"operating_pressure")) {
				operating_pressure = entry5_1->ToElement()->FloatAttribute("value");
				have_pressure = 1;
				
		        unsigned char spi_send[8];
				memcpy(spi_send+4, &operating_pressure, 4);
				spi_proto_send_msg(&spi_state, spi_send, 4);
			}
			
			entry5 = entry5->ToElement()->NextSibling()->ToElement();
		}
		if (!entry5) {
			perror ("[FLUIDMGR] cfg data didn't contain <data name=operating_pressure>");
		}
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
    unsigned char recvbuf[TRANSFER_SIZE];
    unsigned char sendbuf[TRANSFER_SIZE] = {};
    struct spi_state *s = &spi_state;
    spi_proto_initialize(s);

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
	bool closed = 0;
    while (!closed) {
		int ret = spi_proto_prep_msg(s, sendbuf, TRANSFER_SIZE);
		
        //do SPI communication
        int spi_tr_res = spi_transfer(spi_fd, sendbuf, recvbuf, TRANSFER_SIZE);
		
		struct spi_packet pack;
		memcpy(&pack, recvbuf, TRANSFER_SIZE);

        ++count;
    }

    cout << "=== [FluidManager] Simulation stopped." << endl;


    return 0;

}

