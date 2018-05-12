#include "ModuleManager.h"

using namespace std;
using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace sqlite;

database db("amm.db");


ModuleManager::ModuleManager() {
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    command_sub_listener->SetUpstream(this);
    mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(),
                                                   command_sub_listener);

    currentScenario = mgr->GetScenario();

    m_runThread = false;
}

bool ModuleManager::isRunning() {
    return m_runThread;
}

void ModuleManager::Start() {
    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            "Vcom3D",
            "Module_Manager",
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/module_manager_capabilities.xml")
    );
    mgr->SetStatus(OPERATIONAL);

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


void ModuleManager::onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) {
    cout << "Module manager fired command " << c.message() << endl;
    if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
        std::string value = c.message().substr(sysPrefix.size());
        cout << "[MM] We received a SYSTEM action: " << value << endl;
    } else {
        cout << "[MM] Command received: " << c.message() << endl;
    }

}
