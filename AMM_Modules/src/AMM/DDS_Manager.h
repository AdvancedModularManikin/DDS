#pragma once

#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>

#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/attributes/PublisherAttributes.h>

#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/attributes/SubscriberAttributes.h>

#include <fastrtps/Domain.h>

#include <fastrtps/utils/eClock.h>

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/subscriber/SampleInfo.h>

#include "AMMPubSubTypes.h"

#include <fastrtps/Domain.h>

#include "AMM/DDS_Listeners.h"
#include "AMM/ListenerInterface.h"

using namespace eprosima;
using namespace eprosima::fastrtps;

using namespace AMM::Physiology;
using namespace AMM::Simulation;
using namespace AMM::PatientAction::BioGears;

class DDS_Manager
{
public:
    DDS_Manager();
    virtual ~DDS_Manager() {};

    Subscriber* InitializeTickSubscriber(DDS_Listeners::TickSubListener* sub_listener);
    Publisher* InitializeTickPublisher(DDS_Listeners::PubListener* pub_listener);

    Subscriber* InitializeCommandSubscriber(DDS_Listeners::CommandSubListener* sub_listener);
    Publisher* InitializeCommandPublisher(DDS_Listeners::PubListener* pub_listener);

    Subscriber* InitializeNodeSubscriber(DDS_Listeners::NodeSubListener* sub_listener);
    Publisher* InitializeNodePublisher(DDS_Listeners::PubListener* pub_listener);

    const int domainId = 15;
    const char* partitionName = "AMM";
    const std::string tickTopic = "Tick";
    const std::string commandTopic = "Command";
    const std::string nodeTopic = "Data";

private:

    Participant *mp_participant;

    Publisher *tick_publisher;
    Publisher *command_publisher;
    Publisher *node_publisher;

    Subscriber *tick_subscriber;
    Subscriber *command_subscriber;
    Subscriber *node_subscriber;

    TickPubSubType tickType;
    NodePubSubType nodeType;
    CommandPubSubType commandType;
};