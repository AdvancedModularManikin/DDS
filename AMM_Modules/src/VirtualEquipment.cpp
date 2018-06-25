#include "stdafx.h"

#include "AMM/Listeners/VirtualEquipmentListener.h"

using namespace std;

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)> node_path node_path ..." << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n" << endl;
    cerr << "Example: " << name << " ECG HR " << endl;
}

int main(int argc, char *argv[]) {
    std::vector<std::string> node_paths;

    cout << "=== [AMM - Virtual Equipment] ===" << endl;

    if (argc <= 1) {
        show_usage(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }
        node_paths.push_back(arg);

    }

    // create subscription filter
    ostringstream filterString;
    bool first = true;
    for (const std::string &np : node_paths) {
        if (first) {
            filterString << "nodepath = '" << np << "'";
            first = false;
        } else {
            filterString << " OR nodepath = '" << np << "'";
        }
    }
    std::string fString = filterString.str();
    cout << "=== [VirtualEquipment] Subscription filter : " << fString << endl;

    string nodeName = "AMM_VirtualEquipment";
    auto *mgr = new DDS_Manager(nodeName.c_str());
    std::string nodeString(nodeName);
    VirtualEquipmentListener vel;
    vel.SetFilter(&node_paths);
    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    node_sub_listener->SetUpstream(&vel);
    mgr->InitializeSubscriber(AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(), node_sub_listener);


    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            nodeString,
            "Vcom3D",
            nodeName,
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/virtual_equipment_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(nodeString, OPERATIONAL);

    cout << "=== [VirtualEquipment] Ready ..." << endl;


    cout << "\t(frame)\t\tNode Path\t\tValue" << endl;

    std::cin.ignore();

    cout << "\tShutting down the Subscriber." << endl;

    cout << "=== [VirtualEquipment] Simulation stopped." << endl;

    return 0;

}


