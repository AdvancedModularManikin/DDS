#pragma once

#include <mutex>
#include <thread>

#include "ccpp_AMM.h"

#include "PhysiologyInterface.h"


// Forward declare what we will use in our thread
class PhysiologyInterface;

using namespace AMM::Physiology;

class BioGearsThread {

public:
	BioGearsThread(const std::string& logFile);
	virtual ~BioGearsThread();

	std::string _stateFile = nullptr;

	void StartSimulation();
	void StopSimulation();
	void Shutdown();

	void Initialize();
	void SetStateFile(const std::string& stateFile);

	void AdvanceTimeTick();
	Data GetNodePath(const std::string& nodePath);

	void Status();

	Logger* GetLogger() {
		return m_bg->GetLogger();
	}

protected:
	void AdvanceSimTime();
	std::unique_ptr<PhysiologyInterface> m_bg;
	std::thread m_thread;
	std::mutex m_mutex;
	bool m_runThread;
};

