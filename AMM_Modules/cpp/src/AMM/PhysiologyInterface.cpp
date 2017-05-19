#include "PhysiologyInterface.h"



bool PhysiologyInterface::SaveState(std::string stateFile) {
	this->SaveState(stateFile);
	return true;
}

// Load a scenario from an XML file, apply conditions and iterate through the actions
// This bypasses the standard BioGears ExecuteScenario method to avoid resetting the BioGears engine
bool PhysiologyInterface::LoadScenarioFile(std::string scenFile) {

	SEScenario sce(this->GetSubstanceManager());
	sce.LoadFile(scenFile);

	double dT_s = this->GetTimeStep(TimeUnit::s);
	double scenarioTime_s;

	double sampleTime_s = sce.GetDataRequestManager().GetSamplesPerSecond();
	if (sampleTime_s != 0)
		sampleTime_s = 1 / sampleTime_s;
	double currentSampleTime_s = sampleTime_s;		//Sample the first step

	SEAdvanceTime* adv;
	for (SEAction* a : sce.GetActions()) {
		adv = dynamic_cast<SEAdvanceTime*>(a);
		if (adv != nullptr) {
			double time_s = adv->GetTime(TimeUnit::s);
			int count = (int) (time_s / dT_s);
			for (int i = 0; i <= count; i++) {

				this->AdvanceModelTime();
				scenarioTime_s = this->GetSimulationTime(TimeUnit::s);
				currentSampleTime_s += dT_s;
				if (currentSampleTime_s >= sampleTime_s) {
					currentSampleTime_s = 0;
					this->GetEngineTrack()->TrackData(scenarioTime_s);
				}

			}
			continue;
		} else {
			this->ProcessAction(*a);
		}
	}

	return true;
}

// Advance time one tick
void PhysiologyInterface::AdvanceTimeTick() {
	this->AdvanceModelTime();
}

double PhysiologyInterface::GetHeartRate() {
	return this->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min);
}

// ?? - Blood Volume - mL
double PhysiologyInterface::GetBloodVolume() {
	return this->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL);
}

// SYS (ART) - Arterial Systolic Pressure - mmHg
double PhysiologyInterface::GetArterialSystolicPressure() {
	return this->GetCardiovascularSystem()->GetSystolicArterialPressure(
			PressureUnit::mmHg);
}

// DIA (ART) - Arterial Diastolic Pressure - mmHg
double PhysiologyInterface::GetArterialDiastolicPressure() {
	return this->GetCardiovascularSystem()->GetDiastolicArterialPressure(
			PressureUnit::mmHg);
}

// MAP (ART) - Mean Arterial Pressure - mmHg
double PhysiologyInterface::GetMeanArterialPressure() {
	return this->GetCardiovascularSystem()->GetMeanArterialPressure(
			PressureUnit::mmHg);
}

// AP - Arterial Pressure - mmHg
double PhysiologyInterface::GetArterialPressure() {
	return this->GetCardiovascularSystem()->GetArterialPressure(
			PressureUnit::mmHg);
}

// CVP - Central Venous Pressure - mmHg
double PhysiologyInterface::GetMeanCentralVenousPressure() {
	return this->GetCardiovascularSystem()->GetMeanCentralVenousPressure(
			PressureUnit::mmHg);
}

// MCO2 - End Tidal Carbon Dioxide Fraction - unitless %
double PhysiologyInterface::GetEndTidalCarbonDioxideFraction() {
	return (this->GetRespiratorySystem()->GetEndTidalCarbonDioxideFraction() * 7.5);
}

// SPO2 - Oxygen Saturation - unitless %
double PhysiologyInterface::GetOxygenSaturation() {
	return this->GetBloodChemistrySystem()->GetOxygenSaturation();
}

// BR - Respiration Rate - per minute
double PhysiologyInterface::GetRespirationRate() {
	return this->GetRespiratorySystem()->GetRespirationRate(
			FrequencyUnit::Per_min);
}

// T2 - Core Temperature - degrees C
double PhysiologyInterface::GetCoreTemperature() {
	return this->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C);
}

// ECG Waveform in mV
double PhysiologyInterface::GetECGWaveform() {
	double ecgLead3_mV = this->GetElectroCardioGram()->GetLead3ElectricPotential(
			ElectricPotentialUnit::mV);
	return ecgLead3_mV;
}

