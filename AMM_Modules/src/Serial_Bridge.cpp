#include "stdafx.h"

#include "AMM/DDS_Manager.h"

#include "AMM/SerialPort.h"

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <queue>
#include <stack>


using namespace ::boost::asio;
using namespace std;

#define PORT_LINUX "/dev/tty96B0"
// #define PORT_LINUX "/dev/ttyACM0"
// #define PORT_LINUX "/dev/serial/by-id/usb-Adafruit_Feather_32u4-if00"
#define PORT_WINDOW "COM7"

int daemonize = 1;
bool closed = false;
bool transmit = false;
std::string reportPrefix = "[REPORT]";
std::string actionPrefix = "[ACT]";
std::string xmlPrefix = "<?xml";
std::string msg1 = "[Scenario]m1s1=mule1_scene1\n";
std::string msg2 = "[Capability]monitor_level=true\n";
std::string msg3 = "[Config_Data]sound_alarm=false\n";
std::string msg4 = "[Capability]detect_button_press=true\n";
std::string msg5 = "[Config_Data]button_message=some_action_name\n";

Publisher *command_publisher;
queue<string> transmitQ;

void readHandler(boost::array<char,SerialPort::k_readBufferSize> const& buffer, size_t bytesTransferred) {
    std::string rsp;
    std::copy(buffer.begin(), buffer.begin()+bytesTransferred, std::back_inserter(rsp));

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
    }  else if (!rsp.compare(0, xmlPrefix.size(), xmlPrefix)) {
        std::string value = rsp;
        cout << "=== [SERIAL] Recieved an XML snippet: " << value << endl;

        cout << "\t[SERIAL][Sending...] " << msg1 << endl;
        transmitQ.push(msg1);

        cout << "\t[SERIAL][Sending...] " << msg2 << endl;
        transmitQ.push(msg2);
        std::this_thread::sleep_for (std::chrono::milliseconds(100));

        cout << "\t[SERIAL][Sending...] " << msg3 << endl;
        transmitQ.push(msg3);
        std::this_thread::sleep_for (std::chrono::milliseconds(100));

        cout << "\t[SERIAL][Sending...] " << msg4 << endl;
        transmitQ.push(msg4);
        std::this_thread::sleep_for (std::chrono::milliseconds(100));

        cout << "\t[SERIAL][Sending...] " << msg5 << endl;
        transmitQ.push(msg4);
        std::this_thread::sleep_for (std::chrono::milliseconds(100));
    } else {
       cout << "=== [SERIAL][DEBUG] " << rsp << endl;
    }
}

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
      ostringstream cmdMessage;
        cmdMessage << "[AMM_Command]" << c.message() << endl;
      transmitQ.push(cmdMessage.str());
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

    // Set up DDS
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

    command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), pub_listener);

    // Set up serial
    io_service io;
    SerialPort serialPort(io, 115200, PORT_LINUX);
    serialPort.DataRead.connect(&readHandler);
    boost::thread t(
            boost::bind(
                    &boost::asio::io_service::run,
                    &io));

    if(serialPort.Initialize())
    {
        std::cerr << "Initialization failed!" << std::endl;
        return 1;
    }


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

    while (!closed) {
        while(!transmitQ.empty()) {
            serialPort.Write(transmitQ.front());
            transmitQ.pop();
        }
        std::this_thread::sleep_for (std::chrono::seconds(1));
        cout.flush();
    }

    cout << "=== [Serial_Bridge] Simulation stopped." << endl;

    return 0;

}

