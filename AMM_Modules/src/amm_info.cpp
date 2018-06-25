#include <fastrtps/Domain.h>
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastrtps/attributes/SubscriberAttributes.h>

#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/rtps/builtin/data/WriterProxyData.h>
#include <fastrtps/rtps/common/CDRMessage_t.h>

#include <fastrtps/rtps/reader/RTPSReader.h>
#include <fastrtps/rtps/reader/StatefulReader.h>
#include <fastrtps/rtps/reader/ReaderListener.h>
#include <fastrtps/rtps/builtin/discovery/endpoint/EDPSimple.h>

#include "AMMPubSubTypes.h"


using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;

namespace
{
    class gettopicnamesandtypesReaderListener:public ReaderListener
    {
    public:
        std::mutex mapmutex;
        std::map<std::string,std::set<std::string>> topicNtypes;
        void onNewCacheChangeAdded(RTPSReader* reader, const CacheChange_t* const change_in)
        {
            CacheChange_t* change = (CacheChange_t*) change_in;
            if(change->kind == ALIVE)
            {
                WriterProxyData proxyData;
                CDRMessage_t tempMsg;
                tempMsg.msg_endian = change->serializedPayload.encapsulation == PL_CDR_BE ? BIGEND:LITTLEEND;
                tempMsg.length = change->serializedPayload.length;
                memcpy(tempMsg.buffer,change->serializedPayload.data,tempMsg.length);
                if(proxyData.readFromCDRMessage(&tempMsg))
                {
                    mapmutex.lock();
                    topicNtypes[proxyData.topicName()].insert(proxyData.typeName());
                    mapmutex.unlock();
                }
            }
        }
    };
}


int main(int argc, char** argv)
{
    auto node_name = "AMM_Info";
    auto domain_id = 15;

    ParticipantAttributes PParam; //Configuration structure
    PParam.rtps.setName(node_name);
    PParam.rtps.builtin.domainId = domain_id;
    Participant* mp_participant = Domain::createParticipant(PParam);

    gettopicnamesandtypesReaderListener slave_listener_pub;
    gettopicnamesandtypesReaderListener slave_listener_sub;

    std::pair<StatefulReader*,StatefulReader*> EDP_Readers = mp_participant->getEDPReaders();
    auto result = EDP_Readers.first->setListener(&slave_listener_pub);
    result &= EDP_Readers.second->setListener(&slave_listener_sub);
    if (!result)
    {
        printf("Something went wrong\n");
    }

    std::this_thread::sleep_for( std::chrono::seconds(1) );// sniff for a while

    printf("FastRTPS node list\n");
    printf("-------------------\n");
    auto participant_names = mp_participant->getParticipantNames();
    for (auto name : participant_names)
    {
        printf("Node:\t%s\n", name.c_str());
    }

    slave_listener_pub.mapmutex.lock();
    auto subscriber_map = slave_listener_pub.topicNtypes;
    slave_listener_pub.mapmutex.unlock();

    slave_listener_sub.mapmutex.lock();
    auto publisher_map = slave_listener_sub.topicNtypes;
    slave_listener_sub.mapmutex.unlock();

    printf("\n");
    printf("FastRTPS topic list\n");
    printf("-------------------\n");
    printf("Existing subscriber %lu\n", subscriber_map.size());
    for (auto& sub_entry: subscriber_map)
    {
        printf("Topic:\t%s\n", sub_entry.first.c_str());
        for (auto& type : sub_entry.second)
        {
            printf("\tType:\t%s\n", type.c_str());
        }
    }
    printf("-------------------\n");
    printf("Existing publisher %lu\n", publisher_map.size());
    for (auto& pub_entry: publisher_map)
    {
        printf("Topic:\t%s\n", pub_entry.first.c_str());
        for (auto& type : pub_entry.second)
        {
            printf("\tType:\t%s\n", type.c_str());
        }
    }

    return 0;
}