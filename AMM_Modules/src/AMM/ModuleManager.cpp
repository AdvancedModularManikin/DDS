#include "ModuleManager.h"

using namespace std;
using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace sqlite;

ModuleManager::ModuleManager() {
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    auto *status_sub_listener = new DDS_Listeners::StatusSubListener();
    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();

    command_sub_listener->SetUpstream(this);
    status_sub_listener->SetUpstream(this);
    config_sub_listener->SetUpstream(this);

    mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(),
                              command_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::statusTopic, AMM::DataTypes::getStatusType(), status_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::configurationTopic, AMM::DataTypes::getConfigurationType(),
                              config_sub_listener);

    currentScenario = mgr->GetScenario();
    m_runThread = false;
}

bool ModuleManager::isRunning() {
    return m_runThread;
}

void ModuleManager::Start() {
    std::string nodeString(nodeName);
    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            nodeString,
            "Vcom3D",
            "Module_Manager",
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/module_manager_capabilities.xml")
    );
    mgr->SetStatus(nodeString, OPERATIONAL);

    if (!m_runThread) {
        m_runThread = true;
        m_thread = std::thread(&ModuleManager::RunLoop, this);

    }
}

void ModuleManager::RunLoop() {
    while (m_runThread) {
        m_mutex.lock();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        // do work
        m_mutex.unlock();
    }
}

void ModuleManager::ShowStatus() {

}

void ModuleManager::Cleanup() {

}

void ModuleManager::Shutdown() {
    if (m_runThread) {
        m_runThread = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        m_thread.join();
    }

    Cleanup();

}


void ModuleManager::onNewCommandData(AMM::PatientAction::BioGears::Command c) {
    if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
        std::string value = c.message().substr(sysPrefix.size());
        LOG_TRACE << "[ModuleManager][COMMAND] We received a SYSTEM action: " << value;
    } else {
        LOG_TRACE << "[ModuleManager][COMMAND] Command received: " << c.message();
    }

}

void ModuleManager::onNewStatusData(AMM::Capability::Status st) {
    ostringstream statusValue;
    statusValue << st.status_value();
    try {
        database db("amm.db");
        db << "replace into module_status (module_id, module_name, capability, status) values (?,?,?,?);"
           << st.module_id()
           << st.module_name()
           << st.capability()
           << statusValue.str();
    } catch (exception &e) {
        LOG_ERROR << "[ModuleManager][STATUS]" << e.what();
    }
}

void ModuleManager::onNewConfigData(AMM::Capability::Configuration cfg) {
 try {
        database db("amm.db");
        db
                << "replace into module_capabilities (module_id, module_name, manufacturer, model, serial_number, version, capabilities) values (?,?,?,?,?,?,?);"
                << cfg.module_id()
                << cfg.module_name()
                << cfg.manufacturer()
                << cfg.model()
                << cfg.serial_number()
                << cfg.version()
                << cfg.capabilities();
    } catch (exception &e) {
        LOG_ERROR << "[ModuleManager][CONFIG]" << e.what();
    };

}
