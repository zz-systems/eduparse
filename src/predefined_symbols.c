// ------------------------------------------------------------------------------------------------------------
//	Copyright © 2016 Simon Friedrich	<simon.friedrich@iem.thm.de>
//					 Sergej Zuyev		<sergej.zuyev@zz-systems.net>
//
//  This work is free. You can redistribute it and/or modify it under the
//  terms of the Do What The Fuck You Want To Public License, Version 2,
//  as published by Sam Hocevar. See the COPYING file for more details.
// ------------------------------------------------------------------------------------------------------------

#include "predefined_symbols.h"
#include "symtab.h"
#include "lexer.h"
#include "console.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// private -----------------------------------------------------------------------------------------------------

static double __exit		(double unused);
static double __print		(double val);
static double __read		(double unused);
static double __clearstate	(double unused);
static double __help		(double unused);

// builtins access
static struct symbol_t *predefined_symbols[19], 
				**psc = predefined_symbols;

// public -----------------------------------------------------------------------------------------------------

void predefined_symbols_init()
{
	// functions
	*psc = symtab_add_blt_func("abs",	fabs, 1);
	
	*psc++ = symtab_add_blt_func("sin",	 sin, 1);
	*psc++ = symtab_add_blt_func("cos",	 cos, 1);
	*psc++ = symtab_add_blt_func("tan",	 tan, 1);

	*psc++ = symtab_add_blt_func("asin", asin, 1);
	*psc++ = symtab_add_blt_func("acos", acos, 1);
	*psc++ = symtab_add_blt_func("atan", atan, 1);

	*psc++ = symtab_add_blt_func("sqrt", sqrt, 1);
	*psc++ = symtab_add_blt_func("lg",	 log10, 1);
	*psc++ = symtab_add_blt_func("ln",	 log,	1);

	*psc++ = symtab_add_blt_func("print", __print,	1);
	*psc++ = symtab_add_blt_func("read", __read,	0);
	*psc++ = symtab_add_blt_func("exit", __exit,	0);

	*psc++ = symtab_add_blt_func("clearstate",	__clearstate, 0);
	*psc++ = symtab_add_blt_func("help",		__help,		 0);

	// constants
	*psc++ = symtab_add_blt_const("PI",				M_PI);
	*psc++ = symtab_add_blt_const("E",				M_E);

	// obsolete
	*psc++ = symtab_add_blt_const("EXIT_SUCCESS",	EXIT_SUCCESS);
	*psc++ = symtab_add_blt_const("EXIT_FAILURE",	EXIT_FAILURE);

	*psc++ = NULL; // terminator
}

// private -----------------------------------------------------------------------------------------------------
// builtin exit func
static double __exit(double unused)
{
	lexer_tokens_dispose();
	symtab_dispose();

	exit(EXIT_SUCCESS);

	return 0;
}

// builtin print func
static double __print(double val)
{
	if(fabs(ceil(val) - val) < DBL_EPSILON) // intval 2.0, 5.0 etc
		printf("\t = %d\n", (int) val);
	else
		printf("\t = %lf\n", val);

	return 0;
}

// builtin read func
static double __read(double unused)
{	
	double val;
	console_set_color(COLOR_GREEN);
	printf("<- ");
	console_reset_color();

	scanf("%lf", &val);
	getchar();

	return val;
}

// builtin clear-state func
// removes all user-defined symbols to start from scratch
static double __clearstate (double unused)
{
	symtab_clear();

	return 0;
}

// shows basic language feature and lists available builtin constants and functions
static double __help (double unused)
{
	printf("\n");
	printf("Each statement ends with ';'\n");
	printf("Declaration:\n\t let a, b, c = 123; \n");
	printf("Allowed operators:\n\t '=', '+', '-', '*', '/', '^'; \n");
	printf("Example:\n\t let a = 2 * 8 + (8 / 2) ^ 2;  \n\n");

	// builtins:
	printf("Builtin definitions:\n");
	for(psc = predefined_symbols; *psc != NULL; psc++)
	{
		if((*psc)->type == TYPE_VAR) {
			printf("- CONST %s = %lf\n", (*psc)->name, (*psc)->data.val);
		} else if((*psc)->type == TYPE_FUNC) {
			printf("- FUNC (%s) %s \n", (*psc)->arg_cnt > 0 ? "a" : " ", (*psc)->name);
		}
	}

	printf("\n");

	return 0;
}

// ------------------------------------------------------------------------------------------------------------