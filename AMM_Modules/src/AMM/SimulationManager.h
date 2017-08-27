#pragma once

#include <mutex>
#include <thread>

#include "DDS_Manager.h"

#include "DDS_Listeners.h"

#include "ListenerInterface.h"

using namespace std;
using namespace std::chrono;

class SimulationManager : public ListenerInterface {

public:

    SimulationManager();

    virtual ~SimulationManager() {

    };

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

    virtual void onNewNodeData(AMM::Physiology::Node n);

    virtual void onNewTickData(AMM::Simulation::Tick t);

    virtual void onNewCommandData(AMM::PatientAction::BioGears::Command c);

protected:

    std::thread m_thread;
    std::mutex m_mutex;
    bool m_runThread;

    DDS_Manager *mgr = new DDS_Manager();

    Subscriber *command_subscriber;

    int tickCount = 0;
    int sampleRate = 50;

};

