#pragma once

#include "AMM/DDS/AMMPubSubTypes.h"

#include <mutex>
#include <thread>

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <map>

namespace AMM {
    class DataTypes {
    public:
        // AMM Simulation types
        static AMM::Simulation::TickPubSubType *getTickType();

        // AMM Physiology types
        static AMM::Physiology::NodePubSubType *getNodeType();

        static AMM::Physiology::HighFrequencyNodePubSubType *
        getHighFrequencyNodeType();

        static AMM::Physiology::CommandPubSubType *getPhysiologyCommandType();

        // AMM Patient Action / Intervention types
        static AMM::PatientAction::BioGears::CommandPubSubType *getCommandType();

        // AMM Performance types
        static AMM::Performance::StatementPubSubType *getxAPIStatementType();

        // AMM Resource Requirements types
        static AMM::Resource::Requirement::AirPubSubType *getAirRequirementType();

        static AMM::Resource::Requirement::BloodPubSubType *getBloodRequirementType();

        static AMM::Resource::Requirement::Cleaning_SolutionPubSubType *
        getCleaningSolutionRequirementType();

        static AMM::Resource::Requirement::Clear_LiquidPubSubType *
        getClearLiquidRequirementType();

        static AMM::Resource::Requirement::PowerPubSubType *getPowerRequirementType();

        // AMM Resource Supply types
        static AMM::Resource::Supply::AirPubSubType *getAirSupplyType();

        static AMM::Resource::Supply::BloodPubSubType *getBloodSupplyType();

        static AMM::Resource::Supply::Cleaning_SolutionPubSubType *
        getCleaningSolutionSupplyType();

        static AMM::Resource::Supply::Clear_LiquidPubSubType *
        getClearLiquidSupplyType();

        static AMM::Resource::Supply::PowerPubSubType *getPowerSupplyType();

        // AMM Capability types
        static AMM::Capability::ConfigurationPubSubType *getConfigurationType();

        static AMM::Capability::StatusPubSubType *getStatusType();

        static AMM::Capability::ScenarioPubSubType *getScenarioType();

        // AMM Instrument Data type
        static AMM::InstrumentDataPubSubType *getInstrumentDataType();

        static AMM::Performance::AssessmentPubSubType *
        getPerformanceAssessmentDataType();

        static AMM::Render::ModificationPubSubType *getRenderModificationType();

        static AMM::Physiology::ModificationPubSubType *
        getPhysiologyModificationType();

        // Topic names
        static std::string tickTopic;

        static std::string nodeTopic;
        static std::string highFrequencyNodeTopic;

        static std::string commandTopic;
        static std::string physiologyCommandTopic;
        static std::string performanceTopic;

        static std::string airRequirementTopic;
        static std::string bloodRequirementTopic;
        static std::string cleaningSolutionRequirementTopic;
        static std::string clearLiquidRequirementTopic;
        static std::string powerRequirementTopic;

        static std::string airSupplyTopic;
        static std::string bloodSupplyTopic;
        static std::string cleaningSolutionSupplyTopic;
        static std::string clearLiquidSupplyTopic;
        static std::string powerSupplyTopic;

        static std::string statusTopic;
        static std::string configurationTopic;
        static std::string scenarioTopic;

        static std::string renderModTopic;
        static std::string physModTopic;

        static std::string instrumentDataTopic;
    };
};
