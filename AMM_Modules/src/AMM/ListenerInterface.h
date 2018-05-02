#pragma once

#include <fastrtps/rtps/reader/ReaderListener.h>

#include "AMMPubSubTypes.h"


/** Common listeners that can be implemented **/
class ListenerInterface : public ReaderListener {
public:
    ListenerInterface() = default;;

    virtual ~ListenerInterface() {};

    // Include sample info for point-to-point details
    virtual void onNewNodeData(AMM::Physiology::Node n, SampleInfo_t *info) {};

    virtual void onNewTickData(AMM::Simulation::Tick t, SampleInfo_t *info) {};

    virtual void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) {};

    virtual void onNewStatusData(AMM::Capability::Status s, SampleInfo_t *info) {};

    virtual void onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t *info) {};

    std::string sysPrefix = "[SYS]";
    std::string actPrefix = "[ACT]";
    std::string loadPrefix = "LOAD_STATE:";
};