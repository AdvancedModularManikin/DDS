#include "DDS_Manager.h"

using namespace std;
using namespace std::chrono;

using namespace eprosima;
using namespace eprosima::fastrtps;

DDS_Manager::DDS_Manager() {
	ParticipantAttributes PParam;
	PParam.rtps.builtin.domainId = 15;
	PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
	PParam.rtps.setName("AMM");  //You can put here the name you want
	mp_participant = Domain::createParticipant(PParam);

	if (mp_participant == nullptr)
		return;

	Domain::registerType(mp_participant, (TopicDataType*) &tickType);
	Domain::registerType(mp_participant, (TopicDataType*) &nodeType);
	Domain::registerType(mp_participant, (TopicDataType*) &commandType);
}

Publisher* DDS_Manager::InitializeTickPublisher(
		PublisherListener* pub_listener) {
	PublisherAttributes tickWparam;
	tickWparam.topic.topicDataType = tickType.getName();
	tickWparam.topic.topicName = "Tick";
	tick_publisher = Domain::createPublisher(mp_participant, tickWparam,
			pub_listener);
	return tick_publisher;
}

Subscriber* DDS_Manager::InitializeTickSubscriber(
		SubscriberListener* sub_listener) {
	SubscriberAttributes tickRparam;
	tickRparam.topic.topicDataType = tickType.getName();
	tickRparam.topic.topicName = "Tick";
	tick_subscriber = Domain::createSubscriber(mp_participant, tickRparam,
			sub_listener);
	return tick_subscriber;
}

Publisher* DDS_Manager::InitializeNodePublisher(
		PublisherListener* pub_listener) {
	PublisherAttributes nodeWparam;
	nodeWparam.topic.topicDataType = tickType.getName();
	nodeWparam.topic.topicName = "Node";
	node_publisher = Domain::createPublisher(mp_participant, nodeWparam,
			pub_listener);
	return node_publisher;
}

Subscriber* DDS_Manager::InitializeNodeSubscriber(
		SubscriberListener* sub_listener) {
	SubscriberAttributes nodeRparam;
	nodeRparam.topic.topicDataType = nodeType.getName();
	nodeRparam.topic.topicName = "Node";
	node_subscriber = Domain::createSubscriber(mp_participant, nodeRparam,
			sub_listener);
	return node_subscriber;
}

Publisher* DDS_Manager::InitializeCommandPublisher(
		PublisherListener* pub_listener) {
	PublisherAttributes commandWparam;
	commandWparam.topic.topicDataType = tickType.getName();
	commandWparam.topic.topicName = "Command";
	command_publisher = Domain::createPublisher(mp_participant, commandWparam,
			pub_listener);
	return command_publisher;
}

Subscriber* DDS_Manager::InitializeCommandSubscriber(
		SubscriberListener* sub_listener) {
	SubscriberAttributes commandRparam;
	commandRparam.topic.topicDataType = commandType.getName();
	commandRparam.topic.topicName = "Command";
	tick_subscriber = Domain::createSubscriber(mp_participant, commandRparam,
			sub_listener);
	return tick_subscriber;
}

