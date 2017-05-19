#include "BioGearsThread.h"

#include "PhysiologyInterface.h"

using namespace AMM::Physiology;

BioGearsThread::BioGearsThread(const std::string& logFile) : m_thread() {
	m_bg = CreatePhysiologyEngine(logFile);
	m_runThread = false;
}

BioGearsThread::~BioGearsThread() {
	m_runThread = false;
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

void BioGearsThread::Initialize() {
	if (!m_bg->LoadState(_stateFile)) {
		m_bg->GetLogger()->Error("Could not load state, check the error");
		return;
	}
}

void BioGearsThread::Shutdown() {
	m_runThread = false;
	std::this_thread::sleep_for(std::chrono::seconds(2));
	m_thread.~thread();
	std::terminate();
}

void BioGearsThread::StartSimulation() {
	m_runThread = true;
	m_thread = std::thread(&BioGearsThread::AdvanceSimTime, this);
}

void BioGearsThread::StopSimulation() {
	m_runThread = false;
	m_thread.detach();
}

void BioGearsThread::AdvanceSimTime() {
	while (m_runThread) {
		m_mutex.lock();
		m_bg->AdvanceModelTime();
		m_mutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(25)); // let other things happen
	}
}

void BioGearsThread::AdvanceTimeTick() {
	m_mutex.lock();
	m_runThread = true;
	m_bg->AdvanceModelTime();
	m_runThread = false;
	m_mutex.unlock();
}

void BioGearsThread::SetStateFile(const std::string& stateFile) {
	_stateFile = stateFile;
}

Data BioGearsThread::GetNodePath(const std::string& nodePath) {

	Data outputData;

	if (nodePath == "ECG") {
		outputData.node_path = "ECG";
		outputData.unit = "mV";
		outputData.dbl = m_bg->GetECGWaveform();
		outputData.str = "";
	}

	if (nodePath == "HR") {
		outputData.node_path = "HR";
		outputData.unit = "bpm";
		outputData.dbl = m_bg->GetHeartRate();
		outputData.str = "";
	}

	if (nodePath == "EXIT") {
		outputData.node_path = "EXIT";
		outputData.unit = "-1";
		outputData.dbl = -1;
		outputData.str = "-1";
	}

	return outputData;
}

void BioGearsThread::Status() { // On demand call to print vitals to the screen
	m_mutex.lock();
	m_bg->Status();
	m_mutex.unlock();
}
