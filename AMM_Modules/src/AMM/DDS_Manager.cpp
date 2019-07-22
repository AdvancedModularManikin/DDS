#include "DDS_Manager.h"

using namespace std::chrono;

using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

namespace AMM {

    DDS_Manager::DDS_Manager(const char *nodeName) {
        LOG_DEBUG << "Instantiating DDS manager";

        ParticipantAttributes PParam;
        PParam.rtps.builtin.domainId = (uint32_t) domainId;
        // PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
        PParam.rtps.setName(nodeName);
        mp_participant = Domain::createParticipant(PParam);

        if (mp_participant == nullptr) {
            LOG_ERROR << "Unable to create FastRTPS domain participant.";
            return;
        }

        RegisterTypes();

        pub_listener = new DDS_Listeners::PubListener();

        module_id = GenerateID();
        module_name = nodeName;
    }

    Participant *DDS_Manager::GetParticipant() { return mp_participant; }

    std::string DDS_Manager::GenerateID() {
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        return boost::uuids::to_string(uuid);
    }

    void DDS_Manager::RegisterTypes() {
        Domain::registerType(mp_participant, (TopicDataType *) &TickType);

        Domain::registerType(mp_participant, (TopicDataType *) &NodeType);
        Domain::registerType(mp_participant, (TopicDataType *) &HighFrequencyNodeType);
        Domain::registerType(mp_participant, (TopicDataType *) &PhysiologyCommandType);

        // AMM Patient Action / Intervention types
        Domain::registerType(mp_participant, (TopicDataType *) &CommandType);

        // AMM Performance types
        Domain::registerType(mp_participant, (TopicDataType *) &xAPIStatementType);

        // AMM Resource Requirements types
        Domain::registerType(mp_participant, (TopicDataType *) &AirRequirementType);
        Domain::registerType(mp_participant, (TopicDataType *) &BloodRequirementType);
        Domain::registerType(mp_participant, (TopicDataType *) &CleaningSolutionRequirementType);
        Domain::registerType(mp_participant, (TopicDataType *) &ClearLiquidRequirementType);
        Domain::registerType(mp_participant, (TopicDataType *) &PowerRequirementType);

        // AMM Resource Supply types
        Domain::registerType(mp_participant, (TopicDataType *) &AirSupplyType);
        Domain::registerType(mp_participant, (TopicDataType *) &BloodSupplyType);
        Domain::registerType(mp_participant, (TopicDataType *) &CleaningSolutionSupplyType);
        Domain::registerType(mp_participant, (TopicDataType *) &ClearLiquidSupplyType);
        Domain::registerType(mp_participant, (TopicDataType *) &PowerSupplyType);


        // AMM Capability types
        Domain::registerType(mp_participant, (TopicDataType *) &ConfigurationType);
        Domain::registerType(mp_participant, (TopicDataType *) &StatusType);

        // AMM instrument data type
        Domain::registerType(mp_participant, (TopicDataType *) &InstrumentDataType);

        // AMM Modification data types
        Domain::registerType(mp_participant, (TopicDataType *) &PhysiologyModificationType);
        Domain::registerType(mp_participant, (TopicDataType *) &RenderModificationType);

        // AMM performance assessment data
        Domain::registerType(mp_participant, (TopicDataType *) &PerformanceAssessmentDataType);

        Domain::registerType(mp_participant, (TopicDataType *) &LogRecordType);
    }

    Publisher *DDS_Manager::InitializeConfigPublisher(const std::string &topicName,
                                                TopicDataType *topicType,
                                                PublisherListener *pub_listener) {
        PublisherAttributes wparam;
        wparam.topic.topicDataType = topicType->getName();
        wparam.topic.topicName = topicName;
        wparam.topic.topicKind = NO_KEY;
        wparam.qos.m_publishMode.kind = ASYNCHRONOUS_PUBLISH_MODE;
        wparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
        Publisher *gen_publisher = Domain::createPublisher(mp_participant, wparam, pub_listener);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return gen_publisher;
    }

    Publisher *DDS_Manager::InitializePublisher(const std::string &topicName,
                                                TopicDataType *topicType,
                                                PublisherListener *pub_listener) {
        PublisherAttributes wparam;
        wparam.topic.topicDataType = topicType->getName();
        wparam.topic.topicName = topicName;
        // wparam.topic.topicKind = NO_KEY;
        Publisher *gen_publisher = Domain::createPublisher(mp_participant, wparam, pub_listener);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return gen_publisher;
    }

