#include "DDSEntityManager.h"
#include "ccpp_AMM.h"

#include <mutex>
#include <thread>

using namespace AMM;
using namespace AMM::Simulation;
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

	void Cleanup();
	void TickLoop();

protected:
	std::thread m_thread;
	std::mutex m_mutex;
	bool m_runThread;

	DDSEntityManager mgr;
	DataWriter_var dwriter;
	TickDataWriter_var TickWriter;
	int tickCount = 0;
	int sampleRate = 50;
	Tick tick;
	Tick pauseTick;
	Tick shutdownTick;
};

