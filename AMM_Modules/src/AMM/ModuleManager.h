#pragma once

#include "stdafx.h"

#include "AMM/DataTypes.h"

#include "AMM/DDS_Manager.h"

#include "AMM/Listeners/DDS_Listeners.h"

#include "AMM/Listeners/ListenerInterface.h"

#include "AMM/BaseLogger.h"

#include <tinyxml2.h>

#include "thirdparty/sqlite_modern_cpp.h"

using namespace std;
using namespace std::chrono;

using namespace sqlite;
using namespace tinyxml2;

using namespace eprosima;
using namespace eprosima::fastrtps;

namespace AMM {
    class ModuleManager : public ListenerInterface {

    public:

        ModuleManager();

        ~ModuleManager() override = default;

        void Start();

        void RunLoop();

        void Shutdown();

        bool isRunning();

        void ShowStatus();

        void Cleanup();

        void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) override;

        void onNewStatusData(AMM::Capability::Status st) override;

        void onNewConfigData(AMM::Capability::Configuration cfg) override;

        std::string currentScenario;

    protected:

        std::thread m_thread;
        std::mutex m_mutex;
        bool m_runThread;
        const char *nodeName = "AMM_ModuleManager";
        DDS_Manager *mgr = new DDS_Manager(nodeName);


    };

}