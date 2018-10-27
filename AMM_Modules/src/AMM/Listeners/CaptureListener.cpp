#include "CaptureListener.h"

using namespace sqlite;

void CaptureListener::onNewCommandData(AMM::PatientAction::BioGears::Command c,
                                       SampleInfo_t *info) {
    LOG_TRACE << "[CAPTURE][COMMAND]" << c.message();
}

void CaptureListener::onNewStatusData(AMM::Capability::Status st,
                                      SampleInfo_t *info) {
    database db("amm.db");
    LOG_TRACE << "[CAPTURE][STATUS] Received a status message";
    ostringstream statusValue;
    statusValue << st.status_value();
    try {
        db << "replace into module_status (module_name, capability, status) values "
              "(?,?,?);"
           << st.module_name() << st.capability() << statusValue.str();
    } catch (exception &e) {
        LOG_TRACE << "[CAPTURE][STATUS]" << e.what();
    }
};

void CaptureListener::onNewConfigData(AMM::Capability::Configuration cfg,
                                      SampleInfo_t *info) {
    database db("amm.db");
    LOG_TRACE << "[CAPTURE][CONFIG] Received a config message ";
    try {
        db << "replace into module_capabilities (module_name, manufacturer, model, "
              "serial_number, version, capabilities) values (?,?,?,?,?,?);"
           << cfg.module_name() << cfg.manufacturer() << cfg.model()
           << cfg.serial_number() << cfg.version() << cfg.capabilities();
    } catch (exception &e) {
        LOG_ERROR << "[CAPTURE][CONFIG]" << e.what();
    };
};