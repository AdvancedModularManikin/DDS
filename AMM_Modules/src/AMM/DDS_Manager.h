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
    DDS_Manager(const char *nodeName);
    DDS_Manager(const char *nodeName, ParticipantListener *participantListener);
    virtual ~DDS_Manager() = default;;

    void PublishModuleConfiguration(
            const std::string manufacturer,
            const std::string model,
            const std::string serial_number,
            const std::string version,
            const AMM::Capability::AMM_version &amm_version,
            const std::string capabilities
    );
    void PublishModuleConfiguration(
            const std::string manufacturer,
            const std::string model,
            const std::string serial_number,
            const std::string version,
            const std::string capabilities
    );
    void PublishModuleConfiguration(AMM::Capability::Configuration configInstance);

    void SetStatus(AMM::Capability::status_values status);
    void SetStatus(AMM::Capability::status_values status, const std::vector<std::string> &message);
    void SetStatus(AMM::Capability::Status statusInstance);

    Publisher *InitializePublisher(std::string topicName, TopicDataType *topicType, PublisherListener *pub_listener);
    Subscriber *InitializeSubscriber(std::string topicName, TopicDataType *topicType, SubscriberListener *sub_listener);

    Participant *GetParticipant();

    Publisher *InitializeStatusPublisher(PublisherListener *pub_listener);
    Publisher *InitializeConfigPublisher(PublisherListener *pub_listener);
    Publisher *InitializeCommandPublisher(PublisherListener *pub_listener);
    Publisher *InitializeNodePublisher(PublisherListener *pub_listener);
    Publisher *InitializeTickPublisher(PublisherListener *pub_listener);

    Publisher *status_publisher;
    Publisher *config_publisher;
    Publisher *command_publisher;
    Publisher *node_publisher;
    Publisher *tick_publisher;

    const int domainId = 15;
    const char *partitionName = "AMM";

private:
    void InitializeDefaults();

    void RegisterTypes();

    void RegisterPublishers();

    Participant *mp_participant;

    SubscriberListener *default_sub_listener = new DDS_Listeners::DefaultSubListener();
    PublisherListener *default_pub_listener = new DDS_Listeners::PubListener();

};