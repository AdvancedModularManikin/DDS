#pragma once

#include <fstream>
#include <sstream>
#include <streambuf>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/participant/Participant.h>

#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/Publisher.h>

#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/Subscriber.h>

#include <fastrtps/Domain.h>
#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/utils/eClock.h>

#include "AMM/DataTypes.h"

#include "AMM/Listeners/DDS_Listeners.h"

#include "AMM/BaseLogger.h"

using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

namespace AMM {
    class DDS_Manager {
    public:
        explicit DDS_Manager(const char *nodeName);

        /** Publish configuration signatures **/
        void PublishModuleConfiguration(const std::string &local_module_id,
                                        const std::string &module_name,
                                        const std::string &manufacturer,
                                        const std::string &model,
                                        const std::string &serial_number,
                                        const std::string &version,
                                        const std::string &capabilities);

        void
        PublishModuleConfiguration(AMM::Capability::Configuration configInstance);

        /** Set status signatures **/
        void SetStatus(const std::string &local_module_id,
                       const std::string &module_name,
                       AMM::Capability::status_values status);

        void SetStatus(const std::string &local_module_id,
                       const std::string &module_name,
                       AMM::Capability::status_values status,
                       const std::vector<std::string> &message);

        void SetStatus(const std::string &local_module_id,
                       const std::string &module_name, const std::string &capability,
                       AMM::Capability::status_values status);

        void SetStatus(const std::string &local_module_id,
                       const std::string &module_name, const std::string &capability,
                       AMM::Capability::status_values status,
                       const std::vector<std::string> &message);

        void SetStatus(AMM::Capability::Status statusInstance);

        /** Helpers for publishing common data types **/
        void PublishCommand(AMM::PatientAction::BioGears::Command cmdInstance);

        void PublishRenderModification(AMM::Render::Modification modInstance);

        void PublishPhysiologyModification(AMM::Physiology::Modification modInstance);

        void PublishPerformanceData(AMM::Performance::Assessment assessmentInstance);

        void PublishInstrumentData(AMM::InstrumentData instrumentDataInstance);

        void PublishLogRecord(const std::string &message);

        void PublishLogRecord(const std::string &message, const std::string &log_level);

        Publisher *InitializeReliablePublisher(const std::string &topicName,
                                               TopicDataType *topicType,
                                               PublisherListener *pub_listener);

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

        Subscriber *InitializeReliableSubscriber(const std::string &topicName,
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

        std::string module_name;

        AMM::Simulation::TickPubSubType TickType;

        // AMM Physiology types
        AMM::Physiology::NodePubSubType NodeType;

        AMM::Physiology::HighFrequencyNodePubSubType HighFrequencyNodeType;

        AMM::Physiology::CommandPubSubType PhysiologyCommandType;

        // AMM Patient Action / Intervention types
        AMM::PatientAction::BioGears::CommandPubSubType CommandType;

        // AMM Performance types
        AMM::Performance::StatementPubSubType xAPIStatementType;

        // AMM Resource Requirements types
        AMM::Resource::Requirement::AirPubSubType AirRequirementType;

        AMM::Resource::Requirement::BloodPubSubType BloodRequirementType;

        AMM::Resource::Requirement::Cleaning_SolutionPubSubType CleaningSolutionRequirementType;

        AMM::Resource::Requirement::Clear_LiquidPubSubType ClearLiquidRequirementType;

        AMM::Resource::Requirement::PowerPubSubType PowerRequirementType;

        // AMM Resource Supply types
        AMM::Resource::Supply::AirPubSubType AirSupplyType;

        AMM::Resource::Supply::BloodPubSubType BloodSupplyType;

        AMM::Resource::Supply::Cleaning_SolutionPubSubType CleaningSolutionSupplyType;

        AMM::Resource::Supply::Clear_LiquidPubSubType ClearLiquidSupplyType;

        AMM::Resource::Supply::PowerPubSubType PowerSupplyType;

        // AMM Capability types
        AMM::Capability::ConfigurationPubSubType ConfigurationType;

        AMM::Capability::StatusPubSubType StatusType;

        AMM::Capability::ScenarioPubSubType ScenarioType;

        // AMM Instrument Data type
        AMM::InstrumentDataPubSubType InstrumentDataType;

        AMM::Performance::AssessmentPubSubType PerformanceAssessmentDataType;

        AMM::Render::ModificationPubSubType RenderModificationType;

        AMM::Physiology::ModificationPubSubType PhysiologyModificationType;

        AMM::Diagnostics::Log::RecordPubSubType LogRecordType;

    protected:
        // @TODO: Load this from a config file
        const int domainId = 15;
        const std::string scenarioFile = "current_scenario.txt";

        Participant *mp_participant;
        DDS_Listeners::PubListener* pub_listener;
        Publisher *log_publisher;
        bool log_initialized = false;
        Publisher *command_publisher;
        bool command_initialized = false;
        Publisher *settings_publisher;
        bool settings_initialized = false;
        Publisher *physiology_command_publisher;
        bool physcommand_initialized = false;
        Publisher *perfdata_publisher;
        bool perfdata_initialized = false;
        Publisher *physmod_publisher;
        bool physmod_initialized = false;
        Publisher *render_publisher;
        bool render_initialized = false;
        Publisher *config_publisher;
        bool config_initialized = false;
        Publisher *status_publisher;
        bool status_initialized = false;
    };
}
