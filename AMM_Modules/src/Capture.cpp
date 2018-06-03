#include "stdafx.h"

#include "AMM/DDS_Manager.h"

#include "src/AMM/Listeners/CaptureListener.h"

#include <sqlite_modern_cpp.h>

#include <fastrtps/participant/ParticipantListener.h>
#include <fastrtps/rtps/builtin/data/ReaderProxyData.h>

using namespace std;
using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace sqlite;


static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n" << endl;
}

int main(int argc, char *argv[]) {
    bool setup = false;
    int autostart = 0;
    bool wipe = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }

        if (arg == "-s") {
            setup = true;
        }

        if (arg == "-w") {
            wipe = true;
        }
    }


    if (wipe) {
        cout << "[CAPTURE] Wiping tables on startup" << endl;
        try {
            sqlite_config config;
            database db("amm.db", config);
            db << "delete from modules;";
            db << "delete from module_capabilities;";
            db << "delete from module_status;";
        } catch (exception &e) {
            cout << e.what() << endl;
        }
    }

    if (setup) {
        try {
            sqlite_config config;
            database db("amm.db", config);

            cout << "Creating modules table..." << endl;
            db << "create table if not exists modules("
                    "module_id text,"
                    "module_name text,"
                    "timestamp text"
                    ");";
            db << "delete from modules;";

            cout << "Creating module capabilities table..." << endl;
            db << "create table if not exists module_capabilities ("
                    "model text,"
                    "module_id text,"
                    "module_name text,"
                    "manufacturer text,"
                    "serial_number text,"
                    "version text,"
                    "capabilities text,"
                    "timestamp text,"
                    "encounter_id text"
                    ");";

            db << "CREATE UNIQUE INDEX idx_mc_model ON module_capabilities (module_name);";

            db << "delete from module_capabilities;";

            cout << "Creating module status table..." << endl;
            db << "create table if not exists module_status ("
                    "module_id text,"
                    "module_name text,"
                    "capability text,"
                    "status text,"
                    "timestamp text,"
                    "encounter_id text"
                    ");";
            db << "CREATE UNIQUE INDEX idx_ms_model ON module_status (module_name);";
            db << "delete from module_status;";

            cout << "\tCreated AMM database schema." << endl;

        } catch (exception &e) {
            cout << e.what() << endl;
        }
        return 0;
    }


    const char *nodeName = "AMM_Capture";
    std::string nodeString(nodeName);
    auto *mgr = new DDS_Manager(nodeName);

    CaptureListener cL;

    auto *status_sub_listener = new DDS_Listeners::StatusSubListener();
    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();

    status_sub_listener->SetUpstream(&cL);
    config_sub_listener->SetUpstream(&cL);

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
