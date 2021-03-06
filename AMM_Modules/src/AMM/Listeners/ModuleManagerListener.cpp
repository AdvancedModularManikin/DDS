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

    if (cfg.module_name() == "disconnect") {
        try {
            db << "delete from module_capabilities where module_id = ? ;" << cfg.module_id();
        } catch (exception &e) {
            LOG_ERROR << e.what();
        };
    } else {
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
    }
    mapmutex.unlock();
}


void ModuleManagerListener::onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) {
    // Listen for reset?
    int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    GUID_t changeGuid = info->sample_identity.writer_guid();
    std::ostringstream module_guid;
    module_guid << changeGuid;

    if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
        std::string value = c.message().substr(sysPrefix.size());
        if (value.compare("START_SIM") == 0) {

        } else if (value.compare("STOP_SIM") == 0) {

        } else if (value.compare("PAUSE_SIM") == 0) {

        } else if (value.compare("RESET_SIM") == 0) {

        }
    } else if (c.message().compare("CLEAR_LOGS") == 0) {
        ClearEventLog();
        ClearDiagnosticLog();
    } else if (c.message().compare("CLEAR_EVENT_LOG") == 0) {
        ClearEventLog();
    } else if (c.message().compare("CLEAR_DIAGNOSTIC_LOG") == 0) {
        ClearDiagnosticLog();
    }
    logEntry newLogEntry{module_guid.str(), "AMM::Command", lastTick, timestamp, c.message()};
    WriteLogEntry(newLogEntry);

}

void ModuleManagerListener::onNewTickData(AMM::Simulation::Tick t, SampleInfo_t *info) {
    lastTick = t.frame();
}


void ModuleManagerListener::onNewLogRecordData(AMM::Diagnostics::Log::Record r, SampleInfo_t *info) {
    int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    GUID_t changeGuid = info->sample_identity.writer_guid();
    std::ostringstream module_guid;
    module_guid << changeGuid.guidPrefix;
    mapmutex.lock();

    try {
        db
                << "insert into logs (module_guid, module_id, module_name, message, log_level, timestamp) values (?,?,?,?,?,?);"
                << module_guid.str() << r.module_id() << r.module_name() << r.message() << r.log_level() << timestamp;
    } catch (exception &e) {
        LOG_ERROR << e.what();
    };
    mapmutex.unlock();
}

void ModuleManagerListener::onNewRenderModificationData(AMM::Render::Modification rm, SampleInfo_t *info) {
    int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    GUID_t changeGuid = info->sample_identity.writer_guid();
    std::ostringstream module_guid;
    module_guid << changeGuid;

    std::ostringstream logmessage;
    logmessage << "[" << rm.type() << "]" << rm.payload();

    logEntry newLogEntry{module_guid.str(), "AMM::Render::Modification", lastTick, timestamp, logmessage.str()};
    WriteLogEntry(newLogEntry);
};

void ModuleManagerListener::onNewPhysiologyModificationData(AMM::Physiology::Modification pm,
                                                            SampleInfo_t *info) {
    int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    GUID_t changeGuid = info->sample_identity.writer_guid();
    std::ostringstream module_guid;
    module_guid << changeGuid;
    std::string physModName = pm.type();
    std::ostringstream logmessage;
    logmessage << physModName;
    logEntry newLogEntry{module_guid.str(), "AMM::Physiology::Modification",
                         lastTick, timestamp, logmessage.str()};
    WriteLogEntry(newLogEntry);
};



void ModuleManagerListener::onNewPerformanceAssessmentData(AMM::Performance::Assessment a, SampleInfo_t *info) {
    int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    GUID_t changeGuid = info->sample_identity.writer_guid();
    std::ostringstream module_guid;
    module_guid << changeGuid;
    std::string aType = a.assessment_type();
    std::string aInfo = a.assessment_info();
    std::string aLoc = a.location().description();
    std::string aLearnerID = a.learner_id();
    std::ostringstream logmessage;
    logmessage << "Assessment Type: " << aType << "  -  Info: " << aInfo << " - Location: " << aLoc << " - Learner: " << aLearnerID;
    logEntry newLogEntry{module_guid.str(), "AMM::Performance::Assessment",
                         lastTick, timestamp, logmessage.str()};
    WriteLogEntry(newLogEntry);
}

void ModuleManagerListener::ClearEventLog() {
    mapmutex.lock();
    try {
        db << "delete from events;";
    } catch (exception &e) {
        LOG_ERROR << e.what();
    }
    mapmutex.unlock();
}

void ModuleManagerListener::ClearDiagnosticLog() {
    mapmutex.lock();
    try {
        db << "delete from logs;";
    } catch (exception &e) {
        LOG_ERROR << e.what();
    }
    mapmutex.unlock();
}


void ModuleManagerListener::WriteLogEntry(logEntry newLogEntry) {
    mapmutex.lock();
    try {
        db << "insert into events (source, topic, tick, timestamp, data) values "
              "(?,?,?,?,?);"
           << newLogEntry.source << newLogEntry.topic << newLogEntry.tick
           << newLogEntry.timestamp << newLogEntry.data;
    } catch (exception &e) {
        LOG_ERROR << e.what();
    }
    mapmutex.unlock();
}