#include "SimulationManager.h"

using namespace std;
using namespace std::chrono;

SimulationManager::SimulationManager() {

    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    command_sub_listener->SetUpstream(this);

    command_subscriber = mgr->InitializeCommandSubscriber(command_sub_listener);
    auto *pub_listener = new DDS_Listeners::PubListener();

    tick_publisher = mgr->InitializeTickPublisher(pub_listener);
    command_publisher = mgr->InitializeCommandPublisher(pub_listener);

    m_runThread = false;

}

void SimulationManager::StartSimulation() {
    if (!m_runThread) {
        m_runThread = true;
        m_thread = std::thread(&SimulationManager::TickLoop, this);
    }
}

void SimulationManager::StopSimulation() {
    if (m_runThread) {
        m_mutex.lock();
        m_runThread = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        m_mutex.unlock();
        m_thread.detach();
    }
}

int SimulationManager::GetTickCount() {
    return tickCount;
}

bool SimulationManager::isRunning() {
    return m_runThread;
}

void SimulationManager::SetSampleRate(int rate) {
    sampleRate = rate;
}

int SimulationManager::GetSampleRate() {
    return sampleRate;
}

void SimulationManager::SendCommand(const std::string &command) {
    cout << endl << "=== [SimManager][CommandExecutor] Sending a command:" << command << endl;
    AMM::PatientAction::BioGears::Command cmdInstance;
    cmdInstance.message(command);
    command_publisher->write(&cmdInstance);
}

void SimulationManager::TickLoop() {
    using frames = duration<int64_t, ratio<1, 50>>;
    auto nextFrame = system_clock::now();
    auto lastFrame = nextFrame - frames {1};

    while (m_runThread) {

        this_thread::sleep_until(nextFrame);
        m_mutex.lock();

        AMM::Simulation::Tick tickInstance;
        tickInstance.frame(tickCount++);
        tick_publisher->write(&tickInstance);

        lastFrame = nextFrame;
        nextFrame += frames {1};
        m_mutex.unlock();
    }
}

void SimulationManager::Cleanup() {

}

void SimulationManager::Shutdown() {
    if (m_runThread) {
        m_runThread = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        m_thread.join();
    }

    // mgr->SendTick(-1);
    AMM::Simulation::Tick tickInstance;
    tickInstance.frame(-1);
    tick_publisher->write(&tickInstance);

    Cleanup();

}


// Listener events
void SimulationManager::onNewCommandData(AMM::PatientAction::BioGears::Command c) {
    cout << "[SimManager] Command received: " << c.message() << endl;
}

void SimulationManager::onNewNodeData(AMM::Physiology::Node n) {

}

void SimulationManager::onNewTickData(AMM::Simulation::Tick t) {

}
