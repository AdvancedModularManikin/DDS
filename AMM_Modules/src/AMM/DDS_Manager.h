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
#include <fstream>
#include <streambuf>
#include <sstream>


using namespace std;
using namespace eprosima;
using namespace eprosima::fastrtps;

class DDS_Manager {
public:
    DDS_Manager(const char *nodeName);

    DDS_Manager(const char *nodeName, ParticipantListener *participantListener);
//    ~DDS_Manager();

    void PublishModuleConfiguration(
            const std::string &module_name,
            const std::string &manufacturer,
            const std::string &model,
            const std::string &serial_number,
            const std::string &version,
            const std::string &capabilities
    );

    void PublishModuleConfiguration(AMM::Capability::Configuration configInstance);

    void SetStatus(const std::string &module_name, AMM::Capability::status_values status);

    void SetStatus(const std::string &module_name, AMM::Capability::status_values status, const std::vector<std::string> &message);

    void SetStatus(AMM::Capability::Status statusInstance);

    Publisher *InitializePublisher(std::string topicName, TopicDataType *topicType, PublisherListener *pub_listener);

    Subscriber *InitializeSubscriber(std::string topicName,
                                     TopicDataType *topicType,
                                     SubscriberListener *sub_listener,
                                     TopicKind_t topicKind = NO_KEY);

    Participant *GetParticipant();

    Publisher *config_publisher;
    Publisher *status_publisher;

    const int domainId = 15;
    const char *partitionName = "AMM";

    void RegisterTypes();

    Participant *mp_participant;

    std::string GetCapabilitiesAsString(const std::string &filename);

    std::string currentScenario;
    const std::string scenarioFile = "mule1/current_scenario.txt";
    std::string GetScenario();
    void SetScenario(std::string scenario);
};