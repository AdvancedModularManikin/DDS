#include "stdafx.h"

#include "ccpp_AMM.h"

#include "AMM/DDSEntityManager.h"
#include "AMM/BioGearsWrapper.h"
#include "AMM/TickDataListener.h"

using namespace DDS;
using namespace AMM::Physiology;
using namespace AMM::Simulation;

int main(int argc, char *argv[]) {
	char configFile[] = "OSPL_URI=file://ospl.xml";
	putenv(configFile);

	bool closed = false;
	int action;

	Duration_t timeout = { 0, 200000000 };

	TickSeq tickList;
	SampleInfoSeq infoSeq;

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

	TickTypeSupport_var st = new TickTypeSupport();
	mgr.registerType(st.in());

	//create Topic
	char data_topic_name[] = "Data";
	mgr.createTopic(data_topic_name);

	char tick_topic_name[] = "Tick";
	mgr.createTopic(tick_topic_name);

	//create Publisher
	mgr.createPublisher();

	// create DataWriters
	mgr.createWriters();
	DataWriter_var dwriter = mgr.getWriter();
	DataDataWriter_var LifecycleWriter = DataDataWriter::_narrow(dwriter.in());
	DataWriter_var dwriter_stopper = mgr.getWriter_stopper();
	DataDataWriter_var LifecycleWriter_stopper = DataDataWriter::_narrow(
			dwriter_stopper.in());

	// Create readers
	DataReader_var dreader = mgr.getReader();
	TickDataListener *tickListener = new TickDataListener();
	tickListener->m_TickReader = TickDataReader::_narrow(dreader.in());
	checkHandle(tickListener->m_TickReader.in(), "TickDataReader::_narrow");

	cout << "=== [ListenerDataSubscriber] set_listener" << endl;
	DDS::StatusMask mask = DDS::DATA_AVAILABLE_STATUS
			| DDS::REQUESTED_DEADLINE_MISSED_STATUS;
	tickListener->m_TickReader->set_listener(tickListener, mask);
	tickListener->m_closed = false;

	// Create BioGears
	cout << "=== [PhysiologyManager] Spinning up BioGears thread..." << endl;
	BioGearsWrapper bg("wrapper.log");
	bg.SetStateFile("./states/StandardMale@0s.xml");
	bg.InitializeEngine();

	cout << "=== [PhysiologyManager] Ready and waiting..." << endl;
	ReturnCode_t status;

	Data *dataInstance = new Data();

	do {
		cout
				<< " [1]Status, [2]Advance Time Tick, [3]Start, [4]Stop, [5]Run, [6]Publish data, [7]Quit "
				<< endl;
		cin >> action;
		if (action == 1) {
			bg.Status();
		} else if (action == 2) {
			cout << "== Advancing time one tick" << endl;
			bg.AdvanceTimeTick();
		} else if (action == 3) {
			cout << "== Starting simulation..." << endl;
			bg.StartSimulation();
		} else if (action == 4) {
			cout << "== Stopping simulation..." << endl;
			bg.StopSimulation();
		} else if (action == 5) {
			cout << "== Run based on Simulation Manager ticks..." << endl;
			DDS::WaitSet_var ws = new DDS::WaitSet();
			ws->attach_condition(tickListener->m_guardCond);
			DDS::ConditionSeq condSeq;
			while (!tickListener->m_closed) {
				ws->wait(condSeq, timeout);
				tickListener->m_guardCond->set_trigger_value(false);
			}
		} else if (action == 6) {
			cout << "== Outputting ECG..." << endl;
			dataInstance = bg.GetNodePath("ECG");
			status = LifecycleWriter->write(*dataInstance, DDS::HANDLE_NIL);
			status = LifecycleWriter->dispose(*dataInstance, DDS::HANDLE_NIL);
			checkStatus(status, "DataDataWriter::write");

			cout << "== Outputting HR..." << endl;
			dataInstance = bg.GetNodePath("HR");
			status = LifecycleWriter->write(*dataInstance, DDS::HANDLE_NIL);
			status = LifecycleWriter->dispose(*dataInstance, DDS::HANDLE_NIL);
			checkStatus(status, "DataDataWriter::write");
		} else if (action == 7) {
			closed = true;
		} else {

		}
	} while (!closed);

	cout << "=== [PhysiologyManager] Sending -1 values to all topics." << endl;
	dataInstance = bg.GetNodePath("EXIT");
	status = LifecycleWriter->write(*dataInstance, DDS::HANDLE_NIL);
	status = LifecycleWriter->dispose(*dataInstance, DDS::HANDLE_NIL);
	checkStatus(status, "DataDataWriter::write");

	cout << "=== [PhysiologyManager] Shutting down data writers." << endl;
	mgr.deleteWriter(LifecycleWriter_stopper.in());

	/* Remove the Publisher. */
	mgr.deletePublisher();

	/* Remove the Topics. */
	mgr.deleteTopic();

	mgr.deleteParticipant();

	cout << "=== [PhysiologyManager] Data channel closed." << endl;

	cout << "=== [PhysiologyManager] Shutting down BioGears." << endl;
	bg.StopSimulation();
	bg.Shutdown();

	cout << "=== [PhysiologyManager] Shutting down." << endl;

	return 0;
}
