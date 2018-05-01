//
// Created by draum on 4/29/18.
//

#ifndef AMM_MODULES_CORELISTENER_H
#define AMM_MODULES_CORELISTENER_H

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


class CoreListener : public ListenerInterface, public ParticipantListener {
public:
    std::string m_listenerName;
    std::map<GuidPrefix_t, std::string> discovered_prefixes;
    std::mutex mapmutex;
    std::map<std::string, std::vector<std::string>> topicNtypes;
    std::map<GUID_t, std::string> discovered_names;

    CoreListener(const std::string listenerName) {
        m_listenerName = listenerName;
    }

    // static std::map<std::string, std::vector<uint8_t>> parse_key_value(std::vector<uint8_t> kv);

    std::vector<std::string> get_discovered_names() const;

    void onParticipantDiscovery(Participant *, ParticipantDiscoveryInfo info);

    void onReaderMatched(RTPSReader *reader, MatchingInfo &info);

    void onNewCacheChangeAdded(eprosima::fastrtps::rtps::RTPSReader *reader,
                               const eprosima::fastrtps::CacheChange_t *const change);


};


#endif //AMM_MODULES_CORELISTENER_H
