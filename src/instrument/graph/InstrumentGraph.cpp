#include "InstrumentGraph.hpp"


namespace Instrument {
	namespace Graph {
		std::atomic<thread_id_t> _nextThreadId(1);
		std::atomic<taskwait_id_t> _nextTaskwaitId(1);
		std::atomic<task_id_t> _nextTaskId(0);
		std::atomic<usermutex_id_t> _nextUsermutexId(0);
		std::atomic<data_access_id_t::inner_type_t> _nextDataAccessId(1);
		
		std::map<WorkerThread *, thread_id_t> _threadToId;
		task_to_info_map_t _taskToInfoMap;
		data_access_originator_map_t _accessToOriginatorMap;
		task_invocation_info_label_map_t _taskInvocationLabel;
		usermutex_to_id_map_t _usermutexToId;
		execution_sequence_t _executionSequence;
		std::map<data_access_id_t, data_access_id_t> _superAccessByAccess;
		std::map<data_access_id_t, data_access_id_t> _firstSubAccessByAccess;
		
		SpinLock _graphLock;
		
		EnvironmentVariable<bool> _showDependencyStructures("NANOS_GRAPH_SHOW_DEPENDENCY_STRUCTURES", false);
		
		
		access_t &getAccess(data_access_id_t dataAccessId, task_id_t originatorTaskId)
		{
			static access_t invalidResult;
			
			task_info_t &taskInfo = _taskToInfoMap[originatorTaskId];
			task_id_t parentTaskId = taskInfo._parent;
			
			// Assume that the "main" task does not have dependencies
			assert(parentTaskId != -1);
			
			task_info_t &parentInfo = _taskToInfoMap[parentTaskId];
			
			for (phase_t *phase : parentInfo._phaseList) {
				task_group_t *taskGroup = dynamic_cast<task_group_t *>(phase);
				
				if (taskGroup != 0) {
					if (taskGroup->_children.find(originatorTaskId) != taskGroup->_children.end()) {
						// The current phase is the correct one
						return taskGroup->_dataAccessMap[dataAccessId];
					}
				}
			}
			
			assert("Instrument::Graph did not find the data access" == 0);
			return invalidResult;
		}
		
		
		access_t &getAccess(data_access_id_t dataAccessId)
		{
			task_id_t originator = _accessToOriginatorMap[dataAccessId];
			return getAccess(dataAccessId, originator);
		}
		
	}
}
