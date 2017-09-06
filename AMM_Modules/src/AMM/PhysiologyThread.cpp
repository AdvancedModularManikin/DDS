#include "PhysiologyThread.h"

using namespace std;
using namespace AMM::Physiology;

std::vector<std::string> PhysiologyThread::highFrequencyNodes;
std::map<std::string, double (PhysiologyThread::*)()> PhysiologyThread::nodePathTable;

PhysiologyThread::PhysiologyThread(const std::string &logFile, const std::string &stateFile) :
        m_bg(CreateBioGearsEngine(logFile)) {
    // Create our engine with the standard patient
    // m_bg = ;
    if (!m_bg->LoadState(stateFile)) {
        m_bg->GetLogger()->Error("Could not load state, check the error");
        return;
    }

    PopulateNodePathTable();

    m_runThread = false;
}

PhysiologyThread::~PhysiologyThread() {
    m_runThread = false;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void PhysiologyThread::PopulateNodePathTable() {
    // Legacy values
    nodePathTable["ECG"] = &PhysiologyThread::GetECGWaveform;
    nodePathTable["HR"] = &PhysiologyThread::GetHeartRate;
    nodePathTable["SIM_TIME"] = &PhysiologyThread::GetSimulationTime;
    nodePathTable["EXIT"] = &PhysiologyThread::GetShutdownMessage;

    // Cardiovascular System
    nodePathTable["Cardiovascular_HeartRate"] = &PhysiologyThread::GetHeartRate;
    nodePathTable["Cardiovascular_BloodVolume"] = &PhysiologyThread::GetBloodVolume;
    nodePathTable["Cardiovascular_Arterial_Pressure"] = &PhysiologyThread::GetArterialPressure;
    nodePathTable["Cardiovascular_Arterial_Mean_Pressure"] = &PhysiologyThread::GetMeanArterialPressure;
    nodePathTable["Cardiovascular_Arterial_Systolic_Pressure"] = &PhysiologyThread::GetArterialSystolicPressure;
    nodePathTable["Cardiovascular_Arterial_Diastolic_Pressure"] = &PhysiologyThread::GetArterialDiastolicPressure;
    nodePathTable["Cardiovascular_CentralVenous_Mean_Pressure"] = &PhysiologyThread::GetMeanCentralVenousPressure;
    nodePathTable["Cardiovascular_CardiacOutput"] = &PhysiologyThread::GetCardiacOutput;

    // Respiratory System
    nodePathTable["Respiratory_Respiration_Rate"] = &PhysiologyThread::GetRespirationRate;
    nodePathTable["Respiration_EndTidalCarbonDioxide"] = &PhysiologyThread::GetEndTidalCarbonDioxideFraction;
    nodePathTable["Respiratory_Tidal_Volume"] = &PhysiologyThread::GetTidalVolume;
    nodePathTable["Respiratory_LungTotal_Volume"] = &PhysiologyThread::GetTotalLungVolume;
    nodePathTable["Respiratory_LeftPleuralCavity_Volume"] = &PhysiologyThread::GetLeftPleuralCavityVolume;
    nodePathTable["Respiratory_LeftLung_Volume"] = &PhysiologyThread::GetLeftLungVolume;
    nodePathTable["Respiratory_LeftAlveoli_BaseCompliance"] = &PhysiologyThread::GetLeftAlveoliBaselineCompliance;
    nodePathTable["Respiratory_RightPleuralCavity_Volume"] = &PhysiologyThread::GetRightPleuralCavityVolume;
    nodePathTable["Respiratory_RightLung_Volume"] = &PhysiologyThread::GetRightLungVolume;
    nodePathTable["Respiratory_RightAlveoli_BaseCompliance"] = &PhysiologyThread::GetRightAlveoliBaselineCompliance;
    nodePathTable["Respiratory_CarbonDioxide_Exhaled"] = &PhysiologyThread::GetExhaledCO2;

    // Energy system
    nodePathTable["Energy_Core_Temperature"] = &PhysiologyThread::GetCoreTemperature;

    // Blood chemistry system
    nodePathTable["BloodChemistry_WhiteBloodCell_Count"] = &PhysiologyThread::GetWhiteBloodCellCount;
    nodePathTable["BloodChemistry_RedBloodCell_Count"] = &PhysiologyThread::GetRedBloodCellCount;
    nodePathTable["BloodChemistry_BloodUreaNitrogen_Concentration"] = &PhysiologyThread::GetBUN;
    nodePathTable["BloodChemistry_Oxygen_Saturation"] = &PhysiologyThread::GetOxygenSaturation;
    nodePathTable["BloodChemistry_Hemaocrit"] = &PhysiologyThread::GetHematocrit;
    nodePathTable["BloodChemistry_BloodPH"] = &PhysiologyThread::GetBloodPH;
    nodePathTable["BloodChemistry_Arterial_CarbonDioxide_Pressure"] = &PhysiologyThread::GetArterialCarbonDioxidePressure;
    nodePathTable["BloodChemistry_Arterial_Oxygen_Pressure"] = &PhysiologyThread::GetArterialOxygenPressure;

    // Substances
    nodePathTable["Substance_Sodium"] = &PhysiologyThread::GetSodium;
    nodePathTable["Substance_Sodium_Concentration"] = &PhysiologyThread::GetSodiumConcentration;
    nodePathTable["Substance_Bicarbonate"] = &PhysiologyThread::GetBicarbonate;
    nodePathTable["Substance_Bicarbonate_Concentration"] = &PhysiologyThread::GetBicarbonateConcentration;
    nodePathTable["Substance_BaseExcess"] = &PhysiologyThread::GetBaseExcess;
    nodePathTable["Substance_Glucose_Concentration"] = &PhysiologyThread::GetGlucoseConcentration;
    nodePathTable["Substance_Creatinine_Concentration"] = &PhysiologyThread::GetCreatinineConcentration;
    nodePathTable["Substance_Hemoglobin_Concentration"] = &PhysiologyThread::GetHemoglobinConcentration;

    nodePathTable["MetabolicPanel_CarbonDioxide"] = &PhysiologyThread::GetCO2;
    nodePathTable["MetabolicPanel_Potassium"] = &PhysiologyThread::GetPotassium;
    nodePathTable["MetabolicPanel_Chloride"] = &PhysiologyThread::GetChloride;

    nodePathTable["CompleteBloodCount_Platelet"] = &PhysiologyThread::GetPlateletCount;

    // Label which nodes are high-frequency
    highFrequencyNodes = {
            "ECG",
            "Cardiovascular_HeartRate",
            "Cardiovascular_Arterial_Pressure",
            "Respiratory_CarbonDioxide_Exhaled",
            "Respiratory_Respiration_Rate"
    };
}

void PhysiologyThread::Shutdown() {

}

void PhysiologyThread::StartSimulation() {
    m_runThread = true;
    m_thread = std::thread(&PhysiologyThread::AdvanceTime, this);
}

void PhysiologyThread::StopSimulation() {
    if (m_runThread) {
        m_runThread = false;
        m_thread.join();
    }
}

bool PhysiologyThread::LoadState(const std::string &stateFile, double sec) {
    SEScalarTime startTime;
    startTime.SetValue(sec, TimeUnit::s);

    if (!m_bg->LoadState(stateFile, &startTime)) {
        m_bg->GetLogger()->Error("Could not load state, check the error");
        std::cerr << "[BioGears]: ERROR initializing!" << std::endl;
        return false;
    }

    return true;
}

bool PhysiologyThread::SaveState(const std::string &stateFile) {
    m_bg->SaveState(stateFile);
    return true;
}

bool PhysiologyThread::ExecuteCommand(const std::string &cmd) {
    string scenarioFile = "Actions/" + cmd + ".xml";
    return LoadScenarioFile(scenarioFile);
}

// Load a scenario from an XML file, apply conditions and iterate through the actions
// This bypasses the standard BioGears ExecuteScenario method to avoid resetting the BioGears engine
bool PhysiologyThread::LoadScenarioFile(const std::string &scenarioFile) {
    SEScenario sce(m_bg->GetSubstanceManager());
    sce.LoadFile(scenarioFile);

    double dT_s = m_bg->GetTimeStep(TimeUnit::s);
    double scenarioTime_s;

    double sampleTime_s = sce.GetDataRequestManager().GetSamplesPerSecond();
    if (sampleTime_s != 0)
        sampleTime_s = 1 / sampleTime_s;
    double currentSampleTime_s = sampleTime_s;        //Sample the first step

    SEAdvanceTime *adv;
    for (SEAction *a : sce.GetActions()) {
        adv = dynamic_cast<SEAdvanceTime *>(a);
        if (adv != nullptr) {
            double time_s = adv->GetTime(TimeUnit::s);
            auto count = (int) (time_s / dT_s);
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

void PhysiologyThread::AdvanceTime() {
    while (m_runThread) {
        m_mutex.lock();
        m_bg->AdvanceModelTime(1, TimeUnit::s);
        m_mutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(25)); // let other things happen
    }
}

void PhysiologyThread::AdvanceTimeTick() {
    m_mutex.lock();
    m_runThread = true;
    m_bg->AdvanceModelTime();
    m_runThread = false;
    m_mutex.unlock();
}

double PhysiologyThread::GetShutdownMessage() {
    return -1;
}

double PhysiologyThread::GetSimulationTime() {
    return m_bg->GetSimulationTime(TimeUnit::s);
}

double PhysiologyThread::GetNodePath(const std::string &nodePath) {
    std::map<std::string, double (PhysiologyThread::*)()>::iterator entry;
    entry = nodePathTable.find(nodePath);
    if (entry != nodePathTable.end()) {
        return (this->*(entry->second))();
    }

    m_bg->GetLogger()->Error("Unable to access nodePath: " + nodePath);
    return 0;

}

double PhysiologyThread::GetHeartRate() {
    return m_bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min);
}

// ?? - Blood Volume - mL
double PhysiologyThread::GetBloodVolume() {
    return m_bg->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL);
}

// SYS (ART) - Arterial Systolic Pressure - mmHg
double PhysiologyThread::GetArterialSystolicPressure() {
    return m_bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg);
}

