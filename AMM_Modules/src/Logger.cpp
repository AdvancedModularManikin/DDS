#include "AMM/DDS_Manager.h"
#include "AMM/BaseLogger.h"
#include "AMM/Listeners/LoggingListener.h"

using namespace std;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;
using namespace AMM;

int main(int argc, char *argv[]) {
    plog::InitializeLogger();

    using namespace AMM::Capability;
    cout << "=== [AMM - Logger] ===" << endl;

    string action;
    bool closed = false;

    LoggingListener logL("LOG");
    LoggingListener slave_listener_pub("PUB");
    LoggingListener slave_listener_sub("SUB");

    const char *nodeName = "AMM_Logger";
    std::string nodeString(nodeName);
    auto mgr = new DDS_Manager(nodeName);
    Participant *mp_participant = mgr->GetParticipant();

    std::pair<StatefulReader *, StatefulReader *> EDP_Readers =
            mp_participant->getEDPReaders();
    auto result = EDP_Readers.first->setListener(&slave_listener_pub);
    result &= EDP_Readers.second->setListener(&slave_listener_sub);

    auto *log_sub_listener = new DDS_Listeners::LogRecordSubListener();
    log_sub_listener->SetUpstream(&logL);
    mgr->InitializeReliableSubscriber(AMM::DataTypes::logRecordTopic,
                                      AMM::DataTypes::getLogRecordType(),
                                      log_sub_listener);

    // Publish module configuration once we've set all our publishers and
    // listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            mgr->module_id, nodeString, "Vcom3D", "Logger", "00001", "0.0.1",
            mgr->GetCapabilitiesAsString(
                    "mule1/module_capabilities/logger_capabilities.xml"));

    // Normally this would be set AFTER configuration is received, but the logger
    // is always operational
    mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);

    while (!closed) {
        getline(cin, action);
        transform(action.begin(), action.end(), action.begin(), ::toupper);
        if (action == "EXIT") {
            closed = true;
            cout << "=== [Logger] Shutting down." << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        cout.flush();
    }

    cout << "=== [Logger] Exiting." << endl;

    return 0;
}