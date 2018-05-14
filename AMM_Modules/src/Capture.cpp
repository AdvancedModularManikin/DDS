#include "stdafx.h"

#include "AMM/DDS_Manager.h"

#include <sqlite_modern_cpp.h>

#include <fastrtps/participant/ParticipantListener.h>
#include <fastrtps/rtps/builtin/data/ReaderProxyData.h>

using namespace std;
using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace sqlite;

database db("amm.db");

class ModuleListener : public ParticipantListener {
public:
    void onParticipantDiscovery(Participant *, ParticipantDiscoveryInfo info) override {

        if (
                info.rtps.m_status != DISCOVERED_RTPSPARTICIPANT &&
                info.rtps.m_status != REMOVED_RTPSPARTICIPANT &&
                info.rtps.m_status != DROPPED_RTPSPARTICIPANT) {
            cout << "[MM] Participant discovered with an unknown status: " << info.rtps.m_status << endl;
            return;
        }

        std::string name;
        ostringstream module_id;

        if (DISCOVERED_RTPSPARTICIPANT == info.rtps.m_status) {
            module_id << info.rtps.m_guid;
            std::size_t pos = module_id.str().find("|");
            std::string truncated_module_id = module_id.str().substr(0, pos);

            cout << "[MM] Participant " << info.rtps.m_guid << " joined with name " << name << endl;
            cout << "[MM] Stored with truncated ID: " << truncated_module_id << endl;

            try {
                db << "insert into modules (module_id, module_name) values (?,?);"
                   << truncated_module_id
                   << name;
            } catch (exception &e) {
                cout << e.what() << endl;
            }
        }
    else {
        cout << "[MM] Participant " << info.rtps.m_guid << " disconnected " << endl;
        module_id << info.rtps.m_guid;
        std::size_t pos = module_id.str().find("|");
        std::string truncated_module_id = module_id.str().substr(0, pos);
        try {
            db << "delete from modules where module_id=?;"
               << truncated_module_id;
        } catch (exception &e) {
            cout << e.what() << endl;
        }
    }
}

};

class AMMListener : public ListenerInterface {
public:
    std::map<std::string, std::vector<std::string>> topicNtypes;
    std::mutex m_mutex;

    void onNewStatusData(AMM::Capability::Status st, SampleInfo_t *info) override {
        ostringstream module_id;
        module_id << info->sample_identity.writer_guid();
        std::size_t pos = module_id.str().find("|");
        std::string truncated_module_id = module_id.str().substr(0, pos);

        cout << "[MM] Received a status message " << endl;
        cout << "[MM] Writer ID\t" << info->sample_identity.writer_guid() << endl;
        cout << "[MM] Truncated ID\t" << truncated_module_id << endl;
        cout << "[MM]\tValue: " << st.status_value() << endl;
        // Iterate the vector || cout << "[MM]\tMessage: " << s.message() << endl;
        cout << "[MM]\t---" << endl;

	ostringstream statusValue;
	statusValue << st.status_value();
        db << "replace into module_status (module_id, capability, status) values (?,?,?);"
           << truncated_module_id
           << st.capability()
           << statusValue.str();
    };

    void onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t *info) override {
        ostringstream module_id;
        module_id << info->sample_identity.writer_guid();
        std::size_t pos = module_id.str().find("|");
        std::string truncated_module_id = module_id.str().substr(0, pos);
        //    auto timestamp = std::to_string(time(nullptr));

        cout << "[MM] Received a capability config message " << endl;
        cout << "[MM] Writer ID\t" << info->sample_identity.writer_guid() << endl;
        cout << "[MM] Truncated ID\t" << truncated_module_id << endl;
        cout << "[MM]\t---" << endl;


        db
                << "replace into module_capabilities (module_id, manufacturer, model, serial_number, version, capabilities) values (?,?,?,?,?,?);"
                << truncated_module_id
                << cfg.manufacturer()
                << cfg.model()
                << cfg.serial_number()
                << cfg.version()
                << cfg.capabilities();

    };

    void onNewCacheChangeAdded(eprosima::fastrtps::rtps::RTPSReader *reader,
                               const eprosima::fastrtps::CacheChange_t *const change) override {

        /*eprosima::fastrtps::rtps::GUID_t changeGuid;
        iHandle2GUID(changeGuid, change->instanceHandle);
        eprosima::fastrtps::rtps::WriterProxyData proxyData;
        if (change->kind == ALIVE) {
            eprosima::fastrtps::CDRMessage_t tempMsg(0);
            tempMsg.wraps = true;
            tempMsg.msg_endian = change->serializedPayload.encapsulation ==
                                 PL_CDR_BE ? BIGEND : LITTLEEND;
            tempMsg.length = change->serializedPayload.length;
            tempMsg.max_size = change->serializedPayload.max_size;
            tempMsg.buffer = change->serializedPayload.data;
            if (!proxyData.readFromCDRMessage(&tempMsg)) {
                return;
            }
        } else {
            if (!mgr->GetParticipant()->get_remote_writer_info(changeGuid, proxyData)) {
                return;
            }
        }

        std::string partition_str = std::string("AMM");
        for (const auto &partition : proxyData.m_qos.m_partition.getNames()) {
            partition_str += partition;
        }
        string fqdn = partition_str + "/" + proxyData.topicName();

        m_mutex.lock();
        if (change->kind == ALIVE) {
            topicNtypes[fqdn].push_back(proxyData.typeName());

            //cout << "[MM][" << changeGuid << "] Topic " << fqdn << " with type "
            //     << proxyData.typeName() << endl;
        } else {
            auto it = topicNtypes.find(fqdn);
            if (it != topicNtypes.end()) {
                const auto &loc =
                        std::find(std::begin(it->second), std::end(it->second), proxyData.typeName());
                if (loc != std::end(it->second)) {
                    topicNtypes[fqdn].erase(loc, loc + 1);
                    cout << "[MM][" << changeGuid << "] Topic removed " << fqdn << " with type "
                         << proxyData.typeName() << endl;
                } else {
                    cout << "[MM][" << changeGuid << "] Unexpected removal on topic " << fqdn
                         << " with type "
                         << proxyData.typeName() << endl;
                }
            }
        }
        m_mutex.unlock();*/
    }
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
    auto *mgr = new DDS_Manager(nodeName, &mL);

    AMMListener ammL;
    auto *status_sub_listener = new DDS_Listeners::StatusSubListener();
    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();

    status_sub_listener->SetUpstream(&ammL);
    config_sub_listener->SetUpstream(&ammL);

    mgr->InitializeSubscriber(AMM::DataTypes::statusTopic, AMM::DataTypes::getStatusType(), status_sub_listener);
    mgr->InitializeSubscriber(AMM::DataTypes::configurationTopic, AMM::DataTypes::getConfigurationType(),
                              config_sub_listener);


    // Track subscriptions and publishers
    std::pair<StatefulReader *, StatefulReader *> EDP_Readers = mgr->GetParticipant()->getEDPReaders();
    auto result = EDP_Readers.first->setListener(&ammL);
    result &= EDP_Readers.second->setListener(&ammL);

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
