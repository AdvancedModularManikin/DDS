#include "PhysiologyEngineManager.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

#define MAX_DATE 18

using namespace std;
using namespace std::chrono;

namespace AMM {
    PhysiologyEngineManager::PhysiologyEngineManager() {
        if (bg == nullptr) {
            LOG_ERROR << "BioGears thread did not load.";
        }


        auto *tick_sub_listener = new DDS_Listeners::TickSubListener();
        tick_sub_listener->SetUpstream(this);

        auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
        command_sub_listener->SetUpstream(this);

        auto *equipment_sub_listener = new DDS_Listeners::EquipmentSubListener();
        equipment_sub_listener->SetUpstream(this);

        auto *pub_listener = new DDS_Listeners::PubListener();

        tick_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::tickTopic, AMM::DataTypes::getTickType(),
                                                    tick_sub_listener);
        command_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(),
                                                       command_sub_listener);
        equipment_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::instrumentDataTopic,
                                                         AMM::DataTypes::getInstrumentDataType(),
                                                         equipment_sub_listener);

        node_publisher = mgr->InitializePublisher(AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(),
                                                  pub_listener);

        std::string nodeString(nodeName);

        // Publish module configuration once we've set all our publishers and listeners
        // This announces that we're available for configuration
        mgr->PublishModuleConfiguration(
                mgr->module_id,
                nodeString,
                "Vcom3D",
                "PhysiologyEngine",
                "00001",
                "0.0.1",
                mgr->GetCapabilitiesAsString("mule1/module_capabilities/physiology_engine_capabilities.xml")
        );

        // Normally this would be set AFTER configuration is received
        mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);

        m_runThread = false;

    }

    std::string PhysiologyEngineManager::get_random_string(size_t length) {
        auto randchar = []() -> char {
            const char charset[] =
                    "0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[rand() % max_index];
        };
        std::string str(length, 0);
        std::generate_n(str.begin(), length, randchar);
        return str;
    }

    std::string PhysiologyEngineManager::get_filename_date(void) {
        time_t now;
        char the_date[MAX_DATE];

        the_date[0] = '\0';

        now = time(NULL);

        if (now != -1) {
            strftime(the_date, MAX_DATE, "%Y%m%d_%H%M%S", gmtime(&now));
        }

        return std::string(the_date);
    }

    bool PhysiologyEngineManager::isRunning() {
        return m_runThread;
    }

    void PhysiologyEngineManager::TickLoop() {
        while (m_runThread) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

    void PhysiologyEngineManager::SendShutdown() {
        return WriteNodeData("EXIT");
    }

    void PhysiologyEngineManager::PrintAvailableNodePaths() {
        nodePathMap = bg->GetNodePathTable();
        auto it = nodePathMap->begin();
        while (it != nodePathMap->end()) {
            std::string word = it->first;
            cout << word << endl;
            ++it;
        }
    }

    void PhysiologyEngineManager::PrintAllCurrentData() {
        nodePathMap = bg->GetNodePathTable();
        auto it = nodePathMap->begin();
        while (it != nodePathMap->end()) {
            std::string node = it->first;
            double dbl = bg->GetNodePath(node);
            cout << node << "\t\t\t" << dbl << endl;
            ++it;
        }
    }

    int PhysiologyEngineManager::GetNodePathCount() {
        return static_cast<int>(nodePathMap->size());
    }

    void PhysiologyEngineManager::WriteNodeData(string node) {
        AMM::Physiology::Node dataInstance;
        dataInstance.nodepath(node);
        dataInstance.dbl(bg->GetNodePath(node));
        dataInstance.frame(lastFrame);
        node_publisher->write(&dataInstance);
    }


    void PhysiologyEngineManager::PublishData(bool force = false) {
        auto it = nodePathMap->begin();
        while (it != nodePathMap->end()) {
            // High-frequency nodes are published every tick
            // All other nodes are published every % 10 tick
            if ((std::find(bg->highFrequencyNodes.begin(), bg->highFrequencyNodes.end(), it->first) !=
                 bg->highFrequencyNodes.end())
                || (lastFrame % 10) == 0 || force) {
                if (it->first != "EXIT") {
                    try {
                        WriteNodeData(it->first);
                    } catch (exception &e) {
                        LOG_ERROR << "Unable to write node data  " << it->first << ": " << e.what();
                    }
                }
            }
            ++it;
        }
    }

    void PhysiologyEngineManager::StartTickSimulation() {
        std::string stateFile = "./states/StandardMale@0s.xml";
        std::size_t pos = stateFile.find("@");
        std::string state2 = stateFile.substr(pos);
        std::size_t pos2 = state2.find("s");
        std::string state3 = state2.substr(1, pos2 - 1);
        double startPosition = atof(state3.c_str());

        m_mutex.lock();
        bg->LoadState(stateFile.c_str(), startPosition);
        nodePathMap = bg->GetNodePathTable();
        m_mutex.unlock();

        m_runThread = true;
        paused = false;
    }

    void PhysiologyEngineManager::StopTickSimulation() {
        if (m_runThread) {
            m_mutex.lock();
            m_runThread = false;
            paused = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            m_mutex.unlock();
        }
    }

    void PhysiologyEngineManager::StartSimulation() {
        bg->StartSimulation();
    }

    void PhysiologyEngineManager::StopSimulation() {
        bg->StopSimulation();
    }

    void PhysiologyEngineManager::AdvanceTimeTick() {
        bg->AdvanceTimeTick();
    }

    void PhysiologyEngineManager::SetLogging(bool logging_enabled) {
        logging_enabled = logging_enabled;
        bg->logging_enabled = logging_enabled;
    }

    int PhysiologyEngineManager::GetTickCount() {
        return lastFrame;
    }

    void PhysiologyEngineManager::Status() {
        bg->Status();
    }


    void PhysiologyEngineManager::Shutdown() {
        LOG_TRACE << "[PhysiologyManager] Sending -1 values to all topics.";
        SendShutdown();

        LOG_TRACE << "[PhysiologyManager][BG] Shutting down BioGears.";
        bg->Shutdown();

        LOG_TRACE << "[PhysiologyManager][DDS] Shutting down DDS Connections.";
    }


// Listener events
    void PhysiologyEngineManager::onNewNodeData(AMM::Physiology::Node n, SampleInfo_t *info) {
        // Placeholder to listen for higher-weighted node data
    }

    void PhysiologyEngineManager::onNewCommandData(AMM::PatientAction::BioGears::Command cm, SampleInfo_t *info) {
        LOG_INFO << "Command received: " << cm.message();
        if (!cm.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = cm.message().substr(sysPrefix.size());
            LOG_TRACE << "We received a SYSTEM action: " << value;
            if (value.compare("START_ENGINE") == 0 || value.compare("START_SIM") == 0) {
                LOG_TRACE << "Started engine based on Tick Simulation";
                StartTickSimulation();
            } else if (value.compare("STOP_ENGINE") == 0) {
                LOG_TRACE << "Stopped engine";
                StopTickSimulation();
                StopSimulation();
                Shutdown();
            } else if (value.compare("PAUSE_ENGINE") == 0) {
                LOG_TRACE << "Paused engine" << endl;
                StopTickSimulation();
            } else if (value.compare("RESET_SIM") == 0) {
                LOG_TRACE << "Reset simulation, clearing engine data.";
                StopTickSimulation();
                StopSimulation();
            } else if (value.compare("SAVE_STATE") == 0) {
                std::ostringstream ss;
                ss << "./states/SavedState_" << get_filename_date() << "@" << (int) std::round(bg->GetSimulationTime())
                   << "s.xml";
                LOG_TRACE << "Saved state file: " << ss.str();
                bg->SaveState(ss.str());
            } else if (!value.compare(0, loadPrefix.size(), loadPrefix)) {
                StopTickSimulation();
                // stateFile = "./states/" + value.substr(loadPrefix.size()) + ".xml";
                StartTickSimulation();
            }
        } else {
            LOG_TRACE << "Command received: " << cm.message();
            bg->ExecuteCommand(cm.message());
        }
    }

    void PhysiologyEngineManager::onNewTickData(AMM::Simulation::Tick ti, SampleInfo_t *info) {
        if (m_runThread) {
            if (ti.frame() == -1) {
                StopTickSimulation();
                SendShutdown();
            } else if (ti.frame() == -2) {
                paused = true;
            } else if (ti.frame() > 0 || !paused) {
                if (paused) {
                    paused = false;
                }

                // Did we get a frame out of order?  Just mark it with an X for now.
                /**
                if (ti.frame() <= lastFrame) {
                    cout << "x";
                } else {
                    cout << ".";
                }**/

                lastFrame = static_cast<int>(ti.frame());

                // Per-frame stuff happens here
                AdvanceTimeTick();
                PublishData(false);
            } else {
                cout.flush();
            }
        }
    }

    void PhysiologyEngineManager::TestVentilator(const std::string &ventilatorSettings) {
        bg->SetVentilator(ventilatorSettings);
    }

    void PhysiologyEngineManager::onNewInstrumentData(AMM::InstrumentData i, SampleInfo_t *info) {
        LOG_TRACE << "Instrument data for " << i.instrument() << " received with payload: " << i.payload();
        std::string instrument(i.instrument());
        if (instrument == "ventilator") {
            bg->SetVentilator(i.payload());
        }
    }
}
