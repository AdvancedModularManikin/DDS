#include "AMM/DDS/AMMPubSubTypes.h"

#include "AMM/DDS_Manager.h"

#include "AMM/Utility.h"

#include "DDS_Listeners.h"

#include "ListenerInterface.h"

#include "thirdparty/sqlite_modern_cpp.h"

using namespace std;
using namespace sqlite;
using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace std::chrono;

struct logEntry {
    std::string source;
    std::string topic;
    int64_t tick = 0;
    int64_t timestamp;
    std::string data = "";
};

class ModuleManagerListener : public ListenerInterface {
public:
    ModuleManagerListener() : db("amm.db") {};

    void onNewStatusData(AMM::Capability::Status st, SampleInfo_t *info) override;

    void onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t *info) override;

    void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) override;

    void onNewLogRecordData(AMM::Diagnostics::Log::Record r, SampleInfo_t *info) override;

    void onNewPhysiologyModificationData(AMM::Physiology::Modification pm, SampleInfo_t *info) override;

    void onNewPerformanceAssessmentData(AMM::Performance::Assessment a, SampleInfo_t *info) override;

    void onNewRenderModificationData(AMM::Render::Modification rm, SampleInfo_t *info) override;

    void onNewTickData(AMM::Simulation::Tick t, SampleInfo_t *info);

    void WriteLogEntry(logEntry newLogEntry);

    void ClearEventLog();

    void ClearDiagnosticLog();

protected:
    std::mutex mapmutex;
    database db;
    int64_t lastTick = 0;


};
