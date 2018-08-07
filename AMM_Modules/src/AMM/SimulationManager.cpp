#include "SimulationManager.h"

using namespace std;
using namespace std::chrono;

namespace AMM {
    SimulationManager::SimulationManager() {
        auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
        command_sub_listener->SetUpstream(this);
        command_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(),
                                                       command_sub_listener);

        auto *pub_listener = new DDS_Listeners::PubListener();
        tick_publisher = mgr->InitializePublisher(AMM::DataTypes::tickTopic, AMM::DataTypes::getTickType(),
                                                  pub_listener);
        command_publisher = mgr->InitializePublisher(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(),
                                                     pub_listener);
        m_runThread = false;

        currentScenario = mgr->GetScenario();
        std::string nodeString(nodeName);
        mgr->PublishModuleConfiguration(
                mgr->module_id,
                nodeString,
                "Vcom3D",
                "SimulationManager",
                "00001",
                "0.0.1",
                mgr->GetCapabilitiesAsString("mule1/module_capabilities/simulation_manager_capabilities.xml")
        );

        mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);
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
        LOG_INFO << "Sending a command:" << command;
        AMM::PatientAction::BioGears::Command cmdInstance;
        cmdInstance.message(command);
        command_publisher->write(&cmdInstance);
    }

    void SimulationManager::TickLoop() {
        using frames = duration<int64_t, ratio<1, 50>>;
        auto nextFrame = system_clock::now();
        auto lastFrame = nextFrame - frames{1};

        while (m_runThread) {

            this_thread::sleep_until(nextFrame);
            m_mutex.lock();

            AMM::Simulation::Tick tickInstance;
            tickInstance.frame(tickCount++);
            tick_publisher->write(&tickInstance);

            lastFrame = nextFrame;
            nextFrame += frames{1};
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
        if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = c.message().substr(sysPrefix.size());
            LOG_INFO << "We received a SYSTEM action: " << value;
            if (value.compare("START_SIM") == 0) {
                LOG_INFO << "Started simulation";
                StartSimulation();
            } else if (value.compare("STOP_SIM") == 0) {
                LOG_INFO << "Stopped simulation";
                StopSimulation();
                LOG_INFO << "Exited after " << GetTickCount() << " ticks.";
                LOG_INFO << "Shutting down Simulation Manager.";
                Shutdown();
                tickCount = 0;
            } else if (value.compare("PAUSE_SIM") == 0) {
                LOG_INFO << "Paused simulation (can be restarted)";
                StopSimulation();
            } else if (value.compare("RESET_SIM") == 0) {
                StopSimulation();
                tickCount = 0;
                LOG_INFO << "Reset simulation (restart will be a new simulation)";
            } else if (value.compare("LOAD_SCENARIO") == 0) {
                std::string loadScenario = value.substr(loadScenario.size());
                mgr->SetScenario(loadScenario);
            }
        } else {
            LOG_WARNING << "Unknown command received: " << c.message();
        }

    }
}