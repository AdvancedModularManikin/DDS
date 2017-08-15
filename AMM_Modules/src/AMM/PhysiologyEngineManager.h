#include "DDS_Manager.h"

#include "BioGearsThread.h"

#include <mutex>
#include <thread>

using namespace AMM;
using namespace AMM::Simulation;
using namespace AMM::Physiology;
using namespace AMM::PatientAction::BioGears;
using namespace std;
using namespace std::chrono;

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
	void PrintAllCurrentData();
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

	bool autodispose_unregistered_instances = true;


	std::map<std::string, double (BioGearsThread::*)()> nodePathMap;

protected:
	Publisher* tick_publisher;
	Publisher* command_publisher;
	Publisher* node_publisher;

	Subscriber* tick_subscriber;
	Subscriber* command_subscriber;
	Subscriber* node_subscriber;
	DDS_Manager* mgr = new DDS_Manager();

	BioGearsThread* bg = new BioGearsThread("biogears.log", "./states/StandardMale@0s.xml");

	std::thread m_thread;
		std::mutex m_mutex;
		bool m_runThread;

	class PubListener : public PublisherListener
	{
	public:
		PubListener() : n_matched(0){};
		~PubListener(){};
		void onPublicationMatched(Publisher* pub,MatchingInfo& info);
		int n_matched;
	} pub_listener;

	class NodeSubListener : public SubscriberListener
	{
	public:
		NodeSubListener() : n_matched(0),n_msg(0){};
		~NodeSubListener(){};
		void onSubscriptionMatched(Subscriber* sub,MatchingInfo& info);
		void onNewDataMessage(Subscriber* sub);
		SampleInfo_t m_info;
		int n_matched;
		int n_msg;
	} node_sub_listener;

	class CommandSubListener : public SubscriberListener
	{
	public:
		CommandSubListener() : n_matched(0),n_msg(0){};
		~CommandSubListener(){};
		void onSubscriptionMatched(Subscriber* sub,MatchingInfo& info);
		void onNewDataMessage(Subscriber* sub);
		SampleInfo_t m_info;
		int n_matched;
		int n_msg;
	} command_sub_listener;

	class TickSubListener : public SubscriberListener
	{
	public:
		TickSubListener() : n_matched(0),n_msg(0){};
		~TickSubListener(){};
		void onSubscriptionMatched(Subscriber* sub,MatchingInfo& info);
		void onNewDataMessage(Subscriber* sub);
		SampleInfo_t m_info;
		int n_matched;
		int n_msg;
	} tick_sub_listener;

};

