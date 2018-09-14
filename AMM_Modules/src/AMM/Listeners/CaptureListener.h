
#include "AMM/DDS_Manager.h"

#include "DDS_Listeners.h"

#include "ListenerInterface.h"

#include "AMM/BaseLogger.h"

#include "thirdparty/sqlite_modern_cpp.h"

using namespace std;
using namespace sqlite;

class CaptureListener : public ListenerInterface {

    void onNewStatusData(AMM::Capability::Status st) override;

    void onNewConfigData(AMM::Capability::Configuration cfg) override;

    void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) override;

};

