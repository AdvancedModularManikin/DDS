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

class DDS_Manager {
public:
    DDS_Manager();
    virtual ~DDS_Manager() = default;;

    Publisher *InitializeTickPublisher(PublisherListener *pub_listener);

    Subscriber *InitializeTickSubscriber(SubscriberListener *sub_listener);

    Publisher *InitializeCommandPublisher(PublisherListener *pub_listener);

    Subscriber *InitializeCommandSubscriber(SubscriberListener *sub_listener);

    Publisher *InitializeNodePublisher(PublisherListener *pub_listener);

    Subscriber *InitializeNodeSubscriber(SubscriberListener *sub_listener);

    // @TODO: Move these into an XML file, load by profile
    const int domainId = 15;
    const char *partitionName = "AMM";
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

    PublisherListener *default_pub_listener;
    SubscriberListener *default_sub_listener;

    TickPubSubType tickType;
    NodePubSubType nodeType;
    CommandPubSubType commandType;
};