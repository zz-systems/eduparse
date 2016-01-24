// ------------------------------------------------------------------------------------------------------------
//	Copyright © 2016 Simon Friedrich	<simon.friedrich@iem.thm.de>
//					 Sergej Zuyev		<sergej.zuyev@zz-systems.net>
//
//  This work is free. You can redistribute it and/or modify it under the
//  terms of the Do What The Fuck You Want To Public License, Version 2,
//  as published by Sam Hocevar. See the COPYING file for more details.
// ------------------------------------------------------------------------------------------------------------

// ... security first. not. we all LOVE buffer overflows...
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "parser.h"
#include "util.h"
#include "predefined_symbols.h"
#include "console.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
	FILE *input;	
	char buf[4096];
	size_t read_chars;

	predefined_symbols_init();

	// file input
	if(argc >= 3 
		&& (strncmp("/file", argv[1], 5) == 0 || strncmp("/f", argv[1], 2) == 0) 
		&& strncmp("spl", utils_get_file_ext(argv[2]), 3) == 0)
	{
		if((input = fopen(argv[2], "r")) == NULL)
		{
			perror("cannot open file");
			exit(EXIT_FAILURE);
		}

		parser_eval(input);
		fclose(input);
	}
	// interactive
	else
	{
		//if((input = open_mem()) == NULL)
		if((input = fopen("TEMP", "w+")) == NULL)
		{
			perror("cannot open temp file");
			exit(EXIT_FAILURE);
		}

		printf("-------------------------------------------------- Interactive mode ---------------------------------------------------\n");
		printf(">PG-project by Simon Friedrich, Sergej Zuyev [2015-2016]\n");
		printf(">enter 'help();' for help\n\n");

		while(TRUE)
		{
			console_set_color(COLOR_GREEN);
			printf(">>> ");
			console_reset_color();

			fgets(buf, 4096, stdin);

			if(strcmp(buf, "\0") == 0 || strcmp(buf, "\n") == 0)
				continue;
			
			read_chars = strlen(buf);

			fwrite(buf, sizeof(char), read_chars, input);
			fseek(input, -(int)(read_chars + 1), SEEK_END);
			//rewind(input);
			parser_eval(input);
		}		

		fclose(input);
		remove("TEMP");
	}

	getc(stdin);

	return EXIT_SUCCESS;
}