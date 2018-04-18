#pragma once

#include <fastrtps/rtps/reader/ReaderListener.h>

#include "AMMPubSubTypes.h"



class ListenerInterface : public ReaderListener {
public:
    ListenerInterface() = default;;

    virtual ~ListenerInterface() {};

    virtual void onNewNodeData(AMM::Physiology::Node n) {};

    virtual void onNewTickData(AMM::Simulation::Tick t) {};

    virtual void onNewCommandData(AMM::PatientAction::BioGears::Command c) {};

    std::string sysPrefix = "[SYS]";
    std::string actPrefix = "[ACT]";
    std::string loadPrefix = "LOAD_STATE:";
};