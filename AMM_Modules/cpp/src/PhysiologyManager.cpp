#include "stdafx.h"

#include "ccpp_AMM.h"

#include "AMM/DDSEntityManager.h"
#include "AMM/BioGearsThread.h"

using namespace DDS;
using namespace AMM::Physiology;

int main(int argc, char *argv[]) {
	cout << "=== [PhysiologyManager] Starting up." << endl;

	os_time delay_1s = { 1, 0 };

	DDSEntityManager mgr;

	cout << "=== [PhysiologyManager][DDS] Initializing DDS entity manager." << endl;
	// create domain participant
	char partition_name[] = "ContentFilteredTopic AMM";
	mgr.createParticipant(partition_name);

	DataTypeSupport_var dt = new DataTypeSupport();
	mgr.registerType(dt.in());

	//create Topic
	char topic_name[] = "DataExclusive";
	mgr.createTopic(topic_name);

	//create Publisher
	mgr.createPublisher();

	// create DataWriter
	mgr.createWriter();
	DataWriter_var dWriter = mgr.getWriter();
	DataDataWriter_var ContentFilteredTopicDataWriter = DataDataWriter::_narrow(
			dWriter.in());

	Data outputData;

	InstanceHandle_t dataHandle =
			ContentFilteredTopicDataWriter->register_instance(outputData);

	// Publish Events
	ReturnCode_t status;

	SampleInfoSeq infoSeq;
	Duration_t timeout = { 0, 200000000 };

	// Create BioGears
	cout << "=== [PhysiologyManager] Spinning up BioGears thread..." << endl;
	BioGearsThread bgThread("biogears.log");


	cout << "=== [PhysiologyManager] Ready and waiting..." << endl;

	bool closed = false;
	int count = 0;
	int realCount = 0;
	time_t startTime;
	time_t runTime;
	time_t stopTime;
	double elapsedTime;
	time(&startTime);
	int action;
	double rate;

	do {
		cout
				<< " [1]Status, [2]Advance Time Tick, [3]Start, [4]Stop, [5]Run, [6]Publish data, [7]Quit "
				<< endl;
		cin >> action;
		switch (action) {
		case 1:
			bgThread.Status();
			break;
		case 2:
			cout << "== Advancing time one tick" << endl;
			bgThread.AdvanceTimeTick();
			break;
		case 3:
			cout << "== Starting simulation..." << endl;
			bgThread.StartSimulation();
			break;
		case 4:
			cout << "== Stopping simulation..." << endl;
			bgThread.StopSimulation();
			break;
		case 5:
			cout << "== Run based on Simulation Manager ticks..." << endl;
			break;
		case 6:
			cout << "== Outputting ECG..." << endl;
			outputData = bgThread.GetNodePath("ECG");
			status = ContentFilteredTopicDataWriter->write(outputData, dataHandle);
			checkStatus(status, "DataExclusiveWriter::write");

			cout << "== Outputting HR..." << endl;
			outputData = bgThread.GetNodePath("HR");
			status = ContentFilteredTopicDataWriter->write(outputData, dataHandle);
			checkStatus(status, "DataExclusiveWriter::write");
			break;
		case 7:
			closed = true;
			break;
		}

	} while (!closed);


	cout << "=== [PhysiologyManager] Sending -1 values to all topics." << endl;

	// Send -1 to everything, signaling a shutdown
	outputData.node_path = "*";
	outputData.dbl =  - 1;
	ContentFilteredTopicDataWriter->write(outputData, dataHandle);

	cout << "=== [PhysiologyManager] Shutting down data writers." << endl;

	/* Unregister the instances */
	ContentFilteredTopicDataWriter->unregister_instance(outputData, dataHandle);

	/* Remove the DataWriters */
	mgr.deleteWriter(ContentFilteredTopicDataWriter.in());

	/* Remove the Publisher. */
	mgr.deletePublisher();

	/* Remove the Topics. */
	mgr.deleteTopic();

	/* Remove Participant. */
	mgr.deleteParticipant();

	cout << "=== [PhysiologyManager] Data channel closed." << endl;

	os_nanoSleep(delay_1s);

	cout << "=== [PhysiologyManager] Shutting down BioGears." << endl;
	bgThread.StopSimulation();
	bgThread.Shutdown();


	cout << "=== [PhysiologyManager] Shutting down." << endl;

	return 0;
}