// Na+ - Sodium Concentration - mg/dL
double PhysiologyInterface::GetSodiumConcentration() {
	SESubstance* sodium = this->GetSubstanceManager().GetSubstance("Sodium");
	return sodium->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// Na+ - Sodium - mmol/L
double PhysiologyInterface::GetSodium() {
	return GetSodiumConcentration() * 0.43;
}

// Glucose - Glucose Concentration - mg/dL
double PhysiologyInterface::GetGlucoseConcentration() {
	SESubstance* glucose = this->GetSubstanceManager().GetSubstance("Glucose");
	return glucose->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// BUN - BloodUreaNitrogenConcentration - mg/dL
double PhysiologyInterface::GetBUN() {
	return this->GetBloodChemistrySystem()->GetBloodUreaNitrogenConcentration(
			MassPerVolumeUnit::mg_Per_dL);
}

// Creatinine - Creatinine Concentration - mg/dL
double PhysiologyInterface::GetCreatinineConcentration() {
	SESubstance* creatinine = this->GetSubstanceManager().GetSubstance(
			"Creatinine");
	return creatinine->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// RBC - White Blood Cell Count - ct/uL
double PhysiologyInterface::GetWhiteBloodCellCount() {
	return this->GetBloodChemistrySystem()->GetWhiteBloodCellCount(
			AmountPerVolumeUnit::ct_Per_uL);
}

// RBC - Red Blood Cell Count - ct/uL
double PhysiologyInterface::GetRedBloodCellCount() {
	return this->GetBloodChemistrySystem()->GetRedBloodCellCount(
			AmountPerVolumeUnit::ct_Per_uL);
}

// Hgb - Hemoglobin Concentration - g/dL
double PhysiologyInterface::GetHemoglobinConcentration() {
	SESubstance* hemoglobin = this->GetSubstanceManager().GetSubstance(
			"Hemoglobin");
	return hemoglobin->GetBloodConcentration(MassPerVolumeUnit::g_Per_dL);
}

// Hct - Hematocrit - unitless
double PhysiologyInterface::GetHematocrit() {
	return this->GetBloodChemistrySystem()->GetHematocrit();
}

// pH - Blood pH - unitless
double PhysiologyInterface::GetBloodPH() {
	return this->GetBloodChemistrySystem()->GetBloodPH();
}

// PaCO2 - Arterial Carbon Dioxide Pressure - mmHg
double PhysiologyInterface::GetArterialCarbonDioxidePressure() {
	return this->GetBloodChemistrySystem()->GetArterialCarbonDioxidePressure(
			PressureUnit::mmHg);
}

// Pa02 - Arterial Oxygen Pressure - mmHg
double PhysiologyInterface::GetArterialOxygenPressure() {
	return this->GetBloodChemistrySystem()->GetArterialOxygenPressure(
			PressureUnit::mmHg);
}

// n/a - Bicarbonate Concentration - mg/L
double PhysiologyInterface::GetBicarbonateConcentration() {
	SESubstance* bicarbonate = this->GetSubstanceManager().GetSubstance(
			"Bicarbonate");
	return bicarbonate->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// HCO3 - Bicarbonate - Convert to mmol/L
double PhysiologyInterface::GetBicarbonate() {
	// SESubstance* bicarbonate = this->GetSubstanceManager().GetSubstance("Bicarbonate");
	return GetBicarbonateConcentration() * 0.1639;
}

// BE - Base Excess -
double PhysiologyInterface::GetBaseExcess() {
	return (0.93 * GetBicarbonate()) + (13.77 * GetBloodPH()) - 124.58;
}

double PhysiologyInterface::GetCO2() {
	SEComprehensiveMetabolicPanel metabolicPanel(this->GetLogger());
	this->GetPatientAssessment(metabolicPanel);
	SEScalarAmountPerVolume CO2 = metabolicPanel.GetCO2();
	return CO2.GetValue(AmountPerVolumeUnit::mmol_Per_L);
}

// Metabolic panel
bool PhysiologyInterface::UpdateMetabolicPanel() {
	SEComprehensiveMetabolicPanel metabolicPanel(this->GetLogger());
	this->GetPatientAssessment(metabolicPanel);
	return true;
}

double PhysiologyInterface::GetPotassium() {
	SEComprehensiveMetabolicPanel metabolicPanel(this->GetLogger());
	this->GetPatientAssessment(metabolicPanel);
	SEScalarAmountPerVolume potassium = metabolicPanel.GetPotassium();
	return potassium.GetValue(AmountPerVolumeUnit::ct_Per_uL);
}

double PhysiologyInterface::GetChloride() {
	SEComprehensiveMetabolicPanel metabolicPanel(this->GetLogger());
	this->GetPatientAssessment(metabolicPanel);
	SEScalarAmountPerVolume chloride = metabolicPanel.GetChloride();
	return chloride.GetValue(AmountPerVolumeUnit::ct_Per_uL);
}

// CBC
bool PhysiologyInterface::UpdateCompleteBloodCount() {
	SECompleteBloodCount CBC(this->GetLogger());
	this->GetPatientAssessment(CBC);
	return true;
}

// PLT - Platelet Count - ct/uL
double PhysiologyInterface::GetPlateletCount() {
	SECompleteBloodCount CBC(this->GetLogger());
	this->GetPatientAssessment(CBC);
	SEScalarAmountPerVolume plateletCount = CBC.GetPlateletCount();
	return plateletCount.GetValue(AmountPerVolumeUnit::ct_Per_uL);
}

// GetExhaledCO2 - tracheal CO2 partial pressure - mmHg
double PhysiologyInterface::GetExhaledCO2() {
	SESubstance* CO2 = this->GetSubstanceManager().GetSubstance("CarbonDioxide");
	const SEGasCompartment* carina = this->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::Carina);
	return carina->GetSubstanceQuantity(*CO2)->GetPartialPressure(
			PressureUnit::mmHg);

}

// Get Tidal Volume - mL
double PhysiologyInterface::GetTidalVolume() {
	return this->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL);
}

// Get Total Lung Volume - mL
double PhysiologyInterface::GetTotalLungVolume() {
	return this->GetRespiratorySystem()->GetTotalLungVolume(VolumeUnit::mL);
}

// Get Left Lung Volume - mL
double PhysiologyInterface::GetLeftLungVolume() {
	const SEGasCompartment* leftLung = this->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::LeftLung);
	return leftLung->GetVolume(VolumeUnit::mL);
}

// Get Right Lung Volume - mL
double PhysiologyInterface::GetRightLungVolume() {
	const SEGasCompartment* rightLung = this->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::RightLung);
	return rightLung->GetVolume(VolumeUnit::mL);
}

