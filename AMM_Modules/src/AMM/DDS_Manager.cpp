#include "DDS_Manager.h"

using namespace std::chrono;

using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

namespace AMM {


    DDS_Manager::DDS_Manager(const char *nodeName) {
        ParticipantAttributes PParam;
        PParam.rtps.builtin.domainId = (uint32_t) domainId;
        PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
        PParam.rtps.setName(nodeName);
        mp_participant = Domain::createParticipant(PParam);

        if (mp_participant == nullptr) {
            LOG_FATAL << "Unable to create FastRTPS domain participant.";
            return;
        }

        RegisterTypes();

        auto *pub_listener = new DDS_Listeners::PubListener();

        command_publisher = InitializePublisher(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(),
                                                     pub_listener);

        settings_publisher = InitializePublisher(AMM::DataTypes::instrumentDataTopic,
                                                      AMM::DataTypes::getInstrumentDataType(), pub_listener);
        physiology_command_publisher = InitializePublisher(AMM::DataTypes::physiologyCommandTopic, AMM::DataTypes::getPhysiologyCommandType(),
                                                pub_listener);

        perfdata_publisher = InitializePublisher(AMM::DataTypes::performanceTopic,
                                                AMM::DataTypes::getPerformanceAssessmentDataType(),
                                                pub_listener);

        physmod_publisher = InitializePublisher(AMM::DataTypes::physModTopic,
                                                AMM::DataTypes::getPhysiologyModificationType(),
                                                pub_listener);

        render_publisher = InitializePublisher(AMM::DataTypes::renderModTopic,
                                               AMM::DataTypes::getRenderModificationType(),
                                               pub_listener);

        config_publisher = InitializePublisher(AMM::DataTypes::configurationTopic,
                                               AMM::DataTypes::getConfigurationType(),
                                               pub_listener);

        status_publisher = InitializePublisher(AMM::DataTypes::statusTopic, AMM::DataTypes::getStatusType(),
                                               pub_listener);

        module_id = GenerateID();
    }

    Participant *DDS_Manager::GetParticipant() {
        return mp_participant;
    }

    std::string DDS_Manager::GenerateID() {
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        return boost::uuids::to_string(uuid);
    }

    void DDS_Manager::RegisterTypes() {
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getTickType());

        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getNodeType());
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getHighFrequencyNodeType());
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getPhysiologyCommandType());

        // AMM Patient Action / Intervention types
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getCommandType());

        // AMM Performance types
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getxAPIStatementType());

        // AMM Resource Requirements types
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getAirRequirementType());
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getBloodRequirementType());
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getCleaningSolutionRequirementType());
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getClearLiquidRequirementType());
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getPowerRequirementType());

        // AMM Resource Supply types
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getAirSupplyType());
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getBloodSupplyType());
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getCleaningSolutionSupplyType());
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getClearLiquidSupplyType());
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getPowerSupplyType());

        // AMM Capability types
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getConfigurationType());
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getStatusType());

        // AMM instrument data type
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getInstrumentDataType());

        // AMM Modification data types
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getPhysiologyModificationType());
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getRenderModificationType());

        // AMM performance assessment data
        Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getPerformanceAssessmentDataType());

    }

    Publisher *DDS_Manager::InitializePublisher(const std::string &topicName, TopicDataType *topicType,
                                                PublisherListener *pub_listener) {
        PublisherAttributes wparam;
        wparam.topic.topicDataType = topicType->getName();
        wparam.topic.topicName = topicName;
        Publisher *gen_publisher = Domain::createPublisher(mp_participant, wparam, pub_listener);
        return gen_publisher;
    }

    Subscriber *DDS_Manager::InitializeSubscriber(const std::string &topicName, TopicDataType *topicType,
                                                  SubscriberListener *sub_listener,
                                                  TopicKind_t topicKind
    ) {
        SubscriberAttributes rparam;
        rparam.topic.topicDataType = topicType->getName();
        rparam.topic.topicName = topicName;
        rparam.topic.topicKind = topicKind;
        Subscriber *gen_subscriber = Domain::createSubscriber(mp_participant, rparam, sub_listener);
        return gen_subscriber;
    }

/**
 *
 * Allow override of current module ID.  This is for use with bridge modules (TCP, serial) that support
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
    void DDS_Manager::PublishModuleConfiguration(
            const std::string &local_module_id,
            const std::string &module_name,
            const std::string &manufacturer,
            const std::string &model,
            const std::string &serial_number,
            const std::string &version,
            const std::string &capabilities
    ) {
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
        try {
            config_publisher->write(&configInstance);
        } catch (std::exception &e) {
            LOG_ERROR << "[DDS_Manager][config]" << e.what();
        }
    }

    void DDS_Manager::SetStatus(const std::string &local_module_id, const std::string &module_name,
                                AMM::Capability::status_values status) {
        AMM::Capability::Status statusInstance;
        statusInstance.module_id(local_module_id);
        statusInstance.module_name(module_name);
        statusInstance.status_value(status);
        SetStatus(statusInstance);
    }

    void DDS_Manager::SetStatus(const std::string &local_module_id, const std::string &module_name,
                                AMM::Capability::status_values status,
                                const std::vector <std::string> &message) {
        AMM::Capability::Status statusInstance;
        statusInstance.module_id(local_module_id);
        statusInstance.status_value(status);
        statusInstance.message(message);
        statusInstance.module_name(module_name);
        SetStatus(statusInstance);
    }

    void DDS_Manager::SetStatus(const std::string &local_module_id, const std::string &module_name,
                                const std::string &capability, AMM::Capability::status_values status) {
        AMM::Capability::Status statusInstance;
        statusInstance.module_id(local_module_id);
        statusInstance.module_name(module_name);
        statusInstance.status_value(status);
        statusInstance.capability(capability);
        SetStatus(statusInstance);
    }

    void DDS_Manager::SetStatus(const std::string &local_module_id, const std::string &module_name,
                                const std::string &capability, AMM::Capability::status_values status,
                                const std::vector <std::string> &message) {
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
            status_publisher->write(&statusInstance);
        } catch (std::exception &e) {
            LOG_ERROR << "[DDS_Manager][status]" << e.what();
        }
    }

    void DDS_Manager::PublishCommand(AMM::PatientAction::BioGears::Command cmdInstance) {
        command_publisher->write(&cmdInstance);
    }

    void DDS_Manager::PublishRenderModification(AMM::Render::Modification modInstance) {
        render_publisher->write(&modInstance);
    }

    void DDS_Manager::PublishPhysiologyModification(AMM::Physiology::Modification modInstance) {
        physmod_publisher->write(&modInstance);
    }

    void DDS_Manager::PublishInstrumentData(AMM::InstrumentData instrumentDataInstance) {
        settings_publisher->write(&instrumentDataInstance);
    }

    void DDS_Manager::PublishPerformanceData(AMM::Performance::Assessment assessmentInstance) {
        perfdata_publisher->write(&assessmentInstance);
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
