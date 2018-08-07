#include "stdafx.h"

#include "AMM/BaseLogger.h"

#include "AMM/SimulationManager.h"

using namespace std;
using namespace AMM;

bool closed = false;

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" <<
         "\nOptions:\n" <<
         "\t-r,--rate <sample_rate>\tSpecify the sample rate to run at (samples per second)\n" <<
         "\t-a\t\t\tAuto-start ticks\n" <<
         "\t-d\t\t\tDaemonize\n" <<
         "\t-h,--help\t\t\tShow this help message\n" <<
         endl;
}

void show_menu(SimulationManager* simManager) {
    string action;

    cout << endl;
    cout << " [1]Status " << endl;
    cout << " [2]Run/Resume" << endl;
    cout << " [3]Pause/Stop" << endl;
    cout << " [4]Shutdown" << endl;
    cout << " [5]Command console" << endl;
    cout << " >> ";
    getline(cin, action);
    transform(action.begin(), action.end(), action.begin(), ::toupper);

    if (action == "1") {
        if (simManager->isRunning()) {
            cout << " == Running!  At tick count: ";
        } else {
            cout << " == Not currently running, paused at tick count: ";
        }
        cout << simManager->GetTickCount() << endl;
        cout << "  = Operating at " << simManager->GetSampleRate() << " frames per second." << endl;
    } else if (action == "2") {
        if (!simManager->isRunning()) {
            cout << " == Starting simulation..." << endl;
            simManager->StartSimulation();
        } else {
            cout << " == Simulation already running" << endl;
        }
    } else if (action == "3") {
        if (!simManager->isRunning()) {
            cout << " == Stopping simulation..." << endl;
            simManager->StopSimulation();
        } else {
            cout << " == Simulation not running" << endl;
        }
    } else if (action == "4") {
        if (!simManager->isRunning()) {
            cout << " == Simulation not running, but shutting down anyway" << endl;
        } else {
            cout << " == Stopping simulation and sending shutdown notice..." << endl;
        }
        simManager->StopSimulation();
        cout << " == Exited after " << simManager->GetTickCount() << " ticks." << endl;
        cout << "=== [SimManager] Shutting down Simulation Manager." << endl;
        closed = true;
        simManager->Shutdown();
    } else if (action == "5") {
        std::string command;
        bool consoleclosed = false;
        do {
            cout << " Enter a command (exit to return to menu) >>> ";
            getline(cin, command);
            transform(command.begin(), command.end(), command.begin(), ::toupper);
            if (command == "EXIT") {
                consoleclosed = true;
            } else {
                if (command.empty()) {
                    continue;
                }
                simManager->SendCommand(command);
            }
        } while (!consoleclosed);
    } else {
        // unknown menu action
    }
}

int main(int argc, char *argv[]) {
	int sampleRate = 50;
	int daemonize = 0;
	int autostart = 0;
    LOG_INFO << "Simulation Manager starting";
	
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

        if ((arg == "-r") || (arg == "--rate")) {
            istringstream ss(argv[i + 1]);            
            if (!(ss >> sampleRate)) {
                cerr << "Invalid sample rate: " << argv[i + 1] << '\n';
                return 0;
            }
            
        }
    }

	SimulationManager simManager;
	simManager.SetSampleRate(sampleRate);

    if (autostart == 1) {
        LOG_INFO << "Auto-starting simulation";
        simManager.StartSimulation();
    }

    while (!closed) {
        if (daemonize != 1 && autostart != 1) {
            show_menu(&simManager);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    LOG_INFO << "Exiting.";

    return 0;
}

