#include "DDS_Manager.h"

using namespace std;
using namespace std::chrono;

using namespace eprosima;
using namespace eprosima::fastrtps;

DDS_Manager::DDS_Manager() {
    ParticipantAttributes PParam;
    PParam.rtps.builtin.domainId = (uint32_t) domainId;
    PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName(partitionName);
    mp_participant = Domain::createParticipant(PParam);

    if (mp_participant == nullptr) {
        std::cout << "Unable to create FastRTPS domain participant." << endl;
        return;
    }

    Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getTickType());
    Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getNodeType());
    Domain::registerType(mp_participant, (TopicDataType *) AMM::DataTypes::getCommandType());

    // Create some default listeners to use so higher level code doesn't need to require them
    default_pub_listener = new DDS_Listeners::PubListener();
    default_sub_listener = new DDS_Listeners::DefaultSubListener();
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
