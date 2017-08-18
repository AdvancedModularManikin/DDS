#include "stdafx.h"

#include "AMM/SimulationManager.h"

#include <algorithm>

using namespace std;

static void show_usage(std::string name) {
	cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n" << "\t-h,--help\t\tShow this help message\n"
			<< "\t-r,--rate <sample_rate>\tSpecify the sample rate to run at (samples per second) - doesn't do anything yet!"
			<< endl;
}

int main(int argc, char *argv[]) {
	char configFile[] = "OSPL_URI=file://ospl.xml";
	putenv(configFile);

	string action = "";
	SimulationManager simManager;
	bool closed = false;

	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if ((arg == "-h") || (arg == "--help")) {
			show_usage(argv[0]);
			return 0;
		} else if ((arg == "-r") || (arg == "--rate")) {
			istringstream ss(argv[i + 1]);
			int sampleRate;
			if (!(ss >> sampleRate)) {
				cerr << "Invalid sample rate: " << argv[i + 1] << '\n';
				return 0;
			}
			simManager.SetSampleRate(sampleRate);
		}
	}

	do {
		cout << endl;
		cout << " === [AMM - Simulation Manager] ===" << endl;
		cout << " [1]Status " << endl;
		cout << " [2]Run/Resume" << endl;
		cout << " [3]Pause/Stop" << endl;
		cout << " [4]Shutdown" << endl;
		cout << " [5]Command console" << endl;
		cout << " >> ";

		getline(cin, action);
		transform(action.begin(), action.end(), action.begin(), ::toupper);

		if (action == "1") {
			if (simManager.isRunning()) {
				cout << " == Running!  At tick count: ";
			} else {
				cout << " == Not currently running, paused at tick count: ";
			}
			cout << simManager.GetTickCount() << endl;
			cout << "  = Operating at " << simManager.GetSampleRate() << " frames per second." << endl;
		} else if (action == "2") {
			cout << " == Starting simulation..." << endl;
			simManager.StartSimulation();
		} else if (action == "3") {
			cout << " == Stopping simulation..." << endl;
			simManager.StopSimulation();
		} else if (action == "4") {
			cout << " == Stopping simulation and sending shutdown notice..." << endl;
			simManager.StopSimulation();
			cout << " == Exited after " << simManager.GetTickCount() << " ticks." << endl;
			cout << "=== [SimManager] Shutting down Simulation Manager." << endl;
			closed = true;
			simManager.Shutdown();
		} else if (action == "5") {
			std::string command = "";
			bool consoleclosed = false;
			do {
				cout << " Enter a command (exit to return to menu) >>> ";
				getline(cin, command);
				transform(command.begin(), command.end(), command.begin(), ::toupper);
				if (command == "EXIT") {
					consoleclosed = true;
				} else {
					if (command == "") {
						continue;
					}
					simManager.SendCommand(command);
				}
			} while (!consoleclosed);
		} else {

		}
	} while (!closed);

	return 0;
}

