#pragma once

#include "AMM/DDS_Manager.h"

#include "AMM/Listeners/DDS_Listeners.h"

#include "AMM/Listeners/ListenerInterface.h"

#include "AMM/PhysiologyThread.h"

#include "AMM/BaseLogger.h"

#include <chrono>
#include <time.h>

#include <mutex>
#include <thread>

namespace AMM {
    class PhysiologyThread;

    class PhysiologyEngineManager : public ListenerInterface {
    public:
        PhysiologyEngineManager();
        virtual ~PhysiologyEngineManager() override = default;

        AMM::PhysiologyThread *bg = new PhysiologyThread("logs/biogears.log");

        void SetLogging(bool logging_enabled);
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

        void SendShutdown();
        void WriteNodeData(std::string node);
        void TickLoop();
        void AdvanceTimeTick();

        bool closed = false;
        bool paused = false;
        int lastFrame = 0;
        bool logging_enabled = false;
        std::string get_filename_date(void);
        std::string get_random_string(size_t length);

        void TestVentilator(const std::string &ventilatorSettings);

        void onNewNodeData(AMM::Physiology::Node n) override;
        void onNewTickData(AMM::Simulation::Tick ti) override;
        void onNewCommandData(AMM::PatientAction::BioGears::Command cm) override;
        void onNewInstrumentData(AMM::InstrumentData i) override;

        std::map<std::string, double (AMM::PhysiologyThread::*)()> *nodePathMap;

    protected:
        const char *nodeName = "AMM_PhysiologyEngine";

        AMM::DDS_Manager *mgr = new AMM::DDS_Manager(nodeName);

        Publisher *node_publisher;
        Subscriber *tick_subscriber;
        Subscriber *command_subscriber;
        Subscriber *equipment_subscriber;

        std::mutex m_mutex;
        bool m_runThread;

    };

}
