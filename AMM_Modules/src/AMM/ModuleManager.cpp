#include "ModuleManager.h"

using namespace std;
using namespace std::chrono;
using namespace sqlite;

ModuleManager::ModuleManager() {
    m_runThread = false;
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

void ModuleManager::InitializeDB() {

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
void ModuleManager::onNewCommandData(AMM::PatientAction::BioGears::Command c) {

}

void ModuleManager::onNewNodeData(AMM::Physiology::Node n) {

}

void ModuleManager::onNewTickData(AMM::Simulation::Tick t) {

}

void ModuleManager::onNewStatusData(AMM::Capability::Status s) {

}

void ModuleManager::onNewConfigData(AMM::Capability::Configuration cfg) {

}
