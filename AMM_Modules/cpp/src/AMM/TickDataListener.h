#pragma once

#define __LISTENER_H__

#include <string>
#include <sstream>
#include <iostream>
#include "ccpp_dds_dcps.h"

#include "CheckStatus.h"
#include "ccpp_AMM.h"

#include "PhysiologyEngineManager.h"

using namespace AMM::Simulation;
using namespace DDS;

class PhysiologyEngineManager;

class TickDataListener: public virtual DataReaderListener {

public:
	bool m_closed;
	TickDataReader_var m_TickReader;
	GuardCondition_var m_guardCond;
	PhysiologyEngineManager* m_pe;

	TickDataListener() {
		m_guardCond = new GuardCondition();
		m_closed = false;
	}

	virtual ~TickDataListener() {

	};

	void set(PhysiologyEngineManager* pe)
	{
		m_pe = pe;
	}

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

