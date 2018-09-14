#include "AMMPubSubTypes.h"

#include "AMM/DDS_Manager.h"

#include "DDS_Listeners.h"

#include "ListenerInterface.h"



using namespace std;

class VirtualEquipmentListener : public ListenerInterface {

public:

    void SetFilter(std::vector<std::string> * node_paths);

    void onNewNodeData(AMM::Physiology::Node n);

    void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info);

    void onNewTickData(AMM::Simulation::Tick t);

private:
    std::vector<std::string> * node_paths{};
};

