#include "SimulationManager.h"

using namespace std;
using namespace std::chrono;


SimulationManager::SimulationManager() {
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    command_sub_listener->SetUpstream(this);
    command_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(),
                                                   command_sub_listener);

    auto *pub_listener = new DDS_Listeners::PubListener();
    tick_publisher = mgr->InitializePublisher(AMM::DataTypes::tickTopic, AMM::DataTypes::getTickType(), pub_listener);
    command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(),
                                                 pub_listener);
    m_runThread = false;

    currentScenario = mgr->GetScenario();

    mgr->PublishModuleConfiguration(
            "Vcom3D",
            "SimulationManager",
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/simulation_manager_capabilities.xml")
    );

    mgr->SetStatus(OPERATIONAL);
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
void SimulationManager::onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) {
    if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
        std::string value = c.message().substr(sysPrefix.size());
        cout << "[SimManager] We received a SYSTEM action: " << value << endl;
        if (value.compare("START_SIM") == 0) {
            cout << "=== [SimManager] Started simulation" << endl;
            StartSimulation();
        } else if (value.compare("STOP_SIM") == 0) {
            cout << "=== [SimManager] Stopped simulation" << endl;
            StopSimulation();
            cout << " == Exited after " << GetTickCount() << " ticks." << endl;
            cout << "=== [SimManager] Shutting down Simulation Manager." << endl;
            Shutdown();
            tickCount = 0;
        } else if (value.compare("PAUSE_SIM") == 0) {
            cout << "=== [SimManager] Paused simulation (can be restarted)" << endl;
            StopSimulation();
        } else if (value.compare("RESET_SIM") == 0) {
            StopSimulation();
            tickCount = 0;
            cout << "=== [SimManager] Reset simulation (restart will be a new simulation)" << endl;
        } else if (value.compare("LOAD_SCENARIO") == 0) {
            std::string loadScenario = value.substr(loadScenario.size());
            mgr->SetScenario(loadScenario);
        }
    } else {
        cout << "[SimManager] Command received: " << c.message() << endl;
    }

}