// DIA (ART) - Arterial Diastolic Pressure - mmHg
double PhysiologyThread::GetArterialDiastolicPressure() {
    return m_bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg);
}

// MAP (ART) - Mean Arterial Pressure - mmHg
double PhysiologyThread::GetMeanArterialPressure() {
    return m_bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg);
}

// AP - Arterial Pressure - mmHg
double PhysiologyThread::GetArterialPressure() {
    return m_bg->GetCardiovascularSystem()->GetArterialPressure(PressureUnit::mmHg);
}

// CVP - Central Venous Pressure - mmHg
double PhysiologyThread::GetMeanCentralVenousPressure() {
    return m_bg->GetCardiovascularSystem()->GetMeanCentralVenousPressure(PressureUnit::mmHg);
}

// MCO2 - End Tidal Carbon Dioxide Fraction - unitless %
double PhysiologyThread::GetEndTidalCarbonDioxideFraction() {
    return (m_bg->GetRespiratorySystem()->GetEndTidalCarbonDioxideFraction() * 7.5);
}

// SPO2 - Oxygen Saturation - unitless %
double PhysiologyThread::GetOxygenSaturation() {
    return m_bg->GetBloodChemistrySystem()->GetOxygenSaturation();
}

// BR - Respiration Rate - per minute
double PhysiologyThread::GetRespirationRate() {
    return m_bg->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min);
}

