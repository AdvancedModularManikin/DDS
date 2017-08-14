#include "stdafx.h"

#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>

#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/attributes/PublisherAttributes.h>

#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/attributes/SubscriberAttributes.h>

#include <fastrtps/Domain.h>

#include <fastrtps/utils/eClock.h>

#include <fastrtps/fastrtps_fwd.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/subscriber/SampleInfo.h>

#include "AMMPubSubTypes.h"

#include <fastrtps/Domain.h>

using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace std;


class NodeSubListener : public SubscriberListener
{
public:
    NodeSubListener() : n_matched(0),n_msg(0){};
    ~NodeSubListener(){};

    void onNewDataMessage(Subscriber* sub);

    SampleInfo_t m_info;
    int n_matched;
    int n_msg;
} node_sub_listener;

void NodeSubListener::onNewDataMessage(Subscriber* sub)
{
    // Take data
    AMM::Physiology::Node n;

    if(sub->takeNextData(&n, &m_info))
    {
        if(m_info.sampleKind == ALIVE)
        {
            cout << "-- Data rec'd " << endl;
            cout << "\tFrame\t\t" << n.frame() << endl;
            cout << "\tNodePath\t\t" << n.nodepath() << endl;
            cout << "\tValue\t\t" << n.dbl() << endl;

            // Print your structure data here.
            ++n_msg;
            std::cout << "Sample received, count=" << n_msg << std::endl;
        }
    }
}

static void show_usage(std::string name) {
	cerr << "Usage: " << name << " <option(s)> node_path node_path ..." << "\nOptions:\n"
			<< "\t-h,--help\t\tShow this help message\n" << endl;
	cerr << "Example: " << name << " ECG HR " << endl;
}

int main(int argc, char *argv[]) {
	std::vector<std::string> node_paths;

	if (argc <= 1) {
		show_usage(argv[0]);
		return 1;
	}

	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if ((arg == "-h") || (arg == "--help")) {
			show_usage(argv[0]);
			return 0;
		} else {
			node_paths.push_back(arg);
		}
	}

    Participant *mp_participant;


	ParticipantAttributes PParam;
	PParam.rtps.builtin.domainId = 0;
	PParam.rtps.builtin.leaseDuration = c_TimeInfinite;
	PParam.rtps.setName("AMM");  //You can put here the name you want
	mp_participant = Domain::createParticipant(PParam);
	if(mp_participant == nullptr)
		return false;

	AMM::Physiology::NodePubSubType nodeType;
	Domain::registerType(mp_participant,(TopicDataType*) &nodeType);


	// create subscription filter
	ostringstream filterString;
	bool first = true;
	for (std::string np : node_paths) {
		if (first) {
			filterString << "nodepath = '" << np << "'";
			first = false;
		} else {
			filterString << " OR nodepath = '" << np << "'";
		}
	}
	std::string fString = filterString.str();
	cout << "=== [VirtualEquipment] Subscription filter : " << fString << endl;


    Subscriber *node_subscriber;
	SubscriberAttributes nodeRparam;
	nodeRparam.topic.topicKind = NO_KEY;
	nodeRparam.topic.topicDataType = nodeType.getName(); //Must be registered before the creation of the subscriber
	nodeRparam.topic.topicName = "NodeData";
	node_subscriber = Domain::createSubscriber(mp_participant,nodeRparam,(SubscriberListener*)&node_sub_listener);

	if(node_subscriber == nullptr)
		return false;

	std::cout << "Initialized node subscriber." << std::endl;


	cout << "=== [VirtualEquipment] Ready ..." << endl;

	//while (!closed) {
        cout << "Waiting for Data, press Enter to stop the Subscriber. "<<endl;
        std::cin.ignore();
        cout << "Shutting down the Subscriber." << endl;
	//}

	cout << "=== [VirtualEquipment] Simulation stopped." << endl;

	return 0;

}
