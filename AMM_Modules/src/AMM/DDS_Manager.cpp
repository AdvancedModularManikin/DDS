#include "DDS_Manager.h"

using namespace std;
using namespace std::chrono;

using namespace eprosima;
using namespace eprosima::fastrtps;

DDS_Manager::DDS_Manager() {
    DDS_Manager(partitionName);
}

DDS_Manager::DDS_Manager(const char *nodeName) {
    ParticipantAttributes PParam;
    PParam.rtps.builtin.domainId = (uint32_t) domainId;
    PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName(nodeName);
    mp_participant = Domain::createParticipant(PParam);

    if (mp_participant == nullptr) {
        std::cout << "Unable to create FastRTPS domain participant." << endl;
        return;
    }


    RegisterTypes();

    // Create some default listeners to use so higher level code doesn't need to require them
    default_pub_listener = new DDS_Listeners::PubListener();
    default_sub_listener = new DDS_Listeners::DefaultSubListener();
}

Participant* DDS_Manager::GetParticipant() {
    return mp_participant;
}

void DDS_Manager::RegisterTypes() {
    Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getNodeType());
    Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getHighFrequencyNodeType());

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

    // AMM Logging types
    Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getDebugLogType());
    Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getErrorLogType());
    Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getWarningLogType());
    Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getInfoLogType());


}

Publisher *DDS_Manager::InitializePublisher(std::string topicName, TopicDataType *topicType,
                                            PublisherListener *pub_listener) {
    PublisherAttributes wparam;
    wparam.topic.topicDataType = topicType->getName();
    wparam.topic.topicName = topicName;
    Publisher *gen_publisher = Domain::createPublisher(mp_participant, wparam, pub_listener);
    return gen_publisher;
}

Subscriber *DDS_Manager::InitializeSubscriber(std::string topicName, TopicDataType *topicType,
                                             SubscriberListener *sub_listener) {
    SubscriberAttributes rparam;
    rparam.topic.topicDataType = topicType->getName();
    rparam.topic.topicName = topicName;
    Subscriber *gen_subscriber = Domain::createSubscriber(mp_participant, rparam, sub_listener);
    return gen_subscriber;
}
