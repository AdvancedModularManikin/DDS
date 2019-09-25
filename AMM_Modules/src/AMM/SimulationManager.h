#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include <fastcdr/Cdr.h>

#include "AMM/BaseLogger.h"
#include "AMM/DDS_Log_Appender.h"

#include "AMM/DDS_Manager.h"
#include "AMM/DataTypes.h"
#include "AMM/Listeners/DDS_Listeners.h"
#include "AMM/Listeners/ListenerInterface.h"

using namespace std;
using namespace std::chrono;

namespace AMM {
    class SimulationManager : public ListenerInterface {

    public:
        SimulationManager();

        ~SimulationManager() override = default;

        void StartSimulation();

        void StopSimulation();

        void Shutdown();

        void SetSampleRate(int rate);

        int GetSampleRate();

        bool isRunning();

        int GetTickCount();

        void SendCommand(const std::string &command);

        void SendCommand(const AMM::Physiology::CMD type,
                         eprosima::fastcdr::Cdr &data);

        void Cleanup();

        void TickLoop();

        void onNewCommandData(AMM::PatientAction::BioGears::Command c,
                              SampleInfo_t *info) override;

        std::string currentScenario;

    protected:
        std::thread m_thread;
        std::mutex m_mutex;
        bool m_runThread;

        const char *nodeName = "AMM_SimulationManager";

        DDS_Manager *mgr = new DDS_Manager(nodeName);

        Subscriber *command_subscriber;

        Publisher *command_publisher;
        Publisher *physiology_publisher;
        Publisher *tick_publisher;

        int tickCount = 0;
        int sampleRate = 50;
    };
}