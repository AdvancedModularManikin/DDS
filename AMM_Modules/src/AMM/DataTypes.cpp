
#include "DataTypes.h"

std::string AMM::DataTypes::tickTopic = "Tick";

std::string AMM::DataTypes::nodeTopic = "Data";
std::string AMM::DataTypes::highFrequencyNodeTopic = "HighFrequency_Data";

std::string AMM::DataTypes::commandTopic =
        "AMM::PatientAction::BioGears::Command";

std::string AMM::DataTypes::physiologyCommandTopic = "AMM::Physiology::Command";

std::string AMM::DataTypes::performanceTopic = "Performance";

std::string AMM::DataTypes::airRequirementTopic = "Air_Requirement";
std::string AMM::DataTypes::bloodRequirementTopic = "Blood_Requirement";
std::string AMM::DataTypes::cleaningSolutionRequirementTopic =
        "CleaningSolution_Requirement";
std::string AMM::DataTypes::clearLiquidRequirementTopic =
        "ClearLiquid_Requirement";
std::string AMM::DataTypes::powerRequirementTopic = "Power_Requirement";

std::string AMM::DataTypes::airSupplyTopic = "Air_Supply";
std::string AMM::DataTypes::bloodSupplyTopic = "Blood_Supply";
std::string AMM::DataTypes::cleaningSolutionSupplyTopic =
        "CleaningSolution_Supply";
std::string AMM::DataTypes::clearLiquidSupplyTopic = "ClearLiquid_Supply";
std::string AMM::DataTypes::powerSupplyTopic = "Power_Supply";

std::string AMM::DataTypes::statusTopic = "Status";
std::string AMM::DataTypes::configurationTopic = "Configuration";
std::string AMM::DataTypes::scenarioTopic = "Scenario";

std::string AMM::DataTypes::instrumentDataTopic = "InstrumentData";

std::string AMM::DataTypes::renderModTopic = "RenderModification";
std::string AMM::DataTypes::physModTopic = "PhysiologyModification";

std::string AMM::DataTypes::logRecordTopic = "LogRecord";

AMM::Simulation::TickPubSubType *AMM::DataTypes::getTickType() {
    return new AMM::Simulation::TickPubSubType();
}

AMM::Physiology::NodePubSubType *AMM::DataTypes::getNodeType() {
    return new AMM::Physiology::NodePubSubType();
}

AMM::Physiology::HighFrequencyNodePubSubType *
AMM::DataTypes::getHighFrequencyNodeType() {
    return new AMM::Physiology::HighFrequencyNodePubSubType();
};

AMM::Physiology::CommandPubSubType *AMM::DataTypes::getPhysiologyCommandType() {
    return new AMM::Physiology::CommandPubSubType();
};

// AMM Patient Action / Intervention types
AMM::PatientAction::BioGears::CommandPubSubType *
AMM::DataTypes::getCommandType() {
    return new AMM::PatientAction::BioGears::CommandPubSubType();
}

// AMM Performance types
AMM::Performance::StatementPubSubType *AMM::DataTypes::getxAPIStatementType() {
    return new AMM::Performance::StatementPubSubType();
};

// AMM Resource Requirements types
AMM::Resource::Requirement::AirPubSubType *
AMM::DataTypes::getAirRequirementType() {
    return new AMM::Resource::Requirement::AirPubSubType();
};

AMM::Resource::Requirement::BloodPubSubType *
AMM::DataTypes::getBloodRequirementType() {
    return new AMM::Resource::Requirement::BloodPubSubType();
};

AMM::Resource::Requirement::Cleaning_SolutionPubSubType *
AMM::DataTypes::getCleaningSolutionRequirementType() {
    return new AMM::Resource::Requirement::Cleaning_SolutionPubSubType();
};

AMM::Resource::Requirement::Clear_LiquidPubSubType *
AMM::DataTypes::getClearLiquidRequirementType() {
    return new AMM::Resource::Requirement::Clear_LiquidPubSubType();
};

AMM::Resource::Requirement::PowerPubSubType *
AMM::DataTypes::getPowerRequirementType() {
    return new AMM::Resource::Requirement::PowerPubSubType();
};

// AMM Resource Supply types
AMM::Resource::Supply::AirPubSubType *AMM::DataTypes::getAirSupplyType() {
    return new AMM::Resource::Supply::AirPubSubType();
};

AMM::Resource::Supply::BloodPubSubType *AMM::DataTypes::getBloodSupplyType() {
    return new AMM::Resource::Supply::BloodPubSubType();
};

AMM::Resource::Supply::Cleaning_SolutionPubSubType *
AMM::DataTypes::getCleaningSolutionSupplyType() {
    return new AMM::Resource::Supply::Cleaning_SolutionPubSubType();
};

AMM::Resource::Supply::Clear_LiquidPubSubType *
AMM::DataTypes::getClearLiquidSupplyType() {
    return new AMM::Resource::Supply::Clear_LiquidPubSubType();
};

AMM::Resource::Supply::PowerPubSubType *AMM::DataTypes::getPowerSupplyType() {
    return new AMM::Resource::Supply::PowerPubSubType();
};

// AMM Capability types
AMM::Capability::ConfigurationPubSubType *
AMM::DataTypes::getConfigurationType() {
    return new AMM::Capability::ConfigurationPubSubType();
};

AMM::Capability::StatusPubSubType *AMM::DataTypes::getStatusType() {
    return new AMM::Capability::StatusPubSubType();
};

AMM::Capability::ScenarioPubSubType *AMM::DataTypes::getScenarioType() {
    return new AMM::Capability::ScenarioPubSubType();
};

AMM::Performance::AssessmentPubSubType *
AMM::DataTypes::getPerformanceAssessmentDataType() {
    return new AMM::Performance::AssessmentPubSubType();
}

AMM::InstrumentDataPubSubType *AMM::DataTypes::getInstrumentDataType() {
    return new AMM::InstrumentDataPubSubType();
};

AMM::Render::ModificationPubSubType *
AMM::DataTypes::getRenderModificationType() {
    return new AMM::Render::ModificationPubSubType();
};

AMM::Physiology::ModificationPubSubType *
AMM::DataTypes::getPhysiologyModificationType() {
    return new AMM::Physiology::ModificationPubSubType();
};

AMM::Diagnostics::Log::RecordPubSubType *AMM::DataTypes::getLogRecordType() {
    return new AMM::Diagnostics::Log::RecordPubSubType();
};