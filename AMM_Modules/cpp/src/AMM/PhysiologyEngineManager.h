#include "DDSEntityManager.h"
#include "BioGearsThread.h"
#include "TickDataListener.h"

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

class PhysiologyEngineManager {

public:

	PhysiologyEngineManager();
	virtual ~PhysiologyEngineManager() {};

	void StartSimulation();
	void StopSimulation();
	void Shutdown();

	void PublishData(bool force);
	void PrintAvailableNodePaths();
	void Status();

	int GetNodePathCount();
	int GetTickCount();
	bool isRunning();

	void SendCommand(const std::string &command);

	void TickLoop();

	void AdvanceTimeTick();
	bool closed = false;
	bool paused = false;


private:
		void SendShutdown();
		void ReadCommands();
		void ReadTicks();
		Duration_t timeout = { 0, 200000000 };
		bool autodispose_unregistered_instances = true;
		int lastFrame = 0;

		// Initialize some data structures
		TickSeq tickList;
		CommandSeq cmdList;
		SampleInfoSeq infoSeq;

		std::map<std::string, double (BioGearsThread::*)()> nodePathMap;
		Node *dataInstance;


protected:

	DDSEntityManager mgr = new DDSEntityManager(autodispose_unregistered_instances);
	DDSEntityManager tickMgr = new DDSEntityManager(autodispose_unregistered_instances);
	DDSEntityManager cmdMgr = new DDSEntityManager(autodispose_unregistered_instances);

	NodeDataWriter_var LifecycleWriter;
	DataWriter_var cmddwriter;
	CommandDataWriter_var CommandWriter;

	TickDataReader_var TickReader;
	CommandDataReader_var CommandReader;
	BioGearsThread* bg = new BioGearsThread("biogears.log", "./states/StandardMale@0s.xml");

};

