#include "stdafx.h"

#include "AMM/DDS_Manager.h"

#include <iostream>
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
        bool print = false;
        if (n.dbl() == -1.0f) {
            closed = true;
            return;
        }
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c) {

    }
};

int main(int argc, char *argv[]) {
    char opcode, priority;
    int count = 0;

    io_service io;
    serial_port port(io, PORT_LINUX);

    port.set_option(BAUD);
    port.set_option(FLOW);
    port.set_option(PARITY);
    port.set_option(STOP);

    auto *mgr = new DDS_Manager();

    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();

    GenericArduinoListener al;
    node_sub_listener->SetUpstream(&al);
    command_sub_listener->SetUpstream(&al);

    Subscriber *node_subscriber = mgr->InitializeNodeSubscriber(node_sub_listener);
    Subscriber *command_subscriber = mgr->InitializeCommandSubscriber(command_sub_listener);

    cout << "=== [ArduinoSensor] Ready ..." << endl;


    while (!closed) {
        read(port, buffer(&opcode, 1));
        read(port, buffer(&priority, 1));
        cout << opcode << " " << priority << endl;
        ++count;
    }

    cout << "=== [ArduinoSensor] Simulation stopped." << endl;

    return 0;

}

