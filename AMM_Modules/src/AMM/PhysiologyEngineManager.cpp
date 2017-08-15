#include "PhysiologyEngineManager.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

using namespace AMM;
using namespace AMM::Simulation;
using namespace AMM::Physiology;
using namespace AMM::PatientAction::BioGears;
using namespace std;
using namespace std::chrono;

PhysiologyEngineManager::PhysiologyEngineManager() :
		m_thread() {

	cout << "=== [PhysiologyManager] Starting up." << endl;

	/**
	 * Physiology Data DDS Entity manager
	 */
	cout << "=== [PhysiologyManager][DDS] Initializing DDS entity manager (DATA)." << endl;

	mgr->Initialize();

	tick_subscriber = mgr->InitializeTickSubscriber(&tick_sub_listener);
	command_subscriber = mgr->InitializeCommandSubscriber(&command_sub_listener);
	node_publisher = mgr->InitializeNodePublisher(&pub_listener);

	nodePathMap = bg->nodePathTable;
	m_runThread = false;

}

bool PhysiologyEngineManager::isRunning() {
	return m_runThread;
}

void PhysiologyEngineManager::TickLoop() {
	while (m_runThread) {
		// ReadCommands();
		// ReadTicks();
	}
}

void PhysiologyEngineManager::ReadCommands() {

}

void PhysiologyEngineManager::SendShutdown() {

}

void PhysiologyEngineManager::ReadTicks() {
	// Check for a tick
	/*TickReader->take(tickList, infoSeq, LENGTH_UNLIMITED, NOT_READ_SAMPLE_STATE, NEW_VIEW_STATE, ANY_INSTANCE_STATE);
	for (DDS::ULong j = 0; j < tickList.length(); j++) {
		if (tickList[j].frame == -1) {
			cout << "[SHUTDOWN]";
			StopTickSimulation();
			SendShutdown();
		} else if (tickList[j].frame == -2) {
			// Pause signal
			cout << "[PAUSE]";
			paused = true;
		} else if (tickList[j].frame > 0 || !paused) {
			if (paused) {
				cout << "[RESUME]";
				paused = false;
			}

			// Did we get a frame out of order?  Just mark it with an X for now.
			if (tickList[j].frame <= lastFrame) {
				cout << "x";
			} else {
				cout << ".";
			}
			lastFrame = tickList[j].frame;

			// Per-frame stuff happens here
			bg->AdvanceTimeTick();
			PublishData(false);
		}
		cout.flush();
	}
	TickReader->return_loan(tickList, infoSeq);*/
}

void PhysiologyEngineManager::PrintAvailableNodePaths() {
	nodePathMap = bg->nodePathTable;
	std::map<std::string, double (BioGearsThread::*)()>::iterator it = nodePathMap.begin();
	while (it != nodePathMap.end()) {
		std::string word = it->first;
		cout << word << endl;
		it++;
	}
}

void PhysiologyEngineManager::PrintAllCurrentData() {
	nodePathMap = bg->nodePathTable;
	std::map<std::string, double (BioGearsThread::*)()>::iterator it = nodePathMap.begin();
	while (it != nodePathMap.end()) {
		std::string node = it->first;
		double dbl = bg->GetNodePath(node);
		cout << node << "\t\t\t" << dbl << endl;
		it++;
	}
}

int PhysiologyEngineManager::GetNodePathCount() {
	return nodePathMap.size();
}

void PhysiologyEngineManager::WriteNodeData(string node) {
	Node *dataInstance = new Node();
	dataInstance->nodepath(node);
	dataInstance->dbl(bg->GetNodePath(node));
	dataInstance->frame(lastFrame);
	node_publisher->write(&dataInstance);
	delete dataInstance;
}

void PhysiologyEngineManager::PublishData(bool force = false) {
	std::map<std::string, double (BioGearsThread::*)()>::iterator it = nodePathMap.begin();
	while (it != nodePathMap.end()) {
		// High-frequency nodes are published every tick
		// All other nodes are published every % 10 tick
		if ((std::find(bg->highFrequencyNodes.begin(), bg->highFrequencyNodes.end(), it->first) != bg->highFrequencyNodes.end())
				|| (lastFrame % 10) == 0 || force) {
			WriteNodeData(it->first);
		}
		it++;
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

void PhysiologyEngineManager::NodeSubListener::onSubscriptionMatched(Subscriber* sub,MatchingInfo& info)
{
    if (info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "Subscriber matched" << std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "Subscriber unmatched" << std::endl;
    }
}

void PhysiologyEngineManager::NodeSubListener::onNewDataMessage(Subscriber* sub)
{
    // Take data

    if(sub->takeNextData(&st, &m_info))
    {
        if(m_info.sampleKind == ALIVE)
        {
            // Print your structure data here.
            ++n_msg;
            std::cout << "Sample received, count=" << n_msg << std::endl;
        }
    }
}


void PhysiologyEngineManager::CommandSubListener::onSubscriptionMatched(Subscriber* sub,MatchingInfo& info)
{
    if (info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "Subscriber matched" << std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "Subscriber unmatched" << std::endl;
    }
}

void PhysiologyEngineManager::CommandSubListener::onNewDataMessage(Subscriber* sub)
{
    // Take data
    AMM::PatientAction::BioGears::Command st;

    if(sub->takeNextData(&st, &m_info))
    {
        if(m_info.sampleKind == ALIVE)
        {
            // Print your structure data here.
            ++n_msg;
            std::cout << "Sample received, count=" << n_msg << std::endl;
        }
    }
}

void PhysiologyEngineManager::TickSubListener::onSubscriptionMatched(Subscriber* sub,MatchingInfo& info)
{
    if (info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "Subscriber matched" << std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "Subscriber unmatched" << std::endl;
    }
}

void PhysiologyEngineManager::TickSubListener::onNewDataMessage(Subscriber* sub)
{
    // Take data
    AMM::Simulation::Tick st;

    if(sub->takeNextData(&st, &m_info))
    {
        if(m_info.sampleKind == ALIVE)
        {
            if (st.frame() == -1) {
                cout << "[SHUTDOWN]";
                StopTickSimulation();
                SendShutdown();
            } else if (st.frame() == -2) {
                // Pause signal
                cout << "[PAUSE]";
                paused = true;
            } else if (st.frame() > 0 || !paused) {
                if (paused) {
                    cout << "[RESUME]";
                    paused = false;
                }

                // Did we get a frame out of order?  Just mark it with an X for now.
                if (st.frame() <= lastFrame) {
                    cout << "x";
                } else {
                    cout << ".";
                }
                lastFrame = st.frame();

                // Per-frame stuff happens here
                bg->AdvanceTimeTick();
                PublishData(false);
            }
            cout.flush();

            ++n_msg;
            std::cout << "Sample received, count=" << n_msg << std::endl;
        }
    }
}

void PhysiologyEngineManager::PubListener::onPublicationMatched(Publisher* pub,MatchingInfo& info)
{
    if (info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "Publisher matched" << std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "Publisher unmatched" << std::endl;
    }
}


