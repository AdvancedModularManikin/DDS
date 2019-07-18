

#include "LOGGER/headers/AmmListener.hpp"


void AMMListener::onNewHighFrequencyNodeData (HighFrequencyNode hfnode, SampleInfo_t* info) {

   using namespace std;
   cout << "New High Freq Node Data Event" << endl;

}



void AMMListener::onNewNodeData (Node node, SampleInfo_t* info) {

   using namespace std;
   cout << "New Node Data Event" << endl;

}
