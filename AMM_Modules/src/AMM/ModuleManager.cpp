#include "ModuleManager.h"

using namespace std;
using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace sqlite;

database db("amm.db");

class AMMListener : public ParticipantListener {
public:
    std::mutex mapmutex;
    std::map <std::string, std::vector<std::string>> topicNtypes;
    std::map <GUID_t, std::string> discovered_names;

    static std::map <std::string, std::vector<uint8_t>> parse_key_value(std::vector <uint8_t> kv) {
        std::map <std::string, std::vector<uint8_t>> m;

        bool keyfound = false;

        std::string key;
        std::vector <uint8_t> value;
        uint8_t prev = '\0';

        if (kv.size() == 0) {
            goto not_valid;
        }

        for (uint8_t u8 : kv) {
            if (keyfound) {
                if ((u8 == ';') && (prev != ';')) {
                    prev = u8;
                    continue;
                } else if ((u8 != ';') && (prev == ';')) {
                    if (value.size() == 0) {
                        goto not_valid;
                    }
                    m[key] = value;

                    key.clear();
                    value.clear();
                    keyfound = false;
                } else {
                    value.push_back(u8);
                }
            }
            if (!keyfound) {
                if (u8 == '=') {
                    if (key.size() == 0) {
                        goto not_valid;
                    }
                    keyfound = true;
                } else if (isalnum(u8)) {
                    key.push_back(u8);
                } else if ((u8 == '\0') && (key.size() == 0) && (m.size() > 0)) {
                    break;  // accept trailing '\0' characters
                } else if ((prev != ';') || (key.size() > 0)) {
                    goto not_valid;
                }
            }
            prev = u8;
        }
        if (keyfound) {
            if (value.size() == 0) {
                goto not_valid;
            }
            m[key] = value;
        } else if (key.size() > 0) {
            goto not_valid;
        }
        return m;
        not_valid:
        // This is not a failure this is something that can happen because the participant_qos userData
        // is used. Other participants in the system not created by rmw could use userData for something
        // else.
        return std::map < std::string, std::vector < uint8_t >> ();
    }

    void onParticipantDiscovery(Participant *, ParticipantDiscoveryInfo info) override {
        if (
                info.rtps.m_status != DISCOVERED_RTPSPARTICIPANT &&
                info.rtps.m_status != REMOVED_RTPSPARTICIPANT &&
                info.rtps.m_status != DROPPED_RTPSPARTICIPANT) {
            return;
        }

        std::string name;
        ostringstream node_id;
        if (DISCOVERED_RTPSPARTICIPANT == info.rtps.m_status) {
            if (discovered_names.find(info.rtps.m_guid) == discovered_names.end()) {
                auto map = parse_key_value(info.rtps.m_userData);
                auto found = map.find("name");

                if (found != map.end()) {
                    name = std::string(found->second.begin(), found->second.end());
                }
                if (name.empty()) {
                    // use participant name if no name was found in the user data
                    name = info.rtps.m_RTPSParticipantName;
                }
                // ignore discovered participants without a name
                if (!name.empty()) {
                    discovered_names[info.rtps.m_guid] = name;
                }

                cout << "[MM] " << info.rtps.m_guid << " joined with name " << name << endl;

                node_id << info.rtps.m_guid;
                try {
                    db << "insert into nodes (node_id, node_name) values (?,?);"
                       << node_id.str()
                       << name;
                } catch (exception &e) {
                    cout << e.what() << endl;
                }
            }
        } else {
            auto it = discovered_names.find(info.rtps.m_guid);
            // only consider known GUIDs
            if (it != discovered_names.end()) {
                discovered_names.erase(it);
            }
            cout << "[MM] " << info.rtps.m_guid << " disconnected " << endl;
            node_id << info.rtps.m_guid;
            try {
                db << "delete from nodes where node_id=?;"
                   << node_id.str();
            } catch (exception &e) {
                cout << e.what() << endl;
            }
        }
    }


};


ModuleManager::ModuleManager() {
    AMMListener ammL;
    mgr = new DDS_Manager(nodeName, &ammL);
    auto mp_participant = mgr->GetParticipant();
    m_runThread = false;

    auto *status_sub_listener = new DDS_Listeners::StatusSubListener();
    status_sub_listener->SetUpstream(this);

    auto *config_sub_listener = new DDS_Listeners::ConfigSubListener();
    config_sub_listener->SetUpstream(this);

    status_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::statusTopic,
                                                  AMM::DataTypes::getStatusType(),
                                                  status_sub_listener);
    config_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::configurationTopic,
                                                  AMM::DataTypes::getConfigurationType(),
                                                  config_sub_listener);

    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            "Vcom3D",
            "Module_Manager",
            "00001",
            "0.0.1",
            "capabilityString"
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(OPERATIONAL);

}

bool ModuleManager::isRunning() {
    return m_runThread;
}

void ModuleManager::Start() {
    if (!m_runThread) {
        m_runThread = true;
        m_thread = std::thread(&ModuleManager::RunLoop, this);
    }
}

void ModuleManager::RunLoop() {
    while (m_runThread) {
        m_mutex.lock();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        // do work
        m_mutex.unlock();
    }
}


void ModuleManager::Cleanup() {

}

void ModuleManager::Shutdown() {
    if (m_runThread) {
        m_runThread = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        m_thread.join();
    }

    Cleanup();

}

void ModuleManager::onNewStatusData(AMM::Capability::Status s, SampleInfo_t *info) {
    cout << "[MM] Received a status message " << endl;
    cout << "[MM] From " << info->sample_identity.writer_guid() << endl;
    cout << "[MM]\tValue: " << s.status_value() << endl;
    cout << "[MM]\tCapabilities: " << s.capability() << endl;
    // Iterate the vector || cout << "[MM]\tMessage: " << s.message() << endl;
    cout << "[MM]\t---" << endl;

    ostringstream node_id;
    node_id << info->sample_identity.writer_guid();
    auto timestamp = std::to_string(time(nullptr));

    try {
        db << "insert into node_status (node_id, capability, status, timestamp) values (?,?,?,?);"
           << node_id.str()
           << s.capability()
           << "OPERATIONAL"
           << timestamp;
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

void ModuleManager::onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t *info) {
    cout << "[MM] Received a capability config message " << endl;
    cout << "[MM] From " << info->sample_identity.writer_guid() << endl;
    cout << "[MM]\tMfg: " << cfg.manufacturer() << endl;
    cout << "[MM]\tModel: " << cfg.model() << endl;
    cout << "[MM]\tSerial Number: " << cfg.serial_number() << endl;
    cout << "[MM]\tVersion: " << cfg.version() << endl;
    cout << "[MM]\tCapabilities: " << cfg.capabilities() << endl;
    cout << "[MM]\t---" << endl;

    ostringstream node_id;
    node_id << info->sample_identity.writer_guid();
    auto timestamp = std::to_string(time(nullptr));
    try {
        db
                << "insert into node_capabilities (node_id, manufacturer, model, serial_number, version, capabilities, timestamp) values (?,?,?,?,?,?,?);"
                << node_id.str()
                << cfg.manufacturer()
                << cfg.model()
                << cfg.serial_number()
                << cfg.version()
                << cfg.capabilities()
                << timestamp;
    } catch (exception &e) {
        cout << e.what() << endl;
    }

}