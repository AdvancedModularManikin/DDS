
#include "REST/headers/AMM_Rest.hpp"

#include "AMM/BaseLogger.h"
#include "AMM/DDS_Log_Appender.h"
#include "AMM/Utility.h"

using namespace AMM;
using namespace AMM::Capability;

namespace AMM_Rest {

void SendPhysMod (AmmMod& am, DDS_Manager* mgr) {

   LOG_DEBUG << "Publishing a phys mod: " << am.type;
   AMM::Physiology::Modification modInstance;
   modInstance.type(am.type);
   //  modInstance.location.description(am.location);
   modInstance.practitioner(am.practitioner);
   modInstance.payload(am.payload);
   mgr->PublishPhysiologyModification(modInstance);
}

void SendRenderMod (AmmMod& am, DDS_Manager* mgr) {

   LOG_DEBUG << "Publishing a render mod: " << am.type;
   AMM::Render::Modification modInstance;
   modInstance.type(am.type);
   FMA_Location fma_location;
   fma_location.description(am.location);
   modInstance.location(fma_location);
   modInstance.practitioner(am.practitioner);
   modInstance.payload(am.payload);
   mgr->PublishRenderModification(modInstance);
}


} /// namespace AMM_Rest
