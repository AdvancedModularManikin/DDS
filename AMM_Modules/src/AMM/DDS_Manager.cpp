#include "DDS_Manager.h"

// @TODO: Add overrides to make the listeners optional, attach the default listener

using namespace std;
using namespace std::chrono;

using namespace eprosima;
using namespace eprosima::fastrtps;

DDS_Manager::DDS_Manager() {
    ParticipantAttributes PParam;
    PParam.rtps.builtin.domainId = domainId;
    PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
    PParam.rtps.setName(partitionName);
    mp_participant = Domain::createParticipant(PParam);

    if (mp_participant == nullptr) {
        std::cout << "Unable to create FastRTPS domain participant." << endl;
        return;
    }

    Domain::registerType(mp_participant, (TopicDataType *) &tickType);
    Domain::registerType(mp_participant, (TopicDataType *) &nodeType);
    Domain::registerType(mp_participant, (TopicDataType *) &commandType);

    // Create some default listeners to use so higher level code doesn't need to require them
    default_pub_listener = new DDS_Listeners::PubListener();
    default_sub_listener = new DDS_Listeners::DefaultSubListener();
}

Publisher *DDS_Manager::InitializeTickPublisher(
        PublisherListener *pub_listener) {
    PublisherAttributes tickWparam;
    tickWparam.topic.topicDataType = tickType.getName();
    tickWparam.topic.topicName = tickTopic;
    tick_publisher = Domain::createPublisher(mp_participant, tickWparam,
                                             pub_listener);
    return tick_publisher;
}

Subscriber *DDS_Manager::InitializeTickSubscriber(
        SubscriberListener *sub_listener) {
    SubscriberAttributes tickRparam;
    tickRparam.topic.topicDataType = tickType.getName();
    tickRparam.topic.topicName = tickTopic;
    tick_subscriber = Domain::createSubscriber(mp_participant, tickRparam,
                                               sub_listener);
    return tick_subscriber;
}

Publisher *DDS_Manager::InitializeNodePublisher(
        PublisherListener *pub_listener) {
    PublisherAttributes nodeWparam;
    nodeWparam.topic.topicDataType = nodeType.getName();
    nodeWparam.topic.topicName = nodeTopic;
    node_publisher = Domain::createPublisher(mp_participant, nodeWparam,
                                             pub_listener);
    return node_publisher;
}

Subscriber *DDS_Manager::InitializeNodeSubscriber(
        SubscriberListener *sub_listener) {
    SubscriberAttributes nodeRparam;
    nodeRparam.topic.topicDataType = nodeType.getName();
    nodeRparam.topic.topicName = nodeTopic;
    node_subscriber = Domain::createSubscriber(mp_participant, nodeRparam,
                                               sub_listener);
    return node_subscriber;
}

Publisher *DDS_Manager::InitializeCommandPublisher(
        PublisherListener *pub_listener) {
    PublisherAttributes commandWparam;
    commandWparam.topic.topicDataType = commandType.getName();
    commandWparam.topic.topicName = commandTopic;
    command_publisher = Domain::createPublisher(mp_participant, commandWparam,
                                                pub_listener);
    return command_publisher;
}

Subscriber *DDS_Manager::InitializeCommandSubscriber(
        SubscriberListener *sub_listener) {
    SubscriberAttributes commandRparam;
    commandRparam.topic.topicDataType = commandType.getName();
    commandRparam.topic.topicName = commandTopic;
    command_subscriber = Domain::createSubscriber(mp_participant, commandRparam,
                                                  sub_listener);
    return command_subscriber;
}
