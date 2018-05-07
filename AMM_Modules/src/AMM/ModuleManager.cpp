#include "ModuleManager.h"

using namespace std;
using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace sqlite;

database db("amm.db");

static std::map<std::string, std::vector<uint8_t>> parse_key_value(std::vector<uint8_t> kv) {
    std::map<std::string, std::vector<uint8_t>> m;

    bool keyfound = false;

    std::string key;
    std::vector<uint8_t> value;
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
    return std::map<std::string, std::vector<uint8_t >>();
}

ModuleManager::ModuleManager() {
    mgr = new DDS_Manager(nodeName, this);
    mp_participant = mgr->GetParticipant();


    std::pair<StatefulReader *, StatefulReader *> EDP_Readers = mp_participant->getEDPReaders();
    // pub
    auto result = EDP_Readers.first->setListener(this);
    // sub
    result &= EDP_Readers.second->setListener(this);


    DDS_Listeners::StatusSubListener *status_sub_listener = new DDS_Listeners::StatusSubListener();
    DDS_Listeners::ConfigSubListener *config_sub_listener = new DDS_Listeners::ConfigSubListener();

    status_sub_listener->SetUpstream(this);
    config_sub_listener->SetUpstream(this);

    status_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::statusTopic,
                                                  AMM::DataTypes::getStatusType(),
						  status_sub_listener);

    config_subscriber = mgr->InitializeSubscriber(AMM::DataTypes::configurationTopic,
                                                  AMM::DataTypes::getConfigurationType(),
                                                  config_sub_listener);
    m_runThread = false;
}

bool ModuleManager::isRunning() {
    return m_runThread;
}

void ModuleManager::Start() {
    if (!m_runThread) {
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

        m_runThread = true;
        m_thread = std::thread(&ModuleManager::RunLoop, this);
    }
}

void ModuleManager::RunLoop() {
    while (m_runThread) {
      //  m_mutex.lock();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        // do work
//        m_mutex.unlock();
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

void ModuleManager::onNewStatusData(AMM::Capability::Status st, SampleInfo_t *m_info) {
    ostringstream node_id;
    node_id << m_info->sample_identity.writer_guid();
    std::size_t pos = node_id.str().find("|");
    std::string truncated_node_id = node_id.str().substr(0, pos);
    auto timestamp = std::to_string(time(nullptr));

    cout << "[MM] Received a status message " << endl;
    cout << "[MM] Writer ID\t" << m_info->sample_identity.writer_guid() << endl;
    cout << "[MM] Truncated ID\t" << truncated_node_id << endl;
    cout << "[MM]\tValue: " << st.status_value() << endl;
    cout << "[MM]\tCapabilities: " << st.capability() << endl;
    // Iterate the vector || cout << "[MM]\tMessage: " << s.message() << endl;
    cout << "[MM]\t---" << endl;

    try {

        db << "insert into node_status (node_id, capability, status, timestamp) values (?,?,?,?);"
           << truncated_node_id
           << st.capability()
           << "OPERATIONAL"
           << timestamp;
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

void ModuleManager::onNewConfigData(AMM::Capability::Configuration cfg, SampleInfo_t *m_info) {
    ostringstream node_id;
    node_id << m_info->sample_identity.writer_guid();
    std::size_t pos = node_id.str().find("|");
    std::string truncated_node_id = node_id.str().substr(0, pos);
    auto timestamp = std::to_string(time(nullptr));

    cout << "[MM] Received a capability config message " << endl;
    cout << "[MM] Writer ID\t" << m_info->sample_identity.writer_guid() << endl;
    cout << "[MM] Truncated ID\t" << truncated_node_id << endl;
    cout << "[MM]\tMfg: " << cfg.manufacturer() << endl;
    cout << "[MM]\tModel: " << cfg.model() << endl;
    cout << "[MM]\tSerial Number: " << cfg.serial_number() << endl;
    cout << "[MM]\tVersion: " << cfg.version() << endl;
    cout << "[MM]\tCapabilities: " << cfg.capabilities() << endl;
    cout << "[MM]\t---" << endl;

    try {
        db
                << "insert into node_capabilities (node_id, manufacturer, model, serial_number, version, capabilities, timestamp) values (?,?,?,?,?,?,?);"
                << truncated_node_id
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

void ModuleManager::onReaderMatched(RTPSReader *reader, MatchingInfo &info) {
    cout << "[MM] New reader matched: " << info.remoteEndpointGuid;
    cout << " - status " << info.status << endl;
}

void ModuleManager::onNewCacheChangeAdded(eprosima::fastrtps::rtps::RTPSReader *reader,
                           const eprosima::fastrtps::CacheChange_t *const change) {

    eprosima::fastrtps::rtps::GUID_t changeGuid;
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
        if (!mp_participant->get_remote_writer_info(changeGuid, proxyData)) {
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

        cout << "[MM][" << changeGuid << "] Topic " << fqdn << " with type "
             << proxyData.typeName() << endl;
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
    m_mutex.unlock();
}

void ModuleManager::onParticipantDiscovery(Participant *, ParticipantDiscoveryInfo info)  {
    if (
            info.rtps.m_status != DISCOVERED_RTPSPARTICIPANT &&
            info.rtps.m_status != REMOVED_RTPSPARTICIPANT &&
            info.rtps.m_status != DROPPED_RTPSPARTICIPANT) {
        cout << "[MM] Participant discovered with an unknown status: " << info.rtps.m_status << endl;
        return;
    }

    std::string name;
    ostringstream node_id;

    if (DISCOVERED_RTPSPARTICIPANT == info.rtps.m_status) {
        cout << "[MM] Participant discovered!" << endl;

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

            node_id << info.rtps.m_guid;
            std::size_t pos = node_id.str().find("|");
            std::string truncated_node_id = node_id.str().substr(0, pos);

            cout << "[MM] Participant " << info.rtps.m_guid << " joined with name " << name << endl;
            cout << "[MM] Stored with truncated ID: " << truncated_node_id << endl;

            try {
                db << "insert into nodes (node_id, node_name) values (?,?);"
                   << truncated_node_id
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
        cout << "[MM] Participant " << info.rtps.m_guid << " disconnected " << endl;
        node_id << info.rtps.m_guid;
        std::size_t pos = node_id.str().find("|");
        std::string truncated_node_id = node_id.str().substr(0, pos);
        try {
            db << "delete from nodes where node_id=?;"
               << truncated_node_id;
        } catch (exception &e) {
            cout << e.what() << endl;
        }
    }
}

