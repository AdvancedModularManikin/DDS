#include "stdafx.h"
#include "ccpp_AMM.h"
#include "AMM/DDSEntityManager.h"

#include <algorithm>

using namespace std;
using namespace DDS;
using namespace AMM::PatientAction::BioGears;

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

	/* DDS entity manager */
	DDSEntityManager mgr;
	CommandSeq cmdList;
	SampleInfoSeq infoSeq;

	/** Initialization data **/
	const char *partitionName = "AMM";
	char topicName[] = "Command";
	ReturnCode_t status;

	// create domain participant
	mgr.createParticipant(partitionName);

	CommandTypeSupport_var dt = new CommandTypeSupport();
	mgr.registerType(dt.in());
	mgr.createTopic(topicName);
	mgr.createPublisher();
	mgr.createWriter();

	// Publish Events
	DataWriter_var dwriter = mgr.getWriter();
	CommandDataWriter_var CommandWriter = CommandDataWriter::_narrow(
			dwriter.in());

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
			Command cmdInstance;
			cmdInstance.message = DDS::string_dup(action.c_str());
			cout << "=== [CommandExecutor] Sending a command containing:" << endl;
			cout << "    Command : \"" << cmdInstance.message << "\"" << endl;
			status = CommandWriter->write(cmdInstance, DDS::HANDLE_NIL);
			checkStatus(status, "CommandWriter::write");
		}
	} while (!closed);

	mgr.deleteWriter(CommandWriter.in());
	mgr.deletePublisher();
	mgr.deleteTopic();
	mgr.deleteParticipant();

	return 0;

}
