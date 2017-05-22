#include "stdafx.h"

#include "ccpp_AMM.h"

#include "AMM/DDSEntityManager.h"
#include "AMM/BioGearsWrapper.h"

using namespace DDS;
using namespace AMM::Physiology;

int main(int argc, char *argv[]) {
	char configFile[] = "OSPL_URI=file://ospl.xml";
	putenv(configFile);

	bool closed = false;
	int action;

	cout << "=== [PhysiologyManager] Starting up." << endl;

	cout << "=== [PhysiologyManager][DDS] Initializing DDS entity manager."
			<< endl;

	bool autodispose_unregistered_instances = true;
	DDSEntityManager mgr(autodispose_unregistered_instances);

	// create domain participant
	char partition_name[] = "AMM";
	mgr.createParticipant(partition_name);
	//create type
	DataTypeSupport_var mt = new DataTypeSupport();
	mgr.registerType(mt.in());

	//create Topic
	char topic_name[] = "Data";
	mgr.createTopic(topic_name);

	//create Publisher
	mgr.createPublisher();

	// create DataWriters
	mgr.createWriters();
	DataWriter_var dwriter = mgr.getWriter();
	DataDataWriter_var LifecycleWriter = DataDataWriter::_narrow(dwriter.in());
	DataWriter_var dwriter_stopper = mgr.getWriter_stopper();
	DataDataWriter_var LifecycleWriter_stopper = DataDataWriter::_narrow(
			dwriter_stopper.in());

	// Create BioGears
	cout << "=== [PhysiologyManager] Spinning up BioGears thread..." << endl;
	BioGearsWrapper bg("wrapper.log");
	bg.SetStateFile("./states/StandardMale@0s.xml");
	bg.InitializeEngine();

	cout << "=== [PhysiologyManager] Ready and waiting..." << endl;
	ReturnCode_t status;

	do {
		cout
				<< " [1]Status, [2]Advance Time Tick, [3]Start, [4]Stop, [5]Run, [6]Publish data, [7]Quit "
				<< endl;
		cin >> action;
		switch (action) {
		case 1:
			bg.Status();
			break;
		case 2:
			cout << "== Advancing time one tick" << endl;
			bg.AdvanceTimeTick();
			break;
		case 3:
			cout << "== Starting simulation..." << endl;
			bg.StartSimulation();
			break;
		case 4:
			cout << "== Stopping simulation..." << endl;
			bg.StopSimulation();
			break;
		case 5:
			cout << "== Run based on Simulation Manager ticks..." << endl;
			break;
		case 6:
			cout << "== Outputting ECG..." << endl;
			Data *dataInstance = new Data();
			dataInstance->node_path = "ECG";
			dataInstance->unit = "mV";
			dataInstance->dbl = 123.4;
			status = LifecycleWriter->write(*dataInstance, DDS::HANDLE_NIL);
			status = LifecycleWriter->dispose(*dataInstance, DDS::HANDLE_NIL);
			checkStatus(status, "DataDataWriter::write");
			delete dataInstance;

			/*cout << "== Outputting HR..." << endl;
			Data *dataInstance = bg.GetNodePath("HR");
			status = LifecycleWriter->write(*dataInstance, DDS::HANDLE_NIL);
			status = LifecycleWriter->dispose(*dataInstance, DDS::HANDLE_NIL);
			delete dataInstance;*/
			break;
		case 7:
			closed = true;
			break;
		}

	} while (!closed);

	cout << "=== [PhysiologyManager] Sending -1 values to all topics." << endl;

	cout << "=== [PhysiologyManager] Shutting down data writers." << endl;
	cout << "=== [PhysiologyManager] Data channel closed." << endl;

	cout << "=== [PhysiologyManager] Shutting down BioGears." << endl;
	bg.StopSimulation();
	bg.Shutdown();

	cout << "=== [PhysiologyManager] Shutting down." << endl;

	return 0;
}
