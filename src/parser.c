// ------------------------------------------------------------------------------------------------------------
//	Copyright © 2016 Simon Friedrich	<simon.friedrich@iem.thm.de>
//					 Sergej Zuyev		<sergej.zuyev@zz-systems.net>
//
//  This work is free. You can redistribute it and/or modify it under the
//  terms of the Do What The Fuck You Want To Public License, Version 2,
//  as published by Sam Hocevar. See the COPYING file for more details.
// ------------------------------------------------------------------------------------------------------------

#include "lexer.h"
#include "symtab.h"
#include "parser.h"
#include "util.h"
#include "console.h"

#include <setjmp.h> // "exception" handling
#include <stdio.h>	// I/O lib
#include <math.h>

// private -----------------------------------------------------------------------------------------------------

static struct token_t *state;
static jmp_buf exception_target;

static struct token_t *statement();
static struct token_t *expression();
static struct token_t *term();
static struct token_t *operand();
static struct token_t *mod_operand();
static struct token_t *declaration();
static struct token_t *declaration_fragment();
static struct token_t *assignment();


static BOOL eot();
static void skip();
static struct token_t *prev();

static BOOL peek	(enum lexer_tokens token);
static BOOL peekn	(enum lexer_tokens token, unsigned lookahead);

static BOOL accept(enum lexer_tokens token);
static BOOL expect(enum lexer_tokens token);

static void raise_syntactic_error	(struct token_t *erroneus, enum lexer_tokens *expected);
static void raise_semantic_error	(struct token_t *erroneus, const char *message, BOOL is_critical);

// public -----------------------------------------------------------------------------------------------------

void parser_eval(FILE *input)
{
	// pass 1
	lexer_tokenize(input);
	state = lexer_tokens_begin();

	// try
	if(!setjmp(exception_target))	
	{
		// pass 2
		while(!peek(TOKEN_EOT))
		{
			statement();
		}
	}
	// catch(...)
	else							
	{
		// nop;
	}

	lexer_tokens_dispose();
}

// private -----------------------------------------------------------------------------------------------------
// GRAMMAR -----------------------------------------------------------------------------------------------------

// declaration := 'let' (declaration_fragment) [',' (declaration_fragment)]
static struct token_t *declaration()
{
	struct token_t *result;

	result = state;

	do
	{
		declaration_fragment();
	}
	while(accept(TOKEN_FRAG_DELIM));

	return result;
}

// declaration_fragment := identifier [assignment]
static struct token_t *declaration_fragment()
{
	struct token_t *id;
	struct symbol_t *sym;
	
	expect(TOKEN_IDENTIFIER);
	id = prev();

	if(symtab_find(id->identifier))	{
		raise_semantic_error(id, "Already declared", FALSE);
	} else {
		sym = symtab_add_var(id->identifier, 0.0);
		id->value = sym->data.val;
	}

	if(peek(TOKEN_ASSIGN)) {
		return assignment();
	}	

	return id;
}

// assignment := '=' expression
static struct token_t *assignment()
{	
	struct token_t  *id		= state->prev;
	struct symbol_t *sym	= symtab_find(id->identifier);

	if(sym == NULL)	{
		raise_semantic_error(id, "Undefined", TRUE);
	}

	if(sym ->is_builtin) {
		raise_semantic_error(id, "Attemtping to overwrite builtin", TRUE);
	}

	if(expect(TOKEN_ASSIGN)){
		id->value = sym->data.val = expression()->value;
	}

	return id;
}

// statement := (declaration | expression) ';'
static struct token_t *statement()
{	
	struct token_t *result;

	if(accept(TOKEN_DECLARE))
	{
		result = declaration();
	}
	else if(peek(TOKEN_IDENTIFIER) && peekn(TOKEN_ASSIGN, 1))
	{
		accept(TOKEN_IDENTIFIER);

		result = assignment();
	}
	else
	{
		result = expression();	
	}

	expect(TOKEN_STAT_DELIM);

	return result;
}

// expression := term [{(+|-) term}]
static struct token_t *expression()
{
	struct token_t *oper0 = term();
		
	while(peek(TOKEN_PLUS) || peek(TOKEN_MINUS))
	{		
		if(accept(TOKEN_PLUS))
			oper0->value += term()->value;
		else if(accept(TOKEN_MINUS))
			oper0->value -= term()->value;
	}

	return oper0;
}

// term := operand [{(*|/) operand}]
static struct token_t *term()
{
	struct token_t *oper0 = mod_operand();
		
	while(peek(TOKEN_MUL) || peek(TOKEN_DIV))
	{		
		if(accept(TOKEN_MUL))
			oper0->value *= mod_operand()->value;
		else if(accept(TOKEN_DIV))
			oper0->value /= mod_operand()->value;
	}

	return oper0;
}

// mod_operand = [prefix_op] operand [postfix_op]
static struct token_t *mod_operand()
{
	struct token_t *oper, *prefix_op = NULL;	

	if(accept(TOKEN_MINUS))
		prefix_op = prev();

	oper = operand();

	if(accept(TOKEN_EXP))
	{
		oper->value = pow(oper->value, mod_operand()->value);
	}

