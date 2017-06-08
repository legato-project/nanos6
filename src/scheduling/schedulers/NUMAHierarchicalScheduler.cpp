#include "NUMAHierarchicalScheduler.hpp"

#include "../SchedulerInterface.hpp"
#include "../SchedulerGenerator.hpp"

#include "executors/threads/CPUManager.hpp"
#include "hardware/HardwareInfo.hpp"
#include "executors/threads/WorkerThread.hpp"
#include "system/RuntimeInfo.hpp"
#include "tasks/Task.hpp"

#include <cassert>
#include <sstream>


NUMAHierarchicalScheduler::NUMAHierarchicalScheduler()
	: _NUMANodeScheduler(HardwareInfo::getMemoryNodeCount()),
	_readyTasks(HardwareInfo::getMemoryNodeCount()),
	_enabledCPUs(HardwareInfo::getMemoryNodeCount())
{
	size_t NUMANodeCount = HardwareInfo::getMemoryNodeCount();
	std::vector<CPU *> const &cpus = CPUManager::getCPUListReference();

	for (CPU *cpu : cpus) {
		if (cpu != nullptr) {
			_enabledCPUs[cpu->_NUMANodeId] += 1;
		}
	}

	for (size_t idx = 0; idx < NUMANodeCount; ++idx) {
		_NUMANodeScheduler[idx] = SchedulerGenerator::createNUMANodeScheduler();
		
		std::ostringstream oss, oss2;
		oss << "numa-node-" << idx << "-scheduler";
		oss2 << "NUMA Node " << idx << " Scheduler";
		RuntimeInfo::addEntry(oss.str(), oss2.str(), _NUMANodeScheduler[idx]->getName());
	}
}

NUMAHierarchicalScheduler::~NUMAHierarchicalScheduler()
{
	for (SchedulerInterface *sched : _NUMANodeScheduler) {
		delete sched;
	}
}


ComputePlace * NUMAHierarchicalScheduler::addReadyTask(Task *task, ComputePlace *computePlace, ReadyTaskHint hint, bool doGetIdle)
{
	size_t NUMANodeCount = HardwareInfo::getMemoryNodeCount();
	
	/* Get the least loaded NUMA node */
	int min_load = -1;
	int min_idx = -1;

	for (size_t numa = 0; numa < NUMANodeCount; ++numa) {
		if (_enabledCPUs[numa] > 0) {
			if (min_load == -1 || _readyTasks[numa] < min_load) {
				min_load = _readyTasks[numa];
				min_idx = numa;
			}
		}
	}

	assert(min_idx != -1);

	_readyTasks[min_idx] += 1;
	_NUMANodeScheduler[min_idx]->addReadyTask(task, computePlace, hint, false);
	if (doGetIdle) {
		ComputePlace *cp;
		cp = CPUManager::getIdleNUMANodeCPU(min_idx);
		if (cp == nullptr) {
			// If this NUMA node does not have any idle CPUs, get any other idle CPU
			cp = CPUManager::getIdleCPU();
		}

		return cp;
	} else {
		return nullptr;
	}
}


void NUMAHierarchicalScheduler::taskGetsUnblocked(Task *unblockedTask, ComputePlace *hardwarePlace)
{
	/* This is not pretty. But this function is seldom called */
	CPU *cpu = unblockedTask->getThread()->getComputePlace();
	size_t numa_node = cpu->_NUMANodeId;

	_readyTasks[numa_node] += 1;
	_NUMANodeScheduler[numa_node]->taskGetsUnblocked(unblockedTask, hardwarePlace);
}


Task *NUMAHierarchicalScheduler::getReadyTask(ComputePlace *computePlace, Task *currentTask, bool canMarkAsIdle)
{
	size_t numa_node = ((CPU *)computePlace)->_NUMANodeId;
	Task *task = nullptr;

	if (_readyTasks[numa_node] > 0) {
		task = _NUMANodeScheduler[numa_node]->getReadyTask(computePlace, currentTask, false);

		if (task != nullptr) {
			_readyTasks[numa_node] -= 1;
		}
	}

	if (task == nullptr) {
		/* Get the most loaded NUMA node */
		int max_load = _readyTasks[numa_node];
		int max_idx = numa_node;

		for (size_t i = 0; i < _readyTasks.size(); ++i) {
			if (_readyTasks[i] > max_load) {
				max_load = _readyTasks[i];
				max_idx = i;
			}
		}
		
		task = _NUMANodeScheduler[max_idx]->getReadyTask(computePlace, currentTask, false);
		if (task != nullptr) {
			_readyTasks[max_idx] -= 1;
		}
	}

	if (canMarkAsIdle && task == nullptr) {
		CPUManager::cpuBecomesIdle((CPU *) computePlace);
	}
	
	return task;
}


ComputePlace *NUMAHierarchicalScheduler::getIdleComputePlace(bool force)
{
	if (force) {
		return CPUManager::getIdleCPU();
	} else {
		size_t NUMANodeCount = HardwareInfo::getMemoryNodeCount();
		ComputePlace *computePlace = nullptr;

		for (size_t numa = 0; numa < NUMANodeCount; ++numa) {
			if (_enabledCPUs[numa] > 0 && _readyTasks[numa] > 0) {
				computePlace = CPUManager::getIdleNUMANodeCPU(numa);
				if (computePlace != nullptr) {
					break;
				}
			}
		}

		return computePlace;
	}
}

void NUMAHierarchicalScheduler::disableComputePlace(ComputePlace *hardwarePlace)
{
	size_t NUMANode = ((CPU *)hardwarePlace)->_NUMANodeId;

	assert(_enabledCPUs[NUMANode] > 0);
	
	// TODO: do something if a NUMA node has pending tasks but is disabled.
	// This is not an issue now because of task stealing
	
	_enabledCPUs[NUMANode] -= 1;

	_NUMANodeScheduler[NUMANode]->disableComputePlace(hardwarePlace);
}

void NUMAHierarchicalScheduler::enableComputePlace(ComputePlace *hardwarePlace)
{
	size_t NUMANode = ((CPU *)hardwarePlace)->_NUMANodeId;

	_enabledCPUs[NUMANode] += 1;
	_NUMANodeScheduler[NUMANode]->enableComputePlace(hardwarePlace);
}

bool NUMAHierarchicalScheduler::requestPolling(ComputePlace *computePlace, polling_slot_t *pollingSlot)
{
	size_t NUMANode = ((CPU *)computePlace)->_NUMANodeId;
	return _NUMANodeScheduler[NUMANode]->requestPolling(computePlace, pollingSlot);
}

bool NUMAHierarchicalScheduler::releasePolling(ComputePlace *computePlace, polling_slot_t *pollingSlot)
{
	size_t NUMANode = ((CPU *)computePlace)->_NUMANodeId;
	return _NUMANodeScheduler[NUMANode]->releasePolling(computePlace, pollingSlot);
}


std::string NUMAHierarchicalScheduler::getName() const
{
	return "numa-hierarchical";
}