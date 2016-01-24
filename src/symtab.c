// ------------------------------------------------------------------------------------------------------------
//	Copyright © 2016 Simon Friedrich	<simon.friedrich@iem.thm.de>
//					 Sergej Zuyev		<sergej.zuyev@zz-systems.net>
//
//  This work is free. You can redistribute it and/or modify it under the
//  terms of the Do What The Fuck You Want To Public License, Version 2,
//  as published by Sam Hocevar. See the COPYING file for more details.
// ------------------------------------------------------------------------------------------------------------

#include "symtab.h"

#include <string.h>
#include <stdlib.h>

// private -----------------------------------------------------------------------------------------------------

static struct symbol_t *symtab = NULL;

// public -----------------------------------------------------------------------------------------------------

struct symbol_t *symtab_add_var(char *name, double value)
{
	struct symbol_t *entry;

	entry				= NEW(struct symbol_t);
	entry->name			= name;
	entry->type			= TYPE_VAR;
	entry->data.val		= value;
	entry->next			= symtab;
	entry->is_declared	= TRUE;
	entry->is_builtin	= FALSE;

	symtab = entry;

	return entry;
}

struct symbol_t *symtab_add_blt_const(char *name, double value)
{
	struct symbol_t *entry = symtab_add_var(name, value);
	entry->is_builtin = TRUE;

	return entry;
}

struct symbol_t *symtab_add_blt_func(char *name, double (*func)(double), int arg_cnt)
{
	struct symbol_t *entry;

	entry				= NEW(struct symbol_t);
	entry->name			= name;
	entry->type			= TYPE_FUNC;
	entry->data.func	= func;
	entry->next			= symtab;
	entry->is_declared	= TRUE;
	entry->is_builtin	= TRUE;
	entry->arg_cnt		= arg_cnt;

	symtab = entry;
	return entry;
}

struct symbol_t *symtab_find(const char *name)
{
	struct symbol_t *current;
	for(current = symtab; current != NULL; current = current->next)
	{
		if(strcmp(name, current->name) == 0)
			return current;
	}

	return NULL;
}

void symtab_clear()
{
	struct symbol_t *current, *next;
	for(current = symtab; current != NULL; current = next)
	{
		next = current->next;

		if(current->is_builtin)
			continue;

		if(current->name != NULL)
		{
			DELETE(current->name);
			current->name = NULL;
		}
		symtab = next;
		DELETE(current);
	}
}

void symtab_dispose()
{
	struct symbol_t *current, *next;
	for(current = symtab; current != NULL; current = next)
	{
		next = current->next;

		// Only dynamically allocated strings shall be freed
		// Freeing static strings (residing in the readonly memory) leads to a runtime failure
		if(!current->is_builtin && current->name != NULL)
		{
			DELETE(current->name);
			current->name = NULL;
		}

		DELETE(current);
	}
}

// ------------------------------------------------------------------------------------------------------------