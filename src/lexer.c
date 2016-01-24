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

#include <string.h>
#include <ctype.h>

// private -----------------------------------------------------------------------------------------------------
// token stream
static struct token_t *tokens_head, *tokens_current, *tokens_tail;
// current lines array size
static unsigned rows_capacity = 1, cur_col_capacity = 1;
// lines
static char ** lines = NULL;

static struct token_t *fetch_next_token(FILE* input);

static char skip_ws			(FILE *istream, unsigned *row, unsigned *col);
static void lex_op			(FILE *istream, unsigned *row, unsigned *col, struct token_t *target);
static void lex_identifier	(FILE *istream, unsigned *row, unsigned *col, struct token_t *target);
static void lex_number		(FILE *istream, unsigned *row, unsigned *col, struct token_t *target);

// public -----------------------------------------------------------------------------------------------------

void lexer_tokenize(FILE *input)
{
	struct token_t *new_token = tokens_head = NULL;

	while(!feof(input) && (new_token == NULL || new_token->token != TOKEN_UNDEFINED))
	{
		if(new_token == NULL)
			tokens_head = new_token = fetch_next_token(input);
		else 
			new_token->next = fetch_next_token(input);

		if(new_token->next != NULL)
		{
			new_token->next->prev	= new_token;
			new_token				= new_token->next;
		}
	}
	
	// Token stream terminator (tok-tok-tok-term)
	new_token->next = NEW(struct token_t);
	new_token->next->row = new_token->next->col = 0; 
	new_token->next->token = TOKEN_EOT; 
	new_token->next->next = NULL;
	new_token->next->prev = new_token;

	tokens_current = tokens_head;
	tokens_tail = new_token;
}

struct token_t *lexer_tokens_begin()
{
	return tokens_current;
}

void lexer_tokens_dispose()
{	
	struct token_t *current = tokens_head;

	if(tokens_head == NULL)
		return;

	do
	{
		if(current->prev != NULL)
			DELETE(current->prev);
		current->prev = NULL;
	}
	while((current = current->next) != NULL);

	tokens_head = NULL;
}

struct token_t *lexer_next_token()
{
	return tokens_current != NULL ? tokens_current = tokens_current->next : NULL;
}

const char *lexer_get_associated_line(struct token_t* token)
{
	return lines[token->row];
}

// private -----------------------------------------------------------------------------------------------------

// lex next token and append to token stream
static struct token_t *fetch_next_token(FILE* input)
{
	char c;
	int is_ws = 0, lci = 0, has_dot = FALSE;
	char buf[256], *b = buf;
	static unsigned col = 0, row = 0;

	struct token_t *result = NEW(struct token_t);
	result->token = TOKEN_UNDEFINED;	

	if(lines == NULL)
		lines = NEWCAP(char*, CHUNK_SIZE);

	if(row >= rows_capacity)
		lines = utils_resize_string_array(lines, &rows_capacity, CHUNK_SIZE);

	if(col <= 0)
	{
		lines[row] = NEW(char);
		lines[row][0] = '\0';
	}

	// whitespaces
	if(EOF == (c = skip_ws(input, &row, &col)))
	{
		result->token = TOKEN_EOT;
		return result;
	}

	result->prev	= NULL;
	result->next	= NULL;
	result->col		= col;
	result->row		= row;

	// operator
	if(result->token == TOKEN_UNDEFINED)
		lex_op(input, &row, &col, result);

	// identifier
	if(result->token == TOKEN_UNDEFINED)
		lex_identifier(input, &row, &col, result);
	
	// number
	if(result->token == TOKEN_UNDEFINED)
		lex_number(input, &row, &col, result);
	

	return result;
}

// skip whitespaces
static char skip_ws(FILE *istream, unsigned *row, unsigned *col)
{
	char c;
	char cstr[2] = {'\0'};

	do
	{		
		c = cstr[0] = fgetc(istream);
		
		/*if(c == ' ' || c == '\t' || c == '\n')
			lines[*row] = strcat(lines[*row], cstr);*/

		if(c == ' ')
		{
			(*col)++; 
			lines[*row] = strcat(lines[*row], cstr);
		}
		else if(c == '\t')
		{
			(*col) += 4; 
			lines[*row] = strcat(lines[*row], cstr);
		}
		else if(c == '\n')
		{
			(*col) = 0;
			(*row)++;

			if(*row >= rows_capacity)
			{
				lines = utils_resize_string_array(lines, &rows_capacity, CHUNK_SIZE);
			}
		}
		else
		{
			ungetc(c, istream);
			return c;
		}		
	}
	while(c != EOF);

	return c;
}

// lex operators
static void lex_op(FILE *istream, unsigned *row, unsigned *col, struct token_t *target)
{
	enum lexer_tokens result	= TOKEN_UNDEFINED;
	char cstr[2] = {'\0'};
	
	char c = cstr[0] = fgetc(istream);
	

	switch(c)
	{
	case '+': result = TOKEN_PLUS;			break;
	case '-': result = TOKEN_MINUS;			break;
	case '*': result = TOKEN_MUL;			break;
	case '/': result = TOKEN_DIV;			break;

	case '(': result = TOKEN_PAR_L;			break;
	case ')': result = TOKEN_PAR_R;			break;		
	case ';': result = TOKEN_STAT_DELIM;	break;	
	case ',': result = TOKEN_FRAG_DELIM;	break;	
	case '^': result = TOKEN_EXP;			break;
	case '=': result = TOKEN_ASSIGN;		break;	
	}
	
	if(result != TOKEN_UNDEFINED)
	{
		lines[*row] = strcat(lines[*row], cstr);
		(*col)++;
	}
	else ungetc(c, istream);

	target->token = result;
}

// lex identifiers
static void lex_identifier(FILE *istream, unsigned *row, unsigned *col, struct token_t *target)
{	
	char buf[256], *b = buf;

	char c = fgetc(istream);

	// identifier
	if(isalpha(c))
	{
		do { *b++ = c; (*col)++; }
		while((c = fgetc(istream)) != EOF && (isalnum(c) || c == '_'));		

		// EOS
		*b = '\0';

		// append to lines
		lines[*row] = strcat(lines[*row], buf); 

		// keyword
		if(strcmp(buf, "let") == 0)
			target->token = TOKEN_DECLARE;
		else
		{
			target->token = TOKEN_IDENTIFIER;
			target->identifier = NEWCAP(char, strlen(buf) + 1);
			strcpy(target->identifier, buf);
		}
	}

	// restore last char (not satisfying identifier constraints)
	ungetc(c, istream);
}

// lex numbers
static void lex_number(FILE *istream, unsigned *row, unsigned *col, struct token_t *target)
{
	char buf[256], *b = buf;
	char c = fgetc(istream);
	int has_dot = FALSE;

	if((has_dot = (c == '.')) || isdigit(c))
	{
		do { *b++ = c; (*col)++;}
		while((c = fgetc(istream)) != EOF && (isdigit(c) || (!has_dot && (has_dot = (c == '.')))));		

		*b = '\0';
				
		// append to lines
		lines[*row] = strcat(lines[*row], buf); 
		
		target->token = TOKEN_NUMBER;
		// parse value
		target->value = atof(buf);		
	}	

	// restore last char.
	ungetc(c, istream);
}

// ------------------------------------------------------------------------------------------------------------