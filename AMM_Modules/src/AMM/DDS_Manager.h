
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
    bool init();
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


    class PubListener : public PublisherListener
    {
    public:
        PubListener() : n_matched(0){};
        ~PubListener(){};
        void onPublicationMatched(Publisher* pub,MatchingInfo& info);
        int n_matched;
    } pub_listener;

    class SubListener : public SubscriberListener
    {
    public:
        SubListener() : n_matched(0),n_msg(0){};
        ~SubListener(){};
        void onSubscriptionMatched(Subscriber* sub,MatchingInfo& info);
        void onNewDataMessage(Subscriber* sub);
        SampleInfo_t m_info;
        int n_matched;
        int n_msg;
    } sub_listener;

    AMM::Simulation::TickPubSubType tickType;
    AMM::Physiology::NodePubSubType nodeType;
    AMM::PatientAction::BioGears::CommandPubSubType commandType;
};
