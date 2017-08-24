#include "stdafx.h"

#include "AMM/PhysiologyEngineManager.h"

static void show_usage(std::string name) {
    cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n" << "\t-h,--help\t\tShow this help message\n" << endl;
}

int main(int argc, char *argv[]) {
    char configFile[] = "OSPL_URI=file://ospl.xml";
    putenv(configFile);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }
    }

    bool closed = false;
    string action = "";
    PhysiologyEngineManager pe;

    do {
        cout << endl;
        cout << " === [AMM - Physiology Manager] ===" << endl;
        cout << " [1]Status" << endl;
        cout << " [2]Advance Time Tick" << endl;
        cout << " [3]Start\t\t\tRun physiology engine with simulation-manager ticks" << endl;
        cout << " [4]Stop\t\t\tStop running based on simulation-manager ticks" << endl;
        cout << " [5]Publish data\t\tPublish all data, right now (running or not)" << endl;
        cout << " [6]Quit" << endl;
        cout << " >> ";
        getline(cin, action);
        transform(action.begin(), action.end(), action.begin(), ::toupper);

        if (action == "1") {
            pe.Status();
            if (pe.isRunning()) {
                cout << " == Running!  At tick count: ";
            } else {
                cout << " == Not currently running, paused at tick count: ";
            }
            cout << pe.GetTickCount() << endl;
        } else if (action == "2") {
            cout << " == Advancing time one tick" << endl;
            pe.AdvanceTimeTick();
        } else if (action == "3") {
            cout << " == Starting simulation based on ticks..." << endl;
            pe.StartTickSimulation();
        } else if (action == "4") {
            cout << " == Stopping simulation based on ticks..." << endl;
            pe.StopTickSimulation();
        } else if (action == "5") {
            cout << " == Publishing all data" << endl;
            pe.PublishData(true);
            cout << " == Done publishing " << pe.GetNodePathCount() << " items." << endl;
        } else if (action == "6") {
            pe.StopSimulation();
            pe.Shutdown();
            closed = true;
        } else if (action == "LIST") {
            pe.PrintAvailableNodePaths();
        } else if (action == "PRINT") {
            pe.PrintAllCurrentData();
        }
    } while (!closed);

    cout << "=== [PhysiologyManager] Exiting." << endl;

    return 0;
}
