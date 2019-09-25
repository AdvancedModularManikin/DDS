#pragma once

#include "AMM/DDS/AMMPubSubTypes.h"

namespace AMM {
    class DataTypes {
    public:
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

        static std::string logRecordTopic;

        static std::string renderModTopic;
        static std::string physModTopic;

        static std::string instrumentDataTopic;
    };
};
