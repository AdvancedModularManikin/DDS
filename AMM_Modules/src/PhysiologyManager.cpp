#include "stdafx.h"

#include "AMM/PhysiologyEngineManager.h"

using namespace AMM;
using namespace std;

bool closed = false;

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" <<
         "\nOptions:\n" <<
         "\t-a\t\tAuto-start based on ticks\n" <<
         "\t-d\t\tDaemonize\n" <<
         "\t-h,--help\t\tShow this help message\n" <<
         endl;
}

void show_menu(PhysiologyEngineManager *pe) {
    string action;

    cout << endl;
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
        pe->Status();
        if (pe->isRunning()) {
            cout << " == Running!  At tick count: ";
        } else {
            cout << " == Not currently running, paused at tick count: ";
        }
        cout << pe->GetTickCount() << endl;
    } else if (action == "2") {
        cout << " == Advancing time one tick" << endl;
        pe->AdvanceTimeTick();
    } else if (action == "3") {
        if (!pe->isRunning()) {
            cout << " == Starting simulation based on ticks..." << endl;
            pe->StartTickSimulation();
        } else {
            cout << " == Already running" << endl;
        }
    } else if (action == "4") {
        if (pe->isRunning()) {
            cout << " == Stopping simulation based on ticks..." << endl;
            pe->StopTickSimulation();
        } else {
            cout << " == Not running" << endl;
        }
    } else if (action == "5") {
        cout << " == Publishing all data" << endl;
        pe->PublishData(true);
        cout << " == Done publishing " << pe->GetNodePathCount() << " items." << endl;
    } else if (action == "6") {
        if (!pe->isRunning()) {
            cout << " == Simulation not running, but shutting down anyway" << endl;
        }
        pe->StopSimulation();
        pe->Shutdown();
        closed = true;

    } else if (action == "LIST") {
        pe->PrintAvailableNodePaths();
    } else if (action == "PRINT") {
        pe->PrintAllCurrentData();
    }

}

int main(int argc, char *argv[]) {
    int daemonize = 0;
    int autostart = 0;
    cout << "=== [AMM - Physiology Manager] ===" << endl;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }

        if (arg == "-d") {
            daemonize = 1;
        }

        if (arg == "-a") {
            autostart = 1;
        }
    }

    PhysiologyEngineManager pe;

    if (autostart == 1) {
        cout << " == Auto-starting physiology simulation based on ticks" << endl;
        pe.StartTickSimulation();
    }

    while (!closed) {
        if (autostart != 1) {
            show_menu(&pe);
        }
        std::this_thread::sleep_for (std::chrono::seconds(1));
        cout.flush();
    }

    cout << "=== [PhysiologyManager] Exiting." << endl;

    return 0;
}
