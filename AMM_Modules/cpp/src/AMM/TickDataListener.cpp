#include "TickDataListener.h"

using namespace DDS;
using namespace AMM::Simulation;

void TickDataListener::on_data_available(DDS::DataReader_ptr reader) THROW_ORB_EXCEPTIONS
{
	DDS::ReturnCode_t status;
	TickSeq tickList;
	SampleInfoSeq infoSeq;

	status = m_TickReader->read(tickList, infoSeq, LENGTH_UNLIMITED, ANY_SAMPLE_STATE, NEW_VIEW_STATE, ANY_INSTANCE_STATE);
	checkStatus(status, "TickDataReader::read");
	for (DDS::ULong j = 0; j < tickList.length(); j++) {
		// Do every tick...

		if (tickList[j].frame == -1) {
			cout << "[SHUTDOWN]";
			m_pe->closed = true;
			m_pe->SendShutdown();
		} else if (tickList[j].frame == -2) {
			// Pause signal
			cout << "[PAUSE]";
			m_pe->paused = true;
		} else if (tickList[j].frame > 0 || !m_pe->paused) {
			if (m_pe->paused) {
				cout << "[RESUME]";
				m_pe->paused = false;
			}

			// Did we get a frame out of order?  Just mark it with an X for now.
			if (tickList[j].frame <= m_pe->lastFrame) {
				cout << "x";
			} else {
				cout << ".";
			}
			m_pe->lastFrame = tickList[j].frame;

			// Per-frame stuff happens here
			m_pe->AdvanceTimeTick();
			m_pe->PublishData(false);
		}

	}
	status = m_TickReader->return_loan(tickList, infoSeq);
	checkStatus(status, "TickDataReader::return_loan");
	// unblock the waitset in Subscriber main loop
	m_guardCond->set_trigger_value(true);
}
;

void TickDataListener::on_requested_deadline_missed(DDS::DataReader_ptr reader, const DDS::RequestedDeadlineMissedStatus &status)
		THROW_ORB_EXCEPTIONS
		{
	printf("\n=== [TickDataListener::on_requested_deadline_missed] : triggered\n");
	printf("\n=== [TickDataListener::on_requested_deadline_missed] : stopping\n");
	m_closed = true;
	// unblock the waitset in Subscriber main loop
	m_guardCond->set_trigger_value(true);
}
;

void TickDataListener::on_requested_incompatible_qos(DDS::DataReader_ptr reader, const DDS::RequestedIncompatibleQosStatus &status)
		THROW_ORB_EXCEPTIONS
		{
	printf("\n=== [TickDataListener::on_requested_incompatible_qos] : triggered\n");
}
;

void TickDataListener::on_sample_rejected(DDS::DataReader_ptr reader, const DDS::SampleRejectedStatus &status) THROW_ORB_EXCEPTIONS
{
	printf("\n=== [TickDataListener::on_sample_rejected] : triggered\n");
}
;

void TickDataListener::on_liveliness_changed(DDS::DataReader_ptr reader, const DDS::LivelinessChangedStatus &status)
		THROW_ORB_EXCEPTIONS
		{
	printf("\n=== [TickDataListener::on_liveliness_changed] : triggered\n");
}
;

void TickDataListener::on_subscription_matched(DDS::DataReader_ptr reader, const DDS::SubscriptionMatchedStatus &status)
		THROW_ORB_EXCEPTIONS
		{
	printf("\n=== [TickDataListener::on_subscription_matched] : triggered\n");
}
;

void TickDataListener::on_sample_lost(DDS::DataReader_ptr reader, const DDS::SampleLostStatus &status) THROW_ORB_EXCEPTIONS
{
	printf("\n=== [TickDataListener::on_sample_lost] : triggered\n");
}
;
