#define __LISTENER_H__

#include <string>
#include <sstream>
#include <iostream>
#include "ccpp_dds_dcps.h"

#include "CheckStatus.h"
#include "ccpp_AMM.h"

using namespace AMM::Simulation;

// ------------------------------ Listeners ------------------------------
class TickDataListener: public virtual DDS::DataReaderListener {

public:

	bool m_closed;
	TickDataReader_var m_TickReader;
	DDS::GuardCondition_var m_guardCond;

	TickDataListener() {
		m_guardCond = new DDS::GuardCondition();
		m_closed = false;
	}

	virtual ~TickDataListener();

	/* Callback method implementation. */
	virtual void on_data_available(DDS::DataReader_ptr reader)
			THROW_ORB_EXCEPTIONS;

	virtual void on_requested_deadline_missed(DDS::DataReader_ptr reader,
			const DDS::RequestedDeadlineMissedStatus &status)
					THROW_ORB_EXCEPTIONS;

	virtual void on_requested_incompatible_qos(DDS::DataReader_ptr reader,
			const DDS::RequestedIncompatibleQosStatus &status)
					THROW_ORB_EXCEPTIONS;

	virtual void on_sample_rejected(DDS::DataReader_ptr reader,
			const DDS::SampleRejectedStatus &status) THROW_ORB_EXCEPTIONS;

	virtual void on_liveliness_changed(DDS::DataReader_ptr reader,
			const DDS::LivelinessChangedStatus &status) THROW_ORB_EXCEPTIONS;

	virtual void on_subscription_matched(DDS::DataReader_ptr reader,
			const DDS::SubscriptionMatchedStatus &status) THROW_ORB_EXCEPTIONS;

	virtual void on_sample_lost(DDS::DataReader_ptr reader,
			const DDS::SampleLostStatus &status) THROW_ORB_EXCEPTIONS;

};

