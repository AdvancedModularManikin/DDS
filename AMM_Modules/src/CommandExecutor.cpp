#include "stdafx.h"

#include "AMM/DDS_Manager.h"

using namespace std;

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n" << endl;
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }
    }
    const char* nodeName = "AMM_CommandExecutor";
    auto *mgr = new DDS_Manager(nodeName);
    auto *pub_listener = new DDS_Listeners::PubListener();
    Publisher *command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), pub_listener);

    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            "Vcom3D",
            "CommandExecutor",
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/command_executor_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(OPERATIONAL);

    std::string action;
    bool closed = false;
    cout << "=== [CommandExecutor] Enter commands to send and hit enter.  EXIT to quit." << endl;
    do {
        cout << " >>> ";
        getline(cin, action);
        transform(action.begin(), action.end(), action.begin(), ::toupper);
        if (action == "EXIT") {
            closed = true;
        } else {
            if (action.empty()) {
                continue;
            }
            cout << "=== [CommandExecutor] Sending a command:" << action << endl;
            AMM::PatientAction::BioGears::Command cmdInstance;
            cmdInstance.message(action);
            command_publisher->write(&cmdInstance);
        }
    } while (!closed);

    return 0;

}
