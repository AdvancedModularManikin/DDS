#include "stdafx.h"
#include "ccpp_AMM.h"

#include "AMM/SimulationManager.h"

using namespace AMM;

static void show_usage(std::string name) {
	cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n"
			<< "\t-h,--help\t\tShow this help message\n"
			<< "\t-r,--rate <sample_rate>\tSpecify the sample rate to run at (samples per second) - doesn't do anything yet!"
			<< endl;
}

int main(int argc, char *argv[]) {
	char configFile[] = "OSPL_URI=file://ospl.xml";
	putenv(configFile);

	int action;
	SimulationManager simManager;
	os_time delay_1s = { 1, 0 };
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

	cout << " === Simulation Manager" << endl;

	do {
		cout
				<< " [1]Status, [2]Run/Resume, [3]Pause/Stop, [4]Shutdown, [7]Quit  "
				<< endl;
		cin >> action;

		switch (action) {
		case 1:
			if (simManager.isRunning()) {
				cout << " == Running!  At tick count: ";
			} else {
				cout << " == Not currently running, paused at tick count: ";
			}
			cout << simManager.GetTickCount() << endl;
			// cout << "  = Operating at " << simManager.GetSampleRate() << " frames per second." << endl;
			break;
		case 2:
			cout << " == Starting simulation..." << endl;
			simManager.StartSimulation();
			break;
		case 3:
			cout << " == Stopping simulation..." << endl;
			simManager.StopSimulation();
			break;
		case 4:
			cout << " == Stopping simulation and sending shutdown notice..."
					<< endl;
			simManager.StopSimulation();
			cout << " == Exited after " << simManager.GetTickCount()
					<< " ticks." << endl;
			os_nanoSleep(delay_1s);
			cout << "=== [SimManager] Shutting down Simulation Manager."
					<< endl;
			closed = true;
			break;
		case 7:
			cout << "=== [SimManager] Quitting Simulation Manager." << endl;
			closed = true;
			break;
		}
	} while (!closed);

	simManager.Cleanup();
	simManager.Shutdown();
	return 0;
}

