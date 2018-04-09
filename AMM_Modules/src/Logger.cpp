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


    class AMMListener : public ListenerInterface {
    public:
        void onNewCommandData(AMM::PatientAction::BioGears::Command c) override {

            cout << c.message();

        }

    };

    class gettopicnamesandtypesReaderListener : public ReaderListener {
    public:
        std::mutex mapmutex;
        std::map<std::string, std::set<std::string>> topicNtypes;

        void onNewCacheChangeAdded(RTPSReader *reader, const CacheChange_t *const change_in) {
            CacheChange_t *change = (CacheChange_t *) change_in;
            if (change->kind == ALIVE) {
                WriterProxyData proxyData;
                CDRMessage_t tempMsg;
                tempMsg.msg_endian = change->serializedPayload.encapsulation == PL_CDR_BE ? BIGEND : LITTLEEND;
                tempMsg.length = change->serializedPayload.length;
                memcpy(tempMsg.buffer, change->serializedPayload.data, tempMsg.length);
                if (proxyData.readFromCDRMessage(&tempMsg)) {
                    mapmutex.lock();
                    topicNtypes[proxyData.topicName()].insert(proxyData.typeName());
                    mapmutex.unlock();
                }
            }
        }
    };


int main(int argc, char *argv[]) {
    cout << "=== [AMM - Logger] ===" << endl;
    bool closed = false;

    const char *nodeName = "AMM_Logger";
    auto *mgr = new DDS_Manager(nodeName);
    Participant *mp_participant = mgr->GetParticipant();

    if (mp_participant == nullptr) {
        std::cout << " Something went wrong while creating the Subscriber Participant..." << std::endl;
        return -1;
    }

 /*   gettopicnamesandtypesReaderListener slave_listener_pub;
    gettopicnamesandtypesReaderListener slave_listener_sub;

    std::pair<StatefulReader *, StatefulReader *> EDP_Readers = mp_participant->getEDPReaders();
    auto result = EDP_Readers.first->setListener(&slave_listener_pub);
    result &= EDP_Readers.second->setListener(&slave_listener_sub);
    if (!result) {
        printf("Something went wrong\n");
    }*/
    auto *command_sub_listener = new DDS_Listeners::CommandSubListener();


    AMMListener ammL;
    command_sub_listener->SetUpstream(&ammL);
    mgr->InitializeSubscriber(AMM::DataTypes::commandTopic, AMM::DataTypes::getCommandType(), command_sub_listener);

    while (!closed) {
        /*printf("FastRTPS node list\n");
        printf("-------------------\n");
        auto participant_names = mp_participant->getParticipantNames();
        for (auto name : participant_names) {
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
        for (auto &sub_entry: subscriber_map) {
            printf("Topic:\t%s\n", sub_entry.first.c_str());
            for (auto &type : sub_entry.second) {
                printf("\tType:\t%s\n", type.c_str());
            }
        }
        printf("-------------------\n");
        printf("Existing publisher %lu\n", publisher_map.size());
        for (auto &pub_entry: publisher_map) {
            printf("Topic:\t%s\n", pub_entry.first.c_str());
            for (auto &type : pub_entry.second) {
                printf("\tType:\t%s\n", type.c_str());
            }
        }*/
        std::cin.ignore();
    }


    cout << "=== [Logger] Exiting." << endl;


    return 0;

}