/************************************************************************
 *
 * Copyright (c) 2010
 * PrismTech Ltd.
 * All rights Reserved.
 *
 * LOGICAL_NAME:    TickDataListener.cpp
 * FUNCTION:        .
 * MODULE:          .
 * DATE             August 2010.
 ************************************************************************
 *
 * This file contains the implementation of TickDataListener
 *
 ***/

#include "TickDataListener.h"
#include "CheckStatus.h"
#include <sstream>

using namespace DDS;
using namespace AMM::Simulation;

// --------------------------------------------------------------------------------------------------------
//                                              TickDataListener                                     --
// --------------------------------------------------------------------------------------------------------

void TickDataListener::on_data_available(DDS::DataReader_ptr reader)
  THROW_ORB_EXCEPTIONS
{
  DDS::ReturnCode_t status;
  TickSeq tickList;
  SampleInfoSeq infoSeq;

  status = m_TickReader->read(tickList, infoSeq, LENGTH_UNLIMITED,
    ANY_SAMPLE_STATE, NEW_VIEW_STATE, ANY_INSTANCE_STATE);
  checkStatus(status, "MsgDataReader::read");
  printf("=== [TickDataListener::on_data_available] - tickList.length : %d\n", tickList.length());
  for (DDS::ULong j = 0; j < tickList.length(); j++)
    {
      printf("\n    --- message received ---\n");
      printf("\n    frame  : %d\n", tickList[j].frame);
    }
  status = m_TickReader->return_loan(tickList, infoSeq);
  checkStatus(status, "MsgDataReader::return_loan");
  // unblock the waitset in Subscriber main loop
  m_guardCond->set_trigger_value(true);
};

void TickDataListener::on_requested_deadline_missed(DDS::DataReader_ptr
  reader, const DDS::RequestedDeadlineMissedStatus &status)THROW_ORB_EXCEPTIONS
{
  printf("\n=== [TickDataListener::on_requested_deadline_missed] : triggered\n");
  printf("\n=== [TickDataListener::on_requested_deadline_missed] : stopping\n");
  m_closed = true;
  // unblock the waitset in Subscriber main loop
  m_guardCond->set_trigger_value(true);
};

void TickDataListener::on_requested_incompatible_qos(DDS::DataReader_ptr
  reader, const DDS::RequestedIncompatibleQosStatus &status)
  THROW_ORB_EXCEPTIONS
{
  printf("\n=== [TickDataListener::on_requested_incompatible_qos] : triggered\n");
};

void TickDataListener::on_sample_rejected(DDS::DataReader_ptr reader, const
  DDS::SampleRejectedStatus &status)THROW_ORB_EXCEPTIONS
{
  printf("\n=== [TickDataListener::on_sample_rejected] : triggered\n");
};

void TickDataListener::on_liveliness_changed(DDS::DataReader_ptr reader,
  const DDS::LivelinessChangedStatus &status)THROW_ORB_EXCEPTIONS
{
  printf("\n=== [TickDataListener::on_liveliness_changed] : triggered\n");
};

void TickDataListener::on_subscription_matched(DDS::DataReader_ptr reader,
  const DDS::SubscriptionMatchedStatus &status)THROW_ORB_EXCEPTIONS
{
  printf("\n=== [TickDataListener::on_subscription_matched] : triggered\n");
};

void TickDataListener::on_sample_lost(DDS::DataReader_ptr reader, const DDS
  ::SampleLostStatus &status)THROW_ORB_EXCEPTIONS
{
  printf("\n=== [TickDataListener::on_sample_lost] : triggered\n");
};
