#include "DDS_Listeners.h"

using namespace std;

void DDS_Listeners::PubListener::SetUpstream(ListenerInterface* l) {
	upstream = l;
}

void DDS_Listeners::NodeSubListener::SetUpstream(ListenerInterface* l) {
    upstream = l;
}

void DDS_Listeners::TickSubListener::SetUpstream(ListenerInterface* l) {
    upstream = l;
}

void DDS_Listeners::CommandSubListener::SetUpstream(ListenerInterface* l) {
    upstream = l;
}

void DDS_Listeners::NodeSubListener::onSubscriptionMatched(Subscriber* sub,
		MatchingInfo& info) {
	if (info.status == MATCHED_MATCHING) {
		n_matched++;
		std::cout << "Subscriber matched" << std::endl;
	} else {
		n_matched--;
		std::cout << "Subscriber unmatched" << std::endl;
	}
}

void DDS_Listeners::NodeSubListener::onNewDataMessage(Subscriber* sub) {
	cout << "Data on the pipe!" << endl;

	AMM::Physiology::Node n;
	if (sub->takeNextData(&n, &m_info)) {
		if (m_info.sampleKind == ALIVE) {

            if (upstream != nullptr) {
                upstream->onNewNodeData(n);
            }

			++n_msg;
			cout << "-- Data received, count=" << n_msg << std::endl;
			cout << "\tFrame\t\t" << n.frame() << endl;
			cout << "\tNodePath\t\t" << n.nodepath() << endl;
			cout << "\tValue\t\t" << n.dbl() << endl;

		}
	}
}

void DDS_Listeners::CommandSubListener::onSubscriptionMatched(Subscriber* sub,
		MatchingInfo& info) {
	if (info.status == MATCHED_MATCHING) {
		n_matched++;
		std::cout << "Subscriber matched" << std::endl;
	} else {
		n_matched--;
		std::cout << "Subscriber unmatched" << std::endl;
	}
}

void DDS_Listeners::CommandSubListener::onNewDataMessage(Subscriber* sub) {
	AMM::PatientAction::BioGears::Command st;

	if (sub->takeNextData(&st, &m_info)) {
		if (m_info.sampleKind == ALIVE) {
			if (upstream != nullptr) {
                upstream->onNewCommandData(st);
            }
			++n_msg;
			// std::cout << "Sample received, count=" << n_msg << std::endl;
		}
	}
}

void DDS_Listeners::TickSubListener::onSubscriptionMatched(Subscriber* sub,
		MatchingInfo& info) {
	if (info.status == MATCHED_MATCHING) {
		n_matched++;
		std::cout << "Subscriber matched" << std::endl;
	} else {
		n_matched--;
		std::cout << "Subscriber unmatched" << std::endl;
	}
}

void DDS_Listeners::TickSubListener::onNewDataMessage(Subscriber* sub) {
	AMM::Simulation::Tick st;

	if (sub->takeNextData(&st, &m_info)) {
		if (m_info.sampleKind == ALIVE) {
            if (upstream != nullptr) {
                upstream->onNewTickData(st);
            }
			++n_msg;
			// std::cout << "[TICK] Frame " << st.frame() << ", count " << n_msg << endl;
		}
	}
}

void DDS_Listeners::PubListener::onPublicationMatched(Publisher* pub,
		MatchingInfo& info) {
	if (info.status == MATCHED_MATCHING) {
		n_matched++;
		std::cout << "Publisher matched" << std::endl;
	} else {
		n_matched--;
		std::cout << "Publisher unmatched" << std::endl;
	}
}

