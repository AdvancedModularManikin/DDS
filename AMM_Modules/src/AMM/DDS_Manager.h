
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

#include <fastrtps/Domain.h>

using namespace eprosima;
using namespace eprosima::fastrtps;


class DDS_Manager
{
public:
    DDS_Manager();
    virtual ~DDS_Manager();
    bool Initialize();
    void RegisterTypes();
    Subscriber* InitializeTickSubscriber(SubscriberListener* sub_listener);
    Publisher* InitializeTickPublisher(PublisherListener* pub_listener);
    Subscriber* InitializeCommandSubscriber(SubscriberListener* sub_listener);
    Publisher* InitializeCommandPublisher(PublisherListener* pub_listener);
    Subscriber* InitializeNodeSubscriber(SubscriberListener* sub_listener);
    Publisher* InitializeNodePublisher(PublisherListener* pub_listener);
    
    void run();
private:

    Participant *mp_participant;

    Publisher *tick_publisher;
    Publisher *command_publisher;
    Publisher *node_publisher;

    Subscriber *tick_subscriber;
    Subscriber *command_subscriber;
    Subscriber *node_subscriber;

    int tickCount = 0;
    int sampleRate = 50;


    AMM::Simulation::TickPubSubType tickType;
    AMM::Physiology::NodePubSubType nodeType;
    AMM::PatientAction::BioGears::CommandPubSubType commandType;
};
