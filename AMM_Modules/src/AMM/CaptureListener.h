#include "AMMPubSubTypes.h"

#include "AMM/DDS_Manager.h"

#include "AMM/DDS_Listeners.h"

#include "AMM/ListenerInterface.h"

using namespace std;

class CaptureListener : public ListenerInterface {

public:
    void onNewStatusData(AMM::Capability::Status st);

    void onNewConfigData(AMM::Capability::Configuration cfg);

    void onNewCommandData(AMM::PatientAction::BioGears::Command c);

};

