/*
	This file is part of Nanos6 and is licensed under the terms contained in the COPYING file.
	
	Copyright (C) 2015-2017 Barcelona Supercomputing Center (BSC)
*/

#include "HardwareCounters.hpp"


namespace HardwareCounters {
	char const * const _presetCounterNames[total_preset_counter] = {
		"Real frequency",
		"Virtual frequency",
		"IPC",
		"L1 data cache miss ratio",
		"L2 data cache miss ratio",
		"L3 data cache miss ratio",
		"GFLOP/s",
		"Real nsecs",
		"Virtual nsecs",
		"Instructions"
	};
	
};


#include "performance/HardwareCountersThreadLocalData.hpp"
#include "performance/HardwareCountersThreadLocalDataImplementation.hpp"
