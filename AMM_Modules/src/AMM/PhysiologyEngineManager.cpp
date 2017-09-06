#include "PhysiologyEngineManager.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

using namespace std;
using namespace std::chrono;

PhysiologyEngineManager::PhysiologyEngineManager() {

    cout << "=== [PhysiologyManager] Starting up." << endl;

    /**
     * Physiology Data DDS Entity manager
     */
    cout << "=== [PhysiologyManager][DDS] Initializing DDS entity manager (DATA)." << endl;

    auto *tick_sub_listener = new DDS_Listeners::TickSubListener();
    tick_sub_listener->SetUpstream(this);

    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    command_sub_listener->SetUpstream(this);

    auto *pub_listener = new DDS_Listeners::PubListener();

    tick_subscriber = mgr->InitializeTickSubscriber(tick_sub_listener);
    command_subscriber = mgr->InitializeCommandSubscriber(command_sub_listener);
    node_publisher = mgr->InitializeNodePublisher(pub_listener);

    nodePathMap = bg->nodePathTable;
    m_runThread = false;

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
    nodePathMap = bg->nodePathTable;
    auto it = nodePathMap.begin();
    while (it != nodePathMap.end()) {
        std::string word = it->first;
        cout << word << endl;
        ++it;
    }
}

void PhysiologyEngineManager::PrintAllCurrentData() {
    nodePathMap = bg->nodePathTable;
    auto it = nodePathMap.begin();
    while (it != nodePathMap.end()) {
        std::string node = it->first;
        double dbl = bg->GetNodePath(node);
        cout << node << "\t\t\t" << dbl << endl;
        ++it;
    }
}

int PhysiologyEngineManager::GetNodePathCount() {
    return static_cast<int>(nodePathMap.size());
}

void PhysiologyEngineManager::WriteNodeData(string node) {
    AMM::Physiology::Node dataInstance;
    dataInstance.nodepath(node);
    dataInstance.dbl(bg->GetNodePath(node));
    dataInstance.frame(lastFrame);
    node_publisher->write(&dataInstance);
}

void PhysiologyEngineManager::PublishData(bool force = false) {
    auto it = nodePathMap.begin();
    while (it != nodePathMap.end()) {
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
        m_runThread = true;
        m_thread = std::thread(&PhysiologyEngineManager::TickLoop, this);
    }
}

void PhysiologyEngineManager::StopTickSimulation() {
    if (m_runThread) {
        m_mutex.lock();
        m_runThread = false;
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

}

void PhysiologyEngineManager::onNewCommandData(AMM::PatientAction::BioGears::Command c) {
    cout << "[PhysiologyManager] Command received: " << c.message() << endl;
    bg->ExecuteCommand(c.message());
}

void PhysiologyEngineManager::onNewTickData(AMM::Simulation::Tick t) {
    if (m_runThread) {
        if (t.frame() == -1) {
            cout << "[SHUTDOWN]";
            StopTickSimulation();
            SendShutdown();
        } else if (t.frame() == -2) {
            cout << "[PAUSE]";
            paused = true;
        } else if (t.frame() > 0 || !paused) {
            if (paused) {
                cout << "[RESUME]";
                paused = false;
            }

            // Did we get a frame out of order?  Just mark it with an X for now.
            if (t.frame() <= lastFrame) {
                cout << "x";
            } else {
                cout << ".";
            }
            lastFrame = static_cast<int>(t.frame());

            // Per-frame stuff happens here
            bg->AdvanceTimeTick();
            PublishData(false);
        }
        cout.flush();
    }
}