#include "PhysiologyEngineManager.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

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
	 * Physiology Data DDS Entity manager
	 */
	cout << "=== [PhysiologyManager][DDS] Initializing DDS entity manager (DATA)." << endl;

	mgr.createParticipant("AMM");
	NodeTypeSupport_var mt = new NodeTypeSupport();
	mgr.registerType(mt.in());
	mgr.createTopic("Data");
	mgr.createPublisher();
	mgr.createWriters();
	dwriter = mgr.getWriter();
	LifecycleWriter = NodeDataWriter::_narrow(dwriter.in());

	/**
	 * Tick DDS Entity Manager
	 */
	cout << "=== [PhysiologyManager][DDS] Initializing DDS entity manager (TICK)." << endl;
	tickMgr.createParticipant("AMM");
	TickTypeSupport_var tt = new TickTypeSupport();
	tickMgr.registerType(tt.in());
	tickMgr.createTopic("Tick");
	tickMgr.createSubscriber();
	tickMgr.createReader();
	tdreader = tickMgr.getReader();
	TickReader = TickDataReader::_narrow(tdreader.in());
	checkHandle(TickReader.in(), "TickDataReader::_narrow");

	/**
	 * Command DDS Entity Manager
	 */
	cout << "=== [PhysiologyManager][DDS] Initializing DDS entity manager (COMMAND)." << endl;
	cmdMgr.createParticipant("AMM");
	CommandTypeSupport_var ct = new CommandTypeSupport();
	cmdMgr.registerType(ct.in());
	cmdMgr.createTopic("Command");
	cmdMgr.createSubscriber();
	cmdMgr.createReader();
	cdreader = cmdMgr.getReader();
	CommandReader = CommandDataReader::_narrow(cdreader.in());
	checkHandle(CommandReader.in(), "CommandDataReader::_narrow");

	nodePathMap = bg->nodePathTable;

}

bool PhysiologyEngineManager::isRunning() {
	return !closed;
}

void PhysiologyEngineManager::TickListenerLoop() {
	tickListener = new TickDataListener();
	tickListener->m_pe = this;
	tickListener->m_TickReader = TickDataReader::_narrow(tdreader.in());
	DDS::StatusMask mask = DDS::DATA_AVAILABLE_STATUS | DDS::REQUESTED_DEADLINE_MISSED_STATUS;
	tickListener->m_TickReader->set_listener(tickListener, mask);
	cout << "=== [ListenerDataSubscriber] Ready ..." << endl;
	tickListener->m_closed = false;

	DDS::WaitSet_var ws = new DDS::WaitSet();
	ws->attach_condition(tickListener->m_guardCond);

	while (!tickListener->m_closed) {
		ws->wait(condSeq, timeout);
		tickListener->m_guardCond->set_trigger_value(false);
	}

}

void PhysiologyEngineManager::TickLoop() {
	ReadCommands();
	ReadTicks();
}

void PhysiologyEngineManager::ReadCommands() {
	CommandReader->take(cmdList, infoSeq, LENGTH_UNLIMITED, NOT_READ_SAMPLE_STATE, NEW_VIEW_STATE, ANY_INSTANCE_STATE);
	for (DDS::ULong j = 0; j < cmdList.length(); j++) {
		bg->ExecuteCommand(cmdList[j].message.m_ptr);
	}
	CommandReader->return_loan(cmdList, infoSeq);
}

void PhysiologyEngineManager::SendShutdown() {
	Node *dataInstance = new Node();
	dataInstance->nodepath = DDS::string_dup("EXIT");
	dataInstance->dbl = -1;
	dataInstance->frame = -1;
	LifecycleWriter->write(*dataInstance, DDS::HANDLE_NIL);
	LifecycleWriter->dispose(*dataInstance, DDS::HANDLE_NIL);
	delete dataInstance;
}

void PhysiologyEngineManager::ReadTicks() {
	// Check for a tick
	TickReader->take(tickList, infoSeq, LENGTH_UNLIMITED, NOT_READ_SAMPLE_STATE, NEW_VIEW_STATE, ANY_INSTANCE_STATE);
	for (DDS::ULong j = 0; j < tickList.length(); j++) {
		if (tickList[j].frame == -1) {
			cout << "[SHUTDOWN]";
			closed = true;
			SendShutdown();
		} else if (tickList[j].frame == -2) {
			// Pause signal
			cout << "[PAUSE]";
			paused = true;
		} else if (tickList[j].frame > 0 || !paused) {
			if (paused) {
				cout << "[RESUME]";
				paused = false;
			}

			// Did we get a frame out of order?  Just mark it with an X for now.
			if (tickList[j].frame <= lastFrame) {
				cout << "x";
			} else {
				cout << ".";
			}
			lastFrame = tickList[j].frame;

			// Per-frame stuff happens here
			bg->AdvanceTimeTick();
			PublishData(false);
		}
		cout.flush();
	}
	TickReader->return_loan(tickList, infoSeq);
}

void PhysiologyEngineManager::PrintAvailableNodePaths() {
	nodePathMap = bg->nodePathTable;

	std::map<std::string, double (BioGearsThread::*)()>::iterator it = nodePathMap.begin();

	while (it != nodePathMap.end()) {
		std::string word = it->first;
		cout << word << endl;
		it++;
	}
}

int PhysiologyEngineManager::GetNodePathCount() {
	return nodePathMap.size();
}

void PhysiologyEngineManager::WriteNodeData(string node) {
	Node *dataInstance = new Node();
	dataInstance->nodepath = DDS::string_dup(node.c_str());
	dataInstance->dbl = bg->GetNodePath(node);
	dataInstance->frame = lastFrame;
	LifecycleWriter->write(*dataInstance, DDS::HANDLE_NIL);
	LifecycleWriter->dispose(*dataInstance, DDS::HANDLE_NIL);
	delete dataInstance;
}

void PhysiologyEngineManager::PublishData(bool force = false) {
	std::map<std::string, double (BioGearsThread::*)()>::iterator it = nodePathMap.begin();
	while (it != nodePathMap.end()) {

		if ((std::find(bg->highFrequencyNodes.begin(), bg->highFrequencyNodes.end(), it->first) != bg->highFrequencyNodes.end())
				|| (lastFrame % 10) == 0 || force) {
			WriteNodeData(it->first);
		}

		it++;
	}
}

void PhysiologyEngineManager::StartSimulation() {
	bg->StartSimulation();
}

void PhysiologyEngineManager::StopSimulation() {
	bg->StopSimulation();
}

void PhysiologyEngineManager::AdvanceTimeTick() {
	bg->AdvanceTimeTick();
}

int PhysiologyEngineManager::GetTickCount() {
	return lastFrame;
}
void PhysiologyEngineManager::Status() {
	bg->Status();
}

void PhysiologyEngineManager::Shutdown() {

	cout << "=== [PhysiologyManager] Sending -1 values to all topics." << endl;
	SendShutdown();

	cout << "=== [PhysiologyManager][DDS] Shutting down DDS Connections." << endl;
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
	bg->Shutdown();
}
