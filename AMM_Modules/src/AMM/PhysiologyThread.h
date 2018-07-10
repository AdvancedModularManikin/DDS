#pragma once

#include <mutex>
#include <thread>
#include <ctime>
#include <stdexcept>
#include <sstream>

// Boost dependencies
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

// BioGears core
#include "BioGearsPhysiologyEngine.h"
#include "engine/PhysiologyEngineTrack.h"
#include "scenario/requests/SEDataRequest.h"
#include "properties/SEScalarTime.h"

// Units of measurement
#include "patient/SEPatient.h"
#include "patient/assessments/SEPulmonaryFunctionTest.h"
#include "compartment/SECompartmentManager.h"
#include "compartment/fluid/SEGasCompartment.h"
#include "compartment/fluid/SELiquidCompartment.h"
#include "compartment/SECompartmentManager.h"
#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SEEnergySystem.h"
#include "system/physiology/SERespiratorySystem.h"
#include "substance/SESubstanceManager.h"
#include "substance/SESubstance.h"
#include "engine/PhysiologyEngineTrack.h"
#include "utils/SEEventHandler.h"

#include "properties/SEScalarFraction.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarMassPerVolume.h"
#include "properties/SEScalarMassPerAmount.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarTemperature.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEFunctionVolumeVsTime.h"
#include "properties/SEScalarMass.h"
#include "properties/SEScalarLength.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarAmountPerVolume.h"
#include "properties/SEScalarAmountPerMass.h"

#include "system/environment/conditions/SEInitialEnvironment.h"

#include "scenario/SEScenario.h"
#include "scenario/SEScenarioInitialParameters.h"
#include "scenario/SEScenarioExec.h"

#include "scenario/SEAdvanceTime.h"

#include "compartment/SECompartmentManager.h"

#include "cdm/circuit/SECircuitPath.h"
#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SEEnergySystem.h"
#include "system/physiology/SERespiratorySystem.h"
#include "system/equipment/ElectroCardioGram/SEElectroCardioGram.h"
#include "patient/assessments/SEPulmonaryFunctionTest.h"
#include "patient/assessments/SECompleteBloodCount.h"
#include "patient/assessments/SEPatientAssessment.h"
#include "patient/assessments/SEComprehensiveMetabolicPanel.h"
#include "substance/SESubstanceManager.h"
#include "substance/SESubstance.h"

#include "patient/actions/SESubstanceBolus.h"
#include "system/equipment/Anesthesia/SEAnesthesiaMachine.h"
#include "system/equipment/Anesthesia/SEAnesthesiaMachineOxygenBottle.h"
#include "system/equipment/Anesthesia/actions/SEAnesthesiaMachineConfiguration.h"
#include "system/equipment/Anesthesia/actions/SEMaskLeak.h"
#include "system/equipment/Anesthesia/actions/SEOxygenWallPortPressureLoss.h"
#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SERespiratorySystem.h"

#include "AMMPubSubTypes.h"

#include "AMM/BaseLogger.h"

// Forward declare what we will use in our thread
class SESubstance;

class SEEnergySystem;

class SEComprehensiveMetabolicPanel;

class SEGasCompartment;

class SECompleteBloodCount;

class SEAnesthesiaMachineConfiguration;

class SEAnesthesiaMachine;

class PhysiologyEngine;

namespace AMM {
    class PhysiologyThread {

    public:
        PhysiologyThread(const std::string &logFile);

        virtual ~PhysiologyThread();

        bool LoadState(const std::string &stateFile, double sec);

        bool SaveState(const std::string &stateFile);

        bool SaveState();

        bool ExecuteCommand(const std::string &cmd);

        void Shutdown();

        void StartSimulation();

        void StopSimulation();

        void AdvanceTimeTick();

        double GetSimulationTime();

        std::map<std::string, double (PhysiologyThread::*)()> *GetNodePathTable();

        double GetNodePath(const std::string &nodePath);

        void Ventilator(const std::string &ventilatorSettings);

        void Status();

        static std::map<std::string, double (PhysiologyThread::*)()> nodePathTable;
        static std::vector<std::string> highFrequencyNodes;

        std::string getTimestampedFilename(const std::string &basePathname);

    private:
        bool LoadScenarioFile(const std::string &scenarioFile);

        void PopulateNodePathTable();

        void PreloadSubstances();

        void PreloadCompartments();

        double GetShutdownMessage();

        double GetHeartRate();

        double GetBloodVolume();

        double GetArterialSystolicPressure();

        double GetArterialDiastolicPressure();

        double GetMeanArterialPressure();

        double GetArterialPressure();

        double GetMeanCentralVenousPressure();

        double GetEndTidalCarbonDioxideFraction();

        double GetOxygenSaturation();

        double GetRespirationRate();

        double GetCoreTemperature();

        double GetECGWaveform();

        double GetSodiumConcentration();

        double GetSodium();

        double GetGlucoseConcentration();

        double GetBUN();

        double GetCreatinineConcentration();

        double GetWhiteBloodCellCount();

        double GetRedBloodCellCount();

        double GetHemoglobinConcentration();

        double GetHematocrit();

        double GetBloodPH();

        double GetArterialCarbonDioxidePressure();

        double GetArterialOxygenPressure();

        double GetBicarbonateConcentration();

        double GetBicarbonate();

        double GetBaseExcess();

        double GetCO2();

        double GetPotassium();

        double GetChloride();

        double GetPlateletCount();

        double GetExhaledCO2();

        double GetTidalVolume();

        double GetTotalLungVolume();

        double GetLeftLungVolume();

        double GetRightLungVolume();

        double GetLeftPleuralCavityVolume();

        double GetRightPleuralCavityVolume();

        double GetLeftAlveoliBaselineCompliance();

        double GetRightAlveoliBaselineCompliance();

        double GetCardiacOutput();

        SESubstance *sodium;
        SESubstance *glucose;
        SESubstance *creatinine;
        SESubstance *hemoglobin;
        SESubstance *bicarbonate;
        SESubstance *CO2;
        SESubstance *N2;
        SESubstance *O2;
        SESubstance *CO;

        const SEGasCompartment *carina;
        const SEGasCompartment *leftLung;
        const SEGasCompartment *rightLung;

        Logger *GetLogger() {
            return m_pe->GetLogger();
        }


    protected:
        void AdvanceTime();

        std::thread m_thread;
        std::mutex m_mutex;
        bool m_runThread;
        std::unique_ptr<PhysiologyEngine> m_pe;
    };
}