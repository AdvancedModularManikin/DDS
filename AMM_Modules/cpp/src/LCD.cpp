#include "stdafx.h"
#include "ccpp_AMM.h"
#include "AMM/DDSEntityManager.h"

#include "upm/jhd1313m1.hpp"

#define I2C_BUS  0
#define RGB_WHT 0xff,0xff,0xff
#define RGB_RED 0xff,0x00,0x00
#define RGB_GRN 0x00,0xff,0x00
#define RGB_BLU 0x00,0x00,0xff
#define SLEEP_TIME 2

using namespace std;
using namespace DDS;
using namespace AMM::Physiology;
upm::Jhd1313m1* lcd;

void display(string str1, string str2, int red, int green, int blue)
{
	lcd->clear();
	lcd->setColor(red, green, blue);
	lcd->setCursor(0,0); /* first row */
	lcd->write(str1);
	lcd->setCursor(1,2); /* second row */
	lcd->write(str2);
	sleep(SLEEP_TIME);
}

int main(int argc, char *argv[]) {
	char configFile[] = "OSPL_URI=file://ospl.xml";
	putenv(configFile);

	std::vector<std::string> node_paths;

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

	// LCD accessor
	lcd = new upm::Jhd1313m1(I2C_BUS, 0x3e, 0x62);
	
	// create domain participant
	mgr.createParticipant(partitionName);

	//create type
	NodeTypeSupport_var dt = new NodeTypeSupport();
	mgr.registerType(dt.in());

	//create Topic
	mgr.createTopic(topicName);

	//create Subscriber
	mgr.createSubscriber();

	mgr.createReader(false);

	DataReader_var dreader = mgr.getReader();
	NodeDataReader_var PhysiologyDataReader = NodeDataReader::_narrow(dreader.in());

	checkHandle(PhysiologyDataReader.in(), "NodeDataReader::_narrow");

	cout << "=== [LCD-HeartRate] Ready ..." << endl;
	ostringstream displayString;

	while (!closed) {
		status = PhysiologyDataReader->take(msgList, infoSeq, LENGTH_UNLIMITED, ANY_SAMPLE_STATE, ANY_VIEW_STATE,
				ANY_INSTANCE_STATE);
		checkStatus(status, "DataReader::take");
		for (DDS::ULong i = 0; i < msgList.length(); i++) {
			if (infoSeq[i].valid_data) {
				if (msgList[i].dbl == -1.0f || msgList[i].nodepath == "-1") {
					closed = true;
					break;
				}
				if (msgList[i].nodepath == "HR") {
					displayString << msgList[i].dbl;
					display("HR", displayString.str(), RGB_RED);
				}
			}

		}

		status = PhysiologyDataReader->return_loan(msgList, infoSeq);
		checkStatus(status, "DataReader::return_loan");
		os_nanoSleep(delay_200ms);
	}

	cout << "=== [LCD-HeartRate] Simulation stopped." << endl;

	//cleanup
	mgr.deleteReader(PhysiologyDataReader.in());
	mgr.deleteSubscriber();
	mgr.deleteTopic();
	mgr.deleteParticipant();
	
	delete lcd;

	return 0;

}
