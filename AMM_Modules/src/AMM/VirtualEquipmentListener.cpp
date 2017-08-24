#include "VirtualEquipmentListener.h"


VirtualEquipmentListener::VirtualEquipmentListener() {};

void VirtualEquipmentListener::onNewNodeData(AMM::Physiology::Node n) {
    cout << "New node data came in" << endl;
    cout << "\tFrame\t\t" << n.frame() << endl;
    cout << "\tNodePath\t\t" << n.nodepath() << endl;
    cout << "\tValue\t\t" << n.dbl() << endl;
}

void VirtualEquipmentListener::onNewCommandData(AMM::PatientAction::BioGears::Command c) {
    cout << "New command data came in" << endl;
}

void VirtualEquipmentListener::onNewTickData(AMM::Simulation::Tick t) {
    cout << "New tick data came in" << endl;
}
