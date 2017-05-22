#pragma once

#include <mutex>
#include <thread>

#include "CommonDataModel.h"
#include "BioGearsPhysiologyEngine.h"

#include "patient/actions/SEHemorrhage.h"
#include "patient/actions/SESubstanceCompoundInfusion.h"
#include "substance/SESubstanceManager.h"
#include "substance/SESubstanceCompound.h"

#include "system/physiology/SEBloodChemistrySystem.h"
#include "system/physiology/SECardiovascularSystem.h"
#include "system/physiology/SERespiratorySystem.h"
#include "system/equipment/ElectroCardioGram/SEElectroCardioGram.h"

#include "properties/SEScalarFraction.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarMassPerVolume.h"
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
#include "properties/SEScalarElectricPotential.h"

// Forward declare what we will use in our thread
class SESubstance;
class SEEnergySystem;
class SEComprehensiveMetabolicPanel;
class SEGasCompartment;
class SECompleteBloodCount;
class PhysiologyEngine;

class BioGearsThread {
public:
	BioGearsThread(const std::string& logfile);
	virtual ~BioGearsThread();

	void StartSimulation();
	void StopSimulation();
	void Shutdown();
	void AdvanceTimeTick();

	double GetSimulationTime();
	double GetNodePath(const std::string& nodePath);

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

	void Status();

	Logger* GetLogger() {
		return m_bg->GetLogger();
	}

protected:
	void AdvanceTime();

	std::thread m_thread;
	std::mutex m_mutex;
	bool m_runThread;

	std::unique_ptr<PhysiologyEngine> m_bg;
};
