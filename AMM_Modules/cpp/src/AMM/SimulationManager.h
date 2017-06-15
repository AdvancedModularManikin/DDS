#include "DDSEntityManager.h"
#include "ccpp_AMM.h"

#include <mutex>
#include <thread>

using namespace AMM;
using namespace AMM::Simulation;
using namespace AMM::Physiology;
using namespace AMM::PatientAction::BioGears;
using namespace DDS;
using namespace std;
using namespace std::chrono;

class SimulationManager {

public:
	SimulationManager();
	virtual ~SimulationManager();

	void StartSimulation();
	void StopSimulation();
	void Shutdown();

	void SetSampleRate(int rate);
	int GetSampleRate();

	bool isRunning();

	int GetTickCount();

	void SendCommand(const std::string &command);

	void Cleanup();
	void TickLoop();

protected:

	std::thread m_thread;
	std::mutex m_mutex;
	bool m_runThread;

	DDSEntityManager tickMgr;
	DDSEntityManager cmdMgr;

	DataWriter_var tickdwriter;
	TickDataWriter_var TickWriter;

	DataWriter_var cmddwriter;
	CommandDataWriter_var CommandWriter;

	int tickCount = 0;
	int sampleRate = 50;
	Tick tick;
	Tick pauseTick;
	Tick shutdownTick;
};