    Publisher *
    DDS_Manager::InitializeReliablePublisher(const std::string &topicName,
                                             TopicDataType *topicType,
                                             PublisherListener *pub_listener) {
        PublisherAttributes wparam;
        wparam.topic.topicDataType = topicType->getName();
        wparam.topic.topicName = topicName;
        // wparam.topic.topicKind = NO_KEY;
        wparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
        Publisher *gen_publisher = Domain::createPublisher(mp_participant, wparam, pub_listener);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return gen_publisher;
    }

    Subscriber *DDS_Manager::InitializeSubscriber(const std::string &topicName,
                                                  TopicDataType *topicType,
                                                  SubscriberListener *sub_listener,
                                                  TopicKind_t topicKind) {
        SubscriberAttributes rparam;
        rparam.topic.topicDataType = topicType->getName();
        rparam.topic.topicName = topicName;
        rparam.topic.topicKind = topicKind;
        Subscriber *gen_subscriber = Domain::createSubscriber(mp_participant, rparam, sub_listener);
        return gen_subscriber;
    }

    Subscriber *DDS_Manager::InitializeReliableSubscriber(
            const std::string &topicName, TopicDataType *topicType,
            SubscriberListener *sub_listener, TopicKind_t topicKind) {
        SubscriberAttributes rparam;
        rparam.topic.topicDataType = topicType->getName();
        rparam.topic.topicName = topicName;
        rparam.topic.topicKind = topicKind;
        rparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
        Subscriber *gen_subscriber = Domain::createSubscriber(mp_participant, rparam, sub_listener);
        return gen_subscriber;
    }

/**
 *
 * Allow override of current module ID.  This is for use with bridge modules
 * (TCP, serial) that support
 * multiple modules underneath them.
 *
 * @param local_module_id
 * @param module_name
 * @param manufacturer
 * @param model
 * @param serial_number
 * @param version
 * @param capabilities
 */
    void DDS_Manager::PublishModuleConfiguration(const std::string &local_module_id,
                                                 const std::string &module_name,
                                                 const std::string &manufacturer,
                                                 const std::string &model,
                                                 const std::string &serial_number,
                                                 const std::string &version,
                                                 const std::string &capabilities) {
        AMM::Capability::Configuration configInstance;
        configInstance.module_id(local_module_id);
        configInstance.module_name(module_name);
        configInstance.manufacturer(manufacturer);
        configInstance.model(model);
        configInstance.serial_number(serial_number);
        configInstance.version(version);
        configInstance.capabilities(capabilities);
        PublishModuleConfiguration(configInstance);
    }

    void DDS_Manager::PublishModuleConfiguration(AMM::Capability::Configuration configInstance) {
        LOG_DEBUG << "Publishing module configuration";
        try {
            if (!config_initialized) {
                config_publisher = InitializeConfigPublisher(
                        AMM::DataTypes::configurationTopic,
                        &ConfigurationType, pub_listener);
                config_initialized = true;
            }
            config_publisher->write(&configInstance);
        } catch (std::exception &e) {
            LOG_ERROR << e.what();
        }
    }

    void DDS_Manager::PublishLogRecord(const std::string &message) {
        PublishLogRecord(message, "info");
    }

    void DDS_Manager::PublishLogRecord(const std::string &message, const std::string &log_level) {
        long now = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        AMM::Diagnostics::Log::Record logInstance;
        try {
            logInstance.module_id(module_id);
            logInstance.module_name(module_name);
            logInstance.message(message);
            logInstance.log_level(log_level);
            logInstance.timestamp(now);
            if (!log_initialized) {
                log_publisher = InitializeReliablePublisher(
                        AMM::DataTypes::logRecordTopic, &LogRecordType,
                        pub_listener);
                log_initialized = true;
            }
            log_publisher->write(&logInstance);
        } catch (std::exception &e) {
            LOG_ERROR << e.what();
        }
    }

    void DDS_Manager::SetStatus(const std::string &local_module_id,
                                const std::string &module_name,
                                AMM::Capability::status_values status) {
        AMM::Capability::Status statusInstance;
        statusInstance.module_id(local_module_id);
        statusInstance.module_name(module_name);
        statusInstance.status_value(status);
        SetStatus(statusInstance);
    }

    void DDS_Manager::SetStatus(const std::string &local_module_id,
                                const std::string &module_name,
                                AMM::Capability::status_values status,
                                const std::vector<std::string> &message) {
        AMM::Capability::Status statusInstance;
        statusInstance.module_id(local_module_id);
        statusInstance.status_value(status);
        statusInstance.message(message);
        statusInstance.module_name(module_name);
        SetStatus(statusInstance);
    }

