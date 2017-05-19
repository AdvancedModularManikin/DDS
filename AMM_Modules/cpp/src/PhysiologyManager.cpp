#include "stdafx.h"

#include "ccpp_AMM.h"

#include "AMM/DDSEntityManager.h"
#include "AMM/BioGearsThread.h"

using namespace DDS;
using namespace AMM::Physiology;

int main(int argc, char *argv[]) {
	/* DDS entity manager */
	DDSEntityManager mgr;

	/* Others */
	const char *partitionName = "AMM";
	char topicName[] = "Data";
	os_time delay_1s = { 1, 0 };
	os_time delay_200ms = { 0, 200000000 };
	ReturnCode_t status;
	bool closed = false;
	int action;

	cout << "=== [PhysiologyManager] Starting up." << endl;

	cout << "=== [PhysiologyManager][DDS] Initializing DDS entity manager." << endl;
	mgr.createParticipant(partitionName);

	DataTypeSupport_var dt = new DataTypeSupport();
	mgr.registerType(dt.in());

	//create Topic
	mgr.createTopic(topicName);

	//create Publisher
	mgr.createPublisher();

	bool autodispose_unregistered_instances = false;
	mgr.createWriter(autodispose_unregistered_instances);

	DataWriter_var dWriter = mgr.getWriter();
	DataDataWriter_var outputDataWriter = DataDataWriter::_narrow(dWriter.in());

	Data outputData;
	Data holdData;

	InstanceHandle_t dataHandle = outputDataWriter->register_instance(outputData);

	// Create BioGears
	cout << "=== [PhysiologyManager] Spinning up BioGears thread..." << endl;
	BioGearsThread bgThread("biogears.log");
	bgThread.SetStateFile("./states/StandardMale@0s.xml");
	bgThread.Initialize();

	cout << "=== [PhysiologyManager] Ready and waiting..." << endl;

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
			holdData = bgThread.GetNodePath("ECG");
			outputData.unit = holdData.unit;
			outputData.dbl = holdData.dbl;
			outputData.str = holdData.str;
			status = outputDataWriter->write(outputData, dataHandle);
			checkStatus(status, "outputDataWriter::write");

			cout << "== Outputting HR..." << endl;
			holdData = bgThread.GetNodePath("HR");
			outputData.node_path = holdData.node_path; // DDS::string_dup("HR");
						outputData.unit = holdData.unit;
						outputData.dbl = holdData.dbl;
						outputData.str = holdData.str;
			status = outputDataWriter->write(outputData, dataHandle);
			checkStatus(status, "outputDataWriter::write");
			break;
		case 7:
			closed = true;
			break;
		}

	} while (!closed);

	cout << "=== [PhysiologyManager] Sending -1 values to all topics." << endl;

	// Signal shutdown
	 outputData.dbl = -1;
	 outputDataWriter->write(outputData, dataHandle);
	 checkStatus(status, "outputDataWriter::write");


	status = outputDataWriter->dispose(outputData, dataHandle);
	checkStatus(status, "outputDataWriter::dispose");

	status = outputDataWriter->unregister_instance(outputData, dataHandle);
	checkStatus(status, "outputDataWriter::unregister_instance");

	cout << "=== [PhysiologyManager] Shutting down data writers." << endl;
	/* Remove the DataWriters */
	mgr.deleteWriter(outputDataWriter.in());

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
