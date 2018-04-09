#pragma once

#include <mutex>
#include <thread>

#include "DataTypes.h"

#include "DDS_Manager.h"

#include "DDS_Listeners.h"

#include "ListenerInterface.h"

using namespace std;
using namespace std::chrono;

class ModuleManager : public ListenerInterface {

public:

    ModuleManager();

    ~ModuleManager() override = default;;        

	void Start();

	void RunLoop();
	
   void Shutdown();

    bool isRunning();

    void Cleanup();

    void onNewNodeData(AMM::Physiology::Node n) override;

    void onNewTickData(AMM::Simulation::Tick t) override;

    void onNewCommandData(AMM::PatientAction::BioGears::Command c) override;

protected:

    std::thread m_thread;
    std::mutex m_mutex;
    bool m_runThread;

	const char* nodeName = "AMM_ModuleManager";

    DDS_Manager *mgr = new DDS_Manager(nodeName);

    Subscriber *command_subscriber;
    Publisher *command_publisher;
    Publisher *tick_publisher;

};

