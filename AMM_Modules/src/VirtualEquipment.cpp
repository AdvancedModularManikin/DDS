#include "stdafx.h"

#include "AMM/VirtualEquipmentListener.h"

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

    auto *mgr = new DDS_Manager();
    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();
    VirtualEquipmentListener vel;
    vel.SetFilter(&node_paths);
    node_sub_listener->SetUpstream(&vel);
    Subscriber *node_subscriber = mgr->InitializeNodeSubscriber(node_sub_listener);

    if (node_subscriber == nullptr) {
        cout << "=== [VirtualEquipment] Unable to initialize node data subscriber." << endl;
        return false;
    }


    cout << "=== [VirtualEquipment] Ready ..." << endl;


    cout << "\t(frame)\t\tNode Path\t\tValue" << endl;

    std::cin.ignore();

    cout << "\tShutting down the Subscriber." << endl;

    cout << "=== [VirtualEquipment] Simulation stopped." << endl;

    return 0;

}


