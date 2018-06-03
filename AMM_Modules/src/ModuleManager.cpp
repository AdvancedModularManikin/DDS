#include "stdafx.h"

#include "AMM/ModuleManager.h"

using namespace std;
using namespace sqlite;

bool closed = false;
int daemonize = 0;
bool setup = false;
int autostart = 0;
bool wipe = false;

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" <<
         "\nOptions:\n" <<
         "\t-d\t\t\tDaemonize\n" <<
         "\t-h,--help\t\t\tShow this help message\n" <<
         endl;
}

void show_menu(ModuleManager *modManager) {
    string action;

    cout << endl;
    cout << " [1]Status " << endl;
    cout << " [4]Shutdown" << endl;
    cout << " >> ";
    getline(cin, action);
    transform(action.begin(), action.end(), action.begin(), ::toupper);

    if (action == "1") {
        modManager->ShowStatus();
    } else if (action == "4") {
        cout << "=== [ModuleManager] Shutting down Module Manager." << endl;
        closed = true;
        modManager->Shutdown();
    } else {
        // unknown menu action
    }
}

int main(int argc, char *argv[]) {
    cout << "=== [AMM - Module Manager] ===" << endl;

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

    if (wipe) {
        cout << "[ModuleManager] Wiping tables on startup" << endl;
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

            cout << "[ModuleManager] Creating modules table..." << endl;
            db << "create table if not exists modules("
                  "module_id text,"
                  "module_name text,"
                  "timestamp text"
                  ");";
            db << "delete from modules;";

            cout << "[MM] Creating module capabilities table..." << endl;
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

            cout << "[ModuleManager] Creating module status table..." << endl;
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

            cout << "[ModuleManager]\tCreated AMM database schema." << endl;

        } catch (exception &e) {
            cout << e.what() << endl;
        }
        return 0;
    }

    ModuleManager modManager;
    modManager.Start();


    while (!closed) {
        if (autostart != 1) {
            show_menu(&modManager);
        }
    }

    cout << "=== [ModuleManager] Exiting." << endl;

    return 0;
}
