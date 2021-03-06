/*
	This file is part of Nanos6 and is licensed under the terms contained in the COPYING file.
	
	Copyright (C) 2015-2017 Barcelona Supercomputing Center (BSC)
*/

#ifndef MULTIDIMENSIONAL_API_HPP
#define MULTIDIMENSIONAL_API_HPP


#include <nanos6/multidimensional-dependencies.h>
#include <nanos6/dependencies.h>

#include "../DataAccessType.hpp"
#include "../MultidimensionalAPITraversal.hpp"


#ifdef NDEBUG
#define _AI_ inline __attribute((always_inline))
#else
#define _AI_ inline
#endif

#define _UU_ __attribute__((unused))


template <DataAccessType ACCESS_TYPE, bool WEAK, typename... TS>
_AI_ void register_data_access_base(
	void *handler, int symbolIndex, char const *regionText, void *baseAddress,
	long currentDimSize, long currentDimStart, long currentDimEnd
);

template<>
_AI_ void register_data_access_base<READ_ACCESS_TYPE, true>(
	void *handler, _UU_ int symbolIndex, _UU_ char const *regionText, void *baseAddress,
	_UU_ long currentDimSize, _UU_ long currentDimStart, long currentDimEnd
) {
	size_t start = (size_t) baseAddress;
	start += currentDimStart;
	nanos6_register_weak_read_depinfo(handler, (void *) start, currentDimEnd - currentDimStart);
}

template<>
_AI_ void register_data_access_base<READ_ACCESS_TYPE, false>(
	void *handler, _UU_ int symbolIndex, _UU_ char const *regionText, void *baseAddress,
	_UU_ long currentDimSize, long currentDimStart, long currentDimEnd
) {
	size_t start = (size_t) baseAddress;
	start += currentDimStart;
	nanos6_register_read_depinfo(handler, (void *) start, currentDimEnd - currentDimStart);
}

template<>
_AI_ void register_data_access_base<WRITE_ACCESS_TYPE, true>(
	void *handler, _UU_ int symbolIndex, _UU_ char const *regionText, void *baseAddress,
	_UU_ long currentDimSize, long currentDimStart, long currentDimEnd
) {
	size_t start = (size_t) baseAddress;
	start += currentDimStart;
	nanos6_register_weak_write_depinfo(handler, (void *) start, currentDimEnd - currentDimStart);
}

template<>
_AI_ void register_data_access_base<WRITE_ACCESS_TYPE, false>(
	void *handler, _UU_ int symbolIndex, _UU_ char const *regionText, void *baseAddress,
	_UU_ long currentDimSize, long currentDimStart, long currentDimEnd
) {
	size_t start = (size_t) baseAddress;
	start += currentDimStart;
	nanos6_register_write_depinfo(handler, (void *) start, currentDimEnd - currentDimStart);
}

template<>
_AI_ void register_data_access_base<READWRITE_ACCESS_TYPE, true>(
	void *handler, _UU_ int symbolIndex, _UU_ char const *regionText, void *baseAddress,
	_UU_ long currentDimSize, long currentDimStart, long currentDimEnd
) {
	size_t start = (size_t) baseAddress;
	start += currentDimStart;
	nanos6_register_weak_readwrite_depinfo(handler, (void *) start, currentDimEnd - currentDimStart);
}

template<>
_AI_ void register_data_access_base<READWRITE_ACCESS_TYPE, false>(
	void *handler, _UU_ int symbolIndex, _UU_ char const *regionText, void *baseAddress,
	_UU_ long currentDimSize, long currentDimStart, long currentDimEnd
) {
	size_t start = (size_t) baseAddress;
	start += currentDimStart;
	nanos6_register_readwrite_depinfo(handler, (void *) start, currentDimEnd - currentDimStart);
}

template<>
_AI_ void register_data_access_base<CONCURRENT_ACCESS_TYPE, false>(
	void *handler, _UU_ int symbolIndex, _UU_ char const *regionText, void *baseAddress,
	_UU_ long currentDimSize, long currentDimStart, long currentDimEnd
) {
	size_t start = (size_t) baseAddress;
	start += currentDimStart;
	nanos6_register_concurrent_depinfo(handler, (void *) start, currentDimEnd - currentDimStart);
}


template <DataAccessType ACCESS_TYPE, bool WEAK, typename... TS>
static _AI_ void register_data_access(
	void *handler, int symbolIndex, char const *regionText, void *baseAddress,
	TS... dimensions
) {
	size_t startOffset = getStartOffset<>(dimensions...);
	char *currentBaseAddress = (char *) baseAddress;
	currentBaseAddress += startOffset;
	
	size_t size = getDiscreteSize(dimensions...);
	register_data_access_base<ACCESS_TYPE, WEAK>(handler, symbolIndex, regionText, currentBaseAddress, size, 0, size);
}


// The following is only for reductions

template <typename... TS>
static _AI_ void register_reduction_access(
	int reduction_operation, int reduction_index,
	void *handler, int symbolIndex, char const *regionText, void *baseAddress,
	TS... dimensions
) {
	size_t startOffset = getStartOffset<>(dimensions...);
	char *currentBaseAddress = (char *) baseAddress;
	currentBaseAddress += startOffset;
	
	size_t size = getDiscreteSize(dimensions...);
	nanos6_register_region_reduction_depinfo1(
		reduction_operation, reduction_index,
		handler, symbolIndex, regionText, currentBaseAddress,
		size, 0, size
	);
}

#undef _AI_
#undef _UU_


#endif // MULTIDIMENSIONAL_API_HPP
