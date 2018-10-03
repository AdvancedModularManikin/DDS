
#include "AMM/DDS_Manager.h"

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <fcntl.h>    /* For O_RDWR */

// Standard includes for SPI datagram library.
extern "C" {
#include "spi_proto.h"
}
#include "spi_proto_master_datagram.h"

using namespace std;
using namespace AMM;

// Daemonize by default
int daemonize = 1;

const string tourniquet_action = "PROPER_TOURNIQUET";
const string hemorrhage_action = "LEG_HEMORRHAGE";
float heartrate = 40.0;
float breathrate = 12.0;
bool tourniquet_sent = false;
bool hemorrhage_sent = false;
bool closed = false;

// Class to handle DDS communication
class HeartRateListener : public ListenerInterface {

    void onNewNodeData(AMM::Physiology::Node n, SampleInfo_t *info) override {
        bool print = false;
        if (n.nodepath() == "EXIT") {
            closed = true;
            return;
        }

        if (n.nodepath() == "Cardiovascular_HeartRate") {
            heartrate = static_cast<float>(n.dbl());
            print = true;
        }

        if (n.nodepath() == "Respiratory_Respiration_Rate") {
            breathrate = static_cast<float>(n.dbl());
            print = true;
        }

        print = false;
        if (print) {
            cout << "=== [HeartRateLED] Received data :  ("
                 << n.nodepath() << ", " << n.dbl() << ')'
                 << endl;
        }
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) override {
        if (c.message() == tourniquet_action) {
            tourniquet_sent = true;
        }
        if (c.message() == hemorrhage_action) {
            hemorrhage_sent = true;
        }
    }
};

// Convenience functions
static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n" << endl;
}
void delay_ms(unsigned int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}


struct spi_packet spi_recv_msg; // Local variable holding the latest received spi message
bool spi_recv_fresh = false;    // Local flag indicating there is fresh data in spi_recv_msg

// The SPI datagram library requires a callback to copy data from the SPI library to a local variable
void spi_message_handler_callback(struct spi_packet *p /* this pointer is to memory managed by the SPI library */) {
    // Copy data from SPI library memory to local variable spi_recv_msg
    memcpy(&spi_recv_msg, p, sizeof(struct spi_packet));
    // Mark flag indicating spi_recv_msg has fresh data
    spi_recv_fresh = true;
}
// Register message handler callback with SPI library.
void (*spi_callback)(struct spi_packet *p) = spi_message_handler_callback;

// Task containing business logic loop for this module.
void heartrate_led_task(void)
{
    int count = 0;  // Debug variable
    while (!closed) {

        // Sending data via SPI
        unsigned char spi_send_buffer[4];   // SPI library needs a buffer of this type to hold the data it sends
        spi_send_buffer[0] = heartrate;     // Assign data to buffer
        spi_proto_send_msg(
                &spi_proto::p.proto /* boilerplate */,
                spi_send_buffer /* buffer containing data to send */,
                4 /* size of data to send */);

        if (spi_recv_fresh) {   // If there's new SPI data, do something
            // The received data is spi_recv_msg.msg[0]

            // Do something with the data. In this case, print it
            printf("RECV\t");
            for (int i = 0; i < 16; i++) printf("%02x ", spi_recv_msg.msg[i]);
            puts("");

            // Mark the data as handled
            spi_recv_fresh = false;
        }

        ++count;
        delay_ms(100); // TODO make condition variables and wait on either spi_recv_fresh or receipt of any DDS message
    }
}

// This is mostly boilerplate
int main(int argc, char *argv[]) {
    std::thread datagram_thread(datagram_task);     // Required for SPI datagram library
    std::thread heart_rate_thread(heartrate_led_task);  // Required to run module business logic

    // Boilerplate to handle command-line arguments
    cout << "=== [HeartRateLED] Ready ..." << endl;
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

    // DDS Name
    const char *nodeName = "AMM_HeartRateLED";

    // DDS Boilerplate
    std::string nodeString(nodeName);
    auto *mgr = new DDS_Manager(nodeName);
    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    HeartRateListener vel;
    node_sub_listener->SetUpstream(&vel);
    command_sub_listener->SetUpstream(&vel);
    auto *pub_listener = new DDS_Listeners::PubListener();
    mgr->InitializeSubscriber(AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(), node_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), command_sub_listener);
    Publisher *command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic,
                                                            AMM::DataTypes::getCommandType(), pub_listener);


    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            mgr->module_id,
            nodeString,
            "EntropicEngineering",
            "HeartRateLED",
            "00001", // versions
            "0.0.1", // versions
            // This is currently a bad example; I'm not sure where this file is supposed to live
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/heart_rate_led_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);

    //TODO idler loop here
    while(!closed)
	  std::this_thread::sleep_for(std::chrono::milliseconds(100));

    cout << "=== [HeartRateLED] Simulation stopped." << endl;

    return 0;

}
