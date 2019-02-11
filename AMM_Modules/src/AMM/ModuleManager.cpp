#include "ModuleManager.h"

using namespace std;
using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace sqlite;

namespace AMM {
    ModuleManager::ModuleManager() {
        m_runThread = false;
    }

    bool ModuleManager::isRunning() { return m_runThread; }

    void ModuleManager::Start() {
        using namespace AMM::Capability;
        std::string nodeString(nodeName);

        auto *status_sub_listener = new DDS_Listeners::StatusSubListener();
        auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();
        auto *log_sub_listener = new DDS_Listeners::LogRecordSubListener();
        auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
        auto *tick_sub_listener = new DDS_Listeners::TickSubListener();
        auto *physmod_sub_listener = new DDS_Listeners::PhysiologyModificationListener();
        auto *rendermod_sub_listener = new DDS_Listeners::RenderModificationListener();

        auto *mmL = new ModuleManagerListener();

        status_sub_listener->SetUpstream(mmL);
        config_sub_listener->SetUpstream(mmL);
        log_sub_listener->SetUpstream(mmL);
        tick_sub_listener->SetUpstream(mmL);
        physmod_sub_listener->SetUpstream(mmL);
        rendermod_sub_listener->SetUpstream(mmL);
        command_sub_listener->SetUpstream(mmL);

        mgr->InitializeReliableSubscriber(AMM::DataTypes::statusTopic, &mgr->StatusType,
                                          status_sub_listener);
        mgr->InitializeReliableSubscriber(AMM::DataTypes::configurationTopic, &mgr->ConfigurationType,
                                          config_sub_listener);
        mgr->InitializeReliableSubscriber(AMM::DataTypes::logRecordTopic, &mgr->LogRecordType,
                                          log_sub_listener);
        mgr->InitializeSubscriber(AMM::DataTypes::tickTopic, &mgr->TickType, tick_sub_listener);
        mgr->InitializeReliableSubscriber(AMM::DataTypes::renderModTopic, &mgr->RenderModificationType,
                                          rendermod_sub_listener);
        mgr->InitializeReliableSubscriber(AMM::DataTypes::physModTopic, &mgr->PhysiologyModificationType,
                                          physmod_sub_listener);
        mgr->InitializeReliableSubscriber(AMM::DataTypes::commandTopic, &mgr->CommandType,
                                          command_sub_listener);

        currentScenario = mgr->GetScenario();

        // Publish module configuration once we've set all our publishers and listeners
        mgr->PublishModuleConfiguration(
                mgr->module_id, nodeString, "Vcom3D", "Module_Manager", "00001", "0.0.1",
                mgr->GetCapabilitiesAsString(
                        "mule1/module_capabilities/module_manager_capabilities.xml"));

        // This announces that we're available for configuration
        mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);

        if (!m_runThread) {
            m_runThread = true;
            //            m_thread = std::thread(&ModuleManager::RunLoop, this);
        }
    }

    void ModuleManager::RunLoop() {
        while (m_runThread) {

        }
    }

    void ModuleManager::ShowStatus() {}

    void ModuleManager::Cleanup() {}

    void ModuleManager::Shutdown() {
        if (m_runThread) {
            m_runThread = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            //            m_thread.join();
        }

        Cleanup();
    }

}
