/*
	This file is part of Nanos6 and is licensed under the terms contained in the COPYING file.
	
	Copyright (C) 2015-2017 Barcelona Supercomputing Center (BSC)
*/

#include "resolve.h"


#pragma GCC visibility push(default)

void nanos6_preinit()
{
	typedef void nanos6_preinit_t();
	
	static nanos6_preinit_t *symbol = NULL;
	if (__builtin_expect(symbol == NULL, 0)) {
		symbol = (nanos6_preinit_t *) _nanos6_resolve_symbol("nanos6_preinit", "essential", NULL);
	}
	
	(*symbol)();
}


void nanos6_init()
{
	typedef void nanos6_init_t();
	
	static nanos6_init_t *symbol = NULL;
	if (__builtin_expect(symbol == NULL, 0)) {
		symbol = (nanos6_init_t *) _nanos6_resolve_symbol("nanos6_init", "essential", NULL);
	}
	
	(*symbol)();
}


void nanos6_shutdown()
{
	typedef void nanos6_shutdown_t();
	
	static nanos6_shutdown_t *symbol = NULL;
	if (__builtin_expect(symbol == NULL, 0)) {
		symbol = (nanos6_shutdown_t *) _nanos6_resolve_symbol("nanos6_shutdown", "essential", NULL);
	}
	
	(*symbol)();
}


#pragma GCC visibility pop
