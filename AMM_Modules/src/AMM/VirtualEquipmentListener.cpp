#include "VirtualEquipmentListener.h"


void VirtualEquipmentListener::onNewNodeData(AMM::Physiology::Node n) {

    if ( std::find(node_paths->begin(), node_paths->end(), n.nodepath()) != node_paths->end() ) {
        cout << "\t(" << n.frame() << ")\t\t" << n.nodepath() << "\t\t" << n.dbl() << endl;
    }

}

void VirtualEquipmentListener::onNewCommandData(AMM::PatientAction::BioGears::Command c) {
    cout << "[COMMAND]" << c.message() << endl;
}

void VirtualEquipmentListener::onNewTickData(AMM::Simulation::Tick t) {
    // cout << "New tick data came in" << endl;
}

void VirtualEquipmentListener::SetFilter(std::vector<std::string> * np) {
    node_paths = np;
}