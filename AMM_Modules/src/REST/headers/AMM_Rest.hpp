#pragma once

#include <string>

#include "AMM/DDS_Manager.h"

using namespace AMM;

namespace AMM_Rest {

struct AmmMod {
   using namespace std;

   string type;
   string location;
   string practitioner;
   string payload;
};

void SendPhysMod   (AmmMod& am, DDS_Manager* mgr);
void SendRenderMod (AmmMod& am, DDS_Manager* mgr);




} /// namespace AMM_Rest
