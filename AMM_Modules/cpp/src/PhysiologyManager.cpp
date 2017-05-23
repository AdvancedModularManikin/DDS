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
	bool paused = false;
	int action;
	char partition_name[] = "AMM";

	vector<string> node_path_subscriptions = { "ECG", "HR" };

	Duration_t timeout = { 0, 200000000 };
	TickSeq tickList;
	SampleInfoSeq infoSeq;
	bool autodispose_unregistered_instances = true;
	ReturnCode_t status;
	int lastFrame = 0;

	cout << "=== [PhysiologyManager] Starting up." << endl;

	/**
	 * BioGears initialization
	 */
	cout << "=== [PhysiologyManager][BG] Spinning up BioGears thread..."
			<< endl;
	BioGearsWrapper bg("wrapper.log");
	bg.SetStateFile("./states/StandardMale@0s.xml");
	bg.InitializeEngine();

	/**
	 * Physiology Data DDS Entity manager
	 */
	cout
			<< "=== [PhysiologyManager][DDS] Initializing DDS entity manager (DATA)."
			<< endl;
	DDSEntityManager mgr(autodispose_unregistered_instances);
	mgr.createParticipant(partition_name);
	DataTypeSupport_var mt = new DataTypeSupport();
	mgr.registerType(mt.in());
	char data_topic_name[] = "Data";
	mgr.createTopic(data_topic_name);
	mgr.createPublisher();
	mgr.createWriters();
	DataWriter_var dwriter = mgr.getWriter();
	DataDataWriter_var LifecycleWriter = DataDataWriter::_narrow(dwriter.in());
	DataWriter_var dwriter_stopper = mgr.getWriter_stopper();
	DataDataWriter_var LifecycleWriter_stopper = DataDataWriter::_narrow(
			dwriter_stopper.in());
	Data *dataInstance = new Data();

	/**
	 * Tick DDS Entity Manager
	 */
	cout
			<< "=== [PhysiologyManager][DDS] Initializing DDS entity manager (TICK)."
			<< endl;
	DDSEntityManager mgr2(autodispose_unregistered_instances);
	mgr2.createParticipant(partition_name);
	TickTypeSupport_var st = new TickTypeSupport();
	mgr2.registerType(st.in());
	char tick_topic_name[] = "Tick";
	mgr2.createTopic(tick_topic_name);
	mgr2.createSubscriber();
	mgr2.createReader();
	DataReader_var dreader = mgr2.getReader();
	TickDataReader_var TickReader = TickDataReader::_narrow(dreader.in());
	checkHandle(TickReader.in(), "TickDataReader::_narrow");

	/**
	 * Listener code:
	 TickDataListener *tickListener = new TickDataListener();
	 tickListener->m_TickReader = TickDataReader::_narrow(dreader.in());
	 checkHandle(tickListener->m_TickReader.in(), "TickDataReader::_narrow");

	 DDS::StatusMask mask = DDS::DATA_AVAILABLE_STATUS
	 | DDS::REQUESTED_DEADLINE_MISSED_STATUS;
	 tickListener->m_TickReader->set_listener(tickListener, mask);
	 tickListener->m_closed = false;
	 **/

	cout << "=== [PhysiologyManager] Ready and waiting..." << endl << endl;

	do {
		cout
				<< " [1]Status, [2]Advance Time Tick, [3]Start, [4]Stop, [5]Run, [6]Publish data, [7]Quit "
				<< endl;
		cin >> action;

		if (action == 1) {
			bg.Status();
		} else if (action == 2) {
			cout << " == Advancing time one tick" << endl;
			bg.AdvanceTimeTick();
		} else if (action == 3) {
			cout << " == Starting simulation..." << endl;
			bg.StartSimulation();
		} else if (action == 4) {
			cout << " == Stopping simulation..." << endl;
			bg.StopSimulation();
		} else if (action == 5) {
			cout
					<< " == Run based on Simulation Manager ticks (use Sim Manager to pause/stop)";
			cout.flush();
			/**
			 * Testing the listener code --
			 DDS::WaitSet_var ws = new DDS::WaitSet();
			 ws->attach_condition(tickListener->m_guardCond);
			 DDS::ConditionSeq condSeq;
			 while (!tickListener->m_closed) {
			 ws->wait(condSeq, timeout);
			 tickListener->m_guardCond->set_trigger_value(false);
			 }
			 **/
			while (!closed) {
				status = TickReader->take(tickList, infoSeq, LENGTH_UNLIMITED,
						ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
				checkStatus(status, "TickDataReader::take");
				for (DDS::ULong j = 0; j < tickList.length(); j++) {
					if (tickList[j].frame == -1) {
						cout << "[SHUTDOWN]";
						// Shut down signal
						closed = true;
						dataInstance = bg.GetNodePath("EXIT");
						status = LifecycleWriter->write(*dataInstance,
								DDS::HANDLE_NIL);
						status = LifecycleWriter->dispose(*dataInstance,
								DDS::HANDLE_NIL);
						checkStatus(status, "DataDataWriter::write");
					} else if (tickList[j].frame == -2) {
						// Pause signal
						cout << "[PAUSE]";
						cout.flush();
						paused = true;
					} else if (!paused) {
						if (tickList[j].frame <= lastFrame) {
							cout << "x";
						} else {
							cout << ".";
						}
						cout.flush();
						lastFrame = tickList[j].frame;

						// Per-frame stuff happens here
						bg.AdvanceTimeTick();
						for (auto np : node_path_subscriptions) {
							dataInstance = bg.GetNodePath(np);
							status = LifecycleWriter->write(*dataInstance,
									DDS::HANDLE_NIL);
							status = LifecycleWriter->dispose(*dataInstance,
									DDS::HANDLE_NIL);
							checkStatus(status, "DataDataWriter::write");
						}
					}
				}
				status = TickReader->return_loan(tickList, infoSeq);
				checkStatus(status, "TickDataReader::return_loan");
			}
			cout << endl;
		} else if (action == 6) {
			for (auto np : node_path_subscriptions) {
				cout << " == Outputting " << np << " ..." << endl;
				dataInstance = bg.GetNodePath(np);
				status = LifecycleWriter->write(*dataInstance, DDS::HANDLE_NIL);
				status = LifecycleWriter->dispose(*dataInstance,
						DDS::HANDLE_NIL);
				checkStatus(status, "DataDataWriter::write");
			}

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

	cout << "=== [PhysiologyManager][DDS] Shutting down DDS Connections."
			<< endl;
	/**
	 * Shutdown Physiology Data DDS Entity Manager
	 */
	mgr.deleteWriter(LifecycleWriter_stopper.in());
	mgr.deletePublisher();
	mgr.deleteTopic();
	mgr.deleteParticipant();

	/**
	 * Shutdown Tick DDS Entity Manager
	 */
	mgr2.deleteReader(TickReader.in());
	mgr2.deleteSubscriber();
	mgr2.deleteTopic();
	mgr2.deleteParticipant();

	cout << "=== [PhysiologyManager][BG] Shutting down BioGears." << endl;
	bg.StopSimulation();
	bg.Shutdown();

	cout << "=== [PhysiologyManager] Exiting." << endl;

	return 0;
}
