
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

std::string AMM::DataTypes::statusTopic = "Status";
std::string AMM::DataTypes::configurationTopic = "Configuration";


AMM::Simulation::TickPubSubType *AMM::DataTypes::getTickType() {
    return new TickPubSubType();
}

AMM::Physiology::NodePubSubType *AMM::DataTypes::getNodeType() {
    return new NodePubSubType();
}

AMM::Physiology::HighFrequencyNodePubSubType *AMM::DataTypes::getHighFrequencyNodeType() {
    return new HighFrequencyNodePubSubType();
};


// AMM Patient Action / Intervention types
CommandPubSubType *AMM::DataTypes::getCommandType() {
    return new CommandPubSubType();
}


// AMM Performance types
AMM::Performance::StatementPubSubType *AMM::DataTypes::getxAPIStatementType() {
    return new StatementPubSubType();
};

// AMM Resource Requirements types
AMM::Resource::Requirement::AirPubSubType *AMM::DataTypes::getAirRequirementType() {
    return new AMM::Resource::Requirement::AirPubSubType();
};

AMM::Resource::Requirement::BloodPubSubType *AMM::DataTypes::getBloodRequirementType() {
    return new AMM::Resource::Requirement::BloodPubSubType();
};

AMM::Resource::Requirement::Cleaning_SolutionPubSubType *AMM::DataTypes::getCleaningSolutionRequirementType() {
    return new AMM::Resource::Requirement::Cleaning_SolutionPubSubType();
};

AMM::Resource::Requirement::Clear_LiquidPubSubType *AMM::DataTypes::getClearLiquidRequirementType() {
    return new AMM::Resource::Requirement::Clear_LiquidPubSubType();
};

AMM::Resource::Requirement::PowerPubSubType *AMM::DataTypes::getPowerRequirementType() {
    return new AMM::Resource::Requirement::PowerPubSubType();
};

// AMM Resource Supply types
AMM::Resource::Supply::AirPubSubType *AMM::DataTypes::getAirSupplyType() {
    return new AMM::Resource::Supply::AirPubSubType();
};

AMM::Resource::Supply::BloodPubSubType *AMM::DataTypes::getBloodSupplyType() {
    return new AMM::Resource::Supply::BloodPubSubType();
};

AMM::Resource::Supply::Cleaning_SolutionPubSubType *AMM::DataTypes::getCleaningSolutionSupplyType() {
    return new AMM::Resource::Supply::Cleaning_SolutionPubSubType();
};

AMM::Resource::Supply::Clear_LiquidPubSubType *AMM::DataTypes::getClearLiquidSupplyType() {
    return new AMM::Resource::Supply::Clear_LiquidPubSubType();
};

AMM::Resource::Supply::PowerPubSubType *AMM::DataTypes::getPowerSupplyType() {
    return new AMM::Resource::Supply::PowerPubSubType();
};

// AMM Capability types
AMM::Capability::ConfigurationPubSubType *AMM::DataTypes::getConfigurationType() {
    return new ConfigurationPubSubType();
};

AMM::Capability::StatusPubSubType *AMM::DataTypes::getStatusType() {
    return new StatusPubSubType();
};

// AMM Logging types
AMM::Sys::Log::debugPubSubType *AMM::DataTypes::getDebugLogType() {
    return new AMM::Sys::Log::debugPubSubType();
};

AMM::Sys::Log::errorPubSubType *AMM::DataTypes::getErrorLogType() {
    return new AMM::Sys::Log::errorPubSubType();
};

AMM::Sys::Log::warningPubSubType *AMM::DataTypes::getWarningLogType() {
    return new AMM::Sys::Log::warningPubSubType();
};

AMM::Sys::Log::infoPubSubType *AMM::DataTypes::getInfoLogType() {
    return new AMM::Sys::Log::infoPubSubType();
};
