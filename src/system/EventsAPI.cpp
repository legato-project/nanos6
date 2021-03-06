/*
	This file is part of Nanos6 and is licensed under the terms contained in the COPYING file.
	
	Copyright (C) 2015-2017 Barcelona Supercomputing Center (BSC)
*/

#include <cassert>

#include "DataAccessRegistration.hpp"

#include "executors/threads/TaskFinalization.hpp"
#include "executors/threads/ThreadManager.hpp"
#include "executors/threads/WorkerThread.hpp"
#include "tasks/Task.hpp"
#include "tasks/TaskImplementation.hpp"


extern "C" void *nanos6_get_current_event_counter()
{
	WorkerThread *currentThread = WorkerThread::getCurrentWorkerThread();
	assert(currentThread != nullptr);
	
	Task *currentTask = currentThread->getTask();
	assert(currentTask != nullptr);
	
	return currentTask;
}


extern "C" void nanos6_increase_current_task_event_counter(__attribute__((unused)) void *event_counter, unsigned int increment)
{
	assert(event_counter != 0);
	if (increment == 0) return;
	
	WorkerThread *currentThread = WorkerThread::getCurrentWorkerThread();
	assert(currentThread != nullptr);
	
	Task *currentTask = currentThread->getTask();
	assert(currentTask != nullptr);
	assert(event_counter == currentTask);
	
	currentTask->increaseReleaseCount(increment);
}


extern "C" void nanos6_decrease_task_event_counter(void *event_counter, unsigned int decrement)
{
	assert(event_counter != 0);
	if (decrement == 0) return;
	
	Task *task = static_cast<Task *>(event_counter);
	
	// Release dependencies if the event counter becomes zero
	if (task->decreaseReleaseCount(decrement)) {
		// Note: At this moment, the CPU assigned to the current worker thread (if applicable)
		// can be incorrect. We temporally unset the CPU to prevent the dependency system and
		// the memory allocator from using an invalid CPU
		CPU *originalCPU = nullptr;
		WorkerThread *currentThread = WorkerThread::getCurrentWorkerThread();
		if (currentThread != nullptr) {
			originalCPU = currentThread->getComputePlace();
			currentThread->setComputePlace(nullptr);
		}
		
		// Release the accesses
		DataAccessRegistration::unregisterTaskDataAccesses(task, nullptr);
		
		// Try to dispose the task
		if (task->markAsReleased()) {
			TaskFinalization::disposeOrUnblockTask(task, nullptr);
		}
		
		// Restore the previous CPU
		if (currentThread != nullptr) {
			currentThread->setComputePlace(originalCPU);
		}
	}
}

