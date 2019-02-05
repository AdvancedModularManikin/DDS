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

class ModuleManagerListener : public ListenerInterface {
public:
    ModuleManagerListener() : db("amm.db") {};

    void onNewStatusData(AMM::Capability::Status st, SampleInfo_t *info) override;

    void onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t *info) override;

    void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) override;

    void onNewLogRecordData(AMM::Diagnostics::Log::Record r, SampleInfo_t *info) override;

protected:
    std::mutex mapmutex;
    database db;
};
