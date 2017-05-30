#include "stdafx.h"

#include "ccpp_AMM.h"

#include "AMM/DDSEntityManager.h"
#include "AMM/BioGearsThread.h"

using namespace DDS;
using namespace AMM::Physiology;
using namespace AMM::Simulation;
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

	bool closed = false;
	bool paused = false;
	int action;
	char partition_name[] = "AMM";
	char data_topic_name[] = "Data";
	char tick_topic_name[] = "Tick";
	char command_topic_name[] = "Command";

	vector<string> node_path_subscriptions = { "ECG", "HR" };

	Duration_t timeout = { 0, 200000000 };
	TickSeq tickList;
	CommandSeq cmdList;
	SampleInfoSeq infoSeq;
	bool autodispose_unregistered_instances = true;
	ReturnCode_t status;
	int lastFrame = 0;

	cout << "=== [PhysiologyManager] Starting up." << endl;

	/**
	 * BioGears initialization
	 */
	cout << "=== [PhysiologyManager] Spinning up BioGears thread..." << endl;
	BioGearsThread bg("biogears.log");

	/**
	 * Physiology Data DDS Entity manager
	 */
	cout
			<< "=== [PhysiologyManager][DDS] Initializing DDS entity manager (DATA)."
			<< endl;
	DDSEntityManager mgr(autodispose_unregistered_instances);
	mgr.createParticipant(partition_name);
	NodeTypeSupport_var mt = new NodeTypeSupport();
	mgr.registerType(mt.in());

	mgr.createTopic(data_topic_name);
	mgr.createPublisher();
	mgr.createWriters();
	DataWriter_var dwriter = mgr.getWriter();
	NodeDataWriter_var LifecycleWriter = NodeDataWriter::_narrow(dwriter.in());
	DataWriter_var dwriter_stopper = mgr.getWriter_stopper();
	NodeDataWriter_var LifecycleWriter_stopper = NodeDataWriter::_narrow(
			dwriter_stopper.in());
	Node *dataInstance = new Node();

	/**
	 * Tick DDS Entity Manager
	 */
	cout
			<< "=== [PhysiologyManager][DDS] Initializing DDS entity manager (TICK)."
			<< endl;
	DDSEntityManager tickMgr(autodispose_unregistered_instances);
	tickMgr.createParticipant(partition_name);
	TickTypeSupport_var tt = new TickTypeSupport();
	tickMgr.registerType(tt.in());
	tickMgr.createTopic(tick_topic_name);
	tickMgr.createSubscriber();
	tickMgr.createReader();
	DataReader_var tdreader = tickMgr.getReader();
	TickDataReader_var TickReader = TickDataReader::_narrow(tdreader.in());
	checkHandle(TickReader.in(), "TickDataReader::_narrow");

	/**
	 * Command DDS Entity Manager
	 */
	cout
			<< "=== [PhysiologyManager][DDS] Initializing DDS entity manager (COMMAND)."
			<< endl;
	DDSEntityManager cmdMgr(autodispose_unregistered_instances);
	cmdMgr.createParticipant(partition_name);
	CommandTypeSupport_var ct = new CommandTypeSupport();
	cmdMgr.registerType(ct.in());
	cmdMgr.createTopic(command_topic_name);
	cmdMgr.createSubscriber();
	cmdMgr.createReader();
	DataReader_var cdreader = cmdMgr.getReader();
	CommandDataReader_var CommandReader = CommandDataReader::_narrow(
			cdreader.in());
	checkHandle(CommandReader.in(), "CommandDataReader::_narrow");

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
			while (!closed) {

				// Check for a command
				status = CommandReader->take(cmdList, infoSeq, LENGTH_UNLIMITED,
						ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
				checkStatus(status, "CommandDataReader::take");
				for (DDS::ULong j = 0; j < cmdList.length(); j++) {
					bg.ExecuteCommand(cmdList[j].message.m_ptr);
				}
				status = CommandReader->return_loan(cmdList, infoSeq);
				checkStatus(status, "CommandDataReader::return_loan");

				// Check for a tick
				status = TickReader->take(tickList, infoSeq, LENGTH_UNLIMITED,
						ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);
				checkStatus(status, "TickDataReader::take");
				for (DDS::ULong j = 0; j < tickList.length(); j++) {
					if (tickList[j].frame == -1) {
						cout << "[SHUTDOWN]";
						// Shut down signal
						closed = true;
						dataInstance->nodepath = DDS::string_dup("EXIT");
						dataInstance->dbl = -1;
						dataInstance->frame = lastFrame;
						status = LifecycleWriter->write(*dataInstance,
								DDS::HANDLE_NIL);
						status = LifecycleWriter->dispose(*dataInstance,
								DDS::HANDLE_NIL);
						checkStatus(status, "NodeDataWriter::write");
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
						for (string np : node_path_subscriptions) {
							dataInstance->nodepath = DDS::string_dup(np.c_str());
							dataInstance->dbl = bg.GetNodePath(np);
							dataInstance->frame = lastFrame;
							status = LifecycleWriter->write(*dataInstance,
									DDS::HANDLE_NIL);
							status = LifecycleWriter->dispose(*dataInstance,
									DDS::HANDLE_NIL);
							checkStatus(status, "NodeDataWriter::write");
						}
					}
				}
				status = TickReader->return_loan(tickList, infoSeq);
				checkStatus(status, "TickDataReader::return_loan");

			}
			cout << endl;
		} else if (action == 6) {
			for (string np : node_path_subscriptions) {
				cout << " == Outputting " << np << " ..." << endl;
				dataInstance->nodepath = DDS::string_dup(np.c_str());
				dataInstance->dbl = bg.GetNodePath(np);
				dataInstance->frame = lastFrame;
				status = LifecycleWriter->write(*dataInstance, DDS::HANDLE_NIL);
				status = LifecycleWriter->dispose(*dataInstance,
						DDS::HANDLE_NIL);
				checkStatus(status, "NodeDataWriter::write");
			}

		} else if (action == 7) {
			closed = true;
		} else {

		}
	} while (!closed);

	cout << "=== [PhysiologyManager] Sending -1 values to all topics." << endl;
	dataInstance->dbl = -1;
	status = LifecycleWriter->write(*dataInstance, DDS::HANDLE_NIL);
	status = LifecycleWriter->dispose(*dataInstance, DDS::HANDLE_NIL);
	checkStatus(status, "NodeDataWriter::write");

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
	tickMgr.deleteReader(TickReader.in());
	tickMgr.deleteSubscriber();
	tickMgr.deleteTopic();
	tickMgr.deleteParticipant();

	cout << "=== [PhysiologyManager][BG] Shutting down BioGears." << endl;
	bg.StopSimulation();
	bg.Shutdown();

	cout << "=== [PhysiologyManager] Exiting." << endl;

	return 0;
}
