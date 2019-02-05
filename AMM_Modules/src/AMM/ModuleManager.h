#pragma once

#include "AMM/BaseLogger.h"

#include "AMM/DataTypes.h"

#include "AMM/DDS_Manager.h"

#include "AMM/Utility.h"

#include "AMM/Listeners/ListenerInterface.h"

#include "AMM/Listeners/DDS_Listeners.h"

#include "AMM/Listeners/ModuleManagerListener.h"

#include "thirdparty/sqlite_modern_cpp.h"

#include <tinyxml2.h>

using namespace std;
using namespace std::chrono;

using namespace sqlite;
using namespace tinyxml2;

using namespace eprosima;
using namespace eprosima::fastrtps;

namespace AMM {
    class ModuleManager  {

    public:
        ModuleManager();

        ~ModuleManager() = default;

        void Start();

        void RunLoop();

        void Shutdown();

        bool isRunning();

        void ShowStatus();

        void Cleanup();

        std::string currentScenario;


    protected:
        std::thread m_thread;
        bool m_runThread;
        const char *nodeName = "AMM_ModuleManager";
        DDS_Manager *mgr = new DDS_Manager(nodeName);
    };
}