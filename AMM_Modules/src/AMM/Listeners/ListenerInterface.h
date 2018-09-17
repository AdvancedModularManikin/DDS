#pragma once

#include <fastrtps/rtps/reader/ReaderListener.h>

#include "AMMPubSubTypes.h"

using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

/** Common listeners that can be implemented **/
class ListenerInterface : public ReaderListener {
public:
    ListenerInterface() = default;

    ~ListenerInterface() override {};

    virtual void onNewNodeData(AMM::Physiology::Node n, SampleInfo_t* info) {};

    virtual void onNewTickData(AMM::Simulation::Tick ti, SampleInfo_t* info) {};

    virtual void onNewCommandData(AMM::PatientAction::BioGears::Command cm, SampleInfo_t* info) {};

    virtual void onNewStatusData(AMM::Capability::Status st, SampleInfo_t* info) {};

    virtual void onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t* info) {};

    virtual void onNewScenarioData(AMM::Capability::Scenario sc, SampleInfo_t* info) {};

    virtual void onNewInstrumentData(AMM::InstrumentData i, SampleInfo_t* info) {};

    virtual void onNewRenderModificationData(AMM::Render::Modification rm, SampleInfo_t* info) {};

    virtual void onNewPhysiologyModification(AMM::Physiology::Modification pm, SampleInfo_t* info) {};

    std::string sysPrefix = "[SYS]";
    std::string actPrefix = "[ACT]";
    std::string loadPrefix = "LOAD_STATE:";
    std::string loadScenarioPrefix = "LOAD_SCENARIO:";
};