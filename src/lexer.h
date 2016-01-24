// ------------------------------------------------------------------------------------------------------------
//	Copyright © 2016 Simon Friedrich	<simon.friedrich@iem.thm.de>
//					 Sergej Zuyev		<sergej.zuyev@zz-systems.net>
//
//  This work is free. You can redistribute it and/or modify it under the
//  terms of the Do What The Fuck You Want To Public License, Version 2,
//  as published by Sam Hocevar. See the COPYING file for more details.
// ------------------------------------------------------------------------------------------------------------

#ifndef LEXER_H
#define LEXER_H

// ... security first. not. we all LOVE buffer overflows...
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>

// accepted token types for this simple calculator language
enum lexer_tokens 
{
	TOKEN_UNDEFINED,	// internal
	TOKEN_EOT,			// internal
	TOKEN_STAT_DELIM,	// ;
	TOKEN_FRAG_DELIM,	// ,

	// operands =====
	TOKEN_NUMBER,		// [0-9][.]{0-9}
	TOKEN_IDENTIFIER,	// (a-zA-Z)[{(a-zA-Z0-9_)}]

	// parentheses ==
	TOKEN_PAR_L,		// (
	TOKEN_PAR_R,		// )

	// operators ====
	TOKEN_PLUS,			// +
	TOKEN_MINUS,		// -
	TOKEN_MUL,			// *
	TOKEN_DIV,			// /
	TOKEN_EXP,			// ^
	TOKEN_ASSIGN,		// =

	// declaration
	TOKEN_DECLARE,		// let

	TOKEN_TERMINATOR
};

// Human readable token "names"
static const char * lexer_tokens_str[] = 
{
	"Undefined",
	"End of token stream",

	";", ",",

	"number", "identifer",

	"(", ")",

	"+", "-", "*", "/", "^", "=",

	"let",
	"###"
};

// token struct
// contains information about
// - token type
// - value (if any)
// - identifier (if this is an identifier token type - otherwise undefined)
// - previous and next chain entry (double linked list)
// - column, row for error reporting
struct token_t 
{	
	char		*identifier;
	struct token_t *prev, *next;

	enum lexer_tokens token;	
	int			col, row;	

	double		value;
};

// first pass. tokenize the input stream - which will be consumed by the parser
void			lexer_tokenize				(FILE *input);

// return tokenstream head
struct token_t *lexer_tokens_begin			();

// free memory used by token stream
void			lexer_tokens_dispose		();

// fetch the next token and advance the token stream offset
struct token_t *lexer_next_token			();

// get the text line where the tocen appears
const char	   *lexer_get_associated_line	(struct token_t* token);

#endif