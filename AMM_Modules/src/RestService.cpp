

#include "AMM/DDS_Manager.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

using namespace std;
using namespace AMM;
using namespace rapidjson;

static void show_usage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>"
         << "\nOptions:\n"
         << "\t-h,--help\t\tShow this help message\n"
         << endl;
}

int main(int argc, char *argv[]) {
    plog::InitializeLogger();
    using namespace AMM::Capability;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }
    }
    const char *nodeName = "AMM_RESTService";
    std::string nodeString(nodeName);
    auto *mgr = new DDS_Manager(nodeName);

    // Publish module configuration once we've set all our publishers and
    // listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            mgr->module_id, nodeString, "Vcom3D", "REST_Service", "00001", "0.0.1",
            mgr->GetCapabilitiesAsString(
                    "static/module_capabilities/command_executor_capabilities.xml"));

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(mgr->module_id, nodeString, OPERATIONAL);

    // Do something here


    return 0;
}
