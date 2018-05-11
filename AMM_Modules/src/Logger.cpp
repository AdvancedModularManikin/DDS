#include "stdafx.h"

#include <fastrtps/Domain.h>
#include <fastrtps/participant/Participant.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/subscriber/SampleInfo.h>

#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/rtps/builtin/data/WriterProxyData.h>

#include <fastrtps/rtps/reader/ReaderListener.h>
#include <fastrtps/rtps/builtin/discovery/endpoint/EDPSimple.h>

#include <fastrtps/utils/eClock.h>

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/participant/ParticipantListener.h>
#include <fastrtps/rtps/builtin/data/ReaderProxyData.h>

#include <fastrtps/rtps/builtin/discovery/participant/PDPSimple.h>
#include <fastrtps/rtps/builtin/discovery/participant/PDPSimpleListener.h>

#include <fastrtps/rtps/builtin/BuiltinProtocols.h>
#include <fastrtps/rtps/builtin/liveliness/WLP.h>

#include <fastrtps/rtps/builtin/discovery/endpoint/EDPStatic.h>

#include <fastrtps/rtps/resources/AsyncWriterThread.h>

#include <fastrtps/rtps/writer/StatelessWriter.h>
#include <fastrtps/rtps/reader/StatelessReader.h>
#include <fastrtps/rtps/reader/WriterProxy.h>

#include <fastrtps/rtps/history/WriterHistory.h>
#include <fastrtps/rtps/history/ReaderHistory.h>

#include <fastrtps/utils/TimeConversion.h>

#include <fastrtps/rtps/participant/RTPSParticipant.h>

#include "AMM/DataTypes.h"
#include "AMM/DDS_Manager.h"


using namespace std;
using namespace eprosima;
using namespace eprosima::fastrtps;

Participant *mp_participant;

class AMMListener : public ListenerInterface, public ParticipantListener {
public:
    std::string m_listenerName;
    std::mutex mapmutex;
    std::map<std::string, std::vector<std::string>> topicNtypes;
    std::map<GUID_t, std::string> discovered_names;

    AMMListener(const std::string listenerName) {
        m_listenerName = listenerName;
    }

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
        return std::map<std::string, std::vector<uint8_t>>();
    }

    std::vector<std::string> get_discovered_names() const {
        std::vector<std::string> names(discovered_names.size());
        size_t i = 0;
        for (auto it : discovered_names) {
            names[i++] = it.second;
        }
        return names;
    }


    void onParticipantDiscovery(Participant *, ParticipantDiscoveryInfo info) override {
        if (
                info.rtps.m_status != DISCOVERED_RTPSPARTICIPANT &&
                info.rtps.m_status != REMOVED_RTPSPARTICIPANT &&
                info.rtps.m_status != DROPPED_RTPSPARTICIPANT) {
            return;
        }

        if (DISCOVERED_RTPSPARTICIPANT == info.rtps.m_status) {
            // ignore already known GUIDs
            if (discovered_names.find(info.rtps.m_guid) == discovered_names.end()) {
                auto map = parse_key_value(info.rtps.m_userData);
                auto found = map.find("name");
                std::string name;
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
                cout << "[" << m_listenerName << "] " << info.rtps.m_guid << " joined with name " << name << endl;
            }
        } else {
            auto it = discovered_names.find(info.rtps.m_guid);
            // only consider known GUIDs
            if (it != discovered_names.end()) {
                discovered_names.erase(it);
            }
            cout << "[" << m_listenerName << "] " << info.rtps.m_guid << " disconnected " << endl;
        }
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c, SampleInfo_t *info) override {
        cout << "[COMMAND]" << c.message() << endl;
    }

    void onReaderMatched(RTPSReader *reader, MatchingInfo &info) {
        cout << "[" << m_listenerName << "] New reader matched: " << info.remoteEndpointGuid;
        cout << " - status " << info.status << endl;
    }

    void onNewCacheChangeAdded(eprosima::fastrtps::rtps::RTPSReader *reader,
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
        // don't use std::accumulate - schlemiel O(n2)
        for (const auto &partition : proxyData.m_qos.m_partition.getNames()) {
            partition_str += partition;
        }
        string fqdn = partition_str + "/" + proxyData.topicName();

        mapmutex.lock();
        if (change->kind == ALIVE) {
            topicNtypes[fqdn].push_back(proxyData.typeName());

            cout << "[" << m_listenerName << "][" << changeGuid << "] Topic " << fqdn << " with type "
                 << proxyData.typeName() << endl;
        } else {
            auto it = topicNtypes.find(fqdn);
            if (it != topicNtypes.end()) {
                const auto &loc =
                        std::find(std::begin(it->second), std::end(it->second), proxyData.typeName());
                if (loc != std::end(it->second)) {
                    topicNtypes[fqdn].erase(loc, loc + 1);
                    cout << "[" << m_listenerName << "][" << changeGuid << "] Topic removed " << fqdn << " with type "
                         << proxyData.typeName() << endl;
                } else {
                    cout << "[" << m_listenerName << "][" << changeGuid << "] Unexpected removal on topic " << fqdn
                         << " with type "
                         << proxyData.typeName() << endl;
                }
            }
        }
        mapmutex.unlock();
    }
};


int main(int argc, char *argv[]) {
    cout << "=== [AMM - Logger] ===" << endl;

    string action;
    bool closed = false;

    AMMListener ammL("PARTICIPANT");
    AMMListener commandL("COMMAND");
    AMMListener slave_listener_pub("PUB");
    AMMListener slave_listener_sub("SUB");


    const char *nodeName = "AMM_Logger";
    auto mgr = new DDS_Manager(nodeName, &ammL);
    mp_participant = mgr->GetParticipant();

    std::pair<StatefulReader *, StatefulReader *> EDP_Readers = mp_participant->getEDPReaders();
    auto result = EDP_Readers.first->setListener(&slave_listener_pub);
    result &= EDP_Readers.second->setListener(&slave_listener_sub);

    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();
    command_sub_listener->SetUpstream(&commandL);
    mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), command_sub_listener);

    // Publish module configuration once we've set all our publishers and listeners
    // This announces that we're available for configuration
    mgr->PublishModuleConfiguration(
            "Vcom3D",
            "Logger",
            "00001",
            "0.0.1",
            mgr->GetCapabilitiesAsString("mule1/logger_capabilities.xml")
    );

    // Normally this would be set AFTER configuration is received
    mgr->SetStatus(OPERATIONAL);

    while (!closed) {
        getline(cin, action);
        transform(action.begin(), action.end(), action.begin(), ::toupper);
        if (action == "EXIT") {
            closed = true;
            cout << "=== [Logger] Shutting down." << endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        cout.flush();
    }


    cout << "=== [Logger] Exiting." << endl;

    return 0;

}