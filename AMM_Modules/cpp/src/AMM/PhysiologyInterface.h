#pragma once

// BioGears core
#include "bind/biogears-cdm.hxx"
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

class PhysiologyInterface: public PhysiologyEngine {

public:
	PhysiologyInterface();

	virtual ~PhysiologyInterface();

	bool SaveState(std::string stateFile);

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

};

std::unique_ptr<PhysiologyInterface> CreatePhysiologyEngine(const std::string& logfile);

