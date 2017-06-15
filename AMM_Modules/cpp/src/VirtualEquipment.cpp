#include "stdafx.h"
#include "ccpp_AMM.h"
#include "AMM/DDSEntityManager.h"

using namespace DDS;
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

	/* DDS entity manager */
	DDSEntityManager mgr;
	NodeSeq msgList;
	SampleInfoSeq infoSeq;

	/** Initialization data **/
	const char *partitionName = "AMM";
	char topicName[] = "Data";
	os_time delay_200ms = { 0, 200000000 };
	ReturnCode_t status;
	bool closed = false;
	char buf[MAX_MSG_LEN];

	/* Specific to this app */
	char sTopicName[] = "MyDataTopic";
	StringSeq sSeqExpr;

	// create domain participant
	mgr.createParticipant(partitionName);

	//create type

	NodeTypeSupport_var dt = new NodeTypeSupport();
	mgr.registerType(dt.in());

	//create Topic
	mgr.createTopic(topicName);

	//create Subscriber
	mgr.createSubscriber();

	// create subscription filter
	ostringstream filterString;
	bool first = true;
	for (std::string np : node_paths) {
		if (first) {
			filterString << "nodepath = '" << np << "'";
			first = false;
		} else {
			filterString << " OR nodepath = '" << np << "'";
		}
	}
	std::string fString = filterString.str();
	const char* nodePath = fString.c_str();
	snprintf(buf, MAX_MSG_LEN, nodePath);
	DDS::String_var sFilter = DDS::string_dup(buf);
	sSeqExpr.length(0);

	cout << "=== [VirtualEquipment] Subscription filter : " << sFilter << endl;

	// create topic
	mgr.createContentFilteredTopic(sTopicName, sFilter.in(), sSeqExpr);
	// create Filtered DataReader
	mgr.createReader(true);

	DataReader_var dreader = mgr.getReader();
	NodeDataReader_var PhysiologyDataReader = NodeDataReader::_narrow(dreader.in());

	checkHandle(PhysiologyDataReader.in(), "NodeDataReader::_narrow");

	cout << "=== [VirtualEquipment] Ready ..." << endl;

	while (!closed) {
		status = PhysiologyDataReader->take(msgList, infoSeq, LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE,
				ANY_INSTANCE_STATE);
		checkStatus(status, "DataReader::take");
		for (DDS::ULong i = 0; i < msgList.length(); i++) {
			if (infoSeq[i].valid_data) {
				if (msgList[i].dbl == -1.0f) {
					closed = true;
					break;
				}
				cout << "=== [VirtualEquipment] Received data :  (" << msgList[i].nodepath << ", " << msgList[i].dbl << ')'
						<< "\t\t\t[frame: " << msgList[i].frame << "]" << endl;
			}

		}

		status = PhysiologyDataReader->return_loan(msgList, infoSeq);
		checkStatus(status, "DataReader::return_loan");
		// os_nanoSleep(delay_200ms);
	}

	cout << "=== [VirtualEquipment] Simulation stopped." << endl;

	//cleanup
	mgr.deleteReader(PhysiologyDataReader.in());
	mgr.deleteSubscriber();
	mgr.deleteFilteredTopic();
	mgr.deleteTopic();
	mgr.deleteParticipant();

	return 0;

}
