#include "BioGearsWrapper.h"

BioGearsWrapper::BioGearsWrapper(std::string logFile) {
	bg = CreateBioGearsEngine(logFile);
}

BioGearsWrapper::~BioGearsWrapper() {
}

void BioGearsWrapper::SetPatientFile(std::string patFile) {
	_patFile = patFile;
}

void BioGearsWrapper::SetConditionsFile(std::string condFile) {
	_condFile = condFile;
}

void BioGearsWrapper::SetStateFile(std::string stateFile) {
	_stateFile = stateFile;
}

bool BioGearsWrapper::InitializeEngine() {
	if (bg == nullptr) {
		return false;
	}

	bg->GetLogger()->SetLogLevel(log4cpp::Priority::DEBUG);

	if (_stateFile == "") {
		LoadConditionsAndPatient(_condFile, _patFile);
	} else {
		LoadState(_stateFile, 0);
	}
	return true;
}
;

bool BioGearsWrapper::LoadConditionsAndPatient(std::string condFile,
		std::string patFile) {
	std::vector<const SECondition*> conditions;
	SEInitialEnvironment env(bg->GetSubstanceManager());
	env.SetConditionsFile(condFile);
	conditions.push_back(&env);

	if (!bg->InitializeEngine(patFile, &conditions)) {
		bg->GetLogger()->Error(
				"Could not load patient and conditions from file, check the error");
		std::cerr << "[BioGears]: ERROR initializing!" << std::endl;
		return false;
	}

	return true;
}

bool BioGearsWrapper::LoadState(std::string stateFile, double sec) {
	SEScalarTime startTime;
	startTime.SetValue(sec, TimeUnit::s);

	if (!bg->LoadState(stateFile, &startTime)) {
		bg->GetLogger()->Error("Could not load state, check the error");
		std::cerr << "[BioGears]: ERROR initializing!" << std::endl;
		return false;
	}

	return true;
}

bool BioGearsWrapper::SaveState(std::string stateFile) {
	bg->SaveState(stateFile);
	return true;
}

// Load a scenario from an XML file, apply conditions and iterate through the actions
// This bypasses the standard BioGears ExecuteScenario method to avoid resetting the BioGears engine
bool BioGearsWrapper::LoadScenarioFile(std::string scenFile) {
	SEScenario sce(bg->GetSubstanceManager());
	sce.LoadFile(scenFile);

	double dT_s = bg->GetTimeStep(TimeUnit::s);
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

				bg->AdvanceModelTime();
				scenarioTime_s = bg->GetSimulationTime(TimeUnit::s);
				currentSampleTime_s += dT_s;
				if (currentSampleTime_s >= sampleTime_s) {
					currentSampleTime_s = 0;
					bg->GetEngineTrack()->TrackData(scenarioTime_s);
				}

			}
			continue;
		} else {
			bg->ProcessAction(*a);
		}
	}

	return true;
}

void BioGearsWrapper::Shutdown() {
}

void BioGearsWrapper::StartSimulation() {
}

void BioGearsWrapper::StopSimulation() {
}

// Advance time one tick
void BioGearsWrapper::AdvanceTimeTick() {
	bg->AdvanceModelTime();
}

// Advance time by a number of seconds
void BioGearsWrapper::AdvanceModelTime(double sec) {
	bg->AdvanceModelTime(sec, TimeUnit::s);
}

double BioGearsWrapper::GetHeartRate() {
	return bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min);
}

// ?? - Blood Volume - mL
double BioGearsWrapper::GetBloodVolume() {
	return bg->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL);
}

// SYS (ART) - Arterial Systolic Pressure - mmHg
double BioGearsWrapper::GetArterialSystolicPressure() {
	return bg->GetCardiovascularSystem()->GetSystolicArterialPressure(
			PressureUnit::mmHg);
}

// DIA (ART) - Arterial Diastolic Pressure - mmHg
double BioGearsWrapper::GetArterialDiastolicPressure() {
	return bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(
			PressureUnit::mmHg);
}

