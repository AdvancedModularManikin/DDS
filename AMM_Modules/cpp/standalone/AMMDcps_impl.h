#ifndef AMMDCPS_IMPL_H_
#define AMMDCPS_IMPL_H_

#include "ccpp.h"
#include "ccpp_AMM.h"
#include "ccpp_TypeSupport_impl.h"
#include "ccpp_DataWriter_impl.h"
#include "ccpp_DataReader_impl.h"
#include "ccpp_DataReaderView_impl.h"


namespace AMM {

    namespace Simulation {

        class  TickTypeSupportFactory : public ::DDS::TypeSupportFactory_impl
        {
        public:
            TickTypeSupportFactory() {}
            virtual ~TickTypeSupportFactory() {}
        private:
            ::DDS::DataWriter_ptr
            create_datawriter (gapi_dataWriter handle);
        
            ::DDS::DataReader_ptr
            create_datareader (gapi_dataReader handle);
        
            ::DDS::DataReaderView_ptr
            create_view (gapi_dataReaderView handle);
        };
        
        class  TickTypeSupport : public virtual TickTypeSupportInterface,
                                       public ::DDS::TypeSupport_impl
        {
        public:
            virtual ::DDS::ReturnCode_t register_type(
                ::DDS::DomainParticipant_ptr participant,
                const char * type_name) THROW_ORB_EXCEPTIONS;
        
            virtual char * get_type_name() THROW_ORB_EXCEPTIONS;
        
            TickTypeSupport (void);
            virtual ~TickTypeSupport (void);
        
        private:
            TickTypeSupport (const TickTypeSupport &);
            void operator= (const TickTypeSupport &);
        
            static const char *metaDescriptor[];
            static const ::DDS::ULong metaDescriptorArrLength;
        };
        
        typedef TickTypeSupportInterface_var TickTypeSupport_var;
        typedef TickTypeSupportInterface_ptr TickTypeSupport_ptr;
        
