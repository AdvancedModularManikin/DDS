

#include "AMM/DDS_Manager.h"

#include "REST/headers/WebServer.hxx"
#include "REST/headers/Routes.hxx"

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

DDS_Manager* AMMData::mgr;

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
    AMMData::mgr = new DDS_Manager(nodeName);

    // Publish module configuration once we've set all our publishers and
    // listeners
    // This announces that we're available for configuration
    AMMData::mgr->PublishModuleConfiguration(
            AMMData::mgr->module_id, nodeString, "Vcom3D", "REST_Service", "00001", "0.0.1",
            AMMData::mgr->GetCapabilitiesAsString(
                    "static/module_capabilities/command_executor_capabilities.xml"));

    // Normally this would be set AFTER configuration is received
    AMMData::mgr->SetStatus(AMMData::mgr->module_id, nodeString, OPERATIONAL);

	WebServer webServer("0.0.0.0", 9080);
    if (webServer.Init() != 0)
    {
        return 0;
    }

    webServer.Run();



    return 0;
}
