/*
	This file is part of Nanos6 and is licensed under the terms contained in the COPYING file.
	
	Copyright (C) 2015-2017 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASK_DATA_ACCESSES_IMPLEMENTATION_HPP
#define TASK_DATA_ACCESSES_IMPLEMENTATION_HPP

#include <boost/intrusive/parent_from_member.hpp>

#include <InstrumentDependenciesByAccessLinks.hpp>

#include "BottomMapEntry.hpp"
#include "DataAccess.hpp"
#include "TaskDataAccesses.hpp"
#include "TaskDataAccessLinkingArtifacts.hpp"
#include "tasks/Task.hpp"


inline TaskDataAccesses::~TaskDataAccesses()
{
	assert(!hasBeenDeleted());
	
#ifndef NDEBUG
	Task *task = boost::intrusive::get_parent_from_member<Task>(this, &Task::_dataAccesses);
	assert(task != nullptr);
	assert(&task->getDataAccesses() == this);
#endif
	
	assert(_removalCountdown == 0);
	assert(_accesses.empty());
	assert(_subaccessBottomMap.empty());
	
#ifndef NDEBUG
	hasBeenDeleted() = true;
#endif
}


#include "TaskDataAccessLinkingArtifactsImplementation.hpp"


#endif // TASK_DATA_ACCESSES_IMPLEMENTATION_HPP