// T2 - Core Temperature - degrees C
double PhysiologyThread::GetCoreTemperature() {
    return m_bg->GetEnergySystem()->GetCoreTemperature(TemperatureUnit::C);
}

// ECG Waveform in mV
double PhysiologyThread::GetECGWaveform() {
    double ecgLead3_mV = m_bg->GetElectroCardioGram()->GetLead3ElectricPotential(ElectricPotentialUnit::mV);
    return ecgLead3_mV;
}

// Na+ - Sodium Concentration - mg/dL
double PhysiologyThread::GetSodiumConcentration() {
    SESubstance *sodium = m_bg->GetSubstanceManager().GetSubstance("Sodium");
    return sodium->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// Na+ - Sodium - mmol/L
double PhysiologyThread::GetSodium() {
    return GetSodiumConcentration() * 0.43;
}

// Glucose - Glucose Concentration - mg/dL
double PhysiologyThread::GetGlucoseConcentration() {
    SESubstance *glucose = m_bg->GetSubstanceManager().GetSubstance("Glucose");
    return glucose->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// BUN - BloodUreaNitrogenConcentration - mg/dL
double PhysiologyThread::GetBUN() {
    return m_bg->GetBloodChemistrySystem()->GetBloodUreaNitrogenConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// Creatinine - Creatinine Concentration - mg/dL
double PhysiologyThread::GetCreatinineConcentration() {
    SESubstance *creatinine = m_bg->GetSubstanceManager().GetSubstance("Creatinine");
    return creatinine->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// RBC - White Blood Cell Count - ct/uL
double PhysiologyThread::GetWhiteBloodCellCount() {
    return m_bg->GetBloodChemistrySystem()->GetWhiteBloodCellCount(AmountPerVolumeUnit::ct_Per_uL);
}

// RBC - Red Blood Cell Count - ct/uL
double PhysiologyThread::GetRedBloodCellCount() {
    return m_bg->GetBloodChemistrySystem()->GetRedBloodCellCount(AmountPerVolumeUnit::ct_Per_uL);
}

// Hgb - Hemoglobin Concentration - g/dL
double PhysiologyThread::GetHemoglobinConcentration() {
    SESubstance *hemoglobin = m_bg->GetSubstanceManager().GetSubstance("Hemoglobin");
    return hemoglobin->GetBloodConcentration(MassPerVolumeUnit::g_Per_dL);
}

// Hct - Hematocrit - unitless
double PhysiologyThread::GetHematocrit() {
    return m_bg->GetBloodChemistrySystem()->GetHematocrit();
}

// pH - Blood pH - unitless
double PhysiologyThread::GetBloodPH() {
    return m_bg->GetBloodChemistrySystem()->GetBloodPH();
}

// PaCO2 - Arterial Carbon Dioxide Pressure - mmHg
double PhysiologyThread::GetArterialCarbonDioxidePressure() {
    return m_bg->GetBloodChemistrySystem()->GetArterialCarbonDioxidePressure(PressureUnit::mmHg);
}

// Pa02 - Arterial Oxygen Pressure - mmHg
double PhysiologyThread::GetArterialOxygenPressure() {
    return m_bg->GetBloodChemistrySystem()->GetArterialOxygenPressure(PressureUnit::mmHg);
}

// n/a - Bicarbonate Concentration - mg/L
double PhysiologyThread::GetBicarbonateConcentration() {
    SESubstance *bicarbonate = m_bg->GetSubstanceManager().GetSubstance("Bicarbonate");
    return bicarbonate->GetBloodConcentration(MassPerVolumeUnit::mg_Per_dL);
}

// HCO3 - Bicarbonate - Convert to mmol/L
double PhysiologyThread::GetBicarbonate() {
// SESubstance* bicarbonate = m_bg->GetSubstanceManager().GetSubstance("Bicarbonate");
    return GetBicarbonateConcentration() * 0.1639;
}

// BE - Base Excess -
double PhysiologyThread::GetBaseExcess() {
    return (0.93 * GetBicarbonate()) + (13.77 * GetBloodPH()) - 124.58;
}

double PhysiologyThread::GetCO2() {
    SEComprehensiveMetabolicPanel metabolicPanel(m_bg->GetLogger());
    m_bg->GetPatientAssessment(metabolicPanel);
    SEScalarAmountPerVolume CO2 = metabolicPanel.GetCO2();
    return CO2.GetValue(AmountPerVolumeUnit::mmol_Per_L);
}

double PhysiologyThread::GetPotassium() {
    SEComprehensiveMetabolicPanel metabolicPanel(m_bg->GetLogger());
    m_bg->GetPatientAssessment(metabolicPanel);
    SEScalarAmountPerVolume potassium = metabolicPanel.GetPotassium();
    return potassium.GetValue(AmountPerVolumeUnit::ct_Per_uL);
}

double PhysiologyThread::GetChloride() {
    SEComprehensiveMetabolicPanel metabolicPanel(m_bg->GetLogger());
    m_bg->GetPatientAssessment(metabolicPanel);
    SEScalarAmountPerVolume chloride = metabolicPanel.GetChloride();
    return chloride.GetValue(AmountPerVolumeUnit::ct_Per_uL);
}

// PLT - Platelet Count - ct/uL
double PhysiologyThread::GetPlateletCount() {
    SECompleteBloodCount CBC(m_bg->GetLogger());
    m_bg->GetPatientAssessment(CBC);
    SEScalarAmountPerVolume plateletCount = CBC.GetPlateletCount();
    return plateletCount.GetValue(AmountPerVolumeUnit::ct_Per_uL);
}

// GetExhaledCO2 - tracheal CO2 partial pressure - mmHg
double PhysiologyThread::GetExhaledCO2() {
    SESubstance *CO2 = m_bg->GetSubstanceManager().GetSubstance("CarbonDioxide");
    const SEGasCompartment *carina = m_bg->GetCompartments().GetGasCompartment(
            BGE::PulmonaryCompartment::Carina);
    return carina->GetSubstanceQuantity(*CO2)->GetPartialPressure(PressureUnit::mmHg);

}

// Get Tidal Volume - mL
double PhysiologyThread::GetTidalVolume() {
    return m_bg->GetRespiratorySystem()->GetTidalVolume(VolumeUnit::mL);
}

// Get Total Lung Volume - mL
double PhysiologyThread::GetTotalLungVolume() {
    return m_bg->GetRespiratorySystem()->GetTotalLungVolume(VolumeUnit::mL);
}

// Get Left Lung Volume - mL
double PhysiologyThread::GetLeftLungVolume() {
    const SEGasCompartment *leftLung = m_bg->GetCompartments().GetGasCompartment(
            BGE::PulmonaryCompartment::LeftLung);
    return leftLung->GetVolume(VolumeUnit::mL);
}

// Get Right Lung Volume - mL
double PhysiologyThread::GetRightLungVolume() {
    const SEGasCompartment *rightLung = m_bg->GetCompartments().GetGasCompartment(
            BGE::PulmonaryCompartment::RightLung);
    return rightLung->GetVolume(VolumeUnit::mL);
}

// Get Left Lung Pleural Cavity Volume - mL
double PhysiologyThread::GetLeftPleuralCavityVolume() {
    const SEGasCompartment *leftLung = m_bg->GetCompartments().GetGasCompartment(
            BGE::PulmonaryCompartment::LeftPleuralCavity);
    return leftLung->GetVolume(VolumeUnit::mL);
}

// Get Right Lung Pleural Cavity Volume - mL
double PhysiologyThread::GetRightPleuralCavityVolume() {
    const SEGasCompartment *rightLung = m_bg->GetCompartments().GetGasCompartment(
            BGE::PulmonaryCompartment::RightPleuralCavity);
    return rightLung->GetVolume(VolumeUnit::mL);
}

// Get left alveoli baseline compliance (?) volume
double PhysiologyThread::GetLeftAlveoliBaselineCompliance() {
    const SEGasCompartment *leftLung = m_bg->GetCompartments().GetGasCompartment(
            BGE::PulmonaryCompartment::LeftAlveoli);
    return leftLung->GetVolume(VolumeUnit::mL);

}

// Get right alveoli baseline compliance (?) volume
double PhysiologyThread::GetRightAlveoliBaselineCompliance() {
    const SEGasCompartment *rightLung = m_bg->GetCompartments().GetGasCompartment(
            BGE::PulmonaryCompartment::RightAlveoli);
    return rightLung->GetVolume(VolumeUnit::mL);
}

double PhysiologyThread::GetCardiacOutput() {
    return m_bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min);
}

void PhysiologyThread::Status() {
    m_bg->GetLogger()->Info("");
    m_bg->GetLogger()->Info(std::stringstream() << "Simulation Time : " << m_bg->GetSimulationTime(TimeUnit::s) << "s");
    m_bg->GetLogger()->Info(
            std::stringstream() << "Cardiac Output : "
                                << m_bg->GetCardiovascularSystem()->GetCardiacOutput(VolumePerTimeUnit::mL_Per_min)
                                << VolumePerTimeUnit::mL_Per_min);
    m_bg->GetLogger()->Info(
            std::stringstream() << "Blood Volume : " << m_bg->GetCardiovascularSystem()->GetBloodVolume(VolumeUnit::mL)
                                << VolumeUnit::mL);
    m_bg->GetLogger()->Info(
            std::stringstream() << "Mean Arterial Pressure : "
                                << m_bg->GetCardiovascularSystem()->GetMeanArterialPressure(PressureUnit::mmHg)
                                << PressureUnit::mmHg);
    m_bg->GetLogger()->Info(
            std::stringstream() << "Systolic Pressure : "
                                << m_bg->GetCardiovascularSystem()->GetSystolicArterialPressure(PressureUnit::mmHg)
                                << PressureUnit::mmHg);
    m_bg->GetLogger()->Info(
            std::stringstream() << "Diastolic Pressure : "
                                << m_bg->GetCardiovascularSystem()->GetDiastolicArterialPressure(PressureUnit::mmHg)
                                << PressureUnit::mmHg);
    m_bg->GetLogger()->Info(
            std::stringstream() << "Heart Rate : "
                                << m_bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min)
                                << "bpm");
    m_bg->GetLogger()->Info(
            std::stringstream() << "Respiration Rate : "
                                << m_bg->GetRespiratorySystem()->GetRespirationRate(FrequencyUnit::Per_min)
                                << "bpm");
    m_bg->GetLogger()->Info("");
}
