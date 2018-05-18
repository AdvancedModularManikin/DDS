#include "CaptureListener.h"

database db("amm.db");

void CaptureListener::onNewCommandData(AMM::PatientAction::BioGears::Command c) {
    cout << "[CAPTURE][COMMAND]" << c.message() << endl;
}

void CaptureListener::onNewStatusData(AMM::Capability::Status st) {
    cout << "[CAPTURE][STATUS] Received a status message " << endl;
};

void CaptureListener::onNewConfigData(AMM::Capability::Configuration cfg) {
    cout << "[CAPTURE][CONFIG] Received a config message " << endl;
};