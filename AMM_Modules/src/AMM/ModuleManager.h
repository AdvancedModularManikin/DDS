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

    void onNewStatusData(AMM::Capability::Status s, SampleInfo_t *info) override;

    void onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t *info) override;


protected:

    std::thread m_thread;
    std::mutex m_mutex;
    bool m_runThread;

    sqlite::database m_db;

    const char *nodeName = "AMM_ModuleManager";

    DDS_Manager *mgr = new DDS_Manager(nodeName);

    Subscriber *status_subscriber;
    Subscriber *config_subscriber;
    Publisher *config_publisher;


};

