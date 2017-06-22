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
	char partition_name[] = "AMM";
	char data_topic_name[] = "Data";
	char tick_topic_name[] = "Tick";
	char command_topic_name[] = "Command";

	Duration_t timeout = { 0, 200000000 };
	TickSeq tickList;
	CommandSeq cmdList;
	SampleInfoSeq infoSeq;
	bool autodispose_unregistered_instances = true;
	ReturnCode_t status;
	int lastFrame = 0;
	bool closed = false;
	bool paused = false;
	std::map<std::string, double (BioGearsThread::*)()> nodePathMap;
	Node *dataInstance;

	PhysiologyEngine();
	virtual ~PhysiologyEngine();

	void StartSimulation();
	void StopSimulation();
	void Shutdown();

	void ReadTicks();
	void PrintAvailableNodePaths();
	void PublishData();
	bool isRunning();

	int GetTickCount();

	void SendCommand(const std::string &command);

	void Cleanup();
	void TickLoop();

protected:

	DDSEntityManager mgr;
	DDSEntityManager tickMgr;
	DDSEntityManager cmdMgr;

	NodeDataWriter_var LifecycleWriter;

	DataWriter_var cmddwriter;
	CommandDataWriter_var CommandWriter;

	TickDataReader_var TickReader;
	CommandDataReader_var CommandReader;
	BioGearsThread bg;

};

