#pragma once

#include "stdafx.h"

#include <fstream>
#include <streambuf>
#include <sstream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

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

#include "src/AMM/Listeners/DDS_Listeners.h"
#include "src/AMM/Listeners/ListenerInterface.h"

#include "AMM/BaseLogger.h"

using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

class DDS_Manager {
public:
    explicit DDS_Manager(const char *nodeName);

    void PublishModuleConfiguration(
            const std::string &local_module_id,
            const std::string &module_name,
            const std::string &manufacturer,
            const std::string &model,
            const std::string &serial_number,
            const std::string &version,
            const std::string &capabilities
    );

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

    void SetStatus(const std::string &module_name, AMM::Capability::status_values status,
                   const std::vector<std::string> &message);

    void SetStatus(const std::string &module_name, const std::string &capability,
            AMM::Capability::status_values status);

    void SetStatus(const std::string &module_name, const std::string &capability,
            AMM::Capability::status_values status,
                   const std::vector<std::string> &message);

    void SetStatus(const std::string &module_name, const std::string &local_module_id, const std::string &capability,
                   AMM::Capability::status_values status);

    void SetStatus(const std::string &module_name, const std::string &local_module_id, const std::string &capability,
                   AMM::Capability::status_values status,
                   const std::vector<std::string> &message);

    void SetStatus(AMM::Capability::Status statusInstance);

    Publisher *InitializePublisher(std::string topicName, TopicDataType *topicType, PublisherListener *pub_listener);

    Subscriber *InitializeSubscriber(std::string topicName,
                                     TopicDataType *topicType,
                                     SubscriberListener *sub_listener,
                                     TopicKind_t topicKind = NO_KEY);

    Participant *GetParticipant();

    Publisher *config_publisher;
    Publisher *status_publisher;

    // @TODO: Load this from a config file
    const int domainId = 15;

    void RegisterTypes();

    Participant *mp_participant;

    std::string GetCapabilitiesAsString(const std::string &filename);

    std::string currentScenario;
    const std::string scenarioFile = "mule1/current_scenario.txt";

    std::string GetScenario();

    void SetScenario(std::string scenario);

    boost::uuids::uuid uuid;
    std::string module_id;
};