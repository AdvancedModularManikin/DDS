#pragma once

#include "DDS_Manager.h"

#include "DDS_Listeners.h"

#include "ListenerInterface.h"

#include "PhysiologyThread.h"

#include <mutex>
#include <thread>

using namespace std;
using namespace std::chrono;

class PhysiologyEngineManager : public ListenerInterface {

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

    void onNewNodeData(AMM::Physiology::Node n);

    void onNewTickData(AMM::Simulation::Tick t);

    void onNewCommandData(AMM::PatientAction::BioGears::Command c);

private:
    bool autodispose_unregistered_instances = true;
    std::map<std::string, double (PhysiologyThread::*)()> nodePathMap;

protected:
    DDS_Manager *mgr = new DDS_Manager();

    Publisher *node_publisher;
    Subscriber *tick_subscriber;
    Subscriber *command_subscriber;

    PhysiologyThread *bg = new PhysiologyThread("biogears.log", "./states/StandardMale@0s.xml");

    std::thread m_thread;
    std::mutex m_mutex;
    bool m_runThread;

};

