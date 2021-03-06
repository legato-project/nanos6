/*
	This file is part of Nanos6 and is licensed under the terms contained in the COPYING file.
	
	Copyright (C) 2015-2017 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASKLOOP_GENERATOR_HPP
#define TASKLOOP_GENERATOR_HPP

#include <nanos6.h>
#include "Taskloop.hpp"
#include "TaskloopInfo.hpp"

#include <InstrumentAddTask.hpp>
#include <InstrumentTaskStatus.hpp>
#include <InstrumentThreadInstrumentationContextImplementation.hpp>

class TaskloopGenerator {
public:
	static inline Taskloop* createCollaborator(Taskloop *parent)
	{
		assert(parent != nullptr);
		
		nanos6_task_info_t *taskInfo = parent->getTaskInfo();
		nanos6_task_invocation_info_t *taskInvocationInfo = parent->getTaskInvokationInfo();
		
		void *originalArgsBlock = parent->getArgsBlock();
		size_t originalArgsBlockSize = parent->getArgsBlockSize();
		
		Taskloop *taskloop = nullptr;
		void *argsBlock = nullptr;
		
		// Create the task for this partition
		nanos6_create_task(taskInfo, taskInvocationInfo, originalArgsBlockSize, (void **) &argsBlock, (void **) &taskloop, parent->getFlags());
		assert(argsBlock != nullptr);
		assert(taskloop != nullptr);
		
		// Copy the args block
		if (taskInfo->duplicate_args_block != nullptr) {
			taskInfo->duplicate_args_block(originalArgsBlock, argsBlock);
		} else {
			memcpy(argsBlock, originalArgsBlock, originalArgsBlockSize);
		}
		
		// Set the flags
		taskloop->setRunnable(true);
		taskloop->setDelayedRelease(false);
		
		// Set the parent
		taskloop->setParent(parent);
		
		// Instrument the task creation
		Instrument::task_id_t taskInstrumentationId = taskloop->getInstrumentationTaskId();
		Instrument::createdTask(taskloop, taskInstrumentationId);
		Instrument::exitAddTask(taskInstrumentationId);
		
		return taskloop;
	}
};

#endif // TASKLOOP_GENERATOR_HPP
