#include "PhysiologyEngineManager.h"

std::string get_filename_date(void) {
    time_t now;
    char the_date[18];

    the_date[0] = '\0';

    now = time(NULL);

    if (now != -1) {
        strftime(the_date, 18, "%Y%m%d_%H%M%S", gmtime(&now));
    }

    return std::string(the_date);
}

namespace AMM {
    PhysiologyEngineManager::PhysiologyEngineManager() {
        using namespace Capability;

        if (bg == nullptr) {
            LOG_ERROR << "BioGears thread did not load.";
        }
        stateFile = "./states/StandardMale@0s.xml";

        auto *tick_sub_listener = new DDS_Listeners::TickSubListener();
        tick_sub_listener->SetUpstream(this);

        auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
        command_sub_listener->SetUpstream(this);

        auto *physiology_command_sub_listener =
                new DDS_Listeners::PhysiologyCommandSubListener();
        physiology_command_sub_listener->SetUpstream(this);

        auto *equipment_sub_listener = new DDS_Listeners::EquipmentSubListener();
        equipment_sub_listener->SetUpstream(this);

        auto *physmod_sub_listener =
                new DDS_Listeners::PhysiologyModificationListener();
        physmod_sub_listener->SetUpstream(this);

        auto *pub_listener = new DDS_Listeners::PubListener();

        tick_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::tickTopic,
                                                    AMM::DataTypes::getTickType(),
                                                    tick_sub_listener);
        command_subscriber = mgr->InitializeReliableSubscriber(
                AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(),
                command_sub_listener);
        equipment_subscriber = mgr->InitializeReliableSubscriber(
                AMM::DataTypes::instrumentDataTopic,
                AMM::DataTypes::getInstrumentDataType(), equipment_sub_listener);

        physmod_subscriber = mgr->InitializeReliableSubscriber(
                AMM::DataTypes::physModTopic,
                AMM::DataTypes::getPhysiologyModificationType(), physmod_sub_listener);

        node_publisher = mgr->InitializePublisher(
                AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(), pub_listener);

        hf_node_publisher = mgr->InitializePublisher(
                AMM::DataTypes::highFrequencyNodeTopic, AMM::DataTypes::getHighFrequencyNodeType(), pub_listener);

        std::string nodeString(nodeName);

        // Publish module configuration once we've set all our publishers and
        // listeners
        // This announces that we're available for configuration
        mgr->PublishModuleConfiguration(
                mgr->module_id, nodeString, "Vcom3D", "PhysiologyEngine", "00001",
                "0.0.1",
                mgr->GetCapabilitiesAsString(
                        "mule1/module_capabilities/physiology_engine_capabilities.xml"));

