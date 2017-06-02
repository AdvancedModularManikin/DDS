#include "SimulationManager.h"

using namespace AMM;
using namespace AMM::Simulation;
using namespace DDS;
using namespace std;
using namespace std::chrono;

SimulationManager::SimulationManager() :
		m_thread() {

	char partition_name[] = "AMM";
	char tick_topic_name[] = "Tick";
	char cmd_topic_name[] = "Command";

	// Set up tick manager
	tickMgr.createParticipant(partition_name);
	TickTypeSupport_var tt = new TickTypeSupport();
	tickMgr.registerType(tt.in());
	tickMgr.createTopic(tick_topic_name);
	tickMgr.createPublisher();
	tickMgr.createWriters();
	tickdwriter = tickMgr.getWriter();
	TickWriter = TickDataWriter::_narrow(tickdwriter.in());

	pauseTick.frame = -2;
	shutdownTick.frame = -1;

	// Set up command manager
	cmdMgr.createParticipant(partition_name);
	CommandTypeSupport_var dt = new CommandTypeSupport();
	cmdMgr.registerType(dt.in());
	cmdMgr.createTopic(cmd_topic_name);
	cmdMgr.createPublisher();
	cmdMgr.createWriter();
	cmddwriter = cmdMgr.getWriter();
	CommandWriter = CommandDataWriter::_narrow(cmddwriter.in());

	m_runThread = false;

}

SimulationManager::~SimulationManager() {

}

void SimulationManager::StartSimulation() {
	if (!m_runThread) {
		m_runThread = true;
		m_thread = std::thread(&SimulationManager::TickLoop, this);
	}
}

void SimulationManager::StopSimulation() {
	if (m_runThread) {
		m_runThread = false;
		ReturnCode_t status = TickWriter->write(pauseTick, DDS::HANDLE_NIL);
		checkStatus(status, "TickDataWriter::write");
		m_thread.detach();
	}
}

int SimulationManager::GetTickCount() {
	return tickCount;
}

bool SimulationManager::isRunning() {
	return m_runThread;
}

void SimulationManager::SetSampleRate(int rate) {
	sampleRate = rate;
}

int SimulationManager::GetSampleRate() {
	return sampleRate;
}

void SimulationManager::SendCommand(const std::string &command) {
	Command cmdInstance;
	cmdInstance.message = DDS::string_dup(command.c_str());
	cout << "=== [CommandExecutor] Sending a command containing:" << endl;
	cout << "    Command : \"" << cmdInstance.message << "\"" << endl;
	ReturnCode_t status = CommandWriter->write(cmdInstance, DDS::HANDLE_NIL);
	checkStatus(status, "CommandWriter::write");
}

void SimulationManager::TickLoop() {
	using frames = duration<int64_t, ratio<1, 50>>;
	auto nextFrame = system_clock::now();
	auto lastFrame = nextFrame - frames { 1 };

	while (m_runThread) {
		this_thread::sleep_until(nextFrame);
		m_mutex.lock();
		tick.frame = tickCount++;
		ReturnCode_t status = TickWriter->write(tick, DDS::HANDLE_NIL);
		checkStatus(status, "TickDataWriter::write");
		lastFrame = nextFrame;
		nextFrame += frames { 1 };
		m_mutex.unlock();
	}
}

void SimulationManager::Cleanup() {
	tickMgr.deleteWriter(TickWriter.in());
	tickMgr.deletePublisher();
	tickMgr.deleteTopic();
	tickMgr.deleteParticipant();

	cmdMgr.deleteWriter(CommandWriter.in());
	cmdMgr.deletePublisher();
	cmdMgr.deleteTopic();
	cmdMgr.deleteParticipant();
}

void SimulationManager::Shutdown() {
	if (m_runThread) {
		m_runThread = false;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		m_thread.detach();
	}

	ReturnCode_t status = TickWriter->write(shutdownTick, DDS::HANDLE_NIL);
	checkStatus(status, "TickDataWriter::write");

	Cleanup();

	m_thread.~thread();
	std::terminate();
}

