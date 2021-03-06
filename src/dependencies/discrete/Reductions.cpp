/*
	This file is part of Nanos6 and is licensed under the terms contained in the COPYING file.
	
	Copyright (C) 2015-2017 Barcelona Supercomputing Center (BSC)
*/

#include <nanos6.h>

#include <cassert>
#include <algorithm>

#include "tasks/Task.hpp"
#include "tasks/TaskImplementation.hpp"
#include "executors/threads/WorkerThread.hpp"


void *nanos6_get_original_reduction_address(const void *address)
{
	WorkerThread *currentThread = WorkerThread::getCurrentWorkerThread();
	assert(currentThread != nullptr);
	
	Task *currentTask = currentThread->getTask();
	assert(currentTask != nullptr);
	assert(currentTask->getThread() == currentThread);
	
	void *argsBlock = currentTask->getArgsBlock();
	
	if (argsBlock <= address && currentTask > address) {
		void *original = (void*)*(((void**)address) - 1);
		
		TaskDataAccesses &dataAccesses = currentTask->getDataAccesses();
		TaskDataAccesses::iterator it = std::find_if(
				dataAccesses.begin(), dataAccesses.end(),
				[&](const DataAccess &dataAccess) -> bool {
					return dataAccess._type == REDUCTION_ACCESS_TYPE;
				});
		bool isReductionAccess = it != dataAccesses.end();
		
		if (isReductionAccess) {
			return original;
		}
		else {
			return (void*)address;
		}
	}
	else {
		return (void*)address;
	}
}
