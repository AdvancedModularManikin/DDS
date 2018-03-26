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

#include "AMM/DataTypes.h"

#include "AMM/DDS_Listeners.h"
#include "AMM/ListenerInterface.h"

using namespace eprosima;
using namespace eprosima::fastrtps;

class DDS_Manager {
public:
    DDS_Manager();
    virtual ~DDS_Manager() = default;;

    Publisher *InitializePublisher(std::string topicName, TopicDataType *topicType, PublisherListener *pub_listener);
    Subscriber *InitializeSubscriber(std::string topicName, TopicDataType *topicType, SubscriberListener *sub_listener);

    const int domainId = 15;
    const char *partitionName = "AMM";

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

};