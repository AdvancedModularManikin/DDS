#include "PhysiologyEngineManager.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

#define MAX_DATE 18

using namespace std;
using namespace std::chrono;


PhysiologyEngineManager::PhysiologyEngineManager() {

    cout << "=== [PhysiologyManager] Starting up." << endl;
    cout << "=== [PhysiologyManager][DDS] Initializing DDS entity manager (DATA)." << endl;

    auto *tick_sub_listener = new DDS_Listeners::TickSubListener();
    tick_sub_listener->SetUpstream(this);

    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    command_sub_listener->SetUpstream(this);

    auto *pub_listener = new DDS_Listeners::PubListener();

    tick_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::tickTopic, AMM::DataTypes::getTickType(),
                                                tick_sub_listener);
    command_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(),
                                                   command_sub_listener);
    node_publisher = mgr->InitializePublisher(AMM::DataTypes::nodeTopic, AMM::DataTypes::getNodeType(), pub_listener);

    std::string nodeString(nodeName);

    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            nodeString,
            "Vcom3D",
            "PhysiologyEngine",
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/module_capabilities/physiology_engine_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(nodeString, OPERATIONAL);

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
        // Do this while the tickloop is running...  data process is handled by the listener events now
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
                WriteNodeData(it->first);
            }
        }
        ++it;
    }
}

void PhysiologyEngineManager::StartTickSimulation() {
    if (!m_runThread) {
        bg = new PhysiologyThread("./logs/biogears.log");
        std::size_t pos = stateFile.find("@");
        std::string state2 = stateFile.substr(pos);
        std::size_t pos2 = state2.find("s");
        std::string state3 = state2.substr(1, pos2 - 1);
        double startPosition = atof(state3.c_str());
        cout << " === [PhysiologyManager] Loading state file " << stateFile << " at position " << startPosition
             << " seconds" << endl;
        bg->LoadState(stateFile.c_str(), startPosition);
        nodePathMap = bg->GetNodePathTable();
        m_runThread = true;
        paused = false;
        m_thread = std::thread(&PhysiologyEngineManager::TickLoop, this);

    }
}

void PhysiologyEngineManager::StopTickSimulation() {
    if (m_runThread) {
        m_mutex.lock();
        m_runThread = false;
        paused = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        m_mutex.unlock();
        m_thread.detach();
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

int PhysiologyEngineManager::GetTickCount() {
    return lastFrame;
}

void PhysiologyEngineManager::Status() {
    bg->Status();
}


void PhysiologyEngineManager::Shutdown() {
    cout << "=== [PhysiologyManager] Sending -1 values to all topics." << endl;
    SendShutdown();

    cout << "=== [PhysiologyManager][BG] Shutting down BioGears." << endl;
    bg->Shutdown();

    cout << "=== [PhysiologyManager][DDS] Shutting down DDS Connections." << endl;
}


// Listener events
void PhysiologyEngineManager::onNewNodeData(AMM::Physiology::Node n) {
    // Placeholder to listen for higher-weighted node data
}

void PhysiologyEngineManager::onNewCommandData(AMM::PatientAction::BioGears::Command cm) {
    if (!cm.message().compare(0, sysPrefix.size(), sysPrefix)) {
        std::string value = cm.message().substr(sysPrefix.size());
        cout << "[PhysiologyManager] We received a SYSTEM action: " << value << endl;
        if (value.compare("START_ENGINE") == 0 || value.compare("START_SIM") == 0) {
            cout << "=== [PhysiologyManager] Started engine based on Tick Simulation" << endl;
            StartTickSimulation();
        } else if (value.compare("STOP_ENGINE") == 0) {
            cout << "=== [PhysiologyManager] Stopped engine" << endl;
            StopTickSimulation();
            StopSimulation();
            Shutdown();
        } else if (value.compare("PAUSE_ENGINE") == 0) {
            cout << "=== [PhysiologyManager] Paused engine" << endl;
            StopTickSimulation();
        } else if (value.compare("RESET_SIM") == 0) {
            cout << "=== [PhysiologyManager] Reset simulation, clearing engine data " << endl;
            StopTickSimulation();
            StopSimulation();
        } else if (value.compare("SAVE_STATE") == 0) {
            std::ostringstream ss;
            ss << "./states/SavedState_" << get_filename_date() << "@" << (int) std::round(bg->GetSimulationTime())
               << "s.xml";
            cout << "=== [PhysiologyManager] Saved state file: " << ss.str() << endl;
            bg->SaveState(ss.str());
        } else if (!value.compare(0, loadPrefix.size(), loadPrefix)) {
            StopTickSimulation();
            stateFile = "./states/" + value.substr(loadPrefix.size()) + ".xml";
            StartTickSimulation();
        }
    } else {
        cout << "[PhysiologyManager] Command received: " << cm.message() << endl;
        bg->ExecuteCommand(cm.message());
    }
}

void PhysiologyEngineManager::onNewTickData(AMM::Simulation::Tick ti) {
    if (m_runThread) {
        if (ti.frame() == -1) {
            cout << "[SHUTDOWN]" << endl;
            StopTickSimulation();
            SendShutdown();
        } else if (ti.frame() == -2) {
            cout << "[PAUSE]" << endl;
            paused = true;
        } else if (ti.frame() > 0 || !paused) {
            if (paused) {
                cout << "[RESUME]" << endl;
                paused = false;
            }

            // Did we get a frame out of order?  Just mark it with an X for now.
            if (ti.frame() <= lastFrame) {
//                cout << "x";
            } else {
//                cout << ".";
            }
            lastFrame = static_cast<int>(ti.frame());


            // Per-frame stuff happens here
            AdvanceTimeTick();
            PublishData(false);
        } else {
            cout.flush();
        }
    }
}
