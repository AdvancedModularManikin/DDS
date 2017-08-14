#include "DDS_Manager.h"


using namespace std;
using namespace std::chrono;

bool DDS_Manager::init() {
    ParticipantAttributes PParam;
    PParam.rtps.builtin.domainId = 0;
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
    node_subscriber = Domain::createSubscriber(mp_participant,nodeRparam,(SubscriberListener*)&sub_listener);


    if(tick_publisher == nullptr || command_publisher == nullptr || node_subscriber == nullptr)
        return false;

    std::cout << "Initialized publishers and subscribers." << std::endl;
    return true;
}




void DDS_Manager::SubListener::onSubscriptionMatched(Subscriber* sub,MatchingInfo& info)
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

void DDS_Manager::SubListener::onNewDataMessage(Subscriber* sub)
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

void DDS_Manager::PubListener::onPublicationMatched(Publisher* pub,MatchingInfo& info)
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
