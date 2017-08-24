#pragma once


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
#include "ListenerInterface.h"

#include <fastrtps/Domain.h>

using namespace eprosima;
using namespace eprosima::fastrtps;


class DDS_Listeners {

public:

    class PubListener : public PublisherListener {
    public:
        PubListener() : n_matched(0) {};

        virtual ~PubListener() {};

        void onPublicationMatched(Publisher *pub, MatchingInfo &info);

        int n_matched;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream;

    };

    class DefaultSubListener : public SubscriberListener {
    public:
        DefaultSubListener() : n_matched(0), n_msg(0) {};

        ~DefaultSubListener() {};

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info);

        void onNewDataMessage(Subscriber *sub);

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream;
    };

    class NodeSubListener : public SubscriberListener {
    public:
        NodeSubListener() : n_matched(0), n_msg(0) {};

        ~NodeSubListener() {};

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info);

        void onNewDataMessage(Subscriber *sub);

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream;
    };

    class CommandSubListener : public SubscriberListener {
    public:

        CommandSubListener() : n_matched(0), n_msg(0) {};

        ~CommandSubListener() {};

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info);

        void onNewDataMessage(Subscriber *sub);

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream;
    };

    class TickSubListener : public SubscriberListener {
    public:
        TickSubListener() : n_matched(0), n_msg(0) {};

        ~TickSubListener() {};

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info);

        void onNewDataMessage(Subscriber *sub);

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream;
    };

};

