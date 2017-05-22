#include "stdafx.h"
#include "ccpp_AMM.h"
#include "AMM/DDSEntityManager.h"

using namespace DDS;
using namespace AMM::Physiology;

int main(int argc, char *argv[]) {
	char configFile[] = "OSPL_URI=file://ospl.xml";
	putenv(configFile);

	const char* nodePath = "";

	if (argc > 1) {
		 nodePath = argv[1];
	} else {
		cerr << "*** [AMM] Subscription node path not specified." << endl;
		cerr << "*** usage : ./amm_virtual_equipment <node_path>" << endl;
		return -1;
	}

	/* DDS entity manager */
	DDSEntityManager mgr;
	DataSeq msgList;
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
	DataTypeSupport_var dt = new DataTypeSupport();
	mgr.registerType(dt.in());

	//create Topic
	mgr.createTopic(topicName);

	//create Subscriber
	mgr.createSubscriber();

	// create subscription filter
	snprintf(buf, MAX_MSG_LEN, "node_path = '%s'", nodePath);
	DDS::String_var sFilter = DDS::string_dup(buf);
	sSeqExpr.length(0);

	cout << "=== [VirtualEquipment] Subscription filter : " << sFilter << endl;

	// create topic
	mgr.createContentFilteredTopic(sTopicName, sFilter.in(), sSeqExpr);
	// create Filtered DataReader
	mgr.createReader(true);
	DataReader_var dreader = mgr.getReader();
	DataDataReader_var PhysiologyDataReader = DataDataReader::_narrow(
			dreader.in());
	checkHandle(PhysiologyDataReader.in(), "DataDataReader::_narrow");

	cout << "=== [VirtualEquipment] Ready ..." << endl;

	while (!closed) {
		status = PhysiologyDataReader->take(msgList, infoSeq, LENGTH_UNLIMITED,
				ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
		checkStatus(status, "DataReader::take");
		for (DDS::ULong i = 0; i < msgList.length(); i++) {
			if (infoSeq[i].valid_data) {
				if (msgList[i].dbl == -1.0f) {
					closed = true;
					break;
				}
			}
			cout << "=== [VirtualEquipment] Received data :  ("
					<< msgList[i].node_path << ", " << msgList[i].dbl << ')'
					<< endl;
		}

		status = PhysiologyDataReader->return_loan(msgList, infoSeq);
		checkStatus(status, "DataReader::return_loan");
		os_nanoSleep(delay_200ms);
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