// MAP (ART) - Mean Arterial Pressure - mmHg
double BioGearsWrapper::GetMeanArterialPressure() {
	return bg->GetCardiovascularSystem()->GetMeanArterialPressure(
			PressureUnit::mmHg);
}

// AP - Arterial Pressure - mmHg
double BioGearsWrapper::GetArterialPressure() {
	return bg->GetCardiovascularSystem()->GetArterialPressure(
			PressureUnit::mmHg);
}

// CVP - Central Venous Pressure - mmHg
double BioGearsWrapper::GetMeanCentralVenousPressure() {
	return bg->GetCardiovascularSystem()->GetMeanCentralVenousPressure(
			PressureUnit::mmHg);
}

// MCO2 - End Tidal Carbon Dioxide Fraction - unitless %
double BioGearsWrapper::GetEndTidalCarbonDioxideFraction() {
	return (bg->GetRespiratorySystem()->GetEndTidalCarbonDioxideFraction() * 7.5);
}

// SPO2 - Oxygen Saturation - unitless %
double BioGearsWrapper::GetOxygenSaturation() {
	return bg->GetBloodChemistrySystem()->GetOxygenSaturation();
}

// BR - Respiration Rate - per minute
double BioGearsWrapper::GetRespirationRate() {
	return bg->GetRespiratorySystem()->GetRespirationRate(
			FrequencyUnit::Per_min);
}

// T2 - Core Temperature - degrees C
double BioGearsWrapper::GetCoreTemperature() {
	return bg->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C);
}

// ECG Waveform in mV
double BioGearsWrapper::GetECGWaveform() {
	double ecgLead3_mV = bg->GetElectroCardioGram()->GetLead3ElectricPotential(
			ElectricPotentialUnit::mV);
	return ecgLead3_mV;
}

