#include "stdafx.h"

#include "AMM/DDS_Manager.h"

#include <sqlite_modern_cpp.h>

#include <fastrtps/participant/ParticipantListener.h>
#include <fastrtps/rtps/builtin/data/ReaderProxyData.h>

using namespace std;
using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace sqlite;


class AMMListener : public ListenerInterface {
public:
    AMMListener() {
       database db("amm.db");
    }

    std::mutex m_mutex;
    database db;

    void onNewStatusData(AMM::Capability::Status st) override {

        cout << "[MM] Received a status message " << endl;

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

    void onNewConfigData(AMM::Capability::Configuration cfg) override {
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

};

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n" << endl;
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }
    }

    ModuleListener mL;
    const char *nodeName = "AMM_Capture";
    std::string nodeString(nodeName);
    auto *mgr = new DDS_Manager(nodeName, &mL);

    AMMListener ammL;
    auto *status_sub_listener = new DDS_Listeners::StatusSubListener();
    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();

    status_sub_listener->SetUpstream(&ammL);
    config_sub_listener->SetUpstream(&ammL);

    mgr->InitializeSubscriber(AMM::DataTypes::statusTopic, AMM::DataTypes::getStatusType(), status_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::configurationTopic, AMM::DataTypes::getConfigurationType(),
                              config_sub_listener);

    std::string action;
    bool closed = false;
    do {
        cout << " >>> ";
        getline(cin, action);
        transform(action.begin(), action.end(), action.begin(), ::toupper);
        if (action == "EXIT") {
            closed = true;
        } else {
            if (action.empty()) {
                continue;
            }
        }
    } while (!closed);

    return 0;

}
