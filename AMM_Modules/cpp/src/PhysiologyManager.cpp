#include "stdafx.h"

#include "AMM/PhysiologyEngineManager.h"

static void show_usage(std::string name) {
	cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n"
			<< "\t-h,--help\t\tShow this help message\n" << endl;
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
	bool paused = false;
	string action = "";
	PhysiologyEngineManager pe;

	cout << "=== [PhysiologyManager] Ready and waiting..." << endl << endl;

	do {
		cout
				<< " [1]Status, [2]Advance Time Tick, [3]Start, [4]Stop, [5]Run, [6]Publish data, [7]Quit "
				<< endl;
		getline(cin, action);
		transform(action.begin(), action.end(), action.begin(), ::toupper);

		if (action == "1") {
			pe.Status();
		} else if (action == "2") {
			cout << " == Advancing time one tick" << endl;
			pe.AdvanceTimeTick();
		} else if (action == "3") {
			cout << " == Starting simulation..." << endl;
			pe.StartSimulation();
		} else if (action == "4") {
			cout << " == Stopping simulation..." << endl;
			pe.StopSimulation();
		} else if (action == "5") {
			cout
					<< " == Run based on Simulation Manager ticks (use Sim Manager to pause/stop)";
			cout.flush();
			// std::thread PEThread(pe.TickLoop);
			while (!pe.closed) {
				pe.TickLoop();
			}
			cout << endl;
		} else if (action == "6") {
			cout << " == Publishing data" << endl;
			pe.PublishData(true);
			cout << " == Done publishing " << pe.GetNodePathCount() << " items." << endl;
		} else if (action == "7") {
			pe.StopSimulation();
			pe.Shutdown();
		} else if (action == "LIST") {

		}
	} while (!closed);



	cout << "=== [PhysiologyManager] Exiting." << endl;

	return 0;
}
