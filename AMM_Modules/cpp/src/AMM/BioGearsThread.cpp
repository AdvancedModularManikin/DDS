#include "BioGearsThread.h"

using namespace std;

std::map<std::string, double (BioGearsThread::*)() > BioGearsThread::nodePathTable =  {
		// Old style for testing
		 {	"ECG", &BioGearsThread::GetECGWaveform},
		 {	"HR", &BioGearsThread::GetHeartRate},

		 // Cardiovascular System
		 {	"Cardiovascular_HeartRate", &BioGearsThread::GetHeartRate},
		 {	"Cardiovascular_BloodVolume", &BioGearsThread::GetBloodVolume},
		 {	"Cardiovascular_Arterial_Pressure", &BioGearsThread::GetArterialPressure},
		 {	"Cardiovascular_Arterial_Mean_Pressure", &BioGearsThread::GetMeanArterialPressure},
		 {	"Cardiovascular_Arterial_Systolic_Pressure", &BioGearsThread::GetArterialSystolicPressure},
		 {	"Cardiovascular_Arterial_Diastolic_Pressure", &BioGearsThread::GetArterialDiastolicPressure},
		 {	"Cardiovascular_CentralVenous_Mean_Pressure", &BioGearsThread::GetMeanCentralVenousPressure},

		 // Respiratory System
		 {	"Respiratory_Respiration_Rate", &BioGearsThread::GetRespirationRate},
		 {	"Respiration_EndTidalCarbonDioxide", &BioGearsThread::GetEndTidalCarbonDioxideFraction},
		 {	"Respiratory_Tidal_Volume", &BioGearsThread::GetTidalVolume},
		 {	"Respiratory_LungTotal_Volume", &BioGearsThread::GetTotalLungVolume},
		 {	"Respiratory_LeftPleuralCavity_Volume", &BioGearsThread::GetLeftPleuralCavityVolume},
		 {	"Respiratory_LeftLung_Volume", &BioGearsThread::GetLeftLungVolume},
		 {	"Respiratory_LeftAlveoli_BaseCompliance", &BioGearsThread::GetLeftAlveoliBaselineCompliance},
		 {	"Respiratory_RightPleuralCavity_Volume", &BioGearsThread::GetRightPleuralCavityVolume},
		 {	"Respiratory_RightLung_Volume", &BioGearsThread::GetRightLungVolume},
		 {	"Respiratory_RightAlveoli_BaseCompliance", &BioGearsThread::GetRightAlveoliBaselineCompliance},
		 {	"Respiratory_CarbonDioxide_Exhaled", &BioGearsThread::GetExhaledCO2},

		 // Energy system
		 {	"Energy_Core_Temperature", &BioGearsThread::GetCoreTemperature},

		 // Blood chemistry system
		 {	"BloodChemistry_WhiteBloodCell_Count", &BioGearsThread::GetWhiteBloodCellCount},
		 {	"BloodChemistry_RedBloodCell_Count", &BioGearsThread::GetRedBloodCellCount},
		 {	"BloodChemistry_BloodUreaNitrogen_Concentration", &BioGearsThread::GetBUN},
		 {	"BloodChemistry_Oxygen_Saturation", &BioGearsThread::GetOxygenSaturation},
		 {	"BloodChemistry_Hemaocrit", &BioGearsThread::GetHematocrit},
		 {	"BloodChemistry_BloodPH", &BioGearsThread::GetBloodPH},
		 {	"BloodChemistry_Arterial_CarbonDioxide_Pressure", &BioGearsThread::GetArterialCarbonDioxidePressure},
		 {	"BloodChemistry_Arterial_Oxygen_Pressure", &BioGearsThread::GetArterialOxygenPressure},

		 // Substances
		 {	"Substance_Sodium", &BioGearsThread::GetSodium},
		 {	"Substance_Sodium_Concentration", &BioGearsThread::GetSodiumConcentration},
		 {	"Substance_Bicarbonate", &BioGearsThread::GetBicarbonate},
		 {	"Substance_Bicarbonate_Concentration", &BioGearsThread::GetBicarbonateConcentration},
		 {	"Substance_BaseExcess", &BioGearsThread::GetBaseExcess},
		 {	"Substance_Glucose_Concentration", &BioGearsThread::GetGlucoseConcentration},
		 {	"Substance_Creatinine_Concentration", &BioGearsThread::GetCreatinineConcentration},
		 {	"Substance_Hemoglobin_Concentration", &BioGearsThread::GetHemoglobinConcentration},

		 {	"MetabolicPanel_CarbonDioxide", &BioGearsThread::GetCO2},
		 {	"MetabolicPanel_Potassium", &BioGearsThread::GetPotassium},
		 {	"MetabolicPanel_Chloride", &BioGearsThread::GetChloride},

		 {	"CompleteBloodCount_Platelet", &BioGearsThread::GetPlateletCount},

	};


