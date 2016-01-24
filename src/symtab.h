// ------------------------------------------------------------------------------------------------------------
//	Copyright © 2016 Simon Friedrich	<simon.friedrich@iem.thm.de>
//					 Sergej Zuyev		<sergej.zuyev@zz-systems.net>
//
//  This work is free. You can redistribute it and/or modify it under the
//  terms of the Do What The Fuck You Want To Public License, Version 2,
//  as published by Sam Hocevar. See the COPYING file for more details.
// ------------------------------------------------------------------------------------------------------------

#ifndef SYMTAB_H
#define SYMTAB_H

// ... security first. not. we all LOVE buffer overflows...
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "util.h"

// symbol type
enum type_id
{
	TYPE_UNDEFINED,
	TYPE_VAR,
	TYPE_FUNC
};

// symbol struct
// contains information about
// - symbol type
// - value (variable) or function pointer (function)
// - name 
// - argument count for functions
// - is the entry declared (jsut in case)
// - is the entry a builtin entry? => no changes allowed
// -  next chain entry (linked list)
struct symbol_t 
{
	char *name;
	enum type_id type;

	union {
		double val;
		double (*func)(double);
	} data; 
		
	int arg_cnt;
	BOOL is_declared;
	BOOL is_builtin;

	struct symbol_t *next;	
};

// add variable symbol
struct symbol_t *symtab_add_var			(char *name, double value); 
// add builtin const
struct symbol_t	*symtab_add_blt_const	(char *name, double value);
// add builtin function
struct symbol_t	*symtab_add_blt_func	(char *name, double (*func)(double), int arg_cnt);
// find symbol
struct symbol_t	*symtab_find			(const char *name);

// clears all user-defined symbols
void symtab_clear();

// frees all symbols
void symtab_dispose();

#endif