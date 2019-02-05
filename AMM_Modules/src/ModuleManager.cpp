#include "stdafx.h"

#include "AMM/ModuleManager.h"

using namespace std;
using namespace sqlite;
using namespace AMM;

bool closed = false;
int daemonize = 0;
bool setup = false;
int autostart = 0;
bool wipe = false;


void SetupTables() {
    try {
        sqlite_config config;
        database db("amm.db", config);

        LOG_INFO << "Creating event log table...";
        db << "create table if not exists events("
              "source text,"
              "event_guid text,"
              "topic text,"
              "tick bigint,"
              "timestamp bigint,"
              "data text"
              ");";
        db << "delete from events;";

        LOG_INFO << "Creating modules table...";
        db << "create table if not exists modules("
              "module_id text,"
              "module_guid text,"
              "module_name text,"
              "timestamp text"
              ");";
        db << "delete from modules;";

        LOG_INFO << "Creating module capabilities table...";
        db << "create table if not exists module_capabilities ("
              "model text,"
              "module_id text,"
              "module_guid text,"
              "module_name text,"
              "manufacturer text,"
              "serial_number text,"
              "version text,"
              "capabilities text,"
              "timestamp text,"
              "encounter_id text"
              ");";

        db << "CREATE UNIQUE INDEX idx_mc_model ON module_capabilities "
              "(module_name);";

        db << "delete from module_capabilities;";

        LOG_INFO << "[ModuleManager] Creating module status table...";
        db << "create table if not exists module_status ("
              "module_id text,"
              "module_guid text,"
              "module_name text,"
              "capability text,"
              "status text,"
              "timestamp text,"
              "encounter_id text"
              ");";
        db << "CREATE UNIQUE INDEX idx_ms_model ON module_status (module_name);";
        db << "delete from module_status;";

        LOG_INFO << "Creating pubsub table...";
        db << "create table if not exists pubsubs("
              "module_id text,"
              "module_guid text,"
              "topic_type text,"
              "topic text"
              ");";

        LOG_INFO << "Creating log record table...";
        db << "create table if not exists logs("
              "module_id text,"
              "module_guid text,"
              "message text,"
              "log_level text,"
              "timestamp bigint"
              ");";
        db << "delete from logs;";
    } catch (exception &e) {
        LOG_ERROR << e.what();
    }
}

void WipeTables() {
    try {
        sqlite_config config;
        database db("amm.db", config);
        db << "delete from events;";
        db << "delete from modules;";
        db << "delete from module_capabilities;";
        db << "delete from module_status;";
        db << "delete from pubsubs;";
        db << "delete from logs;";
    } catch (exception &e) {
        LOG_ERROR << e.what();
    }
}

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>"
         << "\nOptions:\n"
         << "\t-a\t\t\tAuto start\n"
         << "\t-d\t\t\tDaemonize\n"
         << "\t-s\t\t\tSetup module manager tables\n"
         << "\t-w\t\t\tWipe tables\n"
         << "\t-h,--help\t\t\tShow this help message\n"
         << endl;
}

void show_menu(ModuleManager *modManager) {
    string action;

    cout << endl;
    cout << " [1]Status " << endl;
    cout << " [2]Setup tables" << endl;
    cout << " [3]Wipe tables" << endl;
    cout << " [4]Shutdown" << endl;
    cout << " >> ";
    getline(cin, action);
    transform(action.begin(), action.end(), action.begin(), ::toupper);

    if (action == "1") {
        modManager->ShowStatus();
    } else if (action == "2") {
        SetupTables();
    } else if (action == "3") {
        WipeTables();
    } else if (action == "4") {
        LOG_INFO << "Shutting down Module Manager.";
        closed = true;
        modManager->Shutdown();
    } else {
        // unknown menu action
    }
}


int main(int argc, char *argv[]) {
    plog::InitializeLogger();
    LOG_INFO << "AMM - Module Manager";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }

        if (arg == "-d") {
            daemonize = 1;
        }

        if (arg == "-a") {
            autostart = 1;
        }

        if (arg == "-s") {
            setup = true;
        }

        if (arg == "-w") {
            wipe = true;
        }
    }

    if (setup) {
        LOG_INFO << "Creating AMM database schema.";
        SetupTables();
    }

    if (wipe) {
        LOG_INFO << "Wiping tables on startup";
        WipeTables();
    }

    ModuleManager modManager;
    modManager.Start();

    while (!closed) {
        if (autostart != 1) {
            show_menu(&modManager);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    LOG_INFO << "Exiting.";

    return 0;
}
