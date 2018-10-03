
#include "AMM/DDS_Manager.h"

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <fcntl.h>    /* For O_RDWR */

extern "C" {
#include "spi_proto/spi_proto.h"
}

using namespace std;
using namespace AMM;

// Daemonize by default
int daemonize = 1;

const string tourniquet_action = "PROPER_TOURNIQUET";
const string hemorrhage_action = "LEG_HEMORRHAGE";
float heartrate = 40.0;
float breathrate = 12.0;
bool tourniquet = false;
bool hemorrhage = false;
bool closed = false;

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
            tourniquet = true;
        }
        if (c.message() == hemorrhage_action) {
            hemorrhage = true;
        }
    }
};

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n" << endl;
}

struct spi_packet spi_recv_msg;
bool spi_recv_fresh = false;

void
heartrate_led_callback(struct spi_packet *p) {
    memcpy(&spi_recv_msg, p, sizeof(struct spi_packet));
    spi_recv_fresh = true;
}

void heartrate_led_task(void);

int main(int argc, char *argv[]) {
    std::thread datagram_thread(datagram_task);
    std::thread heart_rate_thread(heartrate_led_task);

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

    const char *nodeName = "AMM_HeartRateLED";
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
            "Vcom3D",
            "HeartRateLED",
            "00001", // versions
            "0.0.1", // versions
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/heart_rate_led_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);


    //TODO idler loop here

    cout << "=== [HeartRateLED] Simulation stopped." << endl;


    return 0;

}

void heartrate_led_task(void)
{
	//TODO install callback somehow -- heartrate_led_callback
    int count = 0;
    while (!closed) {
        unsigned char spi_send[4];
        spi_send[0] = heartrate;
        spi_proto_send_msg(&s, spi_send, 4);
        if (spi_recv_fresh) {
            //the received packed is spi_recv_msg.msg[0]
            spi_recv_fresh = false;
        }

        ++count;
        delay_ms(100); // TODO make condition variables and wait on either spi_recv_fresh or receipt of any DDS message
    }
}