BioGearsThread::BioGearsThread(const std::string &logFile) :
		m_thread() {
	// Create our engine with the standard patient
	m_bg = CreateBioGearsEngine(logFile);
	if (!m_bg->LoadState("./states/StandardMale@0s.xml")) {
		m_bg->GetLogger()->Error("Could not load state, check the error");
		return;
	}

	m_runThread = false;
}

BioGearsThread::~BioGearsThread() {
	m_runThread = false;
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

void BioGearsThread::Shutdown() {
	m_runThread = false;
	std::this_thread::sleep_for(std::chrono::seconds(2));
	m_thread.~thread();
	std::terminate();
}

void BioGearsThread::StartSimulation() {
	m_runThread = true;
	m_thread = std::thread(&BioGearsThread::AdvanceTime, this);
}

void BioGearsThread::StopSimulation() {
	m_runThread = false;
	m_thread.detach();
}

bool BioGearsThread::LoadState(const std::string &stateFile, double sec) {
	SEScalarTime startTime;
	startTime.SetValue(sec, TimeUnit::s);

	if (!m_bg->LoadState(stateFile, &startTime)) {
		m_bg->GetLogger()->Error("Could not load state, check the error");
		std::cerr << "[BioGears]: ERROR initializing!" << std::endl;
		return false;
	}

	return true;
}

bool BioGearsThread::SaveState(const std::string &stateFile) {
	m_bg->SaveState(stateFile);
	return true;
}

bool BioGearsThread::ExecuteCommand(const std::string &cmd) {
	return LoadScenarioFile("Actions/" + cmd + ".xml");
}

// Load a scenario from an XML file, apply conditions and iterate through the actions
// This bypasses the standard BioGears ExecuteScenario method to avoid resetting the BioGears engine
bool BioGearsThread::LoadScenarioFile(const std::string &scenarioFile) {
	SEScenario sce(m_bg->GetSubstanceManager());
	sce.LoadFile(scenarioFile);

	double dT_s = m_bg->GetTimeStep(TimeUnit::s);
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

				m_bg->AdvanceModelTime();
				scenarioTime_s = m_bg->GetSimulationTime(TimeUnit::s);
				currentSampleTime_s += dT_s;
				if (currentSampleTime_s >= sampleTime_s) {
					currentSampleTime_s = 0;
					m_bg->GetEngineTrack()->TrackData(scenarioTime_s);
				}

			}
			continue;
		} else {
			m_bg->ProcessAction(*a);
		}
	}

	return true;
}

void BioGearsThread::AdvanceTime() {
	while (m_runThread) {
		m_mutex.lock();
		m_bg->AdvanceModelTime(1, TimeUnit::s);
		m_mutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(25)); // let other things happen
	}
}

void BioGearsThread::AdvanceTimeTick() {
	m_mutex.lock();
	m_runThread = true;
	m_bg->AdvanceModelTime();
	m_runThread = false;
	m_mutex.unlock();
}

double BioGearsThread::GetSimulationTime() {
	return m_bg->GetSimulationTime(TimeUnit::s);
}

double BioGearsThread::GetNodePath(const std::string &nodePath)  {
	auto entry = nodePathTable.find(nodePath.c_str());
	if (entry != nodePathTable.end()) {
		return (this->*(entry->second))();
	} else {
		m_bg->GetLogger()->Error("Unable to access nodePath: " + nodePath);
		return 0;
	}
}

