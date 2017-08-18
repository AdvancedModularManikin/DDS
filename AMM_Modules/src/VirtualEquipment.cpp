#include "stdafx.h"

#include "AMM/VirtualEquipmentListener.h"

using namespace std;

static void show_usage(std::string name) {
    cerr << "Usage: " << name << " <option(s)> node_path node_path ..." << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n" << endl;
    cerr << "Example: " << name << " ECG HR " << endl;
}

int main(int argc, char *argv[]) {
    std::vector<std::string> node_paths;

    if (argc <= 1) {
        show_usage(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        } else {
            node_paths.push_back(arg);
        }
    }

    // create subscription filter
    ostringstream filterString;
    bool first = true;
    for (std::string np : node_paths) {
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

    auto *tick_sub_listener = new DDS_Listeners::TickSubListener();
    auto *node_sub_listener = new DDS_Listeners::NodeSubListener();


    Subscriber * node_subscriber = mgr->InitializeNodeSubscriber(node_sub_listener);
    Subscriber * tick_subscriber = mgr->InitializeTickSubscriber(tick_sub_listener);

    if (node_subscriber == nullptr) {
        cout << "Unable to initialize node data subscriber." << endl;
        return false;
    }


    std::cout << "Initialized node subscriber and listener." << std::endl;


    cout << "=== [VirtualEquipment] Ready ..." << endl;


    cout << "\tWaiting for Data, press Enter to stop the Subscriber. " << endl;

    std::cin.ignore();

    cout << "\tShutting down the Subscriber." << endl;


    cout << "=== [VirtualEquipment] Simulation stopped." << endl;

    return 0;

}


