#include "stdafx.h"

#include "AMM/DDS_Manager.h"

//#include <sys/ioctl.h>
//#include <sys/socket.h>
//#include <sys/un.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <linux/types.h>
//#include <linux/spi/spidev.h>

using namespace std;

// Daemonize by default
int daemonize = 1;
bool closed = false;
float injamt = 0;
bool new_inj_data = 0;


//TODO replace with SyringeListener
class SyringeListener : public ListenerInterface {

    void onNewNodeData(AMM::Physiology::Node n) override {
        bool print = false;
        if (n.nodepath() == "EXIT") {
            closed = true;
            return;
        }

        if (n.nodepath() == "Syringe_InjectedAmount") {
            injamt = static_cast<float>(n.dbl());
            new_inj_data = true;
        }

        print=false;
        if (print) {
            cout << "=== [Syringe] Received data :  ("
                 << n.nodepath() << ", " << n.dbl() << ')'
                 << endl;
        }
    }
};

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n" << endl;
}

int main(int argc, char *argv[]) {
    cout << "=== [Syringe] Ready ..." << endl;
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


    const char* nodeName = "EE_SyringeReceiver";
    std::string nodeString(nodeName);
    auto *mgr = new DDS_Manager(nodeName);
    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();

    SyringeListener vel;
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
            "EntropicEngineering",
            "SyringeReceiver",
            "00001", // versions
            "0.0.1", // versions
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/syringe_receiver_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus( nodeString, OPERATIONAL);

    while (1) {
      if (new_inj_data) {
        printf("injected: %f\n", injamt);
        new_inj_data = false;
      } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
      if (closed) break;
    }

    cout << "=== [Syringe] Simulation stopped." << endl;


    return 0;

}