double BioGearsThread::GetHeartRate(void) {
	return m_bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min);
}

// ?? - Blood Volume - mL
double BioGearsThread::GetBloodVolume() {
	return m_bg->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL);
}

// SYS (ART) - Arterial Systolic Pressure - mmHg
double BioGearsThread::GetArterialSystolicPressure() {
	return m_bg->GetCardiovascularSystem()->GetSystolicArterialPressure(
			PressureUnit::mmHg);
}

// DIA (ART) - Arterial Diastolic Pressure - mmHg
double BioGearsThread::GetArterialDiastolicPressure() {
	return m_bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(
			PressureUnit::mmHg);
}

// MAP (ART) - Mean Arterial Pressure - mmHg
double BioGearsThread::GetMeanArterialPressure() {
	return m_bg->GetCardiovascularSystem()->GetMeanArterialPressure(
			PressureUnit::mmHg);
}

// AP - Arterial Pressure - mmHg
double BioGearsThread::GetArterialPressure() {
	return m_bg->GetCardiovascularSystem()->GetArterialPressure(
			PressureUnit::mmHg);
}

// CVP - Central Venous Pressure - mmHg
double BioGearsThread::GetMeanCentralVenousPressure() {
	return m_bg->GetCardiovascularSystem()->GetMeanCentralVenousPressure(
			PressureUnit::mmHg);
}

// MCO2 - End Tidal Carbon Dioxide Fraction - unitless %
double BioGearsThread::GetEndTidalCarbonDioxideFraction() {
	return (m_bg->GetRespiratorySystem()->GetEndTidalCarbonDioxideFraction()
			* 7.5);
}

// SPO2 - Oxygen Saturation - unitless %
double BioGearsThread::GetOxygenSaturation() {
	return m_bg->GetBloodChemistrySystem()->GetOxygenSaturation();
}

// BR - Respiration Rate - per minute
double BioGearsThread::GetRespirationRate() {
	return m_bg->GetRespiratorySystem()->GetRespirationRate(
			FrequencyUnit::Per_min);
}

// T2 - Core Temperature - degrees C
double BioGearsThread::GetCoreTemperature() {
	return m_bg->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C);
}

// ECG Waveform in mV
double BioGearsThread::GetECGWaveform() {
	double ecgLead3_mV =
			m_bg->GetElectroCardioGram()->GetLead3ElectricPotential(
					ElectricPotentialUnit::mV);
	return ecgLead3_mV;
}

