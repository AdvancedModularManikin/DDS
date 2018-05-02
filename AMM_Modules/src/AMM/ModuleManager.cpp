#include "ModuleManager.h"

using namespace std;
using namespace std::chrono;
using namespace sqlite;

ModuleManager::ModuleManager() : m_db("amm.db") {
    m_runThread = false;

    auto *pub_listener = new DDS_Listeners::PubListener();

    auto *status_sub_listener = new DDS_Listeners::StatusSubListener();
    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();

    status_sub_listener->SetUpstream(this);
    config_sub_listener->SetUpstream(this);

    status_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::statusTopic, AMM::DataTypes::getStatusType(),
                                                  status_sub_listener);
    config_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::configurationTopic,
                                                  AMM::DataTypes::getConfigurationType(),
                                                  config_sub_listener);

    config_publisher = mgr->InitializePublisher(AMM::DataTypes::configurationTopic,
                                                AMM::DataTypes::getConfigurationType(), pub_listener);

}

bool ModuleManager::isRunning() {
    return m_runThread;
}

void ModuleManager::Start() {
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


// Listener events
void ModuleManager::onNewStatusData(AMM::Capability::Status s, SampleInfo_t *info) {
// store in db
    eprosima::fastrtps::rtps::GUID_t changeGuid;
    iHandle2GUID(changeGuid, info->iHandle);
    cout << "[MM] Received a status message " << endl;
    cout << "[MM] From... " << changeGuid.entityId << endl;
    cout << "[MM]\tValue: " << s.status_value() << endl;
    cout << "[MM]\tCapabilities: " << s.capability() << endl;
    // Iterate the vector || cout << "[MM]\tMessage: " << s.message() << endl;
    cout << "[MM]\t---" << endl;


    // m_db << "insert into node_status (node_id, capability, status, encounter_id) values (?,?,?,?)" << 1 << "jack";
}

void ModuleManager::onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t *info) {
// store in db
    eprosima::fastrtps::rtps::GUID_t changeGuid;
    iHandle2GUID(changeGuid, info->iHandle);
    cout << "[MM] Received a capability config message " << endl;
    cout << "[MM] From... " << changeGuid.entityId << endl;
    cout << "[MM]\tMfg: " << cfg.manufacturer() << endl;
    cout << "[MM]\tModel: " << cfg.model() << endl;
    cout << "[MM]\tSerial Number: " << cfg.serial_number() << endl;
    cout << "[MM]\tVersion: " << cfg.version() << endl;
    cout << "[MM]\tCapabilities: " << cfg.capabilities() << endl;
    cout << "[MM]\t---" << endl;
}
