
#pragma once

#include "AMM/Listeners/ListenerInterface.h"
#include "AMM/Listeners/DDS_Listeners.h"
#include "AMM/DDS_Manager.h"
#include "AMM/DDS/AMMPubSubTypes.h"

using namespace AMM;
using namespace AMM::Physiology;

class AMMListener : public ListenerInterface {

   void onNewHighFrequencyNodeData (HighFrequencyNode hfnode, SampleInfo_t* info) override;
   void onNewNodeData (Node node, SampleInfo_t* info) override;

};
