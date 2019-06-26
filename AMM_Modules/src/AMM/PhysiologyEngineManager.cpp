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

        static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
        static plog::DDS_Log_Appender<plog::TxtFormatter> DDSAppender(mgr);
        plog::init(plog::verbose, &consoleAppender).addAppender(&DDSAppender);

        using namespace Capability;

        if (bg == nullptr) {
            // LOG_ERROR << "BioGears thread did not load.";
        }
        stateFile = "./states/StandardMale@0s.xml";

        auto *tick_sub_listener = new DDS_Listeners::TickSubListener();
        tick_sub_listener->SetUpstream(this);

        auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
        command_sub_listener->SetUpstream(this);

        auto *equipment_sub_listener = new DDS_Listeners::EquipmentSubListener();
        equipment_sub_listener->SetUpstream(this);

        auto *physmod_sub_listener =
                new DDS_Listeners::PhysiologyModificationListener();
        physmod_sub_listener->SetUpstream(this);

        auto *pub_listener = new DDS_Listeners::PubListener();

        tick_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::tickTopic,
                                                    &mgr->TickType,
                                                    tick_sub_listener);

        command_subscriber = mgr->InitializeSubscriber(
                AMM::DataTypes::commandTopic, &mgr->CommandType,
                command_sub_listener);

        equipment_subscriber = mgr->InitializeReliableSubscriber(
                AMM::DataTypes::instrumentDataTopic,
                &mgr->InstrumentDataType, equipment_sub_listener);

        physmod_subscriber = mgr->InitializeReliableSubscriber(
                AMM::DataTypes::physModTopic,
                &mgr->PhysiologyModificationType, physmod_sub_listener);

        node_publisher = mgr->InitializePublisher(
                AMM::DataTypes::nodeTopic, &mgr->NodeType, pub_listener);

        hf_node_publisher = mgr->InitializePublisher(
                AMM::DataTypes::highFrequencyNodeTopic, &mgr->HighFrequencyNodeType, pub_listener);

        std::string nodeString(nodeName);

        // Publish module configuration once we've set all our publishers and
        // listeners
        // This announces that we're available for configuration
        mgr->PublishModuleConfiguration(
                mgr->module_id, nodeString, "Vcom3D", "PhysiologyEngine", "00001",
                "0.0.1",
                mgr->GetCapabilitiesAsString(
                        "static/module_capabilities/physiology_engine_capabilities.xml"));

        // Normally this would be set AFTER configuration is received
        mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);
    }

    bool PhysiologyEngineManager::isRunning() { return running; }

    void PhysiologyEngineManager::TickLoop() {
        while (running) {
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
        try {
            dataInstance.nodepath(node);
            dataInstance.dbl(bg->GetNodePath(node));
            dataInstance.frame(lastFrame);
            node_publisher->write(&dataInstance);
        } catch (std::exception &e) {
            LOG_ERROR << "Unable to write node data  " << node << ": " << e.what();
        }
    }

    void PhysiologyEngineManager::WriteHighFrequencyNodeData(std::string node) {
        AMM::Physiology::HighFrequencyNode dataInstance;
        try {
            dataInstance.nodepath(node);
            dataInstance.dbl(bg->GetNodePath(node));
            dataInstance.frame(lastFrame);
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

        if (!running) {
            LOG_INFO << "We're not running, so time to fire up the thread";
            m_mutex.lock();
            bg = new PhysiologyThread("logs/biogears.log");
            m_mutex.unlock();

            this->SetLogging(logging_enabled);

            m_mutex.lock();
            LOG_INFO << "Loading " << stateFile << " at " << startPosition;
            bg->LoadState(stateFile.c_str(), startPosition);
            m_mutex.unlock();
            nodePathMap = bg->GetNodePathTable();
            running = true;
        }

        running = true;
        paused = false;
    }

    void PhysiologyEngineManager::StopTickSimulation() {
        if (running) {
            m_mutex.lock();
            running = false;
            paused = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            bg->Shutdown();
            m_mutex.unlock();

            delete bg;
        }
    }

    void PhysiologyEngineManager::StartSimulation() { bg->StartSimulation(); }

    void PhysiologyEngineManager::StopSimulation() { bg->StopSimulation(); }

    void PhysiologyEngineManager::AdvanceTimeTick() { bg->AdvanceTimeTick(); }

    void PhysiologyEngineManager::SetLogging(bool log) {
        logging_enabled = log;
        if (bg != nullptr) {
            m_mutex.lock();
            bg->logging_enabled = logging_enabled;
            m_mutex.unlock();
        }
    }

    int PhysiologyEngineManager::GetTickCount() { return lastFrame; }

    void PhysiologyEngineManager::Status() { bg->Status(); }

    void PhysiologyEngineManager::Shutdown() {
        LOG_DEBUG << "[PhysiologyManager] Sending -1 values to all topics.";
        SendShutdown();

        LOG_DEBUG << "[PhysiologyManager][BG] Shutting down BioGears.";
        bg->Shutdown();

        LOG_DEBUG << "[PhysiologyManager][DDS] Shutting down DDS Connections.";
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
                m_mutex.lock();
                bg->SetPain(pm.payload());
                m_mutex.unlock();
            } else if (pm.type() == "hemorrhage") {
                LOG_INFO << "Hemorrhage location: " << pm.location().description();
                if (pm.location().description() == "") {
                    pm.location().description("VenaCava");
                }
                LOG_INFO << "Hemorrhage payload received: " << pm.payload();
                m_mutex.lock();
                bg->SetHemorrhage(pm.location().description(), pm.payload());
                m_mutex.unlock();
            } else {
                LOG_INFO << "Physiology modification received (type " << pm.type() << "): " << pm.payload();
                bg->ExecuteXMLCommand(pm.payload());
            }
        }
    }

    const std::map<std::string, std::string> &PhysiologyEngineManager::GetTissueResistorMap() const {
        return m_TissueResistorMap;
    }

    void PhysiologyEngineManager::BuildTissueResistorMap() {
        m_TissueResistorMap["BoneTissue"] = "BoneE1ToBoneE2";
        m_TissueResistorMap["FatTissue"] = "FatE1ToFatE2";
        m_TissueResistorMap["GutTissue"] = "GutE1ToGutE2";
        m_TissueResistorMap["LeftKidneyTissue"] = "LeftKidneyE1ToLeftKidneyE2";
        m_TissueResistorMap["LeftLungTissue"] = "LeftLungE1ToLeftLungE2";
        m_TissueResistorMap["LiverTissue"] = "LiverE1ToLiverE2";
        m_TissueResistorMap["MuscleTissue"] = "MuscleE1ToMuscleE2";
        m_TissueResistorMap["MyocardiumTissue"] = "MyocardiumE1ToMyocardiumE2";
        m_TissueResistorMap["RightKidneyTissue"] = "RightKidneyE1ToRightKidneyE2";
        m_TissueResistorMap["RightLungTissue"] = "RightLungE1ToRightLungE2";
        m_TissueResistorMap["SkinTissue"] = "SkinE1ToSkinE2";
        m_TissueResistorMap["SpleenTissue"] = "SpleenE1ToSpleenE2";
    }

    void PhysiologyEngineManager::onNewCommandData(AMM::Physiology::Command cm,
                                                   SampleInfo_t *info) {
        using namespace biogears;
        switch (cm.type()) {
            case AMM::Physiology::CMD::PainCommand: {
                LOG_DEBUG << "AMM::Physiology::CMD::PainCommand";

                AMM::Physiology::PainStimulus::Data command;
                eprosima::fastcdr::FastBuffer buffer{&cm.payload()[0], cm.payload().size()};
                eprosima::fastcdr::Cdr cdr{buffer};
                cdr >> command;
                bg->Execute([=](std::unique_ptr<biogears::PhysiologyEngine> engine) {
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
                LOG_DEBUG << "AMM::Physiology::CMD::SepsisCommand";
                AMM::Physiology::Sepsis::Data command;
                eprosima::fastcdr::FastBuffer buffer{&cm.payload()[0], cm.payload().size()};
                eprosima::fastcdr::Cdr cdr{buffer};
                cdr >> command;
                bg->Execute([=](std::unique_ptr<biogears::PhysiologyEngine> engine) {
                    SESepsis sepsis;
                    auto tissueMap = GetTissueResistorMap();
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
                LOG_DEBUG << "Unsupported CMD Type. Value sent was " << cm.type();
                break;
        };
    }

    void PhysiologyEngineManager::onNewCommandData(
            AMM::PatientAction::BioGears::Command cm, SampleInfo_t *info) {
        if (!cm.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = cm.message().substr(sysPrefix.size());
            // LOG_DEBUG << "We received a SYSTEM action: " << value;
            if (value.compare("START_ENGINE") == 0 || value.compare("START_SIM") == 0) {
                LOG_DEBUG << "Started engine based on Tick Simulation";
                StartTickSimulation();
            } else if (value.compare("STOP_ENGINE") == 0) {
                LOG_DEBUG << "Stopped engine";
                StopTickSimulation();
                StopSimulation();
                Shutdown();
            } else if (value.compare("PAUSE_ENGINE") == 0 ||
                       value.compare("PAUSE_SIM") == 0) {
                LOG_DEBUG << "Paused engine";
                // StopTickSimulation();
                paused = true;
            } else if (value.compare("ENABLE_LOGGING") == 0) {
                LOG_DEBUG << "Enabling logging";
                this->SetLogging(true);
            } else if (value.compare("DISABLE_LOGGING") == 0) {
                LOG_DEBUG << "Disabling logging";
                this->SetLogging(false);
            } else if (value.compare("RESET_SIM") == 0) {
                LOG_DEBUG << "Reset simulation, clearing engine data and preparing for next run.";
                StopTickSimulation();
                running = false;
                paused = false;
            } else if (value.compare("SAVE_STATE") == 0) {
                std::ostringstream ss;
                double simTime = bg->GetSimulationTime();
                std::string filenamedate = get_filename_date();
                ss << "./states/SavedState_" << filenamedate << "@" << (int) std::round(simTime) << "s.xml";
                LOG_INFO << "Saved state to " << ss.str();
                bg->SaveState(ss.str());
            } else if (!value.compare(0, loadPrefix.size(), loadPrefix)) {
                StopTickSimulation();
                // StopSimulation();
                stateFile = "./states/" + value.substr(loadPrefix.size()) + ".xml";
                // StartTickSimulation();
            }
        } else {
            LOG_DEBUG << "Command received: " << cm.message();
            bg->ExecuteCommand(cm.message());
        }
    }

    void PhysiologyEngineManager::onNewTickData(AMM::Simulation::Tick ti, SampleInfo_t *info) {
        if (running) {
            if (ti.frame() > 0 || !paused) {
                lastFrame = static_cast<int>(ti.frame());

                // Per-frame stuff happens here
                try {
                    AdvanceTimeTick();
                    PublishData(false);
                } catch (std::exception &e) {
                    LOG_ERROR << "Unable to advance time: " << e.what();
                }
            } else {
                std::cout.flush();
            }
        }
    }

    void PhysiologyEngineManager::onNewInstrumentData(AMM::InstrumentData i, SampleInfo_t *info) {
        LOG_DEBUG << "Instrument data for " << i.instrument()
                  << " received with payload: " << i.payload();
        std::string instrument(i.instrument());
        m_mutex.lock();
        if (instrument == "ventilator") {
            bg->SetVentilator(i.payload());
        } else if (instrument == "bvm_mask") {
            bg->SetBVMMask(i.payload());
        } else if (instrument == "ivpump") {
            bg->SetIVPump(i.payload());
        }
        m_mutex.unlock();
    }
}
