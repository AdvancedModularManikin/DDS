#include "SimulationManager.h"

#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>

#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/attributes/PublisherAttributes.h>

#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/attributes/SubscriberAttributes.h>

#include <fastrtps/Domain.h>

#include <fastrtps/utils/eClock.h>


using namespace std;
using namespace std::chrono;

SimulationManager::SimulationManager() :
		m_thread() {

	m_runThread = false;

}

SimulationManager::~SimulationManager() {

}

bool SimulationManager::Init() {
    ParticipantAttributes PParam;
    PParam.rtps.builtin.domainId = 15;
    PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName("AMM");  //You can put here the name you want
    mp_participant = Domain::createParticipant(PParam);
    if(mp_participant == nullptr)
        return false;

    //Register the types

    Domain::registerType(mp_participant,(TopicDataType*) &tickType);
    Domain::registerType(mp_participant,(TopicDataType*) &nodeType);
    Domain::registerType(mp_participant,(TopicDataType*) &commandType);

    // Create Publisher
    PublisherAttributes tickWparam;
    tickWparam.topic.topicKind = NO_KEY;
    tickWparam.topic.topicDataType = tickType.getName();  //This type MUST be registered
    tickWparam.topic.topicName = "Tick";
    tick_publisher = Domain::createPublisher(mp_participant,tickWparam,(PublisherListener*)&pub_listener);

    PublisherAttributes commandWparam;
    commandWparam.topic.topicKind = NO_KEY;
    commandWparam.topic.topicDataType = tickType.getName();  //This type MUST be registered
    commandWparam.topic.topicName = "Command";
    command_publisher = Domain::createPublisher(mp_participant,commandWparam,(PublisherListener*)&pub_listener);


    SubscriberAttributes nodeRparam;
    nodeRparam.topic.topicKind = NO_KEY;
    nodeRparam.topic.topicDataType = nodeType.getName(); //Must be registered before the creation of the subscriber
    nodeRparam.topic.topicName = "NodeData";
    node_subscriber = Domain::createSubscriber(mp_participant,nodeRparam,(SubscriberListener*)&node_sub_listener);


    if(tick_publisher == nullptr || command_publisher == nullptr || node_subscriber == nullptr)
        return false;

    std::cout << "Initialized publishers and subscribers." << std::endl;
    return true;
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
		// TODO: write a tick
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
    AMM::PatientAction::BioGears::Command cmdInstance;
    cmdInstance.message(command);
    cout << "=== [CommandExecutor] Sending a command containing:" << endl;
    cout << "    Command : \"" << cmdInstance.message() << "\"" << endl;
    command_publisher->write(&cmdInstance);
}

void SimulationManager::TickLoop() {
	using frames = duration<int64_t, ratio<1, 50>>;
	auto nextFrame = system_clock::now();
	auto lastFrame = nextFrame - frames { 1 };

    AMM::Simulation::Tick tick;

	while (m_runThread) {
		this_thread::sleep_until(nextFrame);
		m_mutex.lock();

        tick.frame(tickCount++);
        tick_publisher->write(&tick);

		lastFrame = nextFrame;
		nextFrame += frames { 1 };
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

    AMM::Simulation::Tick tick;
    tick.frame(-1);
    tick_publisher->write(&tick);

	Cleanup();

}





void SimulationManager::NodeSubListener::onSubscriptionMatched(Subscriber* sub,MatchingInfo& info)
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

void SimulationManager::NodeSubListener::onNewDataMessage(Subscriber* sub)
{
    // Take data
    AMM::Physiology::Node st;

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

void SimulationManager::PubListener::onPublicationMatched(Publisher* pub,MatchingInfo& info)
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


