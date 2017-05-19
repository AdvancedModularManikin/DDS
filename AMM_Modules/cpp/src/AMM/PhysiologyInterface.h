#pragma once

// BioGears core

#include "CommonDataModel.h"
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

#include "SRC/Controller/BioGears.h"

class PhysiologyInterface : public PhysiologyEngine, public BioGears
{
public:

  PhysiologyInterface(Logger* logger);
  PhysiologyInterface(const std::string&);
  virtual ~PhysiologyInterface();

  virtual bool LoadState(const std::string& file, const SEScalarTime* simTime = nullptr);
  virtual bool LoadState(const CDM::PhysiologyEngineStateData& state, const SEScalarTime* simTime = nullptr);
  virtual std::unique_ptr<CDM::PhysiologyEngineStateData> SaveState(const std::string& file = "");

  virtual Logger* GetLogger();
  virtual PhysiologyEngineTrack* GetEngineTrack();

  virtual bool InitializeEngine(const std::string& patientFile, const std::vector<const SECondition*>* conditions = nullptr, const PhysiologyEngineConfiguration* config = nullptr);
  virtual bool InitializeEngine(const SEPatient& patient, const std::vector<const SECondition*>* conditions = nullptr, const PhysiologyEngineConfiguration* config = nullptr);

  virtual const PhysiologyEngineConfiguration* GetConfiguration();

  virtual double GetTimeStep(const TimeUnit& unit);
  virtual double GetSimulationTime(const TimeUnit& unit);

  virtual void AdvanceModelTime();
  virtual void AdvanceModelTime(double time, const TimeUnit& unit);
  virtual bool ProcessAction(const SEAction& action);

  virtual SESubstanceManager& GetSubstanceManager();
  virtual void SetEventHandler(SEEventHandler* handler);
  virtual const SEPatient& GetPatient();
  virtual bool GetPatientAssessment(SEPatientAssessment& assessment);

  virtual const SEEnvironment*                         GetEnvironment();
  virtual const SEBloodChemistrySystem*                GetBloodChemistrySystem();
  virtual const SECardiovascularSystem*                GetCardiovascularSystem();
  virtual const SEDrugSystem*                          GetDrugSystem();
  virtual const SEEndocrineSystem*                     GetEndocrineSystem();
  virtual const SEEnergySystem*                        GetEnergySystem();
  virtual const SEGastrointestinalSystem*              GetGastrointestinalSystem();
  virtual const SEHepaticSystem*                       GetHepaticSystem();
  virtual const SENervousSystem*                       GetNervousSystem();
  virtual const SERenalSystem*                         GetRenalSystem();
  virtual const SERespiratorySystem*                   GetRespiratorySystem();
  virtual const SETissueSystem*                        GetTissueSystem();
  virtual const SEAnesthesiaMachine*                   GetAnesthesiaMachine();
  virtual const SEElectroCardioGram*                   GetElectroCardioGram();
  virtual const SEInhaler*                             GetInhaler();

  virtual const SECompartmentManager&                  GetCompartments();


	bool LoadScenarioFile(std::string scenFile);

	void Status();

	// Advance time one tick
	void AdvanceTimeTick();

	double GetHeartRate();

	// ?? - Blood Volume - mL
	double GetBloodVolume();

	// SYS (ART) - Arterial Systolic Pressure - mmHg
	double GetArterialSystolicPressure();

	// DIA (ART) - Arterial Diastolic Pressure - mmHg
	double GetArterialDiastolicPressure();

	// MAP (ART) - Mean Arterial Pressure - mmHg
	double GetMeanArterialPressure();

	// AP - Arterial Pressure - mmHg
	double GetArterialPressure();

	// CVP - Central Venous Pressure - mmHg
	double GetMeanCentralVenousPressure();

	// MCO2 - End Tidal Carbon Dioxide Fraction - unitless %
	double GetEndTidalCarbonDioxideFraction();

	// SPO2 - Oxygen Saturation - unitless %
	double GetOxygenSaturation();

	// BR - Respiration Rate - per minute
	double GetRespirationRate();

	// T2 - Core Temperature - degrees C
	double GetCoreTemperature();

	// ECG Waveform in mV
	double GetECGWaveform();

	// Na+ - Sodium Concentration - mg/dL
	double GetSodiumConcentration();

	// Na+ - Sodium - mmol/L
	double GetSodium();

	// Glucose - Glucose Concentration - mg/dL
	double GetGlucoseConcentration();

	// BUN - BloodUreaNitrogenConcentration - mg/dL
	double GetBUN();

	// Creatinine - Creatinine Concentration - mg/dL
	double GetCreatinineConcentration();

	// RBC - White Blood Cell Count - ct/uL
	double GetWhiteBloodCellCount();

	// RBC - Red Blood Cell Count - ct/uL
	double GetRedBloodCellCount();

	// Hgb - Hemoglobin Concentration - g/dL
	double GetHemoglobinConcentration();

	// Hct - Hematocrit - unitless
	double GetHematocrit();

	// pH - Blood pH - unitless
	double GetBloodPH();

	// PaCO2 - Arterial Carbon Dioxide Pressure - mmHg
	double GetArterialCarbonDioxidePressure();

	// Pa02 - Arterial Oxygen Pressure - mmHg
	double GetArterialOxygenPressure();

	// n/a - Bicarbonate Concentration - mg/L
	double GetBicarbonateConcentration();

	// HCO3 - Bicarbonate - Convert to mmol/L
	double GetBicarbonate();

	// BE - Base Excess -
	double GetBaseExcess();

	double GetCO2();

	// Metabolic panel
	bool UpdateMetabolicPanel();

	double GetPotassium();

	double GetChloride();

	// CBC
	bool UpdateCompleteBloodCount();
	// PLT - Platelet Count - ct/uL
	double GetPlateletCount();
	// GetExhaledCO2 - tracheal CO2 partial pressure - mmHg
	double GetExhaledCO2();

	// Get Tidal Volume - mL
	double GetTidalVolume();

	// Get Total Lung Volume - mL
	double GetTotalLungVolume();

	// Get Left Lung Volume - mL
	double GetLeftLungVolume();

	// Get Right Lung Volume - mL
	double GetRightLungVolume();

	// Get Left Lung Pleural Cavity Volume - mL
	double GetLeftPleuralCavityVolume();

	// Get Right Lung Pleural Cavity Volume - mL
	double GetRightPleuralCavityVolume();

	// Get left alveoli baseline compliance (?) volume
	double GetLeftAlveoliBaselineCompliance();

	// Get right alveoli baseline compliance (?) volume
	double GetRightAlveoliBaselineCompliance();


protected:

  virtual bool IsReady();
  virtual bool InitializeEngine(const std::vector<const SECondition*>* conditions = nullptr, const PhysiologyEngineConfiguration* config = nullptr);

  SEEventHandler*                                 m_EventHandler;
  PhysiologyEngineTrack                           m_EngineTrack;
  std::stringstream                               m_ss;
};


