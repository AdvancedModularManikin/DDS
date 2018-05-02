#include "DDS_Listeners.h"

using namespace std;

void DDS_Listeners::DefaultSubListener::onSubscriptionMatched(Subscriber *sub,
                                                              MatchingInfo &info) {
    if (info.status == MATCHED_MATCHING) {
        n_matched++;
    } else {
        n_matched--;
    }
}

void DDS_Listeners::DefaultSubListener::onNewDataMessage(Subscriber *sub) {
}

void DDS_Listeners::NodeSubListener::onSubscriptionMatched(Subscriber *sub,
                                                           MatchingInfo &info) {
    if (info.status == MATCHED_MATCHING) {
        n_matched++;
    } else {
        n_matched--;
    }
}

void DDS_Listeners::NodeSubListener::onNewDataMessage(Subscriber *sub) {
    AMM::Physiology::Node n;
    if (sub->takeNextData(&n, &m_info)) {
        if (m_info.sampleKind == ALIVE) {
            if (upstream != nullptr) {
                upstream->onNewNodeData(n, &m_info);
            }
            ++n_msg;
        }
    }
}

void DDS_Listeners::CommandSubListener::onSubscriptionMatched(Subscriber *sub,
                                                              MatchingInfo &info) {
    if (info.status == MATCHED_MATCHING) {
        n_matched++;
    } else {
        n_matched--;
    }
}

void DDS_Listeners::CommandSubListener::onNewDataMessage(Subscriber *sub) {
    AMM::PatientAction::BioGears::Command st;

    if (sub->takeNextData(&st, &m_info)) {
        if (m_info.sampleKind == ALIVE) {
            if (upstream != nullptr) {
                upstream->onNewCommandData(st, &m_info);
            }
            ++n_msg;
        }
    }
}

void DDS_Listeners::TickSubListener::onSubscriptionMatched(Subscriber *sub,
                                                           MatchingInfo &info) {
    if (info.status == MATCHED_MATCHING) {
        n_matched++;
    } else {
        n_matched--;
    }
}

void DDS_Listeners::TickSubListener::onNewDataMessage(Subscriber *sub) {
    AMM::Simulation::Tick st;

    if (sub->takeNextData(&st, &m_info)) {
        if (m_info.sampleKind == ALIVE) {
            if (upstream != nullptr) {
                upstream->onNewTickData(st, &m_info);
            }
            ++n_msg;
        }
    }
}

void DDS_Listeners::StatusSubListener::onSubscriptionMatched(Subscriber *sub,
                                                             MatchingInfo &info) {
    if (info.status == MATCHED_MATCHING) {
        n_matched++;
    } else {
        n_matched--;
    }
}

void DDS_Listeners::StatusSubListener::onNewDataMessage(Subscriber *sub) {
    AMM::Capability::Status st;

    if (sub->takeNextData(&st, &m_info)) {
        if (m_info.sampleKind == ALIVE) {
            if (upstream != nullptr) {
                upstream->onNewStatusData(st, &m_info);
            }
            ++n_msg;
        }
    }
}

void DDS_Listeners::ConfigSubListener::onSubscriptionMatched(Subscriber *sub,
                                                             MatchingInfo &info) {
    if (info.status == MATCHED_MATCHING) {
        n_matched++;
    } else {
        n_matched--;
    }
}

void DDS_Listeners::ConfigSubListener::onNewDataMessage(Subscriber *sub) {
    AMM::Capability::Configuration st;

    if (sub->takeNextData(&st, &m_info)) {
        if (m_info.sampleKind == ALIVE) {
            if (upstream != nullptr) {
                upstream->onNewConfigData(st, &m_info);
            }
            ++n_msg;
        }
    }
}


void DDS_Listeners::PubListener::onPublicationMatched(Publisher *pub,
                                                      MatchingInfo &info) {
    if (info.status == MATCHED_MATCHING) {
        n_matched++;
    } else {
        n_matched--;
    }
}

