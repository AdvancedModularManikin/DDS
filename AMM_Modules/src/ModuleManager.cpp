#include "stdafx.h"

#include "AMM/ModuleManager.h"


using namespace std;
using namespace sqlite;

bool closed = false;

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
        cout << "=== [ModManager] Shutting down Module Manager." << endl;
        closed = true;
        modManager->Shutdown();
    } else {
        // unknown menu action
    }
}

int main(int argc, char *argv[]) {
    int daemonize = 0;
    int autostart = 0;

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

    }

    ModuleManager modManager;
    modManager.Start();


    while (!closed) {
        if (autostart != 1) {
            show_menu(&modManager);
        }
    }

    cout << "=== [ModuleManager] Exiting." <<
         endl;

    return 0;
}
