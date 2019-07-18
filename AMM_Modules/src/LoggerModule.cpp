

#include "LOGGER/headers/AmmListener.hpp"
#include "AMM/Listeners/DDS_Listeners.h"
#include "AMM/DDS_Manager.h"
#include "AMM/DataTypes.h"

int main () {

   plog::InitializeLogger();

   using namespace AMM;

   AMMListener ammListener;

   auto* nodeSubListener   = new DDS_Listeners::NodeSubListener();
   auto* hfNodeSubListener = new DDS_Listeners::HighFrequencyNodeSubListener();

   DDS_Manager* mgr = new DDS_Manager("AMM_Logging_Module");

   nodeSubListener->SetUpstream(&ammListener);
   hfNodeSubListener->SetUpstream(&ammListener);

   mgr->InitializeSubscriber(DataTypes::nodeTopic, &mgr->NodeType, nodeSubListener);
   mgr->InitializeSubscriber(DataTypes::highFrequencyNodeTopic, &mgr->HighFrequencyNodeType, hfNodeSubListener);

   using namespace std;
   cin.ignore();

   return 0;
}
