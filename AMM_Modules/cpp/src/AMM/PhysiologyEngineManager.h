#include "DDSEntityManager.h"
#include "ccpp_AMM.h"

#include "BioGearsThread.h"

#include <mutex>
#include <thread>

using namespace AMM;
using namespace AMM::Simulation;
using namespace AMM::Physiology;
using namespace AMM::PatientAction::BioGears;
using namespace DDS;
using namespace std;
using namespace std::chrono;

class TickDataListener;

class PhysiologyEngineManager {

public:

	PhysiologyEngineManager();
	virtual ~PhysiologyEngineManager() {
	};

	void StartSimulation();
	void StopSimulation();
	void Shutdown();

	void StartTickSimulation();
	void StopTickSimulation();


	void PublishData(bool force);
	void PrintAvailableNodePaths();
	void Status();

	int GetNodePathCount();
	int GetTickCount();
	bool isRunning();

	void SendCommand(const std::string &command);
	void SendShutdown();
	void WriteNodeData(string node);
	void TickLoop();

	void AdvanceTimeTick();
	bool closed = false;
	bool paused = false;
	int lastFrame = 0;

private:
	void ReadCommands();
	void ReadTicks();
	Duration_t timeout = { 0, 200000000 };
	bool autodispose_unregistered_instances = true;

	// Initialize some data structures
	TickSeq tickList;
	CommandSeq cmdList;
	SampleInfoSeq infoSeq;

	std::map<std::string, double (BioGearsThread::*)()> nodePathMap;

protected:

	DDSEntityManager mgr = new DDSEntityManager(autodispose_unregistered_instances);
	DDSEntityManager tickMgr = new DDSEntityManager(autodispose_unregistered_instances);
	DDSEntityManager cmdMgr = new DDSEntityManager(autodispose_unregistered_instances);

	DataWriter_var dwriter;
	DataWriter_var dwriter_stopper;

	DataReader_var tdreader;
	DataReader_var cdreader;

	NodeDataWriter_var LifecycleWriter;
	NodeDataWriter_var LifecycleWriter_stopper;

	DataWriter_var cmddwriter;
	CommandDataWriter_var CommandWriter;

	TickDataReader_var TickReader;
	CommandDataReader_var CommandReader;
	BioGearsThread* bg = new BioGearsThread("biogears.log", "./states/StandardMale@0s.xml");

	std::thread m_thread;
		std::mutex m_mutex;
		bool m_runThread;

};

