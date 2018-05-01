#pragma once

#include <mutex>
#include <thread>

#include "DataTypes.h"

#include "DDS_Manager.h"

#include "DDS_Listeners.h"

#include "ListenerInterface.h"

#include "tinyxml2.h"

#include <sqlite_modern_cpp.h>

using namespace std;
using namespace std::chrono;

using namespace sqlite;
using namespace tinyxml2;

class ModuleManager : public ListenerInterface {

public:

    ModuleManager();

    ~ModuleManager() override = default;;

    void Start();

    void RunLoop();

    void Shutdown();

    bool isRunning();

    void Cleanup();

    void InitializeDB();

    void onNewNodeData(AMM::Physiology::Node n) override;

    void onNewTickData(AMM::Simulation::Tick t) override;

    void onNewCommandData(AMM::PatientAction::BioGears::Command c) override;

    void onNewStatusData(AMM::Capability::Status s) override;

    void onNewConfigData(AMM::Capability::Configuration cfg) override;

protected:

    std::thread m_thread;
    std::mutex m_mutex;
    bool m_runThread;

    const char *nodeName = "AMM_ModuleManager";

    DDS_Manager *mgr = new DDS_Manager(nodeName);

};

