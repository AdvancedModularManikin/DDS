#include "stdafx.h"

#include "AMM/DDS_Manager.h"

#include <boost/asio.hpp>

#define PORT_LINUX "/dev/tty96B0"
#define PORT_WINDOW "COM7"


using namespace ::boost::asio;
using namespace std;

// Base serial settings
serial_port_base::baud_rate BAUD(9600);
serial_port_base::flow_control FLOW(serial_port_base::flow_control::none);
serial_port_base::parity PARITY(serial_port_base::parity::none);
serial_port_base::stop_bits STOP(serial_port_base::stop_bits::one);


bool closed = false;

class GenericArduinoListener : public ListenerInterface {
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

int main(int argc, char *argv[]) {
    cout << "=== [AMM - Arduino Sensor Bridge] ===" << endl;

    io_service io;
    serial_port port(io, PORT_LINUX);

    port.set_option(BAUD);
    port.set_option(FLOW);
    port.set_option(PARITY);
    port.set_option(STOP);

    auto * mgr = new DDS_Manager();

    auto * node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto * command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto * pub_listener = new DDS_Listeners::PubListener();
    
    GenericArduinoListener al;
    node_sub_listener->SetUpstream(&al);
    command_sub_listener->SetUpstream(&al);

    Subscriber * node_subscriber = mgr->InitializeNodeSubscriber(node_sub_listener);
    Subscriber * command_subscriber = mgr->InitializeCommandSubscriber(command_sub_listener);
    Publisher * command_publisher = mgr->InitializeCommandPublisher(pub_listener);

    cout << "=== [Arduino_Bridge] Ready ..." << endl;

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
            cout << "=== [ARDUINO] Making REPORT: " << value << endl;
        } else if (!rsp.compare(0, actionPrefix.size(), actionPrefix)) {
            std::string value = rsp.substr(actionPrefix.size());
            cout << "=== [ARDUINO] Sending a command: " << value << endl;
            AMM::PatientAction::BioGears::Command cmdInstance;
            cmdInstance.message(value);
            command_publisher->write(&cmdInstance);
        } else {
            cout << "=== [ARDUINO][DEBUG] " << rsp << endl;
        }


        if (c != '\n') {
            // it timed out
            cout << "We had an Arduino timeout of some kind";
        }

    }

    cout << "=== [Arduino_Bridge] Simulation stopped." << endl;

    return 0;

}

