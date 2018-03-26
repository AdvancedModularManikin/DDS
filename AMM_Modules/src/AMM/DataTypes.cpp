
#include "DataTypes.h"

std::string AMM::DataTypes::tickTopic = "Tick";

std::string AMM::DataTypes::nodeTopic = "Data";
std::string AMM::DataTypes::highFrequencyNodeTopic = "HighFrequency_Data";

std::string AMM::DataTypes::commandTopic = "Command";

std::string AMM::DataTypes::performanceTopic = "Performance";

std::string AMM::DataTypes::airRequirementTopic = "Air_Requirement";
std::string AMM::DataTypes::bloodRequirementTopic = "Blood_Requirement";
std::string AMM::DataTypes::cleaningSolutionRequirementTopic = "CleaningSolution_Requirement";
std::string AMM::DataTypes::clearLiquidRequirementTopic = "ClearLiquid_Requirement";
std::string AMM::DataTypes::powerRequirementTopic = "Power_Requirement";

std::string AMM::DataTypes::airSupplyTopic = "Air_Supply";
std::string AMM::DataTypes::bloodSupplyTopic = "Blood_Supply";
std::string AMM::DataTypes::cleaningSolutionSupplytTopic = "CleaningSolution_Supply";
std::string AMM::DataTypes::clearLiquidSupplyTopic = "ClearLiquid_Supply";
std::string AMM::DataTypes::powerSupplyTopic = "Power_Supply";

AMM::Simulation::TickPubSubType* AMM::DataTypes::getTickType() {
    return new TickPubSubType();
}

AMM::Physiology::NodePubSubType* AMM::DataTypes::getNodeType() {
    return new NodePubSubType();
}

AMM::Physiology::HighFrequencyNodePubSubType* AMM::DataTypes::getHighFrequencyNodeType() {

};


// AMM Patient Action / Intervention types
CommandPubSubType* AMM::DataTypes::getCommandType() {
    return new CommandPubSubType();
}





// AMM Performance types
AMM::Performance::StatementPubSubType* AMM::DataTypes::getxAPIStatementType() {};

// AMM Resource Requirements types
AMM::Resource::Requirement::AirPubSubType* AMM::DataTypes::getAirRequirementType() {};
AMM::Resource::Requirement::BloodPubSubType AMM::DataTypes::getBloodRequirementType() {};
AMM::Resource::Requirement::Cleaning_SolutionPubSubType* AMM::DataTypes::getCleaningSolutionRequirementType() {};
AMM::Resource::Requirement::Clear_LiquidPubSubType* AMM::DataTypes::getClearLiquidRequirementType() {};
AMM::Resource::Requirement::PowerPubSubType* AMM::DataTypes::getPowerRequirementType() {};

// AMM Resource Supply types
AMM::Resource::Supply::AirPubSubType* AMM::DataTypes::getAirSupplyType() { };
AMM::Resource::Supply::BloodPubSubType AMM::DataTypes::getBloodSupplyType() {};
AMM::Resource::Supply::Cleaning_SolutionPubSubType* AMM::DataTypes::getCleaningSolutionSupplyType() {};
AMM::Resource::Supply::Clear_LiquidPubSubType* AMM::DataTypes::getClearLiquidSupplyType() {};
AMM::Resource::Supply::PowerPubSubType* AMM::DataTypes::getPowerSupplyType() {};

// AMM Capability types
AMM::Capability::ConfigurationPubSubType* AMM::DataTypes::getConfigurationType() {};
AMM::Capability::StatusPubSubType* AMM::DataTypes::getStatusType() {};

// AMM Logging types
AMM::Sys::Log::debugPubSubType* AMM::DataTypes::getDebugLogType() {};
AMM::Sys::Log::errorPubSubType* AMM::DataTypes::getErrorLogType() {};
AMM::Sys::Log::warningPubSubType* AMM::DataTypes::getWarningLogType() {};
AMM::Sys::Log::infoPubSubType* AMM::DataTypes::getInfoLogType() {};