// Get Left Lung Pleural Cavity Volume - mL
double PhysiologyInterface::GetLeftPleuralCavityVolume() {
	const SEGasCompartment* leftLung = this->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::LeftPleuralCavity);
	return leftLung->GetVolume(VolumeUnit::mL);
}

// Get Right Lung Pleural Cavity Volume - mL
double PhysiologyInterface::GetRightPleuralCavityVolume() {
	const SEGasCompartment* rightLung = this->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::RightPleuralCavity);
	return rightLung->GetVolume(VolumeUnit::mL);
}

// Get left alveoli baseline compliance (?) volume
double PhysiologyInterface::GetLeftAlveoliBaselineCompliance() {
	const SEGasCompartment* leftLung = this->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::LeftAlveoli);
	return leftLung->GetVolume(VolumeUnit::mL);

}

// Get right alveoli baseline compliance (?) volume
double PhysiologyInterface::GetRightAlveoliBaselineCompliance() {
	const SEGasCompartment* rightLung = this->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::RightAlveoli);
	return rightLung->GetVolume(VolumeUnit::mL);
}

void PhysiologyInterface::Status() {
	this->GetLogger()->Info("");
	this->GetLogger()->Info(
			std::stringstream() << "Simulation Time : "
					<< this->GetSimulationTime(TimeUnit::s) << "s");
	this->GetLogger()->Info(
			std::stringstream() << "Cardiac Output : "
					<< this->GetCardiovascularSystem()->GetCardiacOutput(
							VolumePerTimeUnit::mL_Per_min)
					<< VolumePerTimeUnit::mL_Per_min);
	this->GetLogger()->Info(
			std::stringstream() << "Blood Volume : "
					<< this->GetCardiovascularSystem()->GetBloodVolume(
							VolumeUnit::mL) << VolumeUnit::mL);
	this->GetLogger()->Info(
			std::stringstream() << "Mean Arterial Pressure : "
					<< this->GetCardiovascularSystem()->GetMeanArterialPressure(
							PressureUnit::mmHg) << PressureUnit::mmHg);
	this->GetLogger()->Info(
			std::stringstream() << "Systolic Pressure : "
					<< this->GetCardiovascularSystem()->GetSystolicArterialPressure(
							PressureUnit::mmHg) << PressureUnit::mmHg);
	this->GetLogger()->Info(
			std::stringstream() << "Diastolic Pressure : "
					<< this->GetCardiovascularSystem()->GetDiastolicArterialPressure(
							PressureUnit::mmHg) << PressureUnit::mmHg);
	this->GetLogger()->Info(
			std::stringstream() << "Heart Rate : "
					<< this->GetCardiovascularSystem()->GetHeartRate(
							FrequencyUnit::Per_min) << "bpm");
	this->GetLogger()->Info(
			std::stringstream() << "Respiration Rate : "
					<< this->GetRespiratorySystem()->GetRespirationRate(
							FrequencyUnit::Per_min) << "bpm");
	this->GetLogger()->Info("");
}

