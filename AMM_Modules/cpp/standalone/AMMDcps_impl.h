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

        class  NodeTypeSupportFactory : public ::DDS::TypeSupportFactory_impl
        {
        public:
            NodeTypeSupportFactory() {}
            virtual ~NodeTypeSupportFactory() {}
        private:
            ::DDS::DataWriter_ptr
            create_datawriter (gapi_dataWriter handle);
        
            ::DDS::DataReader_ptr
            create_datareader (gapi_dataReader handle);
        
            ::DDS::DataReaderView_ptr
            create_view (gapi_dataReaderView handle);
        };
        
        class  NodeTypeSupport : public virtual NodeTypeSupportInterface,
                                       public ::DDS::TypeSupport_impl
        {
        public:
            virtual ::DDS::ReturnCode_t register_type(
                ::DDS::DomainParticipant_ptr participant,
                const char * type_name) THROW_ORB_EXCEPTIONS;
        
            virtual char * get_type_name() THROW_ORB_EXCEPTIONS;
        
            NodeTypeSupport (void);
            virtual ~NodeTypeSupport (void);
        
        private:
            NodeTypeSupport (const NodeTypeSupport &);
            void operator= (const NodeTypeSupport &);
        
            static const char *metaDescriptor[];
            static const ::DDS::ULong metaDescriptorArrLength;
        };
        
        typedef NodeTypeSupportInterface_var NodeTypeSupport_var;
        typedef NodeTypeSupportInterface_ptr NodeTypeSupport_ptr;
        
        class  NodeDataWriter_impl : public virtual NodeDataWriter,
                                            public ::DDS::DataWriter_impl
        {
        public:
        
            virtual ::DDS::InstanceHandle_t register_instance(
                const Node & instance_data) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t register_instance_w_timestamp(
                const Node & instance_data,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance(
                const Node & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance_w_timestamp(
                const Node & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write(
                const Node & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write_w_timestamp(
                const Node & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose(
                const Node & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose_w_timestamp(
                const Node & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose(
                const Node & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose_w_timestamp(
                const Node & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                Node & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const Node & instance_data) THROW_ORB_EXCEPTIONS;
        
        
            NodeDataWriter_impl (
                gapi_dataWriter handle
            );
        
            virtual ~NodeDataWriter_impl (void);
        
        private:
            NodeDataWriter_impl(const NodeDataWriter_impl &);
            void operator= (const NodeDataWriter &);
        };
        
        class  NodeDataReader_impl : public virtual NodeDataReader,
                                            public ::DDS::DataReader_impl
        {
            friend class NodeDataReaderView_impl;
        public:
            virtual ::DDS::ReturnCode_t read(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                Node & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                Node & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                Node & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const Node & instance) THROW_ORB_EXCEPTIONS;
        
            NodeDataReader_impl (
                gapi_dataReader handle
            );
        
            virtual ~NodeDataReader_impl(void);
        
        private:
            NodeDataReader_impl(const NodeDataReader &);
            void operator= (const NodeDataReader &);
        
            static ::DDS::ReturnCode_t check_preconditions(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples
            );
        };
        
        class  NodeDataReaderView_impl : public virtual NodeDataReaderView,
                                            public ::DDS::DataReaderView_impl
        {
        public:
            virtual ::DDS::ReturnCode_t read(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                Node & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                Node & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                NodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                Node & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const Node & instance) THROW_ORB_EXCEPTIONS;
        
            NodeDataReaderView_impl (
                gapi_dataReader handle
            );
        
            virtual ~NodeDataReaderView_impl(void);
        
        private:
            NodeDataReaderView_impl(const NodeDataReaderView &);
            void operator= (const NodeDataReaderView &);
        };
        
        class  HighFrequencyNodeTypeSupportFactory : public ::DDS::TypeSupportFactory_impl
        {
        public:
            HighFrequencyNodeTypeSupportFactory() {}
            virtual ~HighFrequencyNodeTypeSupportFactory() {}
        private:
            ::DDS::DataWriter_ptr
            create_datawriter (gapi_dataWriter handle);
        
            ::DDS::DataReader_ptr
            create_datareader (gapi_dataReader handle);
        
            ::DDS::DataReaderView_ptr
            create_view (gapi_dataReaderView handle);
        };
        
        class  HighFrequencyNodeTypeSupport : public virtual HighFrequencyNodeTypeSupportInterface,
                                       public ::DDS::TypeSupport_impl
        {
        public:
            virtual ::DDS::ReturnCode_t register_type(
                ::DDS::DomainParticipant_ptr participant,
                const char * type_name) THROW_ORB_EXCEPTIONS;
        
            virtual char * get_type_name() THROW_ORB_EXCEPTIONS;
        
            HighFrequencyNodeTypeSupport (void);
            virtual ~HighFrequencyNodeTypeSupport (void);
        
        private:
            HighFrequencyNodeTypeSupport (const HighFrequencyNodeTypeSupport &);
            void operator= (const HighFrequencyNodeTypeSupport &);
        
            static const char *metaDescriptor[];
            static const ::DDS::ULong metaDescriptorArrLength;
        };
        
        typedef HighFrequencyNodeTypeSupportInterface_var HighFrequencyNodeTypeSupport_var;
        typedef HighFrequencyNodeTypeSupportInterface_ptr HighFrequencyNodeTypeSupport_ptr;
        
        class  HighFrequencyNodeDataWriter_impl : public virtual HighFrequencyNodeDataWriter,
                                            public ::DDS::DataWriter_impl
        {
        public:
        
            virtual ::DDS::InstanceHandle_t register_instance(
                const HighFrequencyNode & instance_data) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t register_instance_w_timestamp(
                const HighFrequencyNode & instance_data,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance(
                const HighFrequencyNode & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance_w_timestamp(
                const HighFrequencyNode & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write(
                const HighFrequencyNode & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write_w_timestamp(
                const HighFrequencyNode & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose(
                const HighFrequencyNode & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose_w_timestamp(
                const HighFrequencyNode & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose(
                const HighFrequencyNode & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose_w_timestamp(
                const HighFrequencyNode & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                HighFrequencyNode & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const HighFrequencyNode & instance_data) THROW_ORB_EXCEPTIONS;
        
        
            HighFrequencyNodeDataWriter_impl (
                gapi_dataWriter handle
            );
        
            virtual ~HighFrequencyNodeDataWriter_impl (void);
        
        private:
            HighFrequencyNodeDataWriter_impl(const HighFrequencyNodeDataWriter_impl &);
            void operator= (const HighFrequencyNodeDataWriter &);
        };
        
        class  HighFrequencyNodeDataReader_impl : public virtual HighFrequencyNodeDataReader,
                                            public ::DDS::DataReader_impl
        {
            friend class HighFrequencyNodeDataReaderView_impl;
        public:
            virtual ::DDS::ReturnCode_t read(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                HighFrequencyNode & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                HighFrequencyNode & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                HighFrequencyNode & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const HighFrequencyNode & instance) THROW_ORB_EXCEPTIONS;
        
            HighFrequencyNodeDataReader_impl (
                gapi_dataReader handle
            );
        
            virtual ~HighFrequencyNodeDataReader_impl(void);
        
        private:
            HighFrequencyNodeDataReader_impl(const HighFrequencyNodeDataReader &);
            void operator= (const HighFrequencyNodeDataReader &);
        
            static ::DDS::ReturnCode_t check_preconditions(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples
            );
        };
        
        class  HighFrequencyNodeDataReaderView_impl : public virtual HighFrequencyNodeDataReaderView,
                                            public ::DDS::DataReaderView_impl
        {
        public:
            virtual ::DDS::ReturnCode_t read(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                HighFrequencyNode & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                HighFrequencyNode & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                HighFrequencyNodeSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                HighFrequencyNode & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const HighFrequencyNode & instance) THROW_ORB_EXCEPTIONS;
        
            HighFrequencyNodeDataReaderView_impl (
                gapi_dataReader handle
            );
        
            virtual ~HighFrequencyNodeDataReaderView_impl(void);
        
        private:
            HighFrequencyNodeDataReaderView_impl(const HighFrequencyNodeDataReaderView &);
            void operator= (const HighFrequencyNodeDataReaderView &);
        };
        
    }

    namespace PatientAction {

        namespace BioGears {

            class  CommandTypeSupportFactory : public ::DDS::TypeSupportFactory_impl
            {
            public:
                CommandTypeSupportFactory() {}
                virtual ~CommandTypeSupportFactory() {}
            private:
                ::DDS::DataWriter_ptr
                create_datawriter (gapi_dataWriter handle);
            
                ::DDS::DataReader_ptr
                create_datareader (gapi_dataReader handle);
            
                ::DDS::DataReaderView_ptr
                create_view (gapi_dataReaderView handle);
            };
            
            class  CommandTypeSupport : public virtual CommandTypeSupportInterface,
                                           public ::DDS::TypeSupport_impl
            {
            public:
                virtual ::DDS::ReturnCode_t register_type(
                    ::DDS::DomainParticipant_ptr participant,
                    const char * type_name) THROW_ORB_EXCEPTIONS;
            
                virtual char * get_type_name() THROW_ORB_EXCEPTIONS;
            
                CommandTypeSupport (void);
                virtual ~CommandTypeSupport (void);
            
            private:
                CommandTypeSupport (const CommandTypeSupport &);
                void operator= (const CommandTypeSupport &);
            
                static const char *metaDescriptor[];
                static const ::DDS::ULong metaDescriptorArrLength;
            };
            
            typedef CommandTypeSupportInterface_var CommandTypeSupport_var;
            typedef CommandTypeSupportInterface_ptr CommandTypeSupport_ptr;
            
            class  CommandDataWriter_impl : public virtual CommandDataWriter,
                                                public ::DDS::DataWriter_impl
            {
            public:
            
                virtual ::DDS::InstanceHandle_t register_instance(
                    const Command & instance_data) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::InstanceHandle_t register_instance_w_timestamp(
                    const Command & instance_data,
                    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t unregister_instance(
                    const Command & instance_data,
                    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t unregister_instance_w_timestamp(
                    const Command & instance_data,
                    ::DDS::InstanceHandle_t handle,
                    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t write(
                    const Command & instance_data,
                    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t write_w_timestamp(
                    const Command & instance_data,
                    ::DDS::InstanceHandle_t handle,
                    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t dispose(
                    const Command & instance_data,
                    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t dispose_w_timestamp(
                    const Command & instance_data,
                    ::DDS::InstanceHandle_t handle,
                    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t writedispose(
                    const Command & instance_data,
                    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t writedispose_w_timestamp(
                    const Command & instance_data,
                    ::DDS::InstanceHandle_t handle,
                    const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t get_key_value(
                    Command & key_holder,
                    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::InstanceHandle_t lookup_instance(
                    const Command & instance_data) THROW_ORB_EXCEPTIONS;
            
            
                CommandDataWriter_impl (
                    gapi_dataWriter handle
                );
            
                virtual ~CommandDataWriter_impl (void);
            
            private:
                CommandDataWriter_impl(const CommandDataWriter_impl &);
                void operator= (const CommandDataWriter &);
            };
            
            class  CommandDataReader_impl : public virtual CommandDataReader,
                                                public ::DDS::DataReader_impl
            {
                friend class CommandDataReaderView_impl;
            public:
                virtual ::DDS::ReturnCode_t read(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::SampleStateMask sample_states,
                    ::DDS::ViewStateMask view_states,
                    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t take(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::SampleStateMask sample_states,
                    ::DDS::ViewStateMask view_states,
                    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t read_w_condition(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t take_w_condition(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t read_next_sample(
                    Command & received_data,
                    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t take_next_sample(
                    Command & received_data,
                    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t read_instance(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::InstanceHandle_t a_handle,
                    ::DDS::SampleStateMask sample_states,
                    ::DDS::ViewStateMask view_states,
                    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t take_instance(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::InstanceHandle_t a_handle,
                    ::DDS::SampleStateMask sample_states,
                    ::DDS::ViewStateMask view_states,
                    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t read_next_instance(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::InstanceHandle_t a_handle,
                    ::DDS::SampleStateMask sample_states,
                    ::DDS::ViewStateMask view_states,
                    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t take_next_instance(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::InstanceHandle_t a_handle,
                    ::DDS::SampleStateMask sample_states,
                    ::DDS::ViewStateMask view_states,
                    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::InstanceHandle_t a_handle,
                    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::InstanceHandle_t a_handle,
                    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t return_loan(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t get_key_value(
                    Command & key_holder,
                    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::InstanceHandle_t lookup_instance(
                    const Command & instance) THROW_ORB_EXCEPTIONS;
            
                CommandDataReader_impl (
                    gapi_dataReader handle
                );
            
                virtual ~CommandDataReader_impl(void);
            
            private:
                CommandDataReader_impl(const CommandDataReader &);
                void operator= (const CommandDataReader &);
            
                static ::DDS::ReturnCode_t check_preconditions(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples
                );
            };
            
            class  CommandDataReaderView_impl : public virtual CommandDataReaderView,
                                                public ::DDS::DataReaderView_impl
            {
            public:
                virtual ::DDS::ReturnCode_t read(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::SampleStateMask sample_states,
                    ::DDS::ViewStateMask view_states,
                    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t take(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::SampleStateMask sample_states,
                    ::DDS::ViewStateMask view_states,
                    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t read_w_condition(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t take_w_condition(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t read_next_sample(
                    Command & received_data,
                    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t take_next_sample(
                    Command & received_data,
                    ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t read_instance(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::InstanceHandle_t a_handle,
                    ::DDS::SampleStateMask sample_states,
                    ::DDS::ViewStateMask view_states,
                    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t take_instance(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::InstanceHandle_t a_handle,
                    ::DDS::SampleStateMask sample_states,
                    ::DDS::ViewStateMask view_states,
                    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t read_next_instance(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::InstanceHandle_t a_handle,
                    ::DDS::SampleStateMask sample_states,
                    ::DDS::ViewStateMask view_states,
                    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t take_next_instance(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::InstanceHandle_t a_handle,
                    ::DDS::SampleStateMask sample_states,
                    ::DDS::ViewStateMask view_states,
                    ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::InstanceHandle_t a_handle,
                    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq,
                    ::DDS::Long max_samples,
                    ::DDS::InstanceHandle_t a_handle,
                    ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t return_loan(
                    CommandSeq & received_data,
                    ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::ReturnCode_t get_key_value(
                    Command & key_holder,
                    ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
            
                virtual ::DDS::InstanceHandle_t lookup_instance(
                    const Command & instance) THROW_ORB_EXCEPTIONS;
            
                CommandDataReaderView_impl (
                    gapi_dataReader handle
                );
            
                virtual ~CommandDataReaderView_impl(void);
            
            private:
                CommandDataReaderView_impl(const CommandDataReaderView &);
                void operator= (const CommandDataReaderView &);
            };
            
        }

    }

    namespace Performance {

        class  xAPIModifierTypeSupportFactory : public ::DDS::TypeSupportFactory_impl
        {
        public:
            xAPIModifierTypeSupportFactory() {}
            virtual ~xAPIModifierTypeSupportFactory() {}
        private:
            ::DDS::DataWriter_ptr
            create_datawriter (gapi_dataWriter handle);
        
            ::DDS::DataReader_ptr
            create_datareader (gapi_dataReader handle);
        
            ::DDS::DataReaderView_ptr
            create_view (gapi_dataReaderView handle);
        };
        
        class  xAPIModifierTypeSupport : public virtual xAPIModifierTypeSupportInterface,
                                       public ::DDS::TypeSupport_impl
        {
        public:
            virtual ::DDS::ReturnCode_t register_type(
                ::DDS::DomainParticipant_ptr participant,
                const char * type_name) THROW_ORB_EXCEPTIONS;
        
            virtual char * get_type_name() THROW_ORB_EXCEPTIONS;
        
            xAPIModifierTypeSupport (void);
            virtual ~xAPIModifierTypeSupport (void);
        
        private:
            xAPIModifierTypeSupport (const xAPIModifierTypeSupport &);
            void operator= (const xAPIModifierTypeSupport &);
        
            static const char *metaDescriptor[];
            static const ::DDS::ULong metaDescriptorArrLength;
        };
        
        typedef xAPIModifierTypeSupportInterface_var xAPIModifierTypeSupport_var;
        typedef xAPIModifierTypeSupportInterface_ptr xAPIModifierTypeSupport_ptr;
        
        class  xAPIModifierDataWriter_impl : public virtual xAPIModifierDataWriter,
                                            public ::DDS::DataWriter_impl
        {
        public:
        
            virtual ::DDS::InstanceHandle_t register_instance(
                const xAPIModifier & instance_data) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t register_instance_w_timestamp(
                const xAPIModifier & instance_data,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance(
                const xAPIModifier & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance_w_timestamp(
                const xAPIModifier & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write(
                const xAPIModifier & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write_w_timestamp(
                const xAPIModifier & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose(
                const xAPIModifier & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose_w_timestamp(
                const xAPIModifier & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose(
                const xAPIModifier & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose_w_timestamp(
                const xAPIModifier & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                xAPIModifier & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const xAPIModifier & instance_data) THROW_ORB_EXCEPTIONS;
        
        
            xAPIModifierDataWriter_impl (
                gapi_dataWriter handle
            );
        
            virtual ~xAPIModifierDataWriter_impl (void);
        
        private:
            xAPIModifierDataWriter_impl(const xAPIModifierDataWriter_impl &);
            void operator= (const xAPIModifierDataWriter &);
        };
        
        class  xAPIModifierDataReader_impl : public virtual xAPIModifierDataReader,
                                            public ::DDS::DataReader_impl
        {
            friend class xAPIModifierDataReaderView_impl;
        public:
            virtual ::DDS::ReturnCode_t read(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                xAPIModifier & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                xAPIModifier & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                xAPIModifier & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const xAPIModifier & instance) THROW_ORB_EXCEPTIONS;
        
            xAPIModifierDataReader_impl (
                gapi_dataReader handle
            );
        
            virtual ~xAPIModifierDataReader_impl(void);
        
        private:
            xAPIModifierDataReader_impl(const xAPIModifierDataReader &);
            void operator= (const xAPIModifierDataReader &);
        
            static ::DDS::ReturnCode_t check_preconditions(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples
            );
        };
        
        class  xAPIModifierDataReaderView_impl : public virtual xAPIModifierDataReaderView,
                                            public ::DDS::DataReaderView_impl
        {
        public:
            virtual ::DDS::ReturnCode_t read(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                xAPIModifier & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                xAPIModifier & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                xAPIModifierSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                xAPIModifier & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const xAPIModifier & instance) THROW_ORB_EXCEPTIONS;
        
            xAPIModifierDataReaderView_impl (
                gapi_dataReader handle
            );
        
            virtual ~xAPIModifierDataReaderView_impl(void);
        
        private:
            xAPIModifierDataReaderView_impl(const xAPIModifierDataReaderView &);
            void operator= (const xAPIModifierDataReaderView &);
        };
        
        class  StatementTypeSupportFactory : public ::DDS::TypeSupportFactory_impl
        {
        public:
            StatementTypeSupportFactory() {}
            virtual ~StatementTypeSupportFactory() {}
        private:
            ::DDS::DataWriter_ptr
            create_datawriter (gapi_dataWriter handle);
        
            ::DDS::DataReader_ptr
            create_datareader (gapi_dataReader handle);
        
            ::DDS::DataReaderView_ptr
            create_view (gapi_dataReaderView handle);
        };
        
        class  StatementTypeSupport : public virtual StatementTypeSupportInterface,
                                       public ::DDS::TypeSupport_impl
        {
        public:
            virtual ::DDS::ReturnCode_t register_type(
                ::DDS::DomainParticipant_ptr participant,
                const char * type_name) THROW_ORB_EXCEPTIONS;
        
            virtual char * get_type_name() THROW_ORB_EXCEPTIONS;
        
            StatementTypeSupport (void);
            virtual ~StatementTypeSupport (void);
        
        private:
            StatementTypeSupport (const StatementTypeSupport &);
            void operator= (const StatementTypeSupport &);
        
            static const char *metaDescriptor[];
            static const ::DDS::ULong metaDescriptorArrLength;
        };
        
        typedef StatementTypeSupportInterface_var StatementTypeSupport_var;
        typedef StatementTypeSupportInterface_ptr StatementTypeSupport_ptr;
        
        class  StatementDataWriter_impl : public virtual StatementDataWriter,
                                            public ::DDS::DataWriter_impl
        {
        public:
        
            virtual ::DDS::InstanceHandle_t register_instance(
                const Statement & instance_data) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t register_instance_w_timestamp(
                const Statement & instance_data,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance(
                const Statement & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t unregister_instance_w_timestamp(
                const Statement & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write(
                const Statement & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t write_w_timestamp(
                const Statement & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose(
                const Statement & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t dispose_w_timestamp(
                const Statement & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose(
                const Statement & instance_data,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t writedispose_w_timestamp(
                const Statement & instance_data,
                ::DDS::InstanceHandle_t handle,
                const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                Statement & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const Statement & instance_data) THROW_ORB_EXCEPTIONS;
        
        
            StatementDataWriter_impl (
                gapi_dataWriter handle
            );
        
            virtual ~StatementDataWriter_impl (void);
        
        private:
            StatementDataWriter_impl(const StatementDataWriter_impl &);
            void operator= (const StatementDataWriter &);
        };
        
        class  StatementDataReader_impl : public virtual StatementDataReader,
                                            public ::DDS::DataReader_impl
        {
            friend class StatementDataReaderView_impl;
        public:
            virtual ::DDS::ReturnCode_t read(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                Statement & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                Statement & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                Statement & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const Statement & instance) THROW_ORB_EXCEPTIONS;
        
            StatementDataReader_impl (
                gapi_dataReader handle
            );
        
            virtual ~StatementDataReader_impl(void);
        
        private:
            StatementDataReader_impl(const StatementDataReader &);
            void operator= (const StatementDataReader &);
        
            static ::DDS::ReturnCode_t check_preconditions(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples
            );
        };
        
        class  StatementDataReaderView_impl : public virtual StatementDataReaderView,
                                            public ::DDS::DataReaderView_impl
        {
        public:
            virtual ::DDS::ReturnCode_t read(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_w_condition(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_w_condition(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_sample(
                Statement & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_sample(
                Statement & received_data,
                ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_instance(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_instance(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::SampleStateMask sample_states,
                ::DDS::ViewStateMask view_states,
                ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t read_next_instance_w_condition(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t take_next_instance_w_condition(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq,
                ::DDS::Long max_samples,
                ::DDS::InstanceHandle_t a_handle,
                ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t return_loan(
                StatementSeq & received_data,
                ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::ReturnCode_t get_key_value(
                Statement & key_holder,
                ::DDS::InstanceHandle_t handle) THROW_ORB_EXCEPTIONS;
        
            virtual ::DDS::InstanceHandle_t lookup_instance(
                const Statement & instance) THROW_ORB_EXCEPTIONS;
        
            StatementDataReaderView_impl (
                gapi_dataReader handle
            );
        
            virtual ~StatementDataReaderView_impl(void);
        
        private:
            StatementDataReaderView_impl(const StatementDataReaderView &);
            void operator= (const StatementDataReaderView &);
        };
        
    }

    namespace Sys {

        namespace Log {

        }

    }

}

#endif
