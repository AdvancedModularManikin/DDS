#include "PhysiologyThread.h"

namespace AMM {
    using namespace AMM::Physiology;
    std::vector <std::string> PhysiologyThread::highFrequencyNodes;
    std::map<std::string, double (PhysiologyThread::*)()> PhysiologyThread::nodePathTable;

    PhysiologyThread::PhysiologyThread(const std::string &logFile) {
        m_pe = biogears::CreateBioGearsEngine(logFile);


        PopulateNodePathTable();
        m_runThread = false;
    }

    PhysiologyThread::~PhysiologyThread() {
        m_runThread = false;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    void PhysiologyThread::PopulateNodePathTable() {
        highFrequencyNodes.clear();
        nodePathTable.clear();

        // Legacy values
        nodePathTable["ECG"] = &PhysiologyThread::GetECGWaveform;
        nodePathTable["HR"] = &PhysiologyThread::GetHeartRate;
        nodePathTable["SIM_TIME"] = &PhysiologyThread::GetSimulationTime;

        // Cardiovascular System
        nodePathTable["Cardiovascular_HeartRate"] = &PhysiologyThread::GetHeartRate;
        nodePathTable["Cardiovascular_BloodVolume"] = &PhysiologyThread::GetBloodVolume;
        nodePathTable["Cardiovascular_BloodLossPercentage"] = &PhysiologyThread::GetBloodLossPercentage;
        nodePathTable["Cardiovascular_Arterial_Pressure"] = &PhysiologyThread::GetArterialPressure;
        nodePathTable["Cardiovascular_Arterial_Mean_Pressure"] = &PhysiologyThread::GetMeanArterialPressure;
        nodePathTable["Cardiovascular_Arterial_Systolic_Pressure"] = &PhysiologyThread::GetArterialSystolicPressure;
        nodePathTable["Cardiovascular_Arterial_Diastolic_Pressure"] = &PhysiologyThread::GetArterialDiastolicPressure;
        nodePathTable["Cardiovascular_CentralVenous_Mean_Pressure"] = &PhysiologyThread::GetMeanCentralVenousPressure;
        nodePathTable["Cardiovascular_CardiacOutput"] = &PhysiologyThread::GetCardiacOutput;

        //  Respiratory System
        nodePathTable["Respiratory_Respiration_Rate"] = &PhysiologyThread::GetRespirationRate;
        nodePathTable["Respiration_EndTidalCarbonDioxide"] = &PhysiologyThread::GetEndTidalCarbonDioxideFraction;
        nodePathTable["Respiratory_Tidal_Volume"] = &PhysiologyThread::GetTidalVolume;
        nodePathTable["Respiratory_LungTotal_Volume"] = &PhysiologyThread::GetTotalLungVolume;
        nodePathTable["Respiratory_LeftPleuralCavity_Volume"] = &PhysiologyThread::GetLeftPleuralCavityVolume;
        nodePathTable["Respiratory_LeftLung_Volume"] = &PhysiologyThread::GetLeftLungVolume;
        nodePathTable["Respiratory_LeftAlveoli_BaseCompliance"] = &PhysiologyThread::GetLeftAlveoliBaselineCompliance;
        nodePathTable["Respiratory_RightPleuralCavity_Volume"] = &PhysiologyThread::GetRightPleuralCavityVolume;
        nodePathTable["Respiratory_RightLung_Volume"] = &PhysiologyThread::GetRightLungVolume;

        nodePathTable["Respiratory_LeftLung_Tidal_Volume"] = &PhysiologyThread::GetLeftLungTidalVolume;
        nodePathTable["Respiratory_RightLung_Tidal_Volume"] = &PhysiologyThread::GetRightLungTidalVolume;


        nodePathTable["Respiratory_RightAlveoli_BaseCompliance"] = &PhysiologyThread::GetRightAlveoliBaselineCompliance;
        nodePathTable["Respiratory_CarbonDioxide_Exhaled"] = &PhysiologyThread::GetExhaledCO2;

        // Energy system
        nodePathTable["Energy_Core_Temperature"] = &PhysiologyThread::GetCoreTemperature;

        // Nervous
        nodePathTable["Nervous_GetPainVisualAnalogueScale"] = &PhysiologyThread::GetPainVisualAnalogueScale;

        // Blood chemistry system
        nodePathTable["BloodChemistry_WhiteBloodCell_Count"] = &PhysiologyThread::GetWhiteBloodCellCount;
        nodePathTable["BloodChemistry_RedBloodCell_Count"] = &PhysiologyThread::GetRedBloodCellCount;
        nodePathTable["BloodChemistry_BloodUreaNitrogen_Concentration"] = &PhysiologyThread::GetBUN;
        nodePathTable["BloodChemistry_Oxygen_Saturation"] = &PhysiologyThread::GetOxygenSaturation;
        nodePathTable["BloodChemistry_Hemaocrit"] = &PhysiologyThread::GetHematocrit;
        nodePathTable["BloodChemistry_BloodPH"] = &PhysiologyThread::GetBloodPH;
        nodePathTable["BloodChemistry_Arterial_CarbonDioxide_Pressure"] = &PhysiologyThread::GetArterialCarbonDioxidePressure;
        nodePathTable["BloodChemistry_Arterial_Oxygen_Pressure"] = &PhysiologyThread::GetArterialOxygenPressure;
        nodePathTable["BloodChemistry_VenousOxygenPressure"] = &PhysiologyThread::GetVenousOxygenPressure;
        nodePathTable["BloodChemistry_VenousCarbonDioxidePressure"] = &PhysiologyThread::GetVenousCarbonDioxidePressure;

        // Substances
        nodePathTable["Substance_Sodium"] = &PhysiologyThread::GetSodium;
        nodePathTable["Substance_Sodium_Concentration"] = &PhysiologyThread::GetSodiumConcentration;
        nodePathTable["Substance_Bicarbonate"] = &PhysiologyThread::GetBicarbonate;
        nodePathTable["Substance_Bicarbonate_Concentration"] = &PhysiologyThread::GetBicarbonateConcentration;
        nodePathTable["Substance_BaseExcess"] = &PhysiologyThread::GetBaseExcess;
        nodePathTable["Substance_Glucose_Concentration"] = &PhysiologyThread::GetGlucoseConcentration;
        nodePathTable["Substance_Creatinine_Concentration"] = &PhysiologyThread::GetCreatinineConcentration;
        nodePathTable["Substance_Hemoglobin_Concentration"] = &PhysiologyThread::GetHemoglobinConcentration;
        nodePathTable["Substance_Calcium_Concentration"] = &PhysiologyThread::GetCalciumConcentration;
        nodePathTable["Substance_Albumin_Concentration"] = &PhysiologyThread::GetAlbuminConcentration;
        nodePathTable["Substance_Lactate_Concentration"] = &PhysiologyThread::GetLactateConcentration;
        nodePathTable["Substance_Lactate_Concentration_mmol"] = &PhysiologyThread::GetLactateConcentrationMMOL;

        nodePathTable["MetabolicPanel_Bilirubin"] = &PhysiologyThread::GetTotalBilirubin;
        nodePathTable["MetabolicPanel_Protein"] = &PhysiologyThread::GetTotalProtein;
        nodePathTable["MetabolicPanel_CarbonDioxide"] = &PhysiologyThread::GetCO2;
        nodePathTable["MetabolicPanel_Potassium"] = &PhysiologyThread::GetPotassium;
        nodePathTable["MetabolicPanel_Chloride"] = &PhysiologyThread::GetChloride;

        nodePathTable["CompleteBloodCount_Platelet"] = &PhysiologyThread::GetPlateletCount;

        nodePathTable["Renal_UrineProductionRate"] = &PhysiologyThread::GetUrineProductionRate;
        nodePathTable["Urinalysis_SpecificGravity"] = &PhysiologyThread::GetUrineSpecificGravity;
        nodePathTable["Renal_UrineOsmolality"] = &PhysiologyThread::GetUrineOsmolality;
        nodePathTable["Renal_UrineOsmolarity"] = &PhysiologyThread::GetUrineOsmolarity;
        nodePathTable["Renal_BladderGlucose"] = &PhysiologyThread::GetBladderGlucose;

        // Label which nodes are high-frequency
        highFrequencyNodes = {
                "ECG",
                "Cardiovascular_HeartRate",
                "Respiratory_LeftLung_Volume",
                "Respiratory_RightLung_Volume",
                "Cardiovascular_Arterial_Pressure",
                "Respiratory_CarbonDioxide_Exhaled",
                "Respiratory_Respiration_Rate"
        };

    }


    std::map<std::string, double (PhysiologyThread::*)()> *PhysiologyThread::GetNodePathTable() {
        return &nodePathTable;
    }

    void PhysiologyThread::Shutdown() {

    }

    void PhysiologyThread::StartSimulation() {
        if (!m_runThread) {
            m_runThread = true;
            //     m_thread = std::thread(&PhysiologyThread::AdvanceTime, this);
        }
    }

    void PhysiologyThread::StopSimulation() {
        if (m_runThread) {
            m_runThread = false;
            //   m_thread.join();
        }
    }


    bool PhysiologyThread::LoadState(const std::string &stateFile, double sec) {
        biogears::SEScalarTime startTime;
        startTime.SetValue(sec, biogears::TimeUnit::s);

        LOG_INFO << "Loading state file " << stateFile << " at position " << sec << " seconds";
        if (!m_pe->LoadState(stateFile, &startTime)) {
            LOG_ERROR << "Error initializing state";
            return false;
        }

        // preload substances
        sodium = m_pe->GetSubstanceManager().GetSubstance("Sodium");
        glucose = m_pe->GetSubstanceManager().GetSubstance("Glucose");
        creatinine = m_pe->GetSubstanceManager().GetSubstance("Creatinine");
        calcium = m_pe->GetSubstanceManager().GetSubstance("Calcium");
        hemoglobin = m_pe->GetSubstanceManager().GetSubstance("Hemoglobin");
        bicarbonate = m_pe->GetSubstanceManager().GetSubstance("Bicarbonate");
        albumin = m_pe->GetSubstanceManager().GetSubstance("Albumin");
        CO2 = m_pe->GetSubstanceManager().GetSubstance("CarbonDioxide");
        N2 = m_pe->GetSubstanceManager().GetSubstance("Nitrogen");
        O2 = m_pe->GetSubstanceManager().GetSubstance("Oxygen");
        CO = m_pe->GetSubstanceManager().GetSubstance("CarbonMonoxide");
        potassium = m_pe->GetSubstanceManager().GetSubstance("Potassium");
        chloride = m_pe->GetSubstanceManager().GetSubstance("Chloride");
        lactate = m_pe->GetSubstanceManager().GetSubstance("Lactate");

        // preload compartments
        carina = m_pe->GetCompartments().GetGasCompartment(BGE::PulmonaryCompartment::Carina);
        leftLung = m_pe->GetCompartments().GetGasCompartment(BGE::PulmonaryCompartment::LeftLung);
        rightLung = m_pe->GetCompartments().GetGasCompartment(BGE::PulmonaryCompartment::RightLung);
        bladder = m_pe->GetCompartments().GetLiquidCompartment(BGE::UrineCompartment::Bladder);

        startingBloodVolume = 123456.0;
        currentBloodVolume = 0.0;

        if (logging_enabled) {
            InitializeLog();
        }

        return true;
    }

    void PhysiologyThread::InitializeLog() {
        std::string logFilename = Utility::getTimestampedFilename("./logs/AMM_Output_", ".csv");
        LOG_INFO << "Initializing log file: " << logFilename;

        std::fstream fs;
        fs.open(logFilename, std::ios::out);
        fs.close();

        m_pe->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("HeartRate",
                                                                                          biogears::FrequencyUnit::Per_min);
        m_pe->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("MeanArterialPressure",
                                                                                          biogears::PressureUnit::mmHg);
        m_pe->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set(
                "SystolicArterialPressure", biogears::PressureUnit::mmHg);
        m_pe->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set(
                "DiastolicArterialPressure", biogears::PressureUnit::mmHg);
        m_pe->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("RespirationRate",
                                                                                          biogears::FrequencyUnit::Per_min);
        m_pe->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("TidalVolume",
                                                                                          biogears::VolumeUnit::mL);
        m_pe->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("TotalLungVolume",
                                                                                          biogears::VolumeUnit::mL);
        m_pe->GetEngineTrack()->GetDataRequestManager().CreateGasCompartmentDataRequest().Set(
                BGE::PulmonaryCompartment::LeftLung, "Volume");
        m_pe->GetEngineTrack()->GetDataRequestManager().CreateGasCompartmentDataRequest().Set(
                BGE::PulmonaryCompartment::RightLung, "Volume");
        m_pe->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("OxygenSaturation");
        m_pe->GetEngineTrack()->GetDataRequestManager().CreateGasCompartmentDataRequest().Set(
                BGE::PulmonaryCompartment::Carina, "InFlow");
        m_pe->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("BloodVolume",
                                                                                          biogears::VolumeUnit::mL);
        m_pe->GetEngineTrack()->GetDataRequestManager().CreatePhysiologyDataRequest().Set("ArterialBloodPH");
        biogears::SESubstance *logLactate = m_pe->GetSubstanceManager().GetSubstance("Lactate");
        m_pe->GetEngineTrack()->GetDataRequestManager().CreateSubstanceDataRequest().Set(*logLactate,
                                                                                         "BloodConcentration",
                                                                                         biogears::MassPerVolumeUnit::ug_Per_mL);
        m_pe->GetEngineTrack()->GetDataRequestManager().SetResultsFilename(logFilename);
    }

    bool PhysiologyThread::SaveState(const std::string &stateFile) {
        m_pe->SaveState(stateFile);
        return true;
    }

    bool PhysiologyThread::ExecuteCommand(const std::string &cmd) {
        std::string scenarioFile = "Actions/" + cmd + ".xml";
        return LoadScenarioFile(scenarioFile);
    }

    bool PhysiologyThread::Execute(std::function<std::unique_ptr<biogears::PhysiologyEngine>(
            std::unique_ptr < biogears::PhysiologyEngine > && )> func) {
        m_pe = func(std::move(m_pe));
        return true;
    }

    bool PhysiologyThread::ExecuteXMLCommand(const std::string &cmd) {
        char *tmpname = strdup("/tmp/tmp_amm_xml_XXXXXX");
        mkstemp(tmpname);
        std::ofstream out(tmpname);
        out << cmd;
        out.close();
        return LoadScenarioFile(tmpname);
    }


    // Load a scenario from an XML file, apply conditions and iterate through the actions
// This bypasses the standard BioGears ExecuteScenario method to avoid resetting the BioGears engine
    bool PhysiologyThread::LoadScenarioFile(const std::string &scenarioFile) {
        biogears::SEScenario sce(m_pe->GetSubstanceManager());
        sce.Load(scenarioFile);

        double dT_s = m_pe->GetTimeStep(biogears::TimeUnit::s);
        // double scenarioTime_s;

        double sampleTime_s = sce.GetDataRequestManager().GetSamplesPerSecond();
        if (sampleTime_s != 0)
            sampleTime_s = 1 / sampleTime_s;
        double currentSampleTime_s = sampleTime_s;        //Sample the first step

        biogears::SEAdvanceTime *adv;
        for (biogears::SEAction *a : sce.GetActions()) {
            adv = dynamic_cast<biogears::SEAdvanceTime *>(a);
            if (adv != nullptr) {
                double time_s = adv->GetTime(biogears::TimeUnit::s);
                auto count = (int) (time_s / dT_s);
                for (int i = 0; i <= count; i++) {
                    m_pe->AdvanceModelTime();
                    currentSampleTime_s += dT_s;
                    if (currentSampleTime_s >= sampleTime_s) {
                        currentSampleTime_s = 0;
                    }
                }
                continue;
            } else {
                m_pe->ProcessAction(*a);
            }
        }
        return true;
    }

    void PhysiologyThread::AdvanceTimeTick() {
        m_mutex.lock();
        try {
            m_pe->AdvanceModelTime();
        } catch (std::exception &e) {
            LOG_ERROR << "Error advancing time: " << e.what();
            /*const boost::stacktrace::stacktrace* st = boost::get_error_info<traced>(e);
            if (st) {
               LOG_ERROR << *st;
            }*/
        }

        if (logging_enabled) {
            m_pe->GetEngineTrack()->TrackData(m_pe->GetSimulationTime(biogears::TimeUnit::s));
        }
        m_mutex.unlock();
    }

    double PhysiologyThread::GetShutdownMessage() {
        return -1;
    }

    double PhysiologyThread::GetSimulationTime() {
        return m_pe->GetSimulationTime(biogears::TimeUnit::s);
    }

    double PhysiologyThread::GetNodePath(const std::string &nodePath) {
        std::map<std::string, double (PhysiologyThread::*)()>::iterator entry;
        entry = nodePathTable.find(nodePath);
        if (entry != nodePathTable.end()) {
            return (this->*(entry->second))();
        }

        LOG_ERROR << "Unable to access nodepath " << nodePath;
        return 0;

    }

    double PhysiologyThread::GetBloodVolume() {
        currentBloodVolume = m_pe->GetCardiovascularSystem()->GetBloodVolume(biogears::VolumeUnit::mL);
        if (startingBloodVolume == 123456.0) {
            startingBloodVolume = currentBloodVolume;
        }
        return currentBloodVolume;
    }

    double PhysiologyThread::GetBloodLossPercentage() {
        double loss = (startingBloodVolume - currentBloodVolume) / startingBloodVolume;
        return loss;
    }

    double PhysiologyThread::GetHeartRate() {
        return m_pe->GetCardiovascularSystem()->GetHeartRate(biogears::FrequencyUnit::Per_min);
    }

// SYS (ART) - Arterial Systolic Pressure - mmHg
    double PhysiologyThread::GetArterialSystolicPressure() {
        return m_pe->GetCardiovascularSystem()->GetSystolicArterialPressure(biogears::PressureUnit::mmHg);
    }

// DIA (ART) - Arterial Diastolic Pressure - mmHg
    double PhysiologyThread::GetArterialDiastolicPressure() {
        return m_pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(biogears::PressureUnit::mmHg);
    }

// MAP (ART) - Mean Arterial Pressure - mmHg
    double PhysiologyThread::GetMeanArterialPressure() {
        return m_pe->GetCardiovascularSystem()->GetMeanArterialPressure(biogears::PressureUnit::mmHg);
    }

// AP - Arterial Pressure - mmHg
    double PhysiologyThread::GetArterialPressure() {
        return m_pe->GetCardiovascularSystem()->GetArterialPressure(biogears::PressureUnit::mmHg);
    }

// CVP - Central Venous Pressure - mmHg
    double PhysiologyThread::GetMeanCentralVenousPressure() {
        return m_pe->GetCardiovascularSystem()->GetMeanCentralVenousPressure(biogears::PressureUnit::mmHg);
    }

// MCO2 - End Tidal Carbon Dioxide Fraction - unitless %
    double PhysiologyThread::GetEndTidalCarbonDioxideFraction() {
        return (m_pe->GetRespiratorySystem()->GetEndTidalCarbonDioxideFraction() * 7.5);
    }

// SPO2 - Oxygen Saturation - unitless %
    double PhysiologyThread::GetOxygenSaturation() {
        return m_pe->GetBloodChemistrySystem()->GetOxygenSaturation() * 100;
    }

// BR - Respiration Rate - per minute
    double PhysiologyThread::GetRespirationRate() {
        double rr = m_pe->GetRespiratorySystem()->GetRespirationRate(biogears::FrequencyUnit::Per_min);
        double loss = GetBloodLossPercentage();
        rr = rr * (1 + 3 * std::max(0.0, loss - 0.2));
        return rr;
    }

// T2 - Core Temperature - degrees C
    double PhysiologyThread::GetCoreTemperature() {
        return m_pe->GetEnergySystem()->GetCoreTemperature(biogears::TemperatureUnit::C);
    }

// ECG Waveform in mV
    double PhysiologyThread::GetECGWaveform() {
        double ecgLead3_mV = m_pe->GetElectroCardioGram()->GetLead3ElectricPotential(
                biogears::ElectricPotentialUnit::mV);
        return ecgLead3_mV;
    }

// Na+ - Sodium Concentration - mg/dL
    double PhysiologyThread::GetSodiumConcentration() {
        return sodium->GetBloodConcentration(biogears::MassPerVolumeUnit::mg_Per_dL);
    }

// Na+ - Sodium - mmol/L
    double PhysiologyThread::GetSodium() {
        return GetSodiumConcentration() * 0.43;
    }

// Glucose - Glucose Concentration - mg/dL
    double PhysiologyThread::GetGlucoseConcentration() {
        return glucose->GetBloodConcentration(biogears::MassPerVolumeUnit::mg_Per_dL);
    }

// BUN - BloodUreaNitrogenConcentration - mg/dL
    double PhysiologyThread::GetBUN() {
        return m_pe->GetBloodChemistrySystem()->GetBloodUreaNitrogenConcentration(
                biogears::MassPerVolumeUnit::mg_Per_dL);
    }

// Creatinine - Creatinine Concentration - mg/dL
    double PhysiologyThread::GetCreatinineConcentration() {
        return creatinine->GetBloodConcentration(biogears::MassPerVolumeUnit::mg_Per_dL);
    }

    double PhysiologyThread::GetCalciumConcentration() {
        return calcium->GetBloodConcentration(biogears::MassPerVolumeUnit::mg_Per_dL);
    }

    double PhysiologyThread::GetAlbuminConcentration() {
        return albumin->GetBloodConcentration(biogears::MassPerVolumeUnit::g_Per_dL);
    }

    double PhysiologyThread::GetLactateConcentration() {
        lactateConcentration = lactate->GetBloodConcentration(biogears::MassPerVolumeUnit::g_Per_dL);
        return lactateConcentration;
    }

    double PhysiologyThread::GetLactateConcentrationMMOL() {
        return (lactateConcentration * 0.1110) * 1000;
    }


    double PhysiologyThread::GetTotalBilirubin() {
        return m_pe->GetBloodChemistrySystem()->GetTotalBilirubin(biogears::MassPerVolumeUnit::mg_Per_dL);
    }

    double PhysiologyThread::GetTotalProtein() {
        biogears::SEComprehensiveMetabolicPanel metabolicPanel(m_pe->GetLogger());
        m_pe->GetPatientAssessment(metabolicPanel);
        biogears::SEScalarMassPerVolume protein = metabolicPanel.GetTotalProtein();
        return protein.GetValue(biogears::MassPerVolumeUnit::g_Per_dL);
    }

// RBC - White Blood Cell Count - ct/uL
    double PhysiologyThread::GetWhiteBloodCellCount() {
        return m_pe->GetBloodChemistrySystem()->GetWhiteBloodCellCount(biogears::AmountPerVolumeUnit::ct_Per_uL) / 1000;
    }

// RBC - Red Blood Cell Count - ct/uL
    double PhysiologyThread::GetRedBloodCellCount() {
        return m_pe->GetBloodChemistrySystem()->GetRedBloodCellCount(biogears::AmountPerVolumeUnit::ct_Per_uL) /
               1000000;
    }

// Hgb - Hemoglobin Concentration - g/dL
    double PhysiologyThread::GetHemoglobinConcentration() {
        return hemoglobin->GetBloodConcentration(biogears::MassPerVolumeUnit::g_Per_dL);
    }

// Hct - Hematocrit - unitless
    double PhysiologyThread::GetHematocrit() {
        return m_pe->GetBloodChemistrySystem()->GetHematocrit() * 100;
    }

// pH - Blood pH - unitless
    double PhysiologyThread::GetBloodPH() {
        double venousbloodPH = m_pe->GetBloodChemistrySystem()->GetVenousBloodPH();
        bloodPH = venousbloodPH + 0.02 * std::min((1.5 - lactateConcentration), 0.0);
        return bloodPH;
    }

// PaCO2 - Arterial Carbon Dioxide Pressure - mmHg
    double PhysiologyThread::GetArterialCarbonDioxidePressure() {
        return m_pe->GetBloodChemistrySystem()->GetArterialCarbonDioxidePressure(biogears::PressureUnit::mmHg);
    }

// Pa02 - Arterial Oxygen Pressure - mmHg
    double PhysiologyThread::GetArterialOxygenPressure() {
        return m_pe->GetBloodChemistrySystem()->GetArterialOxygenPressure(biogears::PressureUnit::mmHg);
    }

    double PhysiologyThread::GetVenousOxygenPressure() {
        return m_pe->GetBloodChemistrySystem()->GetVenousOxygenPressure(biogears::PressureUnit::mmHg);
    }

    double PhysiologyThread::GetVenousCarbonDioxidePressure() {
        return m_pe->GetBloodChemistrySystem()->GetVenousCarbonDioxidePressure(biogears::PressureUnit::mmHg);
    }

// n/a - Bicarbonate Concentration - mg/L
    double PhysiologyThread::GetBicarbonateConcentration() {
        return bicarbonate->GetBloodConcentration(biogears::MassPerVolumeUnit::mg_Per_dL);
    }

// HCO3 - Bicarbonate - Convert to mmol/L
    double PhysiologyThread::GetBicarbonate() {
        return GetBicarbonateConcentration() * 0.1639;
    }

// BE - Base Excess -
    double PhysiologyThread::GetBaseExcess() {
        return (0.93 * GetBicarbonate()) + (13.77 * GetBloodPH()) - 124.58;
    }

    double PhysiologyThread::GetCO2() {
        biogears::SEComprehensiveMetabolicPanel metabolicPanel(m_pe->GetLogger());
        m_pe->GetPatientAssessment(metabolicPanel);
        biogears::SEScalarAmountPerVolume CO2 = metabolicPanel.GetCO2();
        return CO2.GetValue(biogears::AmountPerVolumeUnit::mmol_Per_L);
    }

    double PhysiologyThread::GetPotassium() {
        biogears::SEComprehensiveMetabolicPanel metabolicPanel(m_pe->GetLogger());
        m_pe->GetPatientAssessment(metabolicPanel);
        biogears::SEScalarAmountPerVolume potassium = metabolicPanel.GetPotassium();
        return potassium.GetValue(biogears::AmountPerVolumeUnit::mmol_Per_L);
    }

    double PhysiologyThread::GetChloride() {
        biogears::SEComprehensiveMetabolicPanel metabolicPanel(m_pe->GetLogger());
        m_pe->GetPatientAssessment(metabolicPanel);
        biogears::SEScalarAmountPerVolume chloride = metabolicPanel.GetChloride();
        return chloride.GetValue(biogears::AmountPerVolumeUnit::mmol_Per_L);
    }

// PLT - Platelet Count - ct/uL
    double PhysiologyThread::GetPlateletCount() {
        biogears::SECompleteBloodCount CBC(m_pe->GetLogger());
        m_pe->GetPatientAssessment(CBC);
        biogears::SEScalarAmountPerVolume plateletCount = CBC.GetPlateletCount();
        return plateletCount.GetValue(biogears::AmountPerVolumeUnit::ct_Per_uL) / 1000;
    }

    double PhysiologyThread::GetUrineProductionRate() {
        return m_pe->GetRenalSystem()->GetUrineProductionRate(biogears::VolumePerTimeUnit::mL_Per_min);
    }

    double PhysiologyThread::GetUrineSpecificGravity() {
        return m_pe->GetRenalSystem()->GetUrineSpecificGravity();
    }

    double PhysiologyThread::GetBladderGlucose() {
        return bladder->GetSubstanceQuantity(*glucose)->GetConcentration().GetValue(
                biogears::MassPerVolumeUnit::mg_Per_dL);
    }


    double PhysiologyThread::GetUrineOsmolality() {
        return m_pe->GetRenalSystem()->GetUrineOsmolality(biogears::OsmolalityUnit::mOsm_Per_kg);

    }

    double PhysiologyThread::GetUrineOsmolarity() {
        return m_pe->GetRenalSystem()->GetUrineOsmolarity(biogears::OsmolarityUnit::mOsm_Per_L);
    }


    // GetExhaledCO2 - tracheal CO2 partial pressure - mmHg
    double PhysiologyThread::GetExhaledCO2() {
        return carina->GetSubstanceQuantity(*CO2)->GetPartialPressure(biogears::PressureUnit::mmHg);
    }

// Get Tidal Volume - mL
    double PhysiologyThread::GetTidalVolume() {
        return m_pe->GetRespiratorySystem()->GetTidalVolume(biogears::VolumeUnit::mL);
    }

// Get Total Lung Volume - mL
    double PhysiologyThread::GetTotalLungVolume() {
        return m_pe->GetRespiratorySystem()->GetTotalLungVolume(biogears::VolumeUnit::mL);
    }

// Get Left Lung Volume - mL
    double PhysiologyThread::GetLeftLungVolume() {
        lung_vol_L = leftLung->GetVolume(biogears::VolumeUnit::mL);
        return lung_vol_L;
    }

// Get Right Lung Volume - mL
    double PhysiologyThread::GetRightLungVolume() {
        lung_vol_R = rightLung->GetVolume(biogears::VolumeUnit::mL);
        return lung_vol_R;
    }

    // Calculate and fetch left lung tidal volume
    double PhysiologyThread::GetLeftLungTidalVolume() {
        if (falling_L) {
            if (lung_vol_L < new_min_L) new_min_L = lung_vol_L;
            else if (lung_vol_L > new_min_L + thresh) {
                falling_L = false;
                min_lung_vol_L = new_min_L;
                new_min_L = 1500.0;
                leftLungTidalVol = max_lung_vol_L - min_lung_vol_L;

                chestrise_pct_L = leftLungTidalVol * 100 / 300; // scale tidal volume to percent of max chest rise
                if (chestrise_pct_L > 100) chestrise_pct_L = 100;    // clamp the value to 0 <= chestrise_pct <= 100
                if (chestrise_pct_L < 0) chestrise_pct_L = 0;
            }
        } else {
            if (lung_vol_L > new_max_L) new_max_L = lung_vol_L;
            else if (lung_vol_L < new_max_L - thresh) {
                falling_L = true;
                max_lung_vol_L = new_max_L;
                new_max_L = 0.0;
                leftLungTidalVol = max_lung_vol_L - min_lung_vol_L;

                chestrise_pct_L = leftLungTidalVol * 100 / 300; // scale tidal volume to percent of max chest rise
                if (chestrise_pct_L > 100) chestrise_pct_L = 100;    // clamp the value to 0 <= chestrise_pct <= 100
                if (chestrise_pct_L < 0) chestrise_pct_L = 0;
            }
        }
        return leftLungTidalVol;
    }

    // Calculate and fetch right lung tidal volume
    double PhysiologyThread::GetRightLungTidalVolume() {
        if (falling_R) {
            if (lung_vol_R < new_min_R) new_min_R = lung_vol_R;
            else if (lung_vol_R > new_min_R + thresh) {
                falling_R = false;
                min_lung_vol_R = new_min_R;
                new_min_R = 1500.0;
                leftLungTidalVol = max_lung_vol_R - min_lung_vol_R;

                chestrise_pct_R = rightLungTidalVol * 100 / 300; // scale tidal volume to percent of max chest rise
                if (chestrise_pct_R > 100) chestrise_pct_R = 100;    // clamp the value to 0 <= chestrise_pct <= 100
                if (chestrise_pct_R < 0) chestrise_pct_R = 0;
            }
        } else {
            if (lung_vol_R > new_max_R) new_max_R = lung_vol_R;
            else if (lung_vol_R < new_max_R - thresh) {
                falling_R = true;
                max_lung_vol_R = new_max_R;
                new_max_R = 0.0;
                rightLungTidalVol = max_lung_vol_R - min_lung_vol_R;

                chestrise_pct_R = rightLungTidalVol * 100 / 300; // scale tidal volume to percent of max chest rise
                if (chestrise_pct_R > 100) chestrise_pct_R = 100;    // clamp the value to 0 <= chestrise_pct <= 100
                if (chestrise_pct_R < 0) chestrise_pct_R = 0;
            }
        }
        return rightLungTidalVol;
    }


    // Get Left Lung Pleural Cavity Volume - mL
    double PhysiologyThread::GetLeftPleuralCavityVolume() {
        return leftLung->GetVolume(biogears::VolumeUnit::mL);
    }

// Get Right Lung Pleural Cavity Volume - mL
    double PhysiologyThread::GetRightPleuralCavityVolume() {
        return rightLung->GetVolume(biogears::VolumeUnit::mL);
    }

// Get left alveoli baseline compliance (?) volume
    double PhysiologyThread::GetLeftAlveoliBaselineCompliance() {
        return leftLung->GetVolume(biogears::VolumeUnit::mL);
    }

// Get right alveoli baseline compliance (?) volume
    double PhysiologyThread::GetRightAlveoliBaselineCompliance() {
        return rightLung->GetVolume(biogears::VolumeUnit::mL);
    }

    double PhysiologyThread::GetCardiacOutput() {
        return m_pe->GetCardiovascularSystem()->GetCardiacOutput(biogears::VolumePerTimeUnit::mL_Per_min);
    }

    double PhysiologyThread::GetPainVisualAnalogueScale() {
        return m_pe->GetNervousSystem()->GetPainVisualAnalogueScale();
    }

    void PhysiologyThread::SetIVPump(const std::string &pumpSettings) {
        LOG_DEBUG << "Got pump settings: " << pumpSettings;
        std::string type, concentration, rate, dose, substance, bagVolume;
        std::vector <std::string> strings = Utility::explode("\n", pumpSettings);

        for (auto str : strings) {
            std::vector <std::string> strs;
            boost::split(strs, str, boost::is_any_of("="));
            auto strs_size = strs.size();
            // Check if it's not a key value pair
            if (strs_size != 2) {
                continue;
            }
            std::string kvp_k = strs[0];
            // all settings for the pump are strings
            std::string kvp_v = strs[1];

            try {
                if (kvp_k == "type") {
                    type = kvp_v;
                } else if (kvp_k == "substance") {
                    substance = kvp_v;
                } else if (kvp_k == "concentration") {
                    concentration = kvp_v;
                } else if (kvp_k == "rate") {
                    rate = kvp_v;
                } else if (kvp_k == "dose") {
                    dose = kvp_v;
                } else if (kvp_k == "amount") {
                    dose = kvp_v;
                } else if (kvp_k == "bagVolume") {
                    bagVolume = kvp_v;
                } else {
                    LOG_INFO << "Unknown pump setting: " << kvp_k << " = " << kvp_v;
                }
            } catch (std::exception &e) {
                LOG_ERROR << "Issue with setting " << e.what();
            }
        }

        try {
            if (type == "infusion") {
                std::string concentrationsMass, concentrationsVol, rateUnit, massUnit, volUnit;
                double rateVal, massVal, volVal, conVal;

                if (substance == "Saline" || substance == "Whole Blood" || substance == "Blood") {
                    if (substance == "Whole Blood") {
                        substance = "Blood";
                    }
                    biogears::SESubstanceCompound *subs = m_pe->GetSubstanceManager().GetCompound(substance);
                    biogears::SESubstanceCompoundInfusion infuse(*subs);
                    std::vector <std::string> bagvol = Utility::explode(" ", bagVolume);
                    volVal = std::stod(bagvol[0]);
                    volUnit = bagvol[1];
                    LOG_DEBUG << "Setting bag volume to " << volVal << " / " << volUnit;
                    if (volUnit == "mL") {
                        infuse.GetBagVolume().SetValue(volVal, biogears::VolumeUnit::mL);
                    } else {
                        infuse.GetBagVolume().SetValue(volVal, biogears::VolumeUnit::L);
                    }

                    std::vector <std::string> rateb = Utility::explode(" ", rate);
                    rateVal = std::stod(rateb[0]);
                    rateUnit = rateb[1];

                    if (rateUnit == "mL/hr") {
                        LOG_DEBUG << "Infusing at " << rateVal << " mL per hour";
                        infuse.GetRate().SetValue(rateVal, biogears::VolumePerTimeUnit::mL_Per_hr);
                    } else {
                        LOG_DEBUG << "Infusing at " << rateVal << " mL per min";
                        infuse.GetRate().SetValue(rateVal, biogears::VolumePerTimeUnit::mL_Per_min);
                    }
                    m_pe->ProcessAction(infuse);
                } else {
                    biogears::SESubstance *subs = m_pe->GetSubstanceManager().GetSubstance(substance);
                    biogears::SESubstanceInfusion infuse(*subs);
                    std::vector <std::string> concentrations = Utility::explode("/", concentration);
                    concentrationsMass = concentrations[0];
                    concentrationsVol = concentrations[1];

                    std::vector <std::string> conmass = Utility::explode(" ", concentrationsMass);
                    massVal = std::stod(conmass[0]);
                    massUnit = conmass[1];
                    std::vector <std::string> convol = Utility::explode(" ", concentrationsVol);
                    volVal = std::stod(convol[0]);
                    volUnit = convol[1];
                    conVal = massVal / volVal;

                    LOG_DEBUG << "Infusing with concentration of " << conVal << " " << massUnit << "/" << volUnit;
                    if (massUnit == "mg" && volUnit == "mL") {
                        infuse.GetConcentration().SetValue(conVal, biogears::MassPerVolumeUnit::mg_Per_mL);
                    } else {
                        infuse.GetConcentration().SetValue(conVal, biogears::MassPerVolumeUnit::mg_Per_mL);
                    }

                    std::vector <std::string> rateb = Utility::explode(" ", rate);
                    rateVal = std::stod(rateb[0]);
                    rateUnit = rateb[1];

                    if (rateUnit == "mL/hr") {
                        LOG_DEBUG << "Infusing at " << rateVal << " mL per hour";
                        infuse.GetRate().SetValue(rateVal, biogears::VolumePerTimeUnit::mL_Per_hr);
                    } else {
                        LOG_DEBUG << "Infusing at " << rateVal << " mL per min";
                        infuse.GetRate().SetValue(rateVal, biogears::VolumePerTimeUnit::mL_Per_min);
                    }
                    m_pe->ProcessAction(infuse);
                }
            } else if (type == "bolus") {
                const biogears::SESubstance *subs = m_pe->GetSubstanceManager().GetSubstance(substance);

                std::string concentrationsMass, concentrationsVol, massUnit, volUnit, doseUnit;
                double massVal, volVal, conVal, doseVal;
                std::vector <std::string> concentrations = Utility::explode("/", concentration);
                concentrationsMass = concentrations[0];
                concentrationsVol = concentrations[1];

                std::vector <std::string> conmass = Utility::explode(" ", concentrationsMass);
                massVal = std::stod(conmass[0]);
                massUnit = conmass[1];
                std::vector <std::string> convol = Utility::explode(" ", concentrationsVol);
                volVal = std::stod(convol[0]);
                volUnit = convol[1];
                conVal = massVal / volVal;

                std::vector <std::string> doseb = Utility::explode(" ", dose);
                doseVal = std::stod(doseb[0]);
                doseUnit = doseb[1];

                biogears::SESubstanceBolus bolus(*subs);
                LOG_DEBUG << "Bolus with concentration of " << conVal << " " << massUnit << "/" << volUnit;
                if (massUnit == "mg" && volUnit == "mL") {
                    bolus.GetConcentration().SetValue(conVal, biogears::MassPerVolumeUnit::mg_Per_mL);
                } else {
                    bolus.GetConcentration().SetValue(conVal, biogears::MassPerVolumeUnit::ug_Per_mL);
                }
                LOG_DEBUG << "Bolus with a dose of  " << doseVal << doseUnit;
                if (doseUnit == "mL") {
                    bolus.GetDose().SetValue(doseVal, biogears::VolumeUnit::mL);
                } else {
                    bolus.GetDose().SetValue(doseVal, biogears::VolumeUnit::uL);
                }
                bolus.SetAdminRoute(CDM::enumBolusAdministration::Intravenous);
                m_pe->ProcessAction(bolus);
            }
        } catch (std::exception &e) {
            LOG_ERROR << "Error processing ivpump action: " << e.what();
        }
    }

    void PhysiologyThread::SetHemorrhage(const std::string &location, const std::string &hemorrhageSettings) {
        LOG_DEBUG << "Setting hemo with location " << location << " and settings: " << hemorrhageSettings;
        double flowRate;
        std::vector <std::string> strings = Utility::explode("\n", hemorrhageSettings);
        for (auto str : strings) {
            std::vector <std::string> strs;
            boost::split(strs, str, boost::is_any_of("=, = "));
            auto strs_size = strs.size();
            LOG_DEBUG << " strs.size is " << strs_size;
            std::string kvp_k = strs[0];
            LOG_DEBUG << "kvp_k is " << kvp_k << " and strs[1] is " << strs[1] << " and strs[2] is " << strs[2]
                      << " and strs[3] is " << strs[3];

            continue;

            if (strs_size != 2 && strs_size != 4) {
                continue;
            }

            // all settings for the ventilator are floats
            try {
                if (kvp_k == "flowrate") {
                    if (strs_size == 2) {
                        flowRate = std::stod(strs[1]);
                    } else if (strs_size == 4) {
                        flowRate = std::stod(strs[3]);
                    }
                } else {
                    LOG_INFO << "Unknown hemorrhage setting: " << kvp_k << " = " << strs[1];
                }
            } catch (std::exception &e) {
                LOG_ERROR << "Issue with setting " << e.what();
            }
        }

        biogears::SEHemorrhage hemorrhage;
        hemorrhage.SetCompartment(location);
        hemorrhage.GetInitialRate().SetValue(flowRate, biogears::VolumePerTimeUnit::mL_Per_min);

        try {
            m_pe->ProcessAction(hemorrhage);
        } catch (std::exception &e) {
            LOG_ERROR << "Error processing hemorrhage action: " << e.what();
        }
    }

    void PhysiologyThread::SetPain(const std::string &painSettings) {
        std::vector <std::string> strings = Utility::explode("\n", painSettings);

        std::string location; //location of pain stimulus, examples "Arm", "Leg"
        double severity; //severity (scale 0-1)

        for (auto str : strings) {
            std::vector <std::string> strs;
            boost::split(strs, str, boost::is_any_of("="));
            auto strs_size = strs.size();
            // Check if it's not a key value pair
            if (strs_size != 2) {
                continue;
            }
            std::string kvp_k = strs[0];
            // all settings for the ventilator are floats
            try {
                if (kvp_k == "location") {
                    location = strs[1];
                } else if (kvp_k == "severity") {
                    severity = std::stod(strs[1]);
                } else {
                    LOG_INFO << "Unknown pain setting: " << kvp_k << " = " << strs[1];
                }
            } catch (std::exception &e) {
                LOG_ERROR << "Issue with setting " << e.what();
            }
            //set up the configuration of the pain stimulus
        }
        biogears::SEPainStimulus PainStimulus; //pain object
        PainStimulus.SetLocation(location);
        PainStimulus.GetSeverity().SetValue(severity);
        try {
            m_pe->ProcessAction(PainStimulus);
        } catch (std::exception &e) {
            LOG_ERROR << "Error processing pain action: " << e.what();
        }
    }

    void PhysiologyThread::SetVentilator(const std::string &ventilatorSettings) {
        std::vector <std::string> strings = Utility::explode("\n", ventilatorSettings);

        biogears::SEAnesthesiaMachineConfiguration AMConfig(m_pe->GetSubstanceManager());
        biogears::SEAnesthesiaMachine &config = AMConfig.GetConfiguration();

        config.GetInletFlow().SetValue(2.0, biogears::VolumePerTimeUnit::L_Per_min);
        config.SetPrimaryGas(CDM::enumAnesthesiaMachinePrimaryGas::Nitrogen);
        config.SetConnection(CDM::enumAnesthesiaMachineConnection::Tube);
        config.SetOxygenSource(CDM::enumAnesthesiaMachineOxygenSource::Wall);
        config.GetReliefValvePressure().SetValue(20.0, biogears::PressureUnit::cmH2O);

        for (auto str : strings) {
            std::vector <std::string> strs;
            boost::split(strs, str, boost::is_any_of("="));
            auto strs_size = strs.size();
            // Check if it's not a key value pair
            if (strs_size != 2) {
                continue;
            }
            std::string kvp_k = strs[0];
            // all settings for the ventilator are floats
            double kvp_v = std::stod(strs[1]);
            try {
                if (kvp_k == "OxygenFraction") {
                    config.GetOxygenFraction().SetValue(kvp_v);
                } else if (kvp_k == "PositiveEndExpiredPressure") {
                    config.GetPositiveEndExpiredPressure().SetValue(kvp_v * 100, biogears::PressureUnit::cmH2O);
                } else if (kvp_k == "RespiratoryRate") {
                    config.GetRespiratoryRate().SetValue(kvp_v, biogears::FrequencyUnit::Per_min);
                } else if (kvp_k == "InspiratoryExpiratoryRatio") {
                    config.GetInspiratoryExpiratoryRatio().SetValue(kvp_v);
                } else if (kvp_k == "TidalVolume") {
                    // empty
                } else if (kvp_k == "VentilatorPressure") {
                    config.GetVentilatorPressure().SetValue(kvp_v * 100, biogears::PressureUnit::cmH2O);
                } else if (kvp_k == " ") {
                    // empty
                } else {
                    LOG_INFO << "Unknown ventilator setting: " << kvp_k << " = " << kvp_v;
                }
            } catch (std::exception &e) {
                LOG_ERROR << "Issue with setting " << e.what();
            }
        }

        try {
            m_pe->ProcessAction(AMConfig);
        } catch (std::exception &e) {
            LOG_ERROR << "Error processing ventilator action: " << e.what();
        }
    }

    void PhysiologyThread::SetBVMMask(const std::string &ventilatorSettings) {
        std::vector <std::string> strings = Utility::explode("\n", ventilatorSettings);

        biogears::SEAnesthesiaMachineConfiguration AMConfig(m_pe->GetSubstanceManager());
        biogears::SEAnesthesiaMachine &config = AMConfig.GetConfiguration();

        config.GetInletFlow().SetValue(2.0, biogears::VolumePerTimeUnit::L_Per_min);
        config.SetPrimaryGas(CDM::enumAnesthesiaMachinePrimaryGas::Air);
        config.SetConnection(CDM::enumAnesthesiaMachineConnection::Mask);
        config.SetOxygenSource(CDM::enumAnesthesiaMachineOxygenSource::Wall);
        config.GetReliefValvePressure().SetValue(20.0, biogears::PressureUnit::cmH2O);

        for (auto str : strings) {
            std::vector <std::string> strs;
            boost::split(strs, str, boost::is_any_of("="));
            auto strs_size = strs.size();
            // Check if it's not a key value pair
            if (strs_size != 2) {
                continue;
            }
            std::string kvp_k = strs[0];
            // all settings for the ventilator are floats
            double kvp_v = std::stod(strs[1]);
            try {
                if (kvp_k == "OxygenFraction") {
                    config.GetOxygenFraction().SetValue(kvp_v);
                } else if (kvp_k == "PositiveEndExpiredPressure") {
                    config.GetPositiveEndExpiredPressure().SetValue(kvp_v * 100, biogears::PressureUnit::cmH2O);
                } else if (kvp_k == "RespiratoryRate") {
                    config.GetRespiratoryRate().SetValue(kvp_v, biogears::FrequencyUnit::Per_min);
                } else if (kvp_k == "InspiratoryExpiratoryRatio") {
                    config.GetInspiratoryExpiratoryRatio().SetValue(kvp_v);
                } else if (kvp_k == "TidalVolume") {
                    // empty
                } else if (kvp_k == "VentilatorPressure") {
                    config.GetVentilatorPressure().SetValue(kvp_v * 100, biogears::PressureUnit::cmH2O);
                } else if (kvp_k == " ") {
                    // empty
                } else {
                    LOG_INFO << "Unknown BVM setting: " << kvp_k << " = " << kvp_v;
                }
            } catch (std::exception &e) {
                LOG_ERROR << "Issue with setting " << e.what();
            }
        }

        try {
            m_pe->ProcessAction(AMConfig);
        } catch (std::exception &e) {
            LOG_ERROR << "Error processing BVM action: " << e.what();
        }
    }

    void PhysiologyThread::Status() {
        m_pe->GetLogger()->Info("");
        m_pe->GetLogger()->Info(
                std::stringstream() << "Simulation Time : " << m_pe->GetSimulationTime(biogears::TimeUnit::s) << "s");
        m_pe->GetLogger()->Info(
                std::stringstream() << "Cardiac Output : "
                                    << m_pe->GetCardiovascularSystem()->GetCardiacOutput(
                                            biogears::VolumePerTimeUnit::mL_Per_min)
                                    << biogears::VolumePerTimeUnit::mL_Per_min);
        m_pe->GetLogger()->Info(
                std::stringstream() << "Blood Volume : "
                                    << m_pe->GetCardiovascularSystem()->GetBloodVolume(biogears::VolumeUnit::mL)
                                    << biogears::VolumeUnit::mL);
        m_pe->GetLogger()->Info(
                std::stringstream() << "Mean Arterial Pressure : "
                                    << m_pe->GetCardiovascularSystem()->GetMeanArterialPressure(
                                            biogears::PressureUnit::mmHg)
                                    << biogears::PressureUnit::mmHg);
        m_pe->GetLogger()->Info(
                std::stringstream() << "Systolic Pressure : "
                                    << m_pe->GetCardiovascularSystem()->GetSystolicArterialPressure(
                                            biogears::PressureUnit::mmHg)
                                    << biogears::PressureUnit::mmHg);
        m_pe->GetLogger()->Info(
                std::stringstream() << "Diastolic Pressure : "
                                    << m_pe->GetCardiovascularSystem()->GetDiastolicArterialPressure(
                                            biogears::PressureUnit::mmHg)
                                    << biogears::PressureUnit::mmHg);
        m_pe->GetLogger()->Info(
                std::stringstream() << "Heart Rate : "
                                    << m_pe->GetCardiovascularSystem()->GetHeartRate(biogears::FrequencyUnit::Per_min)
                                    << "bpm");
        m_pe->GetLogger()->Info(
                std::stringstream() << "Respiration Rate : "
                                    << m_pe->GetRespiratorySystem()->GetRespirationRate(
                                            biogears::FrequencyUnit::Per_min)
                                    << "bpm");
        m_pe->GetLogger()->Info("");
    }
}


