#include "ModuleManagerListener.h"

void ModuleManagerListener::onNewStatusData(AMM::Capability::Status st,
                                            SampleInfo_t *info) {
    ostringstream statusValue;
    statusValue << st.status_value();

    LOG_DEBUG << "[" << st.module_id() << "][" << st.module_name() << "]["
              << st.capability() << "] Status = " << statusValue.str();

    GUID_t changeGuid = info->sample_identity.writer_guid();
    std::ostringstream module_guid;
    module_guid << changeGuid.guidPrefix;
    mapmutex.lock();
    try {
        db << "replace into module_status (module_id, module_guid, module_name, "
              "capability, status) values (?,?,?,?,?);"
           << st.module_id() << module_guid.str() << st.module_name()
           << st.capability() << statusValue.str();
    } catch (exception &e) {
        LOG_ERROR << e.what();
    }
    mapmutex.unlock();
}

void ModuleManagerListener::onNewConfigData(AMM::Capability::Configuration cfg,
                                            SampleInfo_t *info) {
    LOG_DEBUG << "[" << cfg.module_id() << "][" << cfg.module_name() << "] sent capabilities";

    GUID_t changeGuid = info->sample_identity.writer_guid();
    std::ostringstream module_guid;
    module_guid << changeGuid.guidPrefix;
    mapmutex.lock();
    try {
        db << "replace into module_capabilities (module_id, module_guid, "
              "module_name, manufacturer, model, serial_number, version, "
              "capabilities) values (?,?,?,?,?,?,?,?);"
           << cfg.module_id() << module_guid.str() << cfg.module_name()
           << cfg.manufacturer() << cfg.model() << cfg.serial_number()
           << cfg.version() << cfg.capabilities();
    } catch (exception &e) {
        LOG_ERROR << e.what();
    };
    mapmutex.unlock();
}


void ModuleManagerListener::onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) {
    // Listen for reset?
    LOG_INFO << "[COMMAND]" << c.message();
}

void ModuleManagerListener::onNewLogRecordData(AMM::Diagnostics::Log::Record r, SampleInfo_t *info) {
    GUID_t changeGuid = info->sample_identity.writer_guid();
    std::ostringstream module_guid;
    module_guid << changeGuid.guidPrefix;
    mapmutex.lock();
    try {
        db << "insert into logs (module_guid, message, log_level, timestamp) values (?, ?,?,?);"
           << module_guid.str() << r.message() << r.log_level() << r.timestamp();
    } catch (exception &e) {
        LOG_ERROR << e.what();
    };
    mapmutex.unlock();
}