// Na+ - Sodium Concentration - mg/dL
double BioGearsThread::GetSodiumConcentration() {
	SESubstance* sodium = m_bg->GetSubstanceManager().GetSubstance("Sodium");
	return sodium->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// Na+ - Sodium - mmol/L
double BioGearsThread::GetSodium() {
	return GetSodiumConcentration() * 0.43;
}

// Glucose - Glucose Concentration - mg/dL
double BioGearsThread::GetGlucoseConcentration() {
	SESubstance* glucose = m_bg->GetSubstanceManager().GetSubstance("Glucose");
	return glucose->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// BUN - BloodUreaNitrogenConcentration - mg/dL
double BioGearsThread::GetBUN() {
	return m_bg->GetBloodChemistrySystem()->GetBloodUreaNitrogenConcentration(
			MassPerVolumeUnit::mg_Per_dL);
}

// Creatinine - Creatinine Concentration - mg/dL
double BioGearsThread::GetCreatinineConcentration() {
	SESubstance* creatinine = m_bg->GetSubstanceManager().GetSubstance(
			"Creatinine");
	return creatinine->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// RBC - White Blood Cell Count - ct/uL
double BioGearsThread::GetWhiteBloodCellCount() {
	return m_bg->GetBloodChemistrySystem()->GetWhiteBloodCellCount(
			AmountPerVolumeUnit::ct_Per_uL);
}

// RBC - Red Blood Cell Count - ct/uL
double BioGearsThread::GetRedBloodCellCount() {
	return m_bg->GetBloodChemistrySystem()->GetRedBloodCellCount(
			AmountPerVolumeUnit::ct_Per_uL);
}

// Hgb - Hemoglobin Concentration - g/dL
double BioGearsThread::GetHemoglobinConcentration() {
	SESubstance* hemoglobin = m_bg->GetSubstanceManager().GetSubstance(
			"Hemoglobin");
	return hemoglobin->GetBloodConcentration(MassPerVolumeUnit::g_Per_dL);
}

// Hct - Hematocrit - unitless
double BioGearsThread::GetHematocrit() {
	return m_bg->GetBloodChemistrySystem()->GetHematocrit();
}

// pH - Blood pH - unitless
double BioGearsThread::GetBloodPH() {
	return m_bg->GetBloodChemistrySystem()->GetBloodPH();
}

// PaCO2 - Arterial Carbon Dioxide Pressure - mmHg
double BioGearsThread::GetArterialCarbonDioxidePressure() {
	return m_bg->GetBloodChemistrySystem()->GetArterialCarbonDioxidePressure(
			PressureUnit::mmHg);
}

// Pa02 - Arterial Oxygen Pressure - mmHg
double BioGearsThread::GetArterialOxygenPressure() {
	return m_bg->GetBloodChemistrySystem()->GetArterialOxygenPressure(
			PressureUnit::mmHg);
}

// n/a - Bicarbonate Concentration - mg/L
double BioGearsThread::GetBicarbonateConcentration() {
	SESubstance* bicarbonate = m_bg->GetSubstanceManager().GetSubstance(
			"Bicarbonate");
	return bicarbonate->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// HCO3 - Bicarbonate - Convert to mmol/L
double BioGearsThread::GetBicarbonate() {
// SESubstance* bicarbonate = m_bg->GetSubstanceManager().GetSubstance("Bicarbonate");
	return GetBicarbonateConcentration() * 0.1639;
}

// BE - Base Excess -
double BioGearsThread::GetBaseExcess() {
	return (0.93 * GetBicarbonate()) + (13.77 * GetBloodPH()) - 124.58;
}

double BioGearsThread::GetCO2() {
	SEComprehensiveMetabolicPanel metabolicPanel(m_bg->GetLogger());
	m_bg->GetPatientAssessment(metabolicPanel);
	SEScalarAmountPerVolume CO2 = metabolicPanel.GetCO2();
	return CO2.GetValue(AmountPerVolumeUnit::mmol_Per_L);
}

double BioGearsThread::GetPotassium() {
	SEComprehensiveMetabolicPanel metabolicPanel(m_bg->GetLogger());
	m_bg->GetPatientAssessment(metabolicPanel);
	SEScalarAmountPerVolume potassium = metabolicPanel.GetPotassium();
	return potassium.GetValue(AmountPerVolumeUnit::ct_Per_uL);
}

double BioGearsThread::GetChloride() {
	SEComprehensiveMetabolicPanel metabolicPanel(m_bg->GetLogger());
	m_bg->GetPatientAssessment(metabolicPanel);
	SEScalarAmountPerVolume chloride = metabolicPanel.GetChloride();
	return chloride.GetValue(AmountPerVolumeUnit::ct_Per_uL);
}

// PLT - Platelet Count - ct/uL
double BioGearsThread::GetPlateletCount() {
	SECompleteBloodCount CBC(m_bg->GetLogger());
	m_bg->GetPatientAssessment(CBC);
	SEScalarAmountPerVolume plateletCount = CBC.GetPlateletCount();
	return plateletCount.GetValue(AmountPerVolumeUnit::ct_Per_uL);
}

// GetExhaledCO2 - tracheal CO2 partial pressure - mmHg
double BioGearsThread::GetExhaledCO2() {
	SESubstance* CO2 = m_bg->GetSubstanceManager().GetSubstance(
			"CarbonDioxide");
	const SEGasCompartment* carina = m_bg->GetCompartments().GetGasCompartment(
	BGE::PulmonaryCompartment::Carina);
	return carina->GetSubstanceQuantity(*CO2)->GetPartialPressure(
			PressureUnit::mmHg);

}

// Get Tidal Volume - mL
double BioGearsThread::GetTidalVolume() {
	return m_bg->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL);
}

// Get Total Lung Volume - mL
double BioGearsThread::GetTotalLungVolume() {
	return m_bg->GetRespiratorySystem()->GetTotalLungVolume(VolumeUnit::mL);
}

// Get Left Lung Volume - mL
double BioGearsThread::GetLeftLungVolume() {
	const SEGasCompartment* leftLung =
			m_bg->GetCompartments().GetGasCompartment(
			BGE::PulmonaryCompartment::LeftLung);
	return leftLung->GetVolume(VolumeUnit::mL);
}

// Get Right Lung Volume - mL
double BioGearsThread::GetRightLungVolume() {
	const SEGasCompartment* rightLung =
			m_bg->GetCompartments().GetGasCompartment(
			BGE::PulmonaryCompartment::RightLung);
	return rightLung->GetVolume(VolumeUnit::mL);
}

// Get Left Lung Pleural Cavity Volume - mL
double BioGearsThread::GetLeftPleuralCavityVolume() {
	const SEGasCompartment* leftLung =
			m_bg->GetCompartments().GetGasCompartment(
			BGE::PulmonaryCompartment::LeftPleuralCavity);
	return leftLung->GetVolume(VolumeUnit::mL);
}

// Get Right Lung Pleural Cavity Volume - mL
double BioGearsThread::GetRightPleuralCavityVolume() {
	const SEGasCompartment* rightLung =
			m_bg->GetCompartments().GetGasCompartment(
			BGE::PulmonaryCompartment::RightPleuralCavity);
	return rightLung->GetVolume(VolumeUnit::mL);
}

// Get left alveoli baseline compliance (?) volume
double BioGearsThread::GetLeftAlveoliBaselineCompliance() {
	const SEGasCompartment* leftLung =
			m_bg->GetCompartments().GetGasCompartment(
			BGE::PulmonaryCompartment::LeftAlveoli);
	return leftLung->GetVolume(VolumeUnit::mL);

}

// Get right alveoli baseline compliance (?) volume
double BioGearsThread::GetRightAlveoliBaselineCompliance() {
	const SEGasCompartment* rightLung =
			m_bg->GetCompartments().GetGasCompartment(
			BGE::PulmonaryCompartment::RightAlveoli);
	return rightLung->GetVolume(VolumeUnit::mL);
}

void BioGearsThread::Status() {
	m_bg->GetLogger()->Info("");
	m_bg->GetLogger()->Info(
			std::stringstream() << "Simulation Time : "
					<< m_bg->GetSimulationTime(TimeUnit::s) << "s");
	m_bg->GetLogger()->Info(
			std::stringstream() << "Cardiac Output : "
					<< m_bg->GetCardiovascularSystem()->GetCardiacOutput(
							VolumePerTimeUnit::mL_Per_min)
					<< VolumePerTimeUnit::mL_Per_min);
	m_bg->GetLogger()->Info(
			std::stringstream() << "Blood Volume : "
					<< m_bg->GetCardiovascularSystem()->GetBloodVolume(
							VolumeUnit::mL) << VolumeUnit::mL);
	m_bg->GetLogger()->Info(
			std::stringstream() << "Mean Arterial Pressure : "
					<< m_bg->GetCardiovascularSystem()->GetMeanArterialPressure(
							PressureUnit::mmHg) << PressureUnit::mmHg);
	m_bg->GetLogger()->Info(
			std::stringstream() << "Systolic Pressure : "
					<< m_bg->GetCardiovascularSystem()->GetSystolicArterialPressure(
							PressureUnit::mmHg) << PressureUnit::mmHg);
	m_bg->GetLogger()->Info(
			std::stringstream() << "Diastolic Pressure : "
					<< m_bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(
							PressureUnit::mmHg) << PressureUnit::mmHg);
	m_bg->GetLogger()->Info(
			std::stringstream() << "Heart Rate : "
					<< m_bg->GetCardiovascularSystem()->GetHeartRate(
							FrequencyUnit::Per_min) << "bpm");
	m_bg->GetLogger()->Info(
			std::stringstream() << "Respiration Rate : "
					<< m_bg->GetRespiratorySystem()->GetRespirationRate(
							FrequencyUnit::Per_min) << "bpm");
	m_bg->GetLogger()->Info("");
}
