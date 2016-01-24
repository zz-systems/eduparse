// ------------------------------------------------------------------------------------------------------------
//	Copyright © 2016 Simon Friedrich	<simon.friedrich@iem.thm.de>
//					 Sergej Zuyev		<sergej.zuyev@zz-systems.net>
//
//  This work is free. You can redistribute it and/or modify it under the
//  terms of the Do What The Fuck You Want To Public License, Version 2,
//  as published by Sam Hocevar. See the COPYING file for more details.
// ------------------------------------------------------------------------------------------------------------

#include "util.h"
#include <stdlib.h>

// public -----------------------------------------------------------------------------------------------------

char **utils_resize_string_array(char **loc, unsigned *cur_size, unsigned chunk_size)
{
	unsigned i;
	char ** new_arr = (char**) realloc(loc, ((*cur_size) + chunk_size) * sizeof(char*));

	for(i = 0; i < chunk_size; i++)
	{
		new_arr[(*cur_size) + i] = NEW(char);		
		new_arr[(*cur_size) + i][0] = '\0';
	}

	(*cur_size) += chunk_size;

	return new_arr;
}

// opens a memory stream
FILE *utils_open_mem()
{
	static const int buf_size = 1 << 18;

#ifdef WIN32
	FILE *f = fopen("NUL", "w+"); 
	if(!f)
		perror("Error initializing memory stream");

	if(setvbuf(f, NULL, _IOFBF, buf_size))
		perror("Error initializing memory stream buffer");
#else
	FILE *f = fopen("/dev/null", "w+");	
	if(!f)
		perror("Error initializing memory stream");

	if(setbuffer(f, NULL, buf_size))
		perror("Error creating memory stream");
#endif

	return f;
}

const char *utils_get_file_ext(const char *fname)
{
    const char *dot = strrchr(fname, '.');
	
	return (!dot || dot == fname) 
		? ""		// no ext or smth like '.htaccess'
		: dot + 1;
}

// ------------------------------------------------------------------------------------------------------------