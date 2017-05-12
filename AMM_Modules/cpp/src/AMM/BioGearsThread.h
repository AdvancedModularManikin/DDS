
#include <mutex>
#include <thread>

#include "ccpp_AMM.h"

#include "bind/biogears-cdm.hxx"
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
class PhysiologyEngine;


using namespace AMM::Physiology;

class BioGearsThread
{
public:
	BioGearsThread(const std::string& logfile);
	virtual ~BioGearsThread();

	void StartSimulation();
	void StopSimulation();
	void Shutdown();

	void AdvanceTimeTick();
	double GetECGWaveForm();
	double GetHeartRate();
	double GetSimulationTime();
	Data GetNodePath(const std::string& nodePath);

	void Status();

	Logger* GetLogger() { return m_bg->GetLogger(); }

protected:
	void AdvanceTime();

	std::thread m_thread;
	std::mutex  m_mutex;
	bool m_runThread;

	std::unique_ptr<PhysiologyEngine> m_bg;
};

