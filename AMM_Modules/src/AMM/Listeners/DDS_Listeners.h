#pragma once

#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/participant/Participant.h>

#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/Publisher.h>

#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/Subscriber.h>

#include <fastrtps/Domain.h>

#include <fastrtps/utils/eClock.h>

#include <fastrtps/Domain.h>
#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastrtps/subscriber/SubscriberListener.h>

#include "AMM/BaseLogger.h"
#include "AMM/DDS/AMMPubSubTypes.h"
#include "ListenerInterface.h"

using namespace eprosima;
using namespace eprosima::fastrtps;

class DDS_Listeners {

public:
    class PubListener : public PublisherListener {
    public:
        PubListener() : n_matched(0) {};

        ~PubListener() override = default;

        void onPublicationMatched(Publisher *pub, MatchingInfo &info) override;

        int n_matched;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class DefaultSubListener : public SubscriberListener {
    public:
        DefaultSubListener() : n_matched(0), n_msg(0) {};

        ~DefaultSubListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class NodeSubListener : public SubscriberListener {
    public:
        NodeSubListener() : n_matched(0), n_msg(0) {};

        ~NodeSubListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class HighFrequencyNodeSubListener : public SubscriberListener {
    public:
        HighFrequencyNodeSubListener() : n_matched(0), n_msg(0) {};

        ~HighFrequencyNodeSubListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class PhysiologyCommandSubListener : public SubscriberListener {
    public:
        PhysiologyCommandSubListener() : n_matched(0), n_msg(0) {};

        ~PhysiologyCommandSubListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class CommandSubListener : public SubscriberListener {
    public:
        CommandSubListener() : n_matched(0), n_msg(0) {};

        ~CommandSubListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class TickSubListener : public SubscriberListener {
    public:
        TickSubListener() : n_matched(0), n_msg(0) {};

        ~TickSubListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class StatusSubListener : public SubscriberListener {
    public:
        StatusSubListener() : n_matched(0), n_msg(0) {};

        ~StatusSubListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class ConfigSubListener : public SubscriberListener {
    public:
        ConfigSubListener() : n_matched(0), n_msg(0) {};

        ~ConfigSubListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class ScenarioSubListener : public SubscriberListener {
    public:
        ScenarioSubListener() : n_matched(0), n_msg(0) {};

        ~ScenarioSubListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class PhysiologyModificationListener : public SubscriberListener {
    public:
        PhysiologyModificationListener() : n_matched(0), n_msg(0) {};

        ~PhysiologyModificationListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class RenderModificationListener : public SubscriberListener {
    public:
        RenderModificationListener() : n_matched(0), n_msg(0) {};

        ~RenderModificationListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class EquipmentSubListener : public SubscriberListener {
    public:
        EquipmentSubListener() : n_matched(0), n_msg(0) {};

        ~EquipmentSubListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class LogRecordSubListener : public SubscriberListener {
    public:
        LogRecordSubListener() : n_matched(0), n_msg(0) {};

        ~LogRecordSubListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };

    class PerformanceAssessmentSubListener : public SubscriberListener {
    public:
        PerformanceAssessmentSubListener() : n_matched(0), n_msg(0) {};

        ~PerformanceAssessmentSubListener() override = default;

        void onSubscriptionMatched(Subscriber *sub, MatchingInfo &info) override;

        void onNewDataMessage(Subscriber *sub) override;

        SampleInfo_t m_info;
        int n_matched;
        int n_msg;

        void SetUpstream(ListenerInterface *l) { upstream = l; };
        ListenerInterface *upstream{};
    };
};