// Na+ - Sodium Concentration - mg/dL
double BioGearsWrapper::GetSodiumConcentration() {
	SESubstance* sodium = bg->GetSubstanceManager().GetSubstance("Sodium");
	return sodium->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// Na+ - Sodium - mmol/L
double BioGearsWrapper::GetSodium() {
	return GetSodiumConcentration() * 0.43;
}

// Glucose - Glucose Concentration - mg/dL
double BioGearsWrapper::GetGlucoseConcentration() {
	SESubstance* glucose = bg->GetSubstanceManager().GetSubstance("Glucose");
	return glucose->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// BUN - BloodUreaNitrogenConcentration - mg/dL
double BioGearsWrapper::GetBUN() {
	return bg->GetBloodChemistrySystem()->GetBloodUreaNitrogenConcentration(
			MassPerVolumeUnit::mg_Per_dL);
}

// Creatinine - Creatinine Concentration - mg/dL
double BioGearsWrapper::GetCreatinineConcentration() {
	SESubstance* creatinine = bg->GetSubstanceManager().GetSubstance(
			"Creatinine");
	return creatinine->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// RBC - White Blood Cell Count - ct/uL
double BioGearsWrapper::GetWhiteBloodCellCount() {
	return bg->GetBloodChemistrySystem()->GetWhiteBloodCellCount(
			AmountPerVolumeUnit::ct_Per_uL);
}

// RBC - Red Blood Cell Count - ct/uL
double BioGearsWrapper::GetRedBloodCellCount() {
	return bg->GetBloodChemistrySystem()->GetRedBloodCellCount(
			AmountPerVolumeUnit::ct_Per_uL);
}

// Hgb - Hemoglobin Concentration - g/dL
double BioGearsWrapper::GetHemoglobinConcentration() {
	SESubstance* hemoglobin = bg->GetSubstanceManager().GetSubstance(
			"Hemoglobin");
	return hemoglobin->GetBloodConcentration(MassPerVolumeUnit::g_Per_dL);
}

// Hct - Hematocrit - unitless
double BioGearsWrapper::GetHematocrit() {
	return bg->GetBloodChemistrySystem()->GetHematocrit();
}

// pH - Blood pH - unitless
double BioGearsWrapper::GetBloodPH() {
	return bg->GetBloodChemistrySystem()->GetBloodPH();
}

// PaCO2 - Arterial Carbon Dioxide Pressure - mmHg
double BioGearsWrapper::GetArterialCarbonDioxidePressure() {
	return bg->GetBloodChemistrySystem()->GetArterialCarbonDioxidePressure(
			PressureUnit::mmHg);
}

// Pa02 - Arterial Oxygen Pressure - mmHg
double BioGearsWrapper::GetArterialOxygenPressure() {
	return bg->GetBloodChemistrySystem()->GetArterialOxygenPressure(
			PressureUnit::mmHg);
}

// n/a - Bicarbonate Concentration - mg/L
double BioGearsWrapper::GetBicarbonateConcentration() {
	SESubstance* bicarbonate = bg->GetSubstanceManager().GetSubstance(
			"Bicarbonate");
	return bicarbonate->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// HCO3 - Bicarbonate - Convert to mmol/L
double BioGearsWrapper::GetBicarbonate() {
	// SESubstance* bicarbonate = bg->GetSubstanceManager().GetSubstance("Bicarbonate");
	return GetBicarbonateConcentration() * 0.1639;
}

// BE - Base Excess -
double BioGearsWrapper::GetBaseExcess() {
	return (0.93 * GetBicarbonate()) + (13.77 * GetBloodPH()) - 124.58;
}

double BioGearsWrapper::GetCO2() {
	SEComprehensiveMetabolicPanel metabolicPanel(bg->GetLogger());
	bg->GetPatientAssessment(metabolicPanel);
	SEScalarAmountPerVolume CO2 = metabolicPanel.GetCO2();
	return CO2.GetValue(AmountPerVolumeUnit::mmol_Per_L);
	// CO2(in mmol / L) can be estimated by HCO3(in mmol / L) + 0.03 * PaCO2(in mmHg)
	// SESubstance* co2 = bg->GetSubstanceManager().GetSubstance("CarbonDioxide");
	// return co2->GetMassInBlood()
	// return co2->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// Metabolic panel
bool BioGearsWrapper::UpdateMetabolicPanel() {
	SEComprehensiveMetabolicPanel metabolicPanel(bg->GetLogger());
	bg->GetPatientAssessment(metabolicPanel);
	return true;
}

double BioGearsWrapper::GetPotassium() {
	SEComprehensiveMetabolicPanel metabolicPanel(bg->GetLogger());
	bg->GetPatientAssessment(metabolicPanel);
	SEScalarAmountPerVolume potassium = metabolicPanel.GetPotassium();
	return potassium.GetValue(AmountPerVolumeUnit::ct_Per_uL);
}

double BioGearsWrapper::GetChloride() {
	SEComprehensiveMetabolicPanel metabolicPanel(bg->GetLogger());
	bg->GetPatientAssessment(metabolicPanel);
	SEScalarAmountPerVolume chloride = metabolicPanel.GetChloride();
	return chloride.GetValue(AmountPerVolumeUnit::ct_Per_uL);
}

// CBC
bool BioGearsWrapper::UpdateCompleteBloodCount() {
	SECompleteBloodCount CBC(bg->GetLogger());
	bg->GetPatientAssessment(CBC);
	return true;
}

// PLT - Platelet Count - ct/uL
double BioGearsWrapper::GetPlateletCount() {
	SECompleteBloodCount CBC(bg->GetLogger());
	bg->GetPatientAssessment(CBC);
	SEScalarAmountPerVolume plateletCount = CBC.GetPlateletCount();
	return plateletCount.GetValue(AmountPerVolumeUnit::ct_Per_uL);
}

// GetExhaledCO2 - tracheal CO2 partial pressure - mmHg
double BioGearsWrapper::GetExhaledCO2() {
	SESubstance* CO2 = bg->GetSubstanceManager().GetSubstance("CarbonDioxide");
	const SEGasCompartment* carina = bg->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::Carina);
	return carina->GetSubstanceQuantity(*CO2)->GetPartialPressure(
			PressureUnit::mmHg);

}

// Get Tidal Volume - mL
double BioGearsWrapper::GetTidalVolume() {
	return bg->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL);
}

// Get Total Lung Volume - mL
double BioGearsWrapper::GetTotalLungVolume() {
	return bg->GetRespiratorySystem()->GetTotalLungVolume(VolumeUnit::mL);
}

// Get Left Lung Volume - mL
double BioGearsWrapper::GetLeftLungVolume() {
	const SEGasCompartment* leftLung = bg->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::LeftLung);
	return leftLung->GetVolume(VolumeUnit::mL);
}

// Get Right Lung Volume - mL
double BioGearsWrapper::GetRightLungVolume() {
	const SEGasCompartment* rightLung = bg->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::RightLung);
	return rightLung->GetVolume(VolumeUnit::mL);
}

// Get Left Lung Pleural Cavity Volume - mL
double BioGearsWrapper::GetLeftPleuralCavityVolume() {
	const SEGasCompartment* leftLung = bg->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::LeftPleuralCavity);
	return leftLung->GetVolume(VolumeUnit::mL);
}

