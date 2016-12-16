#ifndef CPU_MANAGER_HPP
#define CPU_MANAGER_HPP


#include <mutex>
#include <atomic>

#include <boost/dynamic_bitset.hpp>

#include "hardware/places/ComputePlace.hpp"
#include "lowlevel/SpinLock.hpp"
#include "lowlevel/FatalErrorHandler.hpp"
#include "hardware/HardwareInfo.hpp"

#include "CPU.hpp"


class CPUManager {
private:
	//! \brief CPU mask of the process
	static cpu_set_t _processCPUMask;
	
	//! \brief per-CPU data indexed by system CPU identifier
	static std::vector<CPU *> _cpus;
	
	//! \brief numer of initialized CPUs
	static size_t _totalCPUs;
	
	//! \brief indicates if the thread manager has finished initializing the CPUs
	static std::atomic<bool> _finishedCPUInitialization;
	
	//! \brief threads blocked due to idleness
	static boost::dynamic_bitset<> _idleCPUs;
	
	static SpinLock _idleCPUsLock;
	
	
public:
	static void preinitialize();
	
	static void initialize();
	
	//! \brief get the CPU object assigned to a given numerical system CPU identifier
	static inline CPU *getCPU(size_t systemCPUId);
	
	//! \brief get the maximum number of CPUs that will be used
	static inline long getTotalCPUs();
	
	//! \brief check if initialization has finished
	static inline bool hasFinishedInitialization();
	
	//! \brief get a reference to the CPU mask of the process
	static inline cpu_set_t const &getProcessCPUMaskReference();
	
	//! \brief get a reference to the list of CPUs
	static inline std::vector<CPU *> const &getCPUListReference();
};


inline CPU *CPUManager::getCPU(size_t systemCPUId)
{
	assert(systemCPUId < _cpus.size());
	return _cpus[systemCPUId];
}

inline long CPUManager::getTotalCPUs()
{
	return _totalCPUs;
}

inline bool CPUManager::hasFinishedInitialization()
{
	return _finishedCPUInitialization;
}


inline cpu_set_t const &CPUManager::getProcessCPUMaskReference()
{
	return _processCPUMask;
}

inline std::vector<CPU *> const &CPUManager::getCPUListReference()
{
	return _cpus;
}

#endif // THREAD_MANAGER_HPP