        // Normally this would be set AFTER configuration is received
        mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);

        m_runThread = false;
    }

    bool PhysiologyEngineManager::isRunning() { return m_runThread; }

    void PhysiologyEngineManager::TickLoop() {
        while (m_runThread) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

    void PhysiologyEngineManager::SendShutdown() {
        //return WriteNodeData("EXIT");
    }

    void PhysiologyEngineManager::PrintAvailableNodePaths() {
        nodePathMap = bg->GetNodePathTable();
        auto it = nodePathMap->begin();
        while (it != nodePathMap->end()) {
            std::string word = it->first;
            std::cout << word << std::endl;
            ++it;
        }
    }

    void PhysiologyEngineManager::PrintAllCurrentData() {
        nodePathMap = bg->GetNodePathTable();
        auto it = nodePathMap->begin();
        while (it != nodePathMap->end()) {
            std::string node = it->first;
            double dbl = bg->GetNodePath(node);
            std::cout << node << "\t\t\t" << dbl << std::endl;
            ++it;
        }
    }

    int PhysiologyEngineManager::GetNodePathCount() {
        return static_cast<int>(nodePathMap->size());
    }

    void PhysiologyEngineManager::WriteNodeData(std::string node) {
        AMM::Physiology::Node dataInstance;
        dataInstance.nodepath(node);
        dataInstance.dbl(bg->GetNodePath(node));
        dataInstance.frame(lastFrame);
        try {
            node_publisher->write(&dataInstance);
        } catch (std::exception &e) {
            LOG_ERROR << "Unable to write node data  " << node << ": " << e.what();
        }
    }

    void PhysiologyEngineManager::WriteHighFrequencyNodeData(std::string node) {
        AMM::Physiology::HighFrequencyNode dataInstance;
        dataInstance.nodepath(node);
        dataInstance.dbl(bg->GetNodePath(node));
        dataInstance.frame(lastFrame);
        try {
            hf_node_publisher->write(&dataInstance);
        } catch (std::exception &e) {
            LOG_ERROR << "Unable to write high frequency node data  " << node << ": " << e.what();
        }
    }

    void PhysiologyEngineManager::PublishData(bool force = false) {
        auto it = nodePathMap->begin();
        while (it != nodePathMap->end()) {
            if ((lastFrame % 10) == 0 || force) {
                WriteNodeData(it->first);
            }
            if ((std::find(bg->highFrequencyNodes.begin(), bg->highFrequencyNodes.end(), it->first) !=
                 bg->highFrequencyNodes.end())) {
                WriteHighFrequencyNodeData(it->first);
            }
            ++it;
        }
    }

    void PhysiologyEngineManager::StartTickSimulation() {
        std::size_t pos = stateFile.find("@");
        std::string state2 = stateFile.substr(pos);
        std::size_t pos2 = state2.find("s");
        std::string state3 = state2.substr(1, pos2 - 1);
        double startPosition = atof(state3.c_str());

        if (!paused) {
            m_mutex.lock();
            bg->LoadState(stateFile.c_str(), startPosition);
            nodePathMap = bg->GetNodePathTable();
            m_mutex.unlock();
        }
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

    void PhysiologyEngineManager::StartSimulation() { bg->StartSimulation(); }

    void PhysiologyEngineManager::StopSimulation() { bg->StopSimulation(); }

    void PhysiologyEngineManager::AdvanceTimeTick() { bg->AdvanceTimeTick(); }

    void PhysiologyEngineManager::SetLogging(bool log) {
        logging_enabled = log;
        bg->logging_enabled = logging_enabled;
    }

    int PhysiologyEngineManager::GetTickCount() { return lastFrame; }

    void PhysiologyEngineManager::Status() { bg->Status(); }

    void PhysiologyEngineManager::Shutdown() {
        LOG_TRACE << "[PhysiologyManager] Sending -1 values to all topics.";
        SendShutdown();

        LOG_TRACE << "[PhysiologyManager][BG] Shutting down BioGears.";
        bg->Shutdown();

        LOG_TRACE << "[PhysiologyManager][DDS] Shutting down DDS Connections.";
    }

// Listener events
    void PhysiologyEngineManager::onNewNodeData(AMM::Physiology::Node n,
                                                SampleInfo_t *info) {
        // Placeholder to listen for higher-weighted node data
    }

    void PhysiologyEngineManager::onNewHighFrequencyNodeData(AMM::Physiology::HighFrequencyNode n,
                                                             SampleInfo_t *info) {
        // Placeholder to listen for higher-weighted node data
    }

    void PhysiologyEngineManager::onNewPhysiologyModificationData(
            AMM::Physiology::Modification pm, SampleInfo_t *info) {
        // If the payload is empty, use the type to execute an XML file.
        // Otherwise, the payload is considered to be XML to execute.
        if (pm.payload().empty()) {
            LOG_INFO << "Old-style Physiology modification received: " << pm.type();
            bg->ExecuteCommand(pm.type());
        } else {
            if (pm.type() == "pain") {
                LOG_INFO << "Pain payload received: " << pm.payload();
                bg->SetPain(pm.payload());
            } else {
                LOG_INFO << "Physiology modification received: " << pm.payload();
                bg->ExecuteXMLCommand(pm.payload());
            }
        }
    }

    void PhysiologyEngineManager::onNewCommandData(AMM::Physiology::Command cm,
                                                   SampleInfo_t *info) {
        using namespace biogears;
        switch (cm.type()) {
            case AMM::Physiology::CMD::PainCommand: {
                LOG_TRACE << "AMM::Physiology::CMD::PainCommand";

                AMM::Physiology::PainStimulus::Data command;
                eprosima::fastcdr::FastBuffer buffer{&cm.payload()[0], cm.payload().size()};
                eprosima::fastcdr::Cdr cdr{buffer};
                cdr >> command;
                bg->Execute([=](std::unique_ptr <biogears::PhysiologyEngine> engine) {
                    // Create variables for scenario
                    SEPainStimulus PainStimulus; // pain object
                    PainStimulus.SetLocation(command.location().description());
                    PainStimulus.GetSeverity().SetValue(command.severity());
                    engine->ProcessAction(PainStimulus);
                    return engine;
                });
            }
                break;
            case AMM::Physiology::CMD::SepsisCommand: {
                LOG_TRACE << "AMM::Physiology::CMD::SepsisCommand";
                AMM::Physiology::Sepsis::Data command;
                eprosima::fastcdr::FastBuffer buffer{&cm.payload()[0], cm.payload().size()};
                eprosima::fastcdr::Cdr cdr{buffer};
                cdr >> command;
                bg->Execute([=](std::unique_ptr <biogears::PhysiologyEngine> engine) {
                    // Create variables for scenario
                    SESepsis sepsis; // pain object
                    sepsis.BuildTissueResistorMap();
                    auto tissueMap = sepsis.GetTissueResistorMap();
                    switch (command.location()) {
                        case AMM::Physiology::BoneTissue:
                            sepsis.SetCompartment(tissueMap["BoneTissue"]);
                            break;
                        case AMM::Physiology::FatTissue:
                            sepsis.SetCompartment(tissueMap["FatTissue"]);
                            break;
                        case AMM::Physiology::GutTissue:
                            sepsis.SetCompartment(tissueMap["GutTissue"]);
                            break;
                        case AMM::Physiology::LeftKidneyTissue:
                            sepsis.SetCompartment(tissueMap["LeftKidneyTissue"]);
                            break;
                        case AMM::Physiology::LeftLungTissue:
                            sepsis.SetCompartment(tissueMap["LeftLungTissue"]);
                            break;
                        case AMM::Physiology::LiverTissue:
                            sepsis.SetCompartment(tissueMap["LiverTissue"]);
                            break;
                        case AMM::Physiology::MuscleTissue:
                            sepsis.SetCompartment(tissueMap["MuscleTissue"]);
                            break;
                        case AMM::Physiology::MyocardiumTissue:
                            sepsis.SetCompartment(tissueMap["MyocardiumTissue"]);
                            break;
                        case AMM::Physiology::RightKidneyTissue:
                            sepsis.SetCompartment(tissueMap["RightKidneyTissu"]);
                            break;
                        case AMM::Physiology::RightLungTissue:
                            sepsis.SetCompartment(tissueMap["RightLungTissue"]);
                            break;
                        case AMM::Physiology::SkinTissue:
                            sepsis.SetCompartment(tissueMap["SkinTissue"]);
                            break;
                        case AMM::Physiology::SpleenTissue:
                            sepsis.SetCompartment(tissueMap["SpleenTissue"]);
                            break;
                    }
                    sepsis.GetSeverity().SetValue(command.severity());
                    engine->ProcessAction(sepsis);
                    return engine;
                });
            }
                break;
            default:
                LOG_TRACE << "Unsupported CMD Type. Value sent was " << cm.type();
                break;
        };
    }

    void PhysiologyEngineManager::onNewCommandData(
            AMM::PatientAction::BioGears::Command cm, SampleInfo_t *info) {
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
            } else if (value.compare("PAUSE_ENGINE") == 0 ||
                       value.compare("PAUSE_SIM") == 0) {
                LOG_TRACE << "Paused engine";
                StopTickSimulation();
                paused = true;
            } else if (value.compare("ENABLE_LOGGING") == 0) {
                LOG_TRACE << "Enabling logging";
                this->SetLogging(true);
            } else if (value.compare("DISABLE_LOGGING") == 0) {
                LOG_TRACE << "Disabling logging";
                this->SetLogging(false);
            } else if (value.compare("RESET_SIM") == 0) {
                LOG_TRACE << "Reset simulation, clearing engine data.";
                StopTickSimulation();
                StopSimulation();
            } else if (value.compare("SAVE_STATE") == 0) {
                std::ostringstream ss;
                ss << "./states/SavedState_" << get_filename_date() << "@"
                   << (int) std::round(bg->GetSimulationTime()) << "s.xml";
                LOG_TRACE << "Saved state file: " << ss.str();
                bg->SaveState(ss.str());
            } else if (!value.compare(0, loadPrefix.size(), loadPrefix)) {
                StopTickSimulation();
                stateFile = "./states/" + value.substr(loadPrefix.size()) + ".xml";
                StartTickSimulation();
            }
        } else {
            LOG_TRACE << "Command received: " << cm.message();
            bg->ExecuteCommand(cm.message());
        }
    }

    void PhysiologyEngineManager::onNewTickData(AMM::Simulation::Tick ti,
                                                SampleInfo_t *info) {
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
                std::cout.flush();
            }
        }
    }

    void PhysiologyEngineManager::TestPain(const std::string &painSettings) {
        bg->SetPain(painSettings);
    }

    void PhysiologyEngineManager::TestVentilator(
            const std::string &ventilatorSettings) {
        bg->SetVentilator(ventilatorSettings);
    }

    void PhysiologyEngineManager::TestPump(const std::string &pumpSettings) {
        bg->SetIVPump(pumpSettings);
    }

    void PhysiologyEngineManager::onNewInstrumentData(AMM::InstrumentData i,
                                                      SampleInfo_t *info) {
        LOG_TRACE << "Instrument data for " << i.instrument()
                  << " received with payload: " << i.payload();
        std::string instrument(i.instrument());
        if (instrument == "ventilator") {
            bg->SetVentilator(i.payload());
        }

        if (instrument == "ivpump") {
            bg->SetIVPump(i.payload());
        }
    }
}