// Get Right Lung Pleural Cavity Volume - mL
double BioGearsWrapper::GetRightPleuralCavityVolume() {
	const SEGasCompartment* rightLung = bg->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::RightPleuralCavity);
	return rightLung->GetVolume(VolumeUnit::mL);
}

// Get left alveoli baseline compliance (?) volume
double BioGearsWrapper::GetLeftAlveoliBaselineCompliance() {
	const SEGasCompartment* leftLung = bg->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::LeftAlveoli);
	return leftLung->GetVolume(VolumeUnit::mL);

}

// Get right alveoli baseline compliance (?) volume
double BioGearsWrapper::GetRightAlveoliBaselineCompliance() {
	const SEGasCompartment* rightLung = bg->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::RightAlveoli);
	return rightLung->GetVolume(VolumeUnit::mL);
}

AMM::Physiology::Data BioGearsWrapper::GetNodePath(
		const std::string& nodePath) {

	AMM::Physiology::Data outputData;

	if (nodePath == "ECG") {
		outputData.node_path = "ECG";
		outputData.unit = "mV";
		outputData.dbl = GetECGWaveform();
	}

	if (nodePath == "HR") {
		outputData.node_path = "HR";
		outputData.unit = "bpm";
		outputData.dbl = GetHeartRate();
	}

	return outputData;
}

void BioGearsWrapper::Status() {
	bg->GetLogger()->Info("");
	bg->GetLogger()->Info(
			std::stringstream() << "Simulation Time : "
					<< bg->GetSimulationTime(TimeUnit::s) << "s");
	bg->GetLogger()->Info(
			std::stringstream() << "Cardiac Output : "
					<< bg->GetCardiovascularSystem()->GetCardiacOutput(
							VolumePerTimeUnit::mL_Per_min)
					<< VolumePerTimeUnit::mL_Per_min);
	bg->GetLogger()->Info(
			std::stringstream() << "Blood Volume : "
					<< bg->GetCardiovascularSystem()->GetBloodVolume(
							VolumeUnit::mL) << VolumeUnit::mL);
	bg->GetLogger()->Info(
			std::stringstream() << "Mean Arterial Pressure : "
					<< bg->GetCardiovascularSystem()->GetMeanArterialPressure(
							PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(
			std::stringstream() << "Systolic Pressure : "
					<< bg->GetCardiovascularSystem()->GetSystolicArterialPressure(
							PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(
			std::stringstream() << "Diastolic Pressure : "
					<< bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(
							PressureUnit::mmHg) << PressureUnit::mmHg);
	bg->GetLogger()->Info(
			std::stringstream() << "Heart Rate : "
					<< bg->GetCardiovascularSystem()->GetHeartRate(
							FrequencyUnit::Per_min) << "bpm");
	bg->GetLogger()->Info(
			std::stringstream() << "Respiration Rate : "
					<< bg->GetRespiratorySystem()->GetRespirationRate(
							FrequencyUnit::Per_min) << "bpm");
	bg->GetLogger()->Info("");
}
