#pragma once

#include <mutex>
#include <thread>

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
#include "DataTypes.h"

#include "DDS_Manager.h"

#include "DDS_Listeners.h"

#include "ListenerInterface.h"

#include "tinyxml2.h"

#include <sqlite_modern_cpp.h>

using namespace std;
using namespace std::chrono;

using namespace sqlite;
using namespace tinyxml2;

using namespace eprosima;
using namespace eprosima::fastrtps;

class ModuleManager  {

public:

    ModuleManager();

    ~ModuleManager() = default;;

    void Start();

    void RunLoop();

    void Shutdown();

    bool isRunning();

    void Cleanup();

protected:

    std::thread m_thread;
    std::mutex m_mutex;
    bool m_runThread;

    const char *nodeName = "AMM_ModuleManager";

    DDS_Manager *mgr;

    Subscriber *status_subscriber;
    Subscriber *config_subscriber;
    Publisher *config_publisher;


};

