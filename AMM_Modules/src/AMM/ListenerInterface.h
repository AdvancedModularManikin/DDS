#pragma once

#include "AMMPubSubTypes.h"


class ListenerInterface
{
public:
    ListenerInterface(){};
    virtual ~ListenerInterface(){};

    virtual void onNewNodeData(AMM::Physiology::Node n) {};
    virtual void onNewTickData(AMM::Simulation::Tick t) {};
    virtual void onNewCommandData(AMM::PatientAction::BioGears::Command c) {};
};