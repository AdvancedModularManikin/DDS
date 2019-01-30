
#include <fastrtps/Domain.h>
#include <fastrtps/participant/Participant.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/subscriber/SubscriberListener.h>

#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/rtps/builtin/data/WriterProxyData.h>

#include <fastrtps/rtps/builtin/discovery/endpoint/EDPSimple.h>
#include <fastrtps/rtps/reader/ReaderListener.h>

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

#include <fastrtps/rtps/reader/StatelessReader.h>
#include <fastrtps/rtps/reader/WriterProxy.h>
#include <fastrtps/rtps/writer/StatelessWriter.h>

#include <fastrtps/rtps/history/ReaderHistory.h>
#include <fastrtps/rtps/history/WriterHistory.h>

#include <fastrtps/utils/TimeConversion.h>

#include <fastrtps/rtps/participant/RTPSParticipant.h>

#include "AMM/Utility.hpp"

#include "AMM/DDS/AMMPubSubTypes.h"

#include "AMM/DDS_Manager.h"

#include "DDS_Listeners.h"

#include "ListenerInterface.h"

#include "thirdparty/sqlite_modern_cpp.h"

using namespace std;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;
using namespace sqlite;

class LoggingListener : public ListenerInterface, public ParticipantListener {
public:
    LoggingListener(const std::string &listenerName) { m_listenerName = listenerName; }

    std::vector<std::string> get_discovered_names() const {
        std::vector<std::string> names(discovered_names.size());
        size_t i = 0;
        for (auto it : discovered_names) {
            names[i++] = it.second;
        }
        return names;
    }

    void onParticipantDiscovery(Participant *,
                                ParticipantDiscoveryInfo info) override {
        if (info.rtps.m_status != DISCOVERED_RTPSPARTICIPANT &&
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
                LOG_INFO << "[" << m_listenerName << "] " << info.rtps.m_guid
                         << " joined with name " << name;
            }
        } else {
            auto it = discovered_names.find(info.rtps.m_guid);
            // only consider known GUIDs
            if (it != discovered_names.end()) {
                discovered_names.erase(it);
            }
            LOG_INFO << "[" << m_listenerName << "] " << info.rtps.m_guid
                     << " disconnected ";
        }
    }

    void onNewCommandData(AMM::PatientAction::BioGears::Command c,
                          SampleInfo_t *info) override {
        LOG_INFO << "[COMMAND]" << c.message();
    }

    void onNewLogRecordData(AMM::Diagnostics::Log::Record r,
                            SampleInfo_t *info) override {
        LOG_INFO << "[" << r.log_level() << "]" << r.message();
    }


    void onReaderMatched(RTPSReader *reader, MatchingInfo &info) {
        LOG_INFO << "[" << m_listenerName
                 << "] New reader matched: " << info.remoteEndpointGuid
                 << " - status " << info.status;
    }

    void onNewCacheChangeAdded(RTPSReader *reader,
                               const CacheChange_t *const change) {

        GUID_t changeGuid;
        iHandle2GUID(changeGuid, change->instanceHandle);

        WriterProxyData proxyData;
        if (change->kind == ALIVE) {
            CDRMessage_t tempMsg(0);
            tempMsg.wraps = true;
            tempMsg.msg_endian = change->serializedPayload.encapsulation == PL_CDR_BE
                                 ? BIGEND
                                 : LITTLEEND;
            tempMsg.length = change->serializedPayload.length;
            tempMsg.max_size = change->serializedPayload.max_size;
            tempMsg.buffer = change->serializedPayload.data;
            if (!proxyData.readFromCDRMessage(&tempMsg)) {
                return;
            }
        } else {

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

            LOG_INFO << "[" << m_listenerName << "][" << changeGuid << "] Topic "
                     << fqdn << " with type " << proxyData.typeName();
        } else {
            auto it = topicNtypes.find(fqdn);
            if (it != topicNtypes.end()) {
                const auto &loc = std::find(std::begin(it->second),
                                            std::end(it->second), proxyData.typeName());
                if (loc != std::end(it->second)) {
                    topicNtypes[fqdn].erase(loc, loc + 1);
                    LOG_INFO << "[" << m_listenerName << "][" << changeGuid
                             << "] Topic removed " << fqdn << " with type "
                             << proxyData.typeName();
                } else {
                    LOG_INFO << "[" << m_listenerName << "][" << changeGuid
                             << "] Unexpected removal on topic " << fqdn << " with type "
                             << proxyData.typeName();
                }
            }
        }
        mapmutex.unlock();
    }

protected:
    std::string m_listenerName;
    std::mutex mapmutex;
    std::map<std::string, std::vector<std::string>> topicNtypes;
    std::map<GUID_t, std::string> discovered_names;
};
