#include "stdafx.h"
#include "ccpp_AMM.h"
#include "AMM/DDSEntityManager.h"

using namespace DDS;
using namespace AMM::Physiology;

int main(int argc, char *argv[]) {

	// usage : VirtualEquipment <subscription_string>
	const char *nodePath;
	os_time delay_200ms = { 0, 200000000 };
	char buf[MAX_MSG_LEN];

	if (argc > 1) {
		nodePath = argv[1];
	} else {
		cerr
				<< "*** [AMM] Subscription node path not specified."
				<< endl;
		cerr
				<< "*** usage : ./amm_virtual_equipment <node_path>"
				<< endl;
		return -1;
	}

	DataSeq msgList;
	SampleInfoSeq infoSeq;

	DDSEntityManager mgr;

	// create domain participant
	char partition_name[] = "ContentFilteredTopic AMM";
	mgr.createParticipant(partition_name);

	//create type
	DataTypeSupport_var dt = new DataTypeSupport();
	mgr.registerType(dt.in());

	//create Topic
	char topic_name[] = "DataExclusive";
	mgr.createTopic(topic_name);

	//create Subscriber
	mgr.createSubscriber();

	char sTopicName[] = "MyDataTopic";
	// create subscription filter
	snprintf(buf, MAX_MSG_LEN, "node_path = '%s'",
			nodePath);
	DDS::String_var sFilter = DDS::string_dup(buf);

	// Filter expression
	StringSeq sSeqExpr;
	sSeqExpr.length(0);
	// create topic
	mgr.createContentFilteredTopic(sTopicName, sFilter.in(), sSeqExpr);
	// create Filtered DataReader
	cout << "=== [VirtualEquipment] Subscription filter : "
			<< sFilter << endl;
	mgr.createReader(true);

	DataReader_var dreader = mgr.getReader();
	DataDataReader_var PhysiologyDataReader = DataDataReader::_narrow(
			dreader.in());
	checkHandle(PhysiologyDataReader.in(), "DataDataReader::_narrow");

	cout << "=== [VirtualEquipment] Ready ..." << endl;

	bool closed = false;
	ReturnCode_t status = -1;
	int count = 0;
	while (!closed)
	{
		status = PhysiologyDataReader->take(msgList, infoSeq,
				LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE,
				ANY_INSTANCE_STATE);
		checkStatus(status, "DataDataReader::take");
		for (DDS::ULong i = 0; i < msgList.length(); i++) {
			if (infoSeq[i].valid_data) {
				if (msgList[i].dbl == -1.0f) {
					closed = true;
					break;
				}
			}
			cout
					<< "=== [VirtualEquipment] Received data :  ("
					<< msgList[i].node_path << ", " << msgList[i].dbl << ')'
					<< endl;
		}

		status = PhysiologyDataReader->return_loan(msgList, infoSeq);
		checkStatus(status, "DataDataReader::return_loan");
		os_nanoSleep(delay_200ms);
		++count;
	}

	cout << "=== [VirtualEquipment] Simulation stopped."
			<< endl;

	//cleanup
	mgr.deleteReader(PhysiologyDataReader.in());
	mgr.deleteSubscriber();
	mgr.deleteFilteredTopic();
	mgr.deleteTopic();
	mgr.deleteParticipant();

	return 0;

}
