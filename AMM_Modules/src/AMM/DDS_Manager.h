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

#include "AMM/DataTypes.h"

#include "AMM/Listeners/DDS_Listeners.h"
#include "AMM/Listeners/ListenerInterface.h"

#include "AMM/BaseLogger.h"

using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

namespace AMM {
    class DDS_Manager {
    public:
        explicit DDS_Manager(const char *nodeName);

        /** Publish configuration signatures **/
        void PublishModuleConfiguration(
                const std::string &local_module_id,
                const std::string &module_name,
                const std::string &manufacturer,
                const std::string &model,
                const std::string &serial_number,
                const std::string &version,
                const std::string &capabilities
        );
        void PublishModuleConfiguration(AMM::Capability::Configuration configInstance);

        /** Set status signatures **/
        void SetStatus(const std::string &local_module_id, const std::string &module_name,
                       AMM::Capability::status_values status);

        void SetStatus(const std::string &local_module_id, const std::string &module_name,
                       AMM::Capability::status_values status,
                       const std::vector <std::string> &message);

        void
        SetStatus(const std::string &local_module_id, const std::string &module_name, const std::string &capability,
                  AMM::Capability::status_values status);

        void
        SetStatus(const std::string &local_module_id, const std::string &module_name, const std::string &capability,
                  AMM::Capability::status_values status,
                  const std::vector <std::string> &message);

        void SetStatus(AMM::Capability::Status statusInstance);

        /** Helpers for publishing common data types **/
        void PublishCommand(AMM::PatientAction::BioGears::Command cmdInstance);

        void PublishRenderModification(AMM::Render::Modification modInstance);

        void PublishPhysiologyModification(AMM::Physiology::Modification modInstance);

        void PublishPerformanceData(AMM::Performance::Assessment assessmentInstance);

        void PublishInstrumentData(AMM::InstrumentData instrumentDataInstance);

        /**
         * Generic publisher initialization
         *
         * @param topicName
         * @param topicType
         * @param pub_listener
         * @return
         */
        Publisher *InitializePublisher(const std::string &topicName,
                                       TopicDataType *topicType,
                                       PublisherListener *pub_listener);

        /**
         * Generic subscriber initialization
         *
         * @param topicName
         * @param topicType
         * @param sub_listener
         * @param topicKind
         * @return
         */
        Subscriber *InitializeSubscriber(const std::string &topicName,
                                         TopicDataType *topicType,
                                         SubscriberListener *sub_listener,
                                         TopicKind_t topicKind = NO_KEY);

        void RegisterTypes();

        Participant *GetParticipant();

        std::string GetCapabilitiesAsString(const std::string &filename);

        std::string GenerateID();

        std::string currentScenario;

        std::string GetScenario();

        void SetScenario(std::string scenario);

        std::string module_id;

    protected:
        // @TODO: Load this from a config file
        const int domainId = 15;
        const std::string scenarioFile = "current_scenario.txt";

        Participant *mp_participant;

        Publisher *genPub;
        Publisher *command_publisher;
        Publisher *settings_publisher;
        Publisher *perfdata_publisher;
        Publisher *physmod_publisher;
        Publisher *render_publisher;
        Publisher *config_publisher;
        Publisher *status_publisher;
    };
}