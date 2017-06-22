#include "PhysiologyEngine.h"

using namespace AMM;
using namespace AMM::Simulation;
using namespace AMM::Physiology;
using namespace AMM::PatientAction::BioGears;
using namespace DDS;
using namespace std;
using namespace std::chrono;


PhysiologyEngineManager::PhysiologyEngineManager() {

	cout << "=== [PhysiologyManager] Starting up." << endl;

	/**
	 * BioGears initialization
	 */
	cout << "=== [PhysiologyManager] Spinning up BioGears thread..." << endl;
	bg("biogears.log");

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
	dataInstance = new Node();

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
	TickReader = TickDataReader::_narrow(tdreader.in());
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
	CommandReader = CommandDataReader::_narrow(
			cdreader.in());
	checkHandle(CommandReader.in(), "CommandDataReader::_narrow");

	nodePathMap =
			bg.nodePathTable;

}

void PhysiologyEngineManager::ReadTicks() {
	status = CommandReader->take(cmdList, infoSeq, LENGTH_UNLIMITED,
							NOT_READ_SAMPLE_STATE, NEW_VIEW_STATE,
							ANY_INSTANCE_STATE);
					checkStatus(status, "CommandDataReader::take");
					for (DDS::ULong j = 0; j < cmdList.length(); j++) {
						bg.ExecuteCommand(cmdList[j].message.m_ptr);
					}
					status = CommandReader->return_loan(cmdList, infoSeq);
					checkStatus(status, "CommandDataReader::return_loan");

					// Check for a tick
					status = TickReader->take(tickList, infoSeq, LENGTH_UNLIMITED,
							NOT_READ_SAMPLE_STATE, NEW_VIEW_STATE,
							ANY_INSTANCE_STATE);
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

							std::map<std::string, double (BioGearsThread::*)()>::iterator it =
									nodePathMap.begin();

							while (it != nodePathMap.end()) {
								// If it's a high frequency node or it's frame-time
								if (std::find(bg.highFrequencyNodes.begin(),
										bg.highFrequencyNodes.end(), it->first)
										!= bg.highFrequencyNodes.end()
										|| ((lastFrame % 10) == 0)) {
									dataInstance->nodepath = DDS::string_dup(
											it->first.c_str());
									dataInstance->dbl = bg.GetNodePath(it->first);
									dataInstance->frame = lastFrame;

									status = LifecycleWriter->write(*dataInstance,
											DDS::HANDLE_NIL);
									status = LifecycleWriter->dispose(*dataInstance,
											DDS::HANDLE_NIL);
									checkStatus(status, "NodeDataWriter::write");

								}

								it++;
							}

						}
					}
					status = TickReader->return_loan(tickList, infoSeq);
					checkStatus(status, "TickDataReader::return_loan");
}

void PhysiologyEngineManager::PrintAvailableNodePaths() {
	cout << " == Outputting available node path maps." << endl;

				nodePathMap =
						bg.nodePathTable;

				std::map<std::string, double (BioGearsThread::*)()>::iterator it =
						nodePathMap.begin();

				while (it != nodePathMap.end()) {
					std::string word = it->first;
					cout << word << endl;
					it++;
				}
}

void PhysiologyEngineManager::PublishData() {
	std::map<std::string, double (BioGearsThread::*)()>::iterator it =
						nodePathMap.begin();
				while (it != nodePathMap.end()) {
					cout << " == Outputting " << it->first << " ..." << endl;
					dataInstance->nodepath = DDS::string_dup(it->first.c_str());
					dataInstance->dbl = bg.GetNodePath(it->first);
					dataInstance->frame = lastFrame;

					status = LifecycleWriter->write(*dataInstance, DDS::HANDLE_NIL);
					status = LifecycleWriter->dispose(*dataInstance,
							DDS::HANDLE_NIL);
					checkStatus(status, "NodeDataWriter::write");
					it++;
				}
}

void PhysiologyEngineManager::Shutdown() {
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
		mgr.deleteWriters();
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
		bg.Shutdown();
}
