#include "stdafx.h"

#include "AMM/DDS_Manager.h"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <fcntl.h>    /* For O_RDWR */

#include "spi_proto.h"

#define TRANSFER_SIZE 36
#define SPI_TRANSFER_LEN TRANSFER_SIZE

using namespace std;

// Daemonize by default
int daemonize = 1;

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

int spi_transfer(int fd, unsigned char *tx_buf, unsigned char *rx_buf, __u32 buflen) {
    int ret;
    struct spi_ioc_transfer tr = {tx_buf : (unsigned long) tx_buf, rx_buf : (unsigned long) rx_buf, len : buflen, speed_hz : speed,
            delay_usecs : delay, bits_per_word : bits,};

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        perror("can't send spi message");
    return ret;
}


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

	print=false;
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
heartrate_led_callback(struct spi_packet *p)
{
	memcpy(&spi_recv_msg, p, sizeof(struct spi_packet));
	spi_recv_fresh = true;
}
int main(int argc, char *argv[]) {
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

    int spi_fd = open(device, O_RDWR);
    unsigned char recvbuf[TRANSFER_SIZE];
    unsigned char sendbuf[TRANSFER_SIZE] = {};
    struct spi_state s;
    spi_proto_initialize(&s);

    const char* nodeName = "AMM_HeartRateLED";
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
    Publisher *command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), pub_listener);


    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            nodeString,
            "Vcom3D",
            "HeartRateLED",
            "00001", // versions
            "0.0.1", // versions
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/heart_rate_led_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus( nodeString, OPERATIONAL);


    int count = 0;
    while (!closed) {
        //prepare SPI message
        /*
         heartrate = 60 (Example)
         heartrate/60 = 1 = beats/second
         seconds/ms = 1/1000
         want ms/beat
         beats/second*seconds/ms = beats/ms
         1/beats/ms = ms/beat

         answer = 1/(beats/min * min/sec * sec/ms)
         answer = 1/(hr * (1/60) * 0.001)
         */
        //int spi_msg_full = 1.0/(heartrate * (1.0/60.0) * 0.001);
        unsigned char spi_send[4];
        spi_send[0] = heartrate;
        spi_send[1] = breathrate;
        spi_send[2] = static_cast<unsigned char>(tourniquet);
        spi_send[3] = static_cast<unsigned char>(hemorrhage);
        unsigned char spi_rcvd[4];
	spi_proto_send_msg(&s, spi_send, 4);

        //do SPI communication
        //int spi_tr_res = spi_transfer(spi_fd, spi_send, spi_rcvd, 4);
	//TODO send message
	//TODO prep message
	int ret = spi_proto_prep_msg(&s, sendbuf, SPI_TRANSFER_LEN);
	
	//do transaction
	spi_transfer(spi_fd, sendbuf, recvbuf, SPI_TRANSFER_LEN);
	//printf("amt_read = %d\n", amt_read);
	
	//process buffer into struct
	struct spi_packet pack;
	memcpy(&pack, recvbuf, SPI_TRANSFER_LEN);
	//TODO maybe fixup the CRC byte order?
	
	//process received message
	spi_proto_rcv_msg(&s, &pack, heartrate_led_callback);

        //std::cout << "spi_msg " << std::hex << std::setw(2)
        //	<< std::setfill('0') << (unsigned int) spi_msg << std::endl;
        //std::cout << "spi_rcvd " << std::hex << std::setw(2)
        //	<< std::setfill('0') << (unsigned int) spi_rcvd << std::endl;
        //send press messages based on received SPI
        //the buttons send 1 when they are up and 0 when they are pressed
	if (spi_recv_fresh) {
		if (spi_recv_msg.msg[0] == 'A' && spi_recv_msg.msg[1] == 'C') {
        		//button 2 was pressed
        		//send hemorrhage action
			char strspace[32];
			memcpy(strspace, spi_recv_msg.msg, 32);
			strspace[31] = 0;
			char *act = strspace+8;
        		cout << "=== [HeartRateLED] Sending a command:" << act << endl;
        		AMM::PatientAction::BioGears::Command cmdInstance;
        		cmdInstance.message(act);
        		command_publisher->write(&cmdInstance);
        	}
		spi_recv_fresh = false;
	}


        ++count;
    }

    cout << "=== [HeartRateLED] Simulation stopped." << endl;


    return 0;

}

