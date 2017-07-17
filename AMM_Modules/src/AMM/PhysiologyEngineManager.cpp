#include "PhysiologyEngineManager.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

using namespace AMM;
using namespace AMM::Simulation;
using namespace AMM::Physiology;
using namespace AMM::PatientAction::BioGears;
using namespace DDS;
using namespace std;
using namespace std::chrono;

PhysiologyEngineManager::PhysiologyEngineManager() :
		m_thread() {

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
	dwriter_stopper = mgr.getWriter_stopper();
	LifecycleWriter_stopper = NodeDataWriter::_narrow(dwriter_stopper.in());

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
	nodePathMap = bg->nodePathTable;
	m_runThread = false;

}

bool PhysiologyEngineManager::isRunning() {
	return m_runThread;
}

void PhysiologyEngineManager::TickLoop() {
	while (m_runThread) {
		ReadCommands();
		ReadTicks();
	}
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
			StopTickSimulation();
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

void PhysiologyEngineManager::PrintAllCurrentData() {
	nodePathMap = bg->nodePathTable;
	std::map<std::string, double (BioGearsThread::*)()>::iterator it = nodePathMap.begin();
	while (it != nodePathMap.end()) {
		std::string node = it->first;
		double dbl = bg->GetNodePath(node);
		cout << node << "\t\t\t" << dbl << endl;
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
	// LifecycleWriter->dispose(*dataInstance, DDS::HANDLE_NIL);
	delete dataInstance;
}

void PhysiologyEngineManager::PublishData(bool force = false) {
	std::map<std::string, double (BioGearsThread::*)()>::iterator it = nodePathMap.begin();
	while (it != nodePathMap.end()) {
		if ((std::find(bg->highFrequencyNodes.begin(), bg->highFrequencyNodes.end(), it->first) != bg->highFrequencyNodes.end())
				|| (lastFrame % 3) == 0 || force) {
			WriteNodeData(it->first);
		}
		it++;
	}
}

void PhysiologyEngineManager::StartTickSimulation() {
	if (!m_runThread) {
		m_runThread = true;
		m_thread = std::thread(&PhysiologyEngineManager::TickLoop, this);
	}
}

void PhysiologyEngineManager::StopTickSimulation() {
	if (m_runThread) {
		m_mutex.lock();
		m_runThread = false;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		m_mutex.unlock();
		m_thread.detach();
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

	cout << "=== [PhysiologyManager][BG] Shutting down BioGears." << endl;
	bg->Shutdown();

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

}
