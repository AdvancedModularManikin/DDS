#include "stdafx.h"

#include "RTPS/AMMSubscriber.h"

#include <fastrtps/Domain.h>

using namespace std;
using namespace eprosima;
using namespace eprosima::fastrtps;

using namespace AMM::Physiology;

static void show_usage(std::string name) {
	cerr << "Usage: " << name << " <option(s)> node_path node_path ..." << "\nOptions:\n"
			<< "\t-h,--help\t\tShow this help message\n" << endl;
	cerr << "Example: " << name << " ECG HR " << endl;
}

int main(int argc, char *argv[]) {
	char configFile[] = "OSPL_URI=file://ospl.xml";
	putenv(configFile);

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
    bool closed = false;

	for (std::string np : node_paths) {
		if (first) {
			filterString << "nodepath = '" << np << "'";
			first = false;
		} else {
			filterString << " OR nodepath = '" << np << "'";
		}
	}


	cout << "=== [VirtualEquipment] Subscription filter : " << filterString.str() << endl;


	cout << "=== [VirtualEquipment] Ready ..." << endl;

	while (!closed) {

	}

	cout << "=== [VirtualEquipment] Simulation stopped." << endl;


	return 0;

}