    void DDS_Manager::SetStatus(const std::string &local_module_id,
                                const std::string &module_name,
                                const std::string &capability,
                                AMM::Capability::status_values status) {
        AMM::Capability::Status statusInstance;
        statusInstance.module_id(local_module_id);
        statusInstance.module_name(module_name);
        statusInstance.status_value(status);
        statusInstance.capability(capability);
        SetStatus(statusInstance);
    }

    void DDS_Manager::SetStatus(const std::string &local_module_id,
                                const std::string &module_name,
                                const std::string &capability,
                                AMM::Capability::status_values status,
                                const std::vector<std::string> &message) {
        AMM::Capability::Status statusInstance;
        statusInstance.module_id(local_module_id);
        statusInstance.status_value(status);
        statusInstance.message(message);
        statusInstance.module_name(module_name);
        statusInstance.capability(capability);
        SetStatus(statusInstance);
    }

    void DDS_Manager::SetStatus(AMM::Capability::Status statusInstance) {
        try {
            if (!status_initialized) {
                status_publisher = InitializeReliablePublisher(
                        AMM::DataTypes::statusTopic, &StatusType,
                        pub_listener);
                status_initialized = true;
            }
            status_publisher->write(&statusInstance);
        } catch (std::exception &e) {
            LOG_ERROR << e.what();
        }
    }

    void DDS_Manager::PublishCommand(
            AMM::PatientAction::BioGears::Command cmdInstance) {
        if (!command_initialized) {
            command_publisher =
                    InitializeReliablePublisher(AMM::DataTypes::commandTopic,
                                                &CommandType, pub_listener);
            command_initialized = true;
        }
        try {
            command_publisher->write(&cmdInstance);
        } catch (std::exception &e) {
            LOG_ERROR << e.what();
        }
    }

    void DDS_Manager::PublishRenderModification(
            AMM::Render::Modification modInstance) {
        if (!render_initialized) {
            render_publisher = InitializeReliablePublisher(
                    AMM::DataTypes::renderModTopic,
                    &RenderModificationType, pub_listener);
            render_initialized = true;
        }
        try {
            render_publisher->write(&modInstance);
        } catch (std::exception &e) {
            LOG_ERROR << e.what();
        }
    }

    void DDS_Manager::PublishPhysiologyModification(
            AMM::Physiology::Modification modInstance) {
        if (!physmod_initialized) {
            physmod_publisher = InitializeReliablePublisher(
                    AMM::DataTypes::physModTopic,
                    &PhysiologyModificationType, pub_listener);
            physcommand_initialized = true;
        }
        try {
            physmod_publisher->write(&modInstance);
        } catch (std::exception &e) {
            LOG_ERROR << e.what();
        }
    }

    void DDS_Manager::PublishInstrumentData(
            AMM::InstrumentData instrumentDataInstance) {
        if (!settings_initialized) {
            settings_publisher = InitializeReliablePublisher(
                    AMM::DataTypes::instrumentDataTopic,
                    &InstrumentDataType, pub_listener);
            settings_initialized = true;
        }
        try {
            settings_publisher->write(&instrumentDataInstance);
        } catch (std::exception &e) {
            LOG_ERROR << e.what();
        }
    }

    void DDS_Manager::PublishPerformanceData(
            AMM::Performance::Assessment assessmentInstance) {
        if (!perfdata_initialized) {
            perfdata_publisher = InitializeReliablePublisher(
                    AMM::DataTypes::performanceTopic,
                    &PerformanceAssessmentDataType, pub_listener);
            perfdata_initialized = true;
        }
        try {
            perfdata_publisher->write(&assessmentInstance);
        } catch (std::exception &e) {
            LOG_ERROR << e.what();
        }
    }

    std::string DDS_Manager::GetCapabilitiesAsString(const std::string &filename) {
        std::ifstream ifs(filename);
        std::string capabilitiesContent((std::istreambuf_iterator<char>(ifs)),
                                        (std::istreambuf_iterator<char>()));
        return capabilitiesContent;
    }

    std::string DDS_Manager::GetScenario() {
        std::ifstream t(scenarioFile);
        std::string str((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
        currentScenario = str;
        return currentScenario;
    }

    void DDS_Manager::SetScenario(std::string scenario) {
        currentScenario = scenario;
        std::ofstream out(scenarioFile);
        out << currentScenario;
        out.close();
    }

}
