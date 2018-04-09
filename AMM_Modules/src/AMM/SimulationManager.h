#pragma once

#include <mutex>
#include <thread>

#include "DataTypes.h"

#include "DDS_Manager.h"

#include "DDS_Listeners.h"

#include "ListenerInterface.h"

using namespace std;
using namespace std::chrono;

class SimulationManager : public ListenerInterface {

public:

    SimulationManager();

    ~SimulationManager() override = default;;

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

    void onNewNodeData(AMM::Physiology::Node n) override;

    void onNewTickData(AMM::Simulation::Tick t) override;

    void onNewCommandData(AMM::PatientAction::BioGears::Command c) override;

protected:

    std::thread m_thread;
    std::mutex m_mutex;
    bool m_runThread;

    const char* nodeName = "AMM_SimulationManager";

    DDS_Manager *mgr = new DDS_Manager(nodeName);

    Subscriber *command_subscriber;

    Publisher *command_publisher;
    Publisher *tick_publisher;

    int tickCount = 0;
    int sampleRate = 50;

};

