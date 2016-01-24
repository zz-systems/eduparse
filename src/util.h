// ------------------------------------------------------------------------------------------------------------
//	Copyright © 2016 Simon Friedrich	<simon.friedrich@iem.thm.de>
//					 Sergej Zuyev		<sergej.zuyev@zz-systems.net>
//
//  This work is free. You can redistribute it and/or modify it under the
//  terms of the Do What The Fuck You Want To Public License, Version 2,
//  as published by Sam Hocevar. See the COPYING file for more details.
// ------------------------------------------------------------------------------------------------------------

#ifndef UTIL_H
#define UTIL_H

// ... security first. not. we all LOVE buffer overflows...
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 
#endif

#include <string.h>
#include <stdio.h>

#ifndef NULL
#define NULL 0
#endif

#define CHUNK_SIZE (5)

#define NEW(type)			((type*) malloc(sizeof(type)))
#define NEWCAP(type, size)	((type*) malloc(size * sizeof(type)))

#define DELETE(var) free(var)

#define BOOL short
#define TRUE 1
#define FALSE 0

// resizes an array of strings
char **utils_resize_string_array(char **loc, unsigned *old_size, unsigned chunk_size);

// open memory stream (seems not to work)
FILE *utils_open_mem();

// get the extension of a file name
const char *utils_get_file_ext(const char *fname);

#endif