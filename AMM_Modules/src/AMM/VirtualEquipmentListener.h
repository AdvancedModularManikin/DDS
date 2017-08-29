#include "AMMPubSubTypes.h"

#include "AMM/DDS_Manager.h"

#include "AMM/DDS_Listeners.h"

#include "AMM/ListenerInterface.h"

using namespace std;

class VirtualEquipmentListener : public ListenerInterface {

public:

    VirtualEquipmentListener();

    virtual ~VirtualEquipmentListener() {};

    void SetFilter(std::vector<std::string> * node_paths);

    void onNewNodeData(AMM::Physiology::Node n);

    void onNewCommandData(AMM::PatientAction::BioGears::Command c);

    void onNewTickData(AMM::Simulation::Tick t);

private:
    std::vector<std::string> * node_paths;
};

