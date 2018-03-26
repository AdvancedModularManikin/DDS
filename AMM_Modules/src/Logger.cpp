#include "stdafx.h"

#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>

#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/rtps/reader/ReaderListener.h>

#include <fastrtps/subscriber/Subscriber.h>

#include <fastrtps/Domain.h>
#include <fastrtps/subscriber/SampleInfo.h>

#include <fastrtps/utils/eClock.h>

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/participant/ParticipantListener.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/rtps/builtin/data/ReaderProxyData.h>
#include <fastrtps/rtps/builtin/data/WriterProxyData.h>

#include <fastrtps/rtps/builtin/discovery/participant/PDPSimple.h>
#include <fastrtps/rtps/builtin/discovery/participant/PDPSimpleListener.h>

#include <fastrtps/rtps/builtin/BuiltinProtocols.h>
#include <fastrtps/rtps/builtin/liveliness/WLP.h>

#include <fastrtps/rtps/builtin/discovery/endpoint/EDPSimple.h>
#include <fastrtps/rtps/builtin/discovery/endpoint/EDPStatic.h>

#include <fastrtps/rtps/resources/AsyncWriterThread.h>

#include "fastrtps/subscriber/SubscriberListener.h"

#include "fastrtps/rtps/RTPSDomain.h"

#include <fastrtps/rtps/writer/StatelessWriter.h>
#include <fastrtps/rtps/reader/StatelessReader.h>
#include <fastrtps/rtps/reader/WriterProxy.h>

#include <fastrtps/rtps/history/WriterHistory.h>
#include <fastrtps/rtps/history/ReaderHistory.h>

#include <fastrtps/utils/TimeConversion.h>

#include <fastrtps/rtps/participant/RTPSParticipant.h>

using namespace std;
using namespace eprosima;
using namespace eprosima::fastrtps;

class CustomReaderListener : public ReaderListener {
    void onNewCacheChangeAdded(RTPSReader *reader, const CacheChange_t *const change) {
        (void) reader;
        cout << "CACHE CHANGE!  ";
//        if (change->kind == ALIVE) {
            WriterProxyData proxyData;

            CDRMessage_t tempMsg(0);
            tempMsg.wraps = true;
            tempMsg.msg_endian = change->serializedPayload.encapsulation == PL_CDR_BE ? BIGEND : LITTLEEND;
            tempMsg.length = change->serializedPayload.length;
            tempMsg.max_size = change->serializedPayload.max_size;
            tempMsg.buffer = change->serializedPayload.data;

            if (proxyData.readFromCDRMessage(&tempMsg)) {
                cout << " - ";
                cout << "[GUID " << change->writerGUID << "] ";
                cout << proxyData.m_remoteAtt.endpoint.getUserDefinedID();
                cout << " (KIND: " << change->kind << ") ";
                cout << proxyData.topicName() << " => " << proxyData.typeName();
                cout << endl;
            }
//        }
    }
};


int main(int argc, char *argv[]) {
    cout << "=== [AMM - Logger] ===" << endl;


    ParticipantAttributes Pparam;
    Pparam.rtps.builtin.domainId = 15;
    Pparam.rtps.builtin.leaseDuration = c_TimeInfinite;
    Pparam.rtps.setName("AMM");

    Participant *mp_participant = Domain::createParticipant(Pparam);
    if (mp_participant == nullptr) {
        std::cout << " Something went wrong while creating the Subscriber Participant..." << std::endl;
        return -1;
    }
    CustomReaderListener *my_readerListenerSub = new(CustomReaderListener);
    CustomReaderListener *my_readerListenerPub = new(CustomReaderListener);

    std::pair<StatefulReader *, StatefulReader *> EDPReaders = mp_participant->getEDPReaders();
    EDPReaders.first->setListener(my_readerListenerSub);
    EDPReaders.second->setListener(my_readerListenerPub);

    std::cout << "Logger running. Please press enter to stop the logger" << std::endl;
    std::cin.ignore();

    return 0;

}