#include "stdafx.h"

#include "AMM/DDS_Manager.h"

using namespace std;

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

    auto *mgr = new DDS_Manager();
    auto * pub_listener = new DDS_Listeners::PubListener();
    Publisher * command_publisher = mgr->InitializeCommandPublisher(pub_listener);

	std::string action = "";
	bool closed = false;
	cout << "=== [CommandExecutor] Enter commands to send and hit enter.  EXIT to quit." << endl;
	do {
		cout << " >>> ";
		getline(cin, action);
		transform(action.begin(), action.end(), action.begin(), ::toupper);
		if (action == "EXIT") {
			closed = true;
		} else {
			if (action == "") {
				continue;
			}
            AMM::PatientAction::BioGears::Command cmdInstance;
            cmdInstance.message(action);
            cout << "=== [CommandExecutor] Sending a command containing:" << endl;
            cout << "    Command : \"" << cmdInstance.message() << "\"" << endl;
            command_publisher->write(&cmdInstance);
		}
	} while (!closed);

	return 0;

}