        class  TickDataWriter_impl : public virtual TickDataWriter,
                                            public ::DDS::DataWriter_impl
        {
        public:
        
            virtual ::DDS::InstanceHandle_t register_instance(
                const Tick & instance_data) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t register_instance_w_timestamp(
                const Tick & instance_data,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance(
                const Tick & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance_w_timestamp(
                const Tick & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write(
                const Tick & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write_w_timestamp(
                const Tick & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose(
                const Tick & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose_w_timestamp(
                const Tick & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose(
                const Tick & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose_w_timestamp(
                const Tick & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                Tick & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const Tick & instance_data) THROW_ORB_EXCEPTIONS;
        
        
            TickDataWriter_impl (
                gapi_dataWriter handle
            );
        
            virtual ~TickDataWriter_impl (void);
        
        private:
            TickDataWriter_impl(const TickDataWriter_impl &);
            void operator= (const TickDataWriter &);
        };
        
        class  TickDataReader_impl : public virtual TickDataReader,
                                            public ::DDS::DataReader_impl
        {
            friend class TickDataReaderView_impl;
        public:
            virtual ::DDS::ReturnCode_t read(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                Tick & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                Tick & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                Tick & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const Tick & instance) THROW_ORB_EXCEPTIONS;
        
            TickDataReader_impl (
                gapi_dataReader handle
            );
        
            virtual ~TickDataReader_impl(void);
        
        private:
            TickDataReader_impl(const TickDataReader &);
            void operator= (const TickDataReader &);
        
            static ::DDS::ReturnCode_t check_preconditions(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples
            );
        };
        
        class  TickDataReaderView_impl : public virtual TickDataReaderView,
                                            public ::DDS::DataReaderView_impl
        {
        public:
            virtual ::DDS::ReturnCode_t read(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                Tick & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                Tick & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                TickSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                Tick & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const Tick & instance) THROW_ORB_EXCEPTIONS;
        
            TickDataReaderView_impl (
                gapi_dataReader handle
            );
        
            virtual ~TickDataReaderView_impl(void);
        
        private:
            TickDataReaderView_impl(const TickDataReaderView &);
            void operator= (const TickDataReaderView &);
        };
        
    }

    namespace Physiology {

        class  DataTypeSupportFactory : public ::DDS::TypeSupportFactory_impl
        {
        public:
            DataTypeSupportFactory() {}
            virtual ~DataTypeSupportFactory() {}
        private:
            ::DDS::DataWriter_ptr
            create_datawriter (gapi_dataWriter handle);
        
            ::DDS::DataReader_ptr
            create_datareader (gapi_dataReader handle);
        
            ::DDS::DataReaderView_ptr
            create_view (gapi_dataReaderView handle);
        };
        
        class  DataTypeSupport : public virtual DataTypeSupportInterface,
                                       public ::DDS::TypeSupport_impl
        {
        public:
            virtual ::DDS::ReturnCode_t register_type(
                ::DDS::DomainParticipant_ptr participant,
                const char * type_name) THROW_ORB_EXCEPTIONS;
        
            virtual char * get_type_name() THROW_ORB_EXCEPTIONS;
        
            DataTypeSupport (void);
            virtual ~DataTypeSupport (void);
        
        private:
            DataTypeSupport (const DataTypeSupport &);
            void operator= (const DataTypeSupport &);
        
            static const char *metaDescriptor[];
            static const ::DDS::ULong metaDescriptorArrLength;
        };
        
        typedef DataTypeSupportInterface_var DataTypeSupport_var;
        typedef DataTypeSupportInterface_ptr DataTypeSupport_ptr;
        
        class  DataDataWriter_impl : public virtual DataDataWriter,
                                            public ::DDS::DataWriter_impl
        {
        public:
        
            virtual ::DDS::InstanceHandle_t register_instance(
                const Data & instance_data) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t register_instance_w_timestamp(
                const Data & instance_data,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance(
                const Data & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance_w_timestamp(
                const Data & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write(
                const Data & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write_w_timestamp(
                const Data & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose(
                const Data & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose_w_timestamp(
                const Data & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose(
                const Data & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose_w_timestamp(
                const Data & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                Data & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const Data & instance_data) THROW_ORB_EXCEPTIONS;
        
        
            DataDataWriter_impl (
                gapi_dataWriter handle
            );
        
            virtual ~DataDataWriter_impl (void);
        
        private:
            DataDataWriter_impl(const DataDataWriter_impl &);
            void operator= (const DataDataWriter &);
        };
        
        class  DataDataReader_impl : public virtual DataDataReader,
                                            public ::DDS::DataReader_impl
        {
            friend class DataDataReaderView_impl;
        public:
            virtual ::DDS::ReturnCode_t read(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                Data & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                Data & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                Data & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const Data & instance) THROW_ORB_EXCEPTIONS;
        
            DataDataReader_impl (
                gapi_dataReader handle
            );
        
            virtual ~DataDataReader_impl(void);
        
        private:
            DataDataReader_impl(const DataDataReader &);
            void operator= (const DataDataReader &);
        
            static ::DDS::ReturnCode_t check_preconditions(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples
            );
        };
        
        class  DataDataReaderView_impl : public virtual DataDataReaderView,
                                            public ::DDS::DataReaderView_impl
        {
        public:
            virtual ::DDS::ReturnCode_t read(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                Data & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                Data & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                DataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                Data & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const Data & instance) THROW_ORB_EXCEPTIONS;
        
            DataDataReaderView_impl (
                gapi_dataReader handle
            );
        
            virtual ~DataDataReaderView_impl(void);
        
        private:
            DataDataReaderView_impl(const DataDataReaderView &);
            void operator= (const DataDataReaderView &);
        };
        
        class  HighFrequencyDataTypeSupportFactory : public ::DDS::TypeSupportFactory_impl
        {
        public:
            HighFrequencyDataTypeSupportFactory() {}
            virtual ~HighFrequencyDataTypeSupportFactory() {}
        private:
            ::DDS::DataWriter_ptr
            create_datawriter (gapi_dataWriter handle);
        
            ::DDS::DataReader_ptr
            create_datareader (gapi_dataReader handle);
        
            ::DDS::DataReaderView_ptr
            create_view (gapi_dataReaderView handle);
        };
        
        class  HighFrequencyDataTypeSupport : public virtual HighFrequencyDataTypeSupportInterface,
                                       public ::DDS::TypeSupport_impl
        {
        public:
            virtual ::DDS::ReturnCode_t register_type(
                ::DDS::DomainParticipant_ptr participant,
                const char * type_name) THROW_ORB_EXCEPTIONS;
        
            virtual char * get_type_name() THROW_ORB_EXCEPTIONS;
        
            HighFrequencyDataTypeSupport (void);
            virtual ~HighFrequencyDataTypeSupport (void);
        
        private:
            HighFrequencyDataTypeSupport (const HighFrequencyDataTypeSupport &);
            void operator= (const HighFrequencyDataTypeSupport &);
        
            static const char *metaDescriptor[];
            static const ::DDS::ULong metaDescriptorArrLength;
        };
        
        typedef HighFrequencyDataTypeSupportInterface_var HighFrequencyDataTypeSupport_var;
        typedef HighFrequencyDataTypeSupportInterface_ptr HighFrequencyDataTypeSupport_ptr;
        
        class  HighFrequencyDataDataWriter_impl : public virtual HighFrequencyDataDataWriter,
                                            public ::DDS::DataWriter_impl
        {
        public:
        
            virtual ::DDS::InstanceHandle_t register_instance(
                const HighFrequencyData & instance_data) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t register_instance_w_timestamp(
                const HighFrequencyData & instance_data,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance(
                const HighFrequencyData & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance_w_timestamp(
                const HighFrequencyData & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write(
                const HighFrequencyData & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write_w_timestamp(
                const HighFrequencyData & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose(
                const HighFrequencyData & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose_w_timestamp(
                const HighFrequencyData & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose(
                const HighFrequencyData & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose_w_timestamp(
                const HighFrequencyData & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                HighFrequencyData & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const HighFrequencyData & instance_data) THROW_ORB_EXCEPTIONS;
        
        
            HighFrequencyDataDataWriter_impl (
                gapi_dataWriter handle
            );
        
            virtual ~HighFrequencyDataDataWriter_impl (void);
        
        private:
            HighFrequencyDataDataWriter_impl(const HighFrequencyDataDataWriter_impl &);
            void operator= (const HighFrequencyDataDataWriter &);
        };
        
        class  HighFrequencyDataDataReader_impl : public virtual HighFrequencyDataDataReader,
                                            public ::DDS::DataReader_impl
        {
            friend class HighFrequencyDataDataReaderView_impl;
        public:
            virtual ::DDS::ReturnCode_t read(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                HighFrequencyData & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                HighFrequencyData & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                HighFrequencyData & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const HighFrequencyData & instance) THROW_ORB_EXCEPTIONS;
        
            HighFrequencyDataDataReader_impl (
                gapi_dataReader handle
            );
        
            virtual ~HighFrequencyDataDataReader_impl(void);
        
        private:
            HighFrequencyDataDataReader_impl(const HighFrequencyDataDataReader &);
            void operator= (const HighFrequencyDataDataReader &);
        
            static ::DDS::ReturnCode_t check_preconditions(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples
            );
        };
        
        class  HighFrequencyDataDataReaderView_impl : public virtual HighFrequencyDataDataReaderView,
                                            public ::DDS::DataReaderView_impl
        {
        public:
            virtual ::DDS::ReturnCode_t read(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                HighFrequencyData & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                HighFrequencyData & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                HighFrequencyDataSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                HighFrequencyData & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const HighFrequencyData & instance) THROW_ORB_EXCEPTIONS;
        
            HighFrequencyDataDataReaderView_impl (
                gapi_dataReader handle
            );
        
            virtual ~HighFrequencyDataDataReaderView_impl(void);
        
        private:
            HighFrequencyDataDataReaderView_impl(const HighFrequencyDataDataReaderView &);
            void operator= (const HighFrequencyDataDataReaderView &);
        };
        
    }

}

#endif
