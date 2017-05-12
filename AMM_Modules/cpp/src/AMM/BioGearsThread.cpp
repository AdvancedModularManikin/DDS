#include "BioGearsThread.h"

using namespace AMM::Physiology;

BioGearsThread::BioGearsThread(const std::string& logfile) :
		m_thread() {
	// Create our engine with the standard patient
	m_bg = CreateBioGearsEngine(logfile);
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

double BioGearsThread::GetECGWaveForm() {
	return m_bg->GetElectroCardioGram()->GetLead3ElectricPotential(
			ElectricPotentialUnit::mV);
}

double BioGearsThread::GetHeartRate() {
	return m_bg->GetCardiovascularSystem()->GetHeartRate(FrequencyUnit::Per_min);
}

double BioGearsThread::GetSimulationTime() {
	return m_bg->GetSimulationTime(TimeUnit::s);
}

Data BioGearsThread::GetNodePath(const std::string& nodePath) {
	Data outputData;

	if (nodePath == "ECG") {
		outputData.node_path = DDS::string_dup("ECG");
		outputData.unit = "mV";
		outputData.dbl = GetECGWaveForm();
	}

	if (nodePath == "HR") {
		outputData.node_path = DDS::string_dup("HR");
		outputData.unit = "bpm";
		outputData.dbl = GetHeartRate();
	}

	return outputData;
}

void BioGearsThread::Status() { // On demand call to print vitals to the screen
	m_mutex.lock();
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
	m_mutex.unlock();
}