	if(prefix_op != NULL && prefix_op->token == TOKEN_MINUS)
		oper->value = -oper->value;

	return oper;
}

// identifier
static struct token_t *identifier()
{
	struct token_t *id = state->prev, *expr;
	struct symbol_t *sym;

	if((sym = symtab_find(id->identifier)) == NULL)
		raise_semantic_error(id, "Undeclared item", TRUE);

	if(accept(TOKEN_PAR_L)) // call
	{
		if(sym->type != TYPE_FUNC)
			raise_semantic_error(id, "Non-function used like a function", TRUE);
		if(sym->arg_cnt == 0)
		{
			if(!accept(TOKEN_PAR_R)) // overridden error msg (semantic)
				raise_semantic_error(id, "Too many args for", TRUE);

			id->value = sym->data.func(0);
		}
		else if(sym->arg_cnt == 1)
		{
			if(peek(TOKEN_PAR_R))
				raise_semantic_error(id, "Too few args for", TRUE);

			expr = expression();
			
			expect(TOKEN_PAR_R);

			id->value = sym->data.func(expr->value);
		}
	}
	else
	{
		if(sym->type != TYPE_VAR)
			raise_semantic_error(id, "Non-variable used like a variable", TRUE);

		id->value = sym->data.val;
	}

	return id;
}

// operand := number | identifier | '(' expression ')'
static struct token_t *operand()
{	
	enum lexer_tokens *expected;
	struct token_t *temp;

	if(accept(TOKEN_NUMBER))			// num
	{
		return state->prev;
	}	
	else if(accept(TOKEN_IDENTIFIER))	// var | fun
	{
		return identifier();
	}	
	else if(accept(TOKEN_PAR_L))		// ( expr )
	{
		temp = expression();

		expect(TOKEN_PAR_R);

		return temp;
	}
	else // syntax error
	{
		expected = NEWCAP(enum lexer_tokens, 3);
		expected[0] = TOKEN_NUMBER;
		expected[1] = TOKEN_IDENTIFIER;
		expected[2] = TOKEN_PAR_L;

		raise_syntactic_error(state, expected);			
		return NULL; // unreachable
	}
}

// UTIL --------------------------------------------------------------------------------------------------------

// End of token stream
static BOOL eot()
{
	return state == NULL || state->token == TOKEN_EOT;
}

// Skip current token
static void skip()
{
	state = lexer_next_token();
}

// Get previous token
static struct token_t *prev()
{
	return state != NULL ? state->prev : NULL;
}

// lookahead = 1
static BOOL peek(enum lexer_tokens token)
{
	if(state != NULL && state->token == token)
		return TRUE;

	return FALSE;
}

// lookahead = N
static BOOL peekn(enum lexer_tokens token, unsigned lookahead)
{
	struct token_t *current = state;

	for(; lookahead > 0; lookahead--)
		current = state->next;

	if(current != NULL && current->token == token)
		return TRUE;

	return FALSE;
}

// non-failing token check
static BOOL accept(enum lexer_tokens token)
{
	if(state != NULL && state->token == token)
	{
		state = lexer_next_token();
		return TRUE;
	}
	return FALSE;
}

// failing token check
static BOOL expect(enum lexer_tokens token)
{
	enum lexer_tokens *expected;

	if(state != NULL && state->token == token)
	{
		state = lexer_next_token();
		return TRUE;
	}

	expected = NEWCAP(enum lexer_tokens, 2);
	expected[0] = token;
	expected[1] = TOKEN_UNDEFINED;

	raise_syntactic_error(state->token != TOKEN_EOT ? state : state->prev, expected);
	return FALSE;
}

// Syntax error handler
static void raise_syntactic_error(struct token_t *erroneous, enum lexer_tokens *expected)
{
	int col;
	enum lexer_tokens *current = expected;
	const char *line = lexer_get_associated_line(erroneous);

	console_set_color(COLOR_RED);

	printf("Line %d: syntax error, expected ", erroneous->row + 1);

	while(*current != TOKEN_UNDEFINED && *current > 0 && *current < TOKEN_TERMINATOR)		
	{
		printf("'%s'", lexer_tokens_str[*current]);
		if(*(current + 1) != TOKEN_UNDEFINED)
			printf(" or ");

		current++;
	}
	printf(" :\n%s\n", line);

	for(col = 0; col < erroneous->col; col++)
		printf("-");

	printf("^\n\n");

	console_reset_color();

	DELETE(expected);

	longjmp(exception_target, TRUE);
}

// Semantic error handler
static void raise_semantic_error(struct token_t *erroneous, const char *message, BOOL is_critical)
{
	int col;
	const char *line = lexer_get_associated_line(erroneous);

	console_set_color(is_critical ? COLOR_RED : COLOR_YELLOW);

	printf("Line %d: %s '%s':\n%s\n", erroneous->row + 1, message, erroneous->identifier, line);

	for(col = 0; col <= erroneous->col; col++)
		printf("-");

	printf("^\n\n");

	console_reset_color();

	if(is_critical)
		longjmp(exception_target, TRUE);
}

// ------------------------------------------------------------------------------------------------------------