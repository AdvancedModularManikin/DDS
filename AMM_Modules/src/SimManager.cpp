#include "stdafx.h"

#include "AMM/BaseLogger.h"

#include "AMM/SimulationManager.h"

using namespace std;
using namespace AMM;

bool closed = false;

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>" <<
         "\nOptions:\n" <<
         "\t-r,--rate <sample_rate>\tSpecify the sample rate to run at (samples per second)\n" <<
         "\t-a\t\t\tAuto-start ticks\n" <<
         "\t-d\t\t\tDaemonize\n" <<
         "\t-h,--help\t\t\tShow this help message\n" <<
         endl;
}

void show_menu(SimulationManager* simManager) {
    using namespace AMM::Physiology;
    string action;

    //std::endl is an automatic flush and should be avoided unless required.
    cout << "\n"
      " [1]Status\n"
      " [2]Run/Resume\n"
      " [3]Pause/Stop\n"
      " [4]Shutdown\n"
      " [5]Command console\n"
      " [6]Sepsis Test\n"
      " [7]Pain Test\n"
      " >> ";
    getline(cin, action);
    transform(action.begin(), action.end(), action.begin(), ::toupper);

    if (action == "1") {
        if (simManager->isRunning()) {
            cout << " == Running!  At tick count: ";
        } else {
            cout << " == Not currently running, paused at tick count: ";
        }
        cout << simManager->GetTickCount() << endl;
        cout << "  = Operating at " << simManager->GetSampleRate() << " frames per second." << endl;
    } else if (action == "2") {
        if (!simManager->isRunning()) {
            cout << " == Starting simulation..." << endl;
            simManager->StartSimulation();
        } else {
            cout << " == Simulation already running" << endl;
        }
    } else if (action == "3") {
        if (!simManager->isRunning()) {
            cout << " == Stopping simulation..." << endl;
            simManager->StopSimulation();
        } else {
            cout << " == Simulation not running" << endl;
        }
    } else if (action == "4") {
        if (!simManager->isRunning()) {
            cout << " == Simulation not running, but shutting down anyway" << endl;
        } else {
            cout << " == Stopping simulation and sending shutdown notice..." << endl;
        }
        simManager->StopSimulation();
        cout << " == Exited after " << simManager->GetTickCount() << " ticks." << endl;
        cout << "=== [SimManager] Shutting down Simulation Manager." << endl;
        closed = true;
        simManager->Shutdown();
    } else if (action == "5") {
        std::string command;
        bool consoleclosed = false;
        do {
            cout << " Enter a command (exit to return to menu) >>> ";
            getline(cin, command);
            transform(command.begin(), command.end(), command.begin(), ::toupper);
            if (command == "EXIT") {
                consoleclosed = true;
            } else {
                if (command.empty()) {
                    continue;
                }
                simManager->SendCommand(command);
            }
        } while (!consoleclosed);
    } else if ( action == "6" ) { // Sepsis Test
        using namespace AMM::Physiology::Sepsis;
        Sepsis::Data sepsis;

        int choice;
        double severity;

        std::cout  <<  "Enter Sepsis paramaters"
                       "\nCompartments:"
                       "\n\t[ 1] BoneTissue"
                       "\n\t[ 2] FatTissue"
                       "\n\t[ 3] GutTissue"
                       "\n\t[ 4] LeftKidneyTissue"
                       "\n\t[ 5] LeftLungTissue"
                       "\n\t[ 6] LiverTissue"
                       "\n\t[ 7] MuscleTissue"
                       "\n\t[ 8] MyocardiumTissue"
                       "\n\t[ 9] RightKidneyTissue"
                       "\n\t[10] RightLungTissue"
                       "\n\t[11] SkinTissue"
                       "\n\t[12] SpleenTissue"
                       "\nEnter a tissue compartment: ";
        std::cin  >> choice; 
        std::cout << "Sepsis Severity: ";
        std::cin  >> severity; sepsis.severity(severity);

        switch (choice) {
            case 1:   sepsis.location(AMM::Physiology::BoneTissue); break;
            case 2:   sepsis.location(AMM::Physiology::FatTissue); break;
            case 3:   sepsis.location(AMM::Physiology::GutTissue); break;
            case 4:   sepsis.location(AMM::Physiology::LeftKidneyTissue); break;
            case 5:   sepsis.location(AMM::Physiology::LeftLungTissue); break;
            case 6:   sepsis.location(AMM::Physiology::LiverTissue); break;
            case 7:   sepsis.location(AMM::Physiology::MuscleTissue); break;
            case 8:   sepsis.location(AMM::Physiology::MyocardiumTissue); break;
            case 9:   sepsis.location(AMM::Physiology::RightKidneyTissue); break;
            case 10:  sepsis.location(AMM::Physiology::RightLungTissue); break;
            case 11:  sepsis.location(AMM::Physiology::SkinTissue); break;
            case 12:  sepsis.location(AMM::Physiology::SpleenTissue); break;
        }

        auto buffer = eprosima::fastcdr::FastBuffer();
        eprosima::fastcdr::Cdr data{buffer};
        data << sepsis;
        std::cout << "Sending Sepsis Message\n";
        simManager->SendCommand(AMM::Physiology::SepsisCommand, data);
    } else if ( action == "7" ) { // Paint Test
        using namespace AMM::Physiology::PainStimulus;
        PainStimulus::Data pain;
        FMA_Location loc;
        int id;
        std::string description;
        double severity;
        
        std::cout << "Enter Pain Stimulous paramaters:\n"
                     "Location ID: "; 
        std::cin  >> id; loc.id(id);
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Location description: ";
        std::getline(std::cin,description); loc.description(description);
        pain.location(loc);
        std::cout << "Pain Severity: ";
        std::cin  >> severity; pain.severity(severity);
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        auto buffer = eprosima::fastcdr::FastBuffer();
        eprosima::fastcdr::Cdr data{buffer};
        data << pain;
        std::cout << "Sending Pain Message\n";
        simManager->SendCommand(AMM::Physiology::PainCommand, data);
    } else {
        // unknown menu action
    }
}

int main(int argc, char *argv[]) {
	int sampleRate = 50;
	int daemonize = 0;
	int autostart = 0;
    LOG_INFO << "Simulation Manager starting";
	
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

        if ((arg == "-r") || (arg == "--rate")) {
            istringstream ss(argv[i + 1]);            
            if (!(ss >> sampleRate)) {
                cerr << "Invalid sample rate: " << argv[i + 1] << '\n';
                return 0;
            }
            
        }
    }

	SimulationManager simManager;
	simManager.SetSampleRate(sampleRate);

    if (autostart == 1) {
        LOG_INFO << "Auto-starting simulation";
        simManager.StartSimulation();
    }

    while (!closed) {
        if (daemonize != 1 && autostart != 1) {
            show_menu(&simManager);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    LOG_INFO << "Exiting.";

    return 0;
}

