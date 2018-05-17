#pragma once

#include <fastrtps/rtps/reader/ReaderListener.h>

#include "AMMPubSubTypes.h"


/** Common listeners that can be implemented **/
class ListenerInterface : public ReaderListener {
public:
    ListenerInterface() = default;;

    virtual ~ListenerInterface() {};

    virtual void onNewNodeData(AMM::Physiology::Node n, SampleInfo_t *info) {};

    virtual void onNewTickData(AMM::Simulation::Tick ti, SampleInfo_t *info) {};

    virtual void onNewCommandData(AMM::PatientAction::BioGears::Command cm, SampleInfo_t *info) {};

    virtual void onNewStatusData(AMM::Capability::Status st) {};

    virtual void onNewConfigData(AMM::Capability::Configuration cfg) {};

    virtual void onNewScenarioData(AMM::Capability::Scenario sc) {};

    std::string sysPrefix = "[SYS]";
    std::string actPrefix = "[ACT]";
    std::string loadPrefix = "LOAD_STATE:";
    std::string loadScenarioPrefix = "LOAD_SCENARIO:";
};