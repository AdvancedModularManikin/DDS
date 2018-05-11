#pragma once

#include "AMMPubSubTypes.h"

#include <mutex>
#include <thread>

#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include <map>

using namespace AMM::Simulation;
using namespace AMM::Physiology;
using namespace AMM::PatientAction::BioGears;
using namespace AMM::Performance;
using namespace AMM::Resource::Requirement;
using namespace AMM::Resource::Supply;
using namespace AMM::Capability;
using namespace AMM::Sys::Log;

namespace AMM {
    class DataTypes {
    public:
        // AMM Simulation types
        static AMM::Simulation::TickPubSubType* getTickType();

        // AMM Physiology types
        static AMM::Physiology::NodePubSubType* getNodeType();
        static AMM::Physiology::HighFrequencyNodePubSubType* getHighFrequencyNodeType();

        // AMM Patient Action / Intervention types
        static AMM::PatientAction::BioGears::CommandPubSubType* getCommandType();

        // AMM Performance types
        static AMM::Performance::StatementPubSubType* getxAPIStatementType();

        // AMM Resource Requirements types
        static AMM::Resource::Requirement::AirPubSubType* getAirRequirementType();
        static AMM::Resource::Requirement::BloodPubSubType* getBloodRequirementType();
        static AMM::Resource::Requirement::Cleaning_SolutionPubSubType* getCleaningSolutionRequirementType();
        static AMM::Resource::Requirement::Clear_LiquidPubSubType* getClearLiquidRequirementType();
        static AMM::Resource::Requirement::PowerPubSubType* getPowerRequirementType();

        // AMM Resource Supply types
        static AMM::Resource::Supply::AirPubSubType* getAirSupplyType();
        static AMM::Resource::Supply::BloodPubSubType* getBloodSupplyType();
        static AMM::Resource::Supply::Cleaning_SolutionPubSubType* getCleaningSolutionSupplyType();
        static AMM::Resource::Supply::Clear_LiquidPubSubType* getClearLiquidSupplyType();
        static AMM::Resource::Supply::PowerPubSubType* getPowerSupplyType();

        // AMM Capability types
        static AMM::Capability::ConfigurationPubSubType* getConfigurationType();
        static AMM::Capability::StatusPubSubType* getStatusType();
        static AMM::Capability::ScenarioPubSubType* getScenarioType();

        // AMM Logging types
        static AMM::Sys::Log::debugPubSubType* getDebugLogType();
        static AMM::Sys::Log::errorPubSubType* getErrorLogType();
        static AMM::Sys::Log::warningPubSubType* getWarningLogType();
        static AMM::Sys::Log::infoPubSubType* getInfoLogType();

        // Topic names
        static std::string tickTopic;

        static std::string nodeTopic;
        static std::string highFrequencyNodeTopic;

        static std::string commandTopic;

        static std::string performanceTopic;

        static std::string airRequirementTopic;
        static std::string bloodRequirementTopic;
        static std::string cleaningSolutionRequirementTopic;
        static std::string clearLiquidRequirementTopic;
        static std::string powerRequirementTopic;

        static std::string airSupplyTopic;
        static std::string bloodSupplyTopic;
        static std::string cleaningSolutionSupplytTopic;
        static std::string clearLiquidSupplyTopic;
        static std::string powerSupplyTopic;

        static std::string statusTopic;
        static std::string configurationTopic;
        static std::string scenarioTopic;

    };
};
