#include "stdafx.h"

#include "AMM/DDS_Manager.h"

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

#define PORT_LINUX "/dev/ttyACM0"
//#define PORT_LINUX "/dev/serial/by-id/usb-Adafruit_Feather_32u4-if00"
#define PORT_WINDOW "COM7"


using namespace ::boost::asio;
using namespace std;

// Base serial settings
serial_port_base::baud_rate BAUD(9600);
serial_port_base::flow_control FLOW(serial_port_base::flow_control::none);
serial_port_base::parity PARITY(serial_port_base::parity::none);
serial_port_base::stop_bits STOP(serial_port_base::stop_bits::one);

// Daemonize by default
int daemonize = 1;

bool closed = false;

class GenericSerialListener : public ListenerInterface {
    void onNewNodeData(AMM::Physiology::Node n) {
      if (n.nodepath() == "EXIT") {
	  cout << "Shutting down simulation based on shutdown node-data from physiology engine."  << endl;
	  closed = true;
	  return;
        }
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c) {
      cout << "We got a command from elsewhere: " << c.message() << endl;
    }
};

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n" << "\t-h,--help\t\tShow this help message\n" << endl;
}


int main(int argc, char *argv[]) {
    cout << "=== [AMM - Serial Module Bridge] ===" << endl;

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

    io_service io;
    serial_port port(io, PORT_LINUX);

    port.set_option(BAUD);
    port.set_option(FLOW);
    port.set_option(PARITY);
    port.set_option(STOP);

    const char* nodeName = "AMM_Serial_Bridge";
    auto *mgr = new DDS_Manager(nodeName);

    auto * node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto * command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto * pub_listener = new DDS_Listeners::PubListener();
    
    GenericSerialListener al;
    node_sub_listener->SetUpstream(&al);
    command_sub_listener->SetUpstream(&al);

    mgr->InitializeSubscriber(AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(), node_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), command_sub_listener);

    Publisher *command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), pub_listener);


    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            "Vcom3D",
            "Serial_Bridge",
            "00001",
            "0.0.1",
            "capabilityString"
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(OPERATIONAL);

    cout << "=== [Serial_Bridge] Ready ..." << endl;

    std::string reportPrefix = "[REPORT]";
    std::string actionPrefix = "[ACT]";

    while (!closed) {
        char c;
        std::string rsp;

        while (boost::asio::read(port, boost::asio::buffer(&c, 1)) && c != '\n') {
            rsp += c;
        }

        if (!rsp.compare(0, reportPrefix.size(), reportPrefix)) {
            std::string value = rsp.substr(reportPrefix.size());
            cout << "=== [SERIAL] Making REPORT: " << value << endl;
        } else if (!rsp.compare(0, actionPrefix.size(), actionPrefix)) {
            std::string value = rsp.substr(actionPrefix.size());
            cout << "=== [SERIAL] Sending a command: " << value << endl;
            AMM::PatientAction::BioGears::Command cmdInstance;
            boost::trim_right(value);
            cmdInstance.message(value);
            command_publisher->write(&cmdInstance);
        } else {
            cout << "=== [SERIAL][DEBUG] " << rsp << endl;
        }


        if (c != '\n') {
            cout << "We had a serial timeout of some kind";
        }

    }

    cout << "=== [Serial_Bridge] Simulation stopped." << endl;

    return 0;

}

