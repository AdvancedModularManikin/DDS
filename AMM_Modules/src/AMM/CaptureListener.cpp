#include "CaptureListener.h"

using namespace sqlite;


void CaptureListener::onNewCommandData(AMM::PatientAction::BioGears::Command c) {
    cout << "[CAPTURE][COMMAND]" << c.message() << endl;
}

void CaptureListener::onNewStatusData(AMM::Capability::Status st) {
    database db("amm.db");
    cout << "[CAPTURE][STATUS] Received a status message " << endl;
    ostringstream statusValue;
    statusValue << st.status_value();
    try {
        db << "replace into module_status (module_name, capability, status) values (?,?,?);"
           << st.module_name()
           << st.capability()
           << statusValue.str();
    } catch (exception &e) {
        cout << e.what() << endl;
    }
};

void CaptureListener::onNewConfigData(AMM::Capability::Configuration cfg) {
    database db("amm.db");
    cout << "[CAPTURE][CONFIG] Received a config message " << endl;
    try {
        db
                << "replace into module_capabilities (module_name, manufacturer, model, serial_number, version, capabilities) values (?,?,?,?,?,?);"
                << cfg.module_name()
                << cfg.manufacturer()
                << cfg.model()
                << cfg.serial_number()
                << cfg.version()
                << cfg.capabilities();
    } catch (exception &e) {
        cout << e.what() << endl;
    };

};