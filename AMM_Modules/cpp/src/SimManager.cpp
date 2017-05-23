#include "stdafx.h"
#include "ccpp_AMM.h"

#include "AMM/SimulationManager.h"

using namespace AMM;

bool closed = false;

int main(int argc, char *argv[]) {
	char configFile[] = "OSPL_URI=file://ospl.xml";
	putenv(configFile);


	int action;
	SimulationManager simManager;
	os_time delay_1s = { 1, 0 };

	cout << " === Simulation Manager" << endl;

	do {
		cout << " [1]Status, [2]Run/Resume, [3]Pause/Stop, [4]Shutdown, [7]Quit  " << endl;
		cin >> action;

		switch (action) {
		case 1:
			if (simManager.isRunning()) {
				cout << " == Running!  At tick count: ";
			} else {
				cout << " == Not currently running, paused at tick count: ";
			}
			cout << simManager.GetTickCount() << endl;
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
			cout << " == Stopping simulation and sending shutdown notice..." << endl;
			simManager.StopSimulation();
			cout << " == Exited after " << simManager.GetTickCount() << " ticks." << endl;
			os_nanoSleep (delay_1s);
			cout << "=== [SimManager] Shutting down Simulation Manager." << endl;
			simManager.Cleanup();
			simManager.Shutdown();
			closed = true;
			break;
		case 7:
			closed = true;
			break;
		}
	} while (!closed);

	return 0;
}

