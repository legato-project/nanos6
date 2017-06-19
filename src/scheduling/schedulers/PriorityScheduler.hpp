#ifndef PRIORITY_SCHEDULER_HPP
#define PRIORITY_SCHEDULER_HPP


#include <atomic>
#include <deque>
#include <queue>
#include <vector>

#include "../SchedulerInterface.hpp"
#include "lowlevel/PaddedTicketSpinLock.hpp"
#include "lowlevel/TicketSpinLock.hpp"
#include "executors/threads/CPU.hpp"


class Task;


class PriorityScheduler: public SchedulerInterface {
	typedef PaddedTicketSpinLock<> spinlock_t;
	
	
	
	
	struct TaskPriorityCompare {
		inline bool operator()(Task *a, Task *b);
	};
	
	typedef std::priority_queue<Task *, std::vector<Task *>, TaskPriorityCompare> task_queue_t;
	
	
	spinlock_t _globalLock;
	
	task_queue_t _readyTasks;
	task_queue_t _unblockedTasks;
	
	std::atomic<polling_slot_t *> _pollingSlot;
	
	
public:
	PriorityScheduler(int numaNodeIndex);
	~PriorityScheduler();
	
	ComputePlace *addReadyTask(Task *task, ComputePlace *computePlace, ReadyTaskHint hint, bool doGetIdle = true);
	
	void taskGetsUnblocked(Task *unblockedTask, ComputePlace *computePlace);
	
	Task *getReadyTask(ComputePlace *computePlace, Task *currentTask = nullptr, bool canMarkAsIdle = true);
	
	ComputePlace *getIdleComputePlace(bool force=false);
	
	void disableComputePlace(ComputePlace *computePlace);
	
	bool requestPolling(ComputePlace *computePlace, polling_slot_t *pollingSlot);
	bool releasePolling(ComputePlace *computePlace, polling_slot_t *pollingSlot);
	
	std::string getName() const;
};


#endif // PRIORITY_SCHEDULER_HPP

