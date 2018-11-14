#pragma once

#include <chrono>
#include <time.h>

#include <mutex>
#include <thread>

#include "AMM/DDS_Manager.h"

#include "AMM/Listeners/DDS_Listeners.h"

#include "AMM/PhysiologyThread.h"

#include "AMM/BaseLogger.h"

#include <fastcdr/Cdr.h>
#include <fastcdr/FastBuffer.h>

#include <biogears/cdm/patient/actions/SEPainStimulus.h>
#include <biogears/cdm/patient/actions/SESepsis.h>

namespace AMM {
    class PhysiologyThread;

    class PhysiologyEngineManager : public ListenerInterface {
    public:
        PhysiologyEngineManager();

        virtual ~PhysiologyEngineManager() override = default;

        PhysiologyThread *bg = new PhysiologyThread("logs/biogears.log");
        std::string stateFile;

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

        void WriteHighFrequencyNodeData(std::string node);

        void TickLoop();

        void AdvanceTimeTick();

        bool closed = false;
        bool paused = false;
        int lastFrame = 0;
        bool logging_enabled = false;

        void TestVentilator(const std::string &ventilatorSettings);

        void TestPump(const std::string &pumpSettings);

        void TestPain(const std::string &painSettings);

        void onNewNodeData(Physiology::Node n, SampleInfo_t *info) override;

        void onNewHighFrequencyNodeData(Physiology::HighFrequencyNode n, SampleInfo_t *info) override;

        void onNewTickData(Simulation::Tick ti, SampleInfo_t *info) override;

        void onNewCommandData(Physiology::Command cm, SampleInfo_t *info) override;

        void onNewCommandData(PatientAction::BioGears::Command cm,
                              SampleInfo_t *info) override;

        void onNewInstrumentData(InstrumentData i, SampleInfo_t *info) override;

        void onNewPhysiologyModificationData(Physiology::Modification,
                                             SampleInfo_t *info) override;

        std::map<std::string, double (PhysiologyThread::*)()> *nodePathMap;

    protected:
        const char *nodeName = "AMM_PhysiologyEngine";

        DDS_Manager *mgr = new DDS_Manager(nodeName);

        Publisher *hf_node_publisher;
        Publisher *node_publisher;
        Subscriber *tick_subscriber;
        Subscriber *command_subscriber;
        Subscriber *equipment_subscriber;
        Subscriber *physmod_subscriber;

        std::mutex m_mutex;
        bool m_runThread;
    };
}
