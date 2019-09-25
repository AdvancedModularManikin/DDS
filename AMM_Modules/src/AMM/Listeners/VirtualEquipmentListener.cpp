#include "VirtualEquipmentListener.h"

using namespace std;

void VirtualEquipmentListener::onNewNodeData(AMM::Physiology::Node n,
                                             SampleInfo_t *info) {

    if (std::find(node_paths->begin(), node_paths->end(), n.nodepath()) !=
        node_paths->end()) {
        cout << "\t(" << n.frame() << ")\t\t" << n.nodepath() << "\t\t" << n.dbl()
             << endl;
    }
}

void VirtualEquipmentListener::onNewCommandData(
        AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) {
    cout << "[COMMAND]" << c.message() << endl;
}

void VirtualEquipmentListener::onNewTickData(AMM::Simulation::Tick t,
                                             SampleInfo_t *info) {
    // cout << "New tick data came in" << endl;
}

void VirtualEquipmentListener::SetFilter(std::vector<std::string> *np) {
    node_paths = np;
}