#pragma once


#include <fastrtps/Domain.h>
#include <fastrtps/participant/Participant.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/subscriber/SubscriberListener.h>

#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/rtps/builtin/data/WriterProxyData.h>

#include <fastrtps/rtps/reader/ReaderListener.h>

#include <fastrtps/utils/eClock.h>

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/participant/ParticipantListener.h>
#include <fastrtps/rtps/builtin/data/ReaderProxyData.h>

#include <fastrtps/rtps/builtin/BuiltinProtocols.h>
#include <fastrtps/rtps/builtin/liveliness/WLP.h>

#include <fastrtps/rtps/resources/AsyncWriterThread.h>

#include <fastrtps/rtps/reader/StatelessReader.h>
#include <fastrtps/rtps/writer/StatelessWriter.h>

#include <fastrtps/rtps/history/ReaderHistory.h>
#include <fastrtps/rtps/history/WriterHistory.h>

#include <fastrtps/utils/TimeConversion.h>

#include <fastrtps/rtps/participant/RTPSParticipant.h>

#include "AMM/DDS/AMMPubSubTypes.h"

using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

/** Common listeners that can be implemented **/
class ListenerInterface : public ReaderListener {
public:
    ListenerInterface() = default;

    ~ListenerInterface() override {};

    virtual void onNewNodeData(AMM::Physiology::Node n, SampleInfo_t *info) {};

    virtual void onNewHighFrequencyNodeData(AMM::Physiology::HighFrequencyNode n, SampleInfo_t *info) {};

    virtual void onNewTickData(AMM::Simulation::Tick ti, SampleInfo_t *info) {};

    virtual void onNewCommandData(AMM::PatientAction::BioGears::Command cm, SampleInfo_t *info) {};

    virtual void onNewCommandData(AMM::Physiology::Command cm, SampleInfo_t *info) {};

    virtual void onNewStatusData(AMM::Capability::Status st, SampleInfo_t *info) {};

    virtual void onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t *info) {};

    virtual void onNewScenarioData(AMM::Capability::Scenario sc, SampleInfo_t *info) {};

    virtual void onNewInstrumentData(AMM::InstrumentData i, SampleInfo_t *info) {};

    virtual void onNewRenderModificationData(AMM::Render::Modification rm, SampleInfo_t *info) {};

    virtual void onNewPhysiologyModificationData(AMM::Physiology::Modification pm, SampleInfo_t *info) {};

    virtual void onNewLogRecordData(AMM::Diagnostics::Log::Record r, SampleInfo_t *info) {};

    virtual void onNewPerformanceAssessmentData(AMM::Performance::Assessment a, SampleInfo_t *info) {};

    std::string sysPrefix = "[SYS]";
    std::string actPrefix = "[ACT]";
    std::string loadPrefix = "LOAD_STATE:";
    std::string loadScenarioPrefix = "LOAD_SCENARIO:";
};
