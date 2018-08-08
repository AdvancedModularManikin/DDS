#include "stdafx.h"

#include "AMM/PhysiologyEngineManager.h"

#include "AMM/BaseLogger.h"

using namespace AMM;
using namespace std;

bool closed = false;

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" <<
         "\nOptions:\n" <<
         "\t-a\t\tAuto-start based on ticks\n" <<
         "\t-l\t\tEnable physiology CSV logging\n" <<
         "\t-h,--help\t\tShow this help message\n" <<
         endl;
}

void show_menu(AMM::PhysiologyEngineManager *pe) {
    string action;

    std::cout << std::endl;
    std::cout << " [1]Status" << std::endl;
    std::cout << " [2]Advance Time Tick" << std::endl;
    std::cout << " [3]Start\t\t\tRun physiology engine with simulation-manager ticks" << std::endl;
    std::cout << " [4]Stop\t\t\tStop running based on simulation-manager ticks" << std::endl;
    std::cout << " [5]Publish data\t\tPublish all data, right now (running or not)" << std::endl;
    std::cout << " [6]Test ventilator" << std::endl;
    std::cout << " [7]Quit" << std::endl;
    std::cout << " >> ";
    getline(cin, action);
    transform(action.begin(), action.end(), action.begin(), ::toupper);

    if (action == "1") {
        pe->Status();
        if (pe->isRunning()) {
            std::cout << " == Running!  At tick count: ";
        } else {
            std::cout << " == Not currently running, paused at tick count: ";
        }
        std::cout << pe->GetTickCount() << std::endl;
    } else if (action == "2") {
        std::cout << " == Advancing time one tick" << std::endl;
        pe->AdvanceTimeTick();
    } else if (action == "3") {
        if (!pe->isRunning()) {
            std::cout << " == Starting simulation based on ticks..." << std::endl;
            pe->StartTickSimulation();
        } else {
            std::cout << " == Already running" << std::endl;
        }
    } else if (action == "4") {
        if (pe->isRunning()) {
            std::cout << " == Stopping simulation based on ticks..." << std::endl;
            pe->StopTickSimulation();
        } else {
            std::cout << " == Not running" << std::endl;
        }
    } else if (action == "5") {
        std::cout << " == Publishing all data" << std::endl;
        pe->PublishData(true);
        std::cout << " == Done publishing " << pe->GetNodePathCount() << " items." << std::endl;
    } else if (action == "6") {
        std::cout << "Testing ventilator settings";
        std::string payload = "OxygenFraction=0.5\nPositiveEndExpiredPressure=0.5\nRespiratoryRate=50\nTidalVolume=0.5\nVentilatorPressure=0.1";
        pe->TestVentilator(payload);
    } else if (action == "7") {
        if (!pe->isRunning()) {
            std::cout << " == Simulation not running, but shutting down anyway" << std::endl;
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
    int autostart = 0;
    bool logging = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }

        if (arg == "-l") {
            logging = true;
        }

        if (arg == "-a") {
            autostart = 1;
        }
    }

    AMM::PhysiologyEngineManager *pe = new AMM::PhysiologyEngineManager();
    pe->SetLogging(logging);

    if (autostart == 1) {
        LOG_INFO << "Auto-starting physiology simulation based on ticks";
        pe->StartTickSimulation();
    }

    while (!closed) {
        if (autostart != 1) {
            show_menu(pe);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        cout.flush();
    }

    LOG_INFO << "Exiting.";

    return 0;
}
