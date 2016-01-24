// ------------------------------------------------------------------------------------------------------------
//	Copyright © 2016 Simon Friedrich	<simon.friedrich@iem.thm.de>
//					 Sergej Zuyev		<sergej.zuyev@zz-systems.net>
//
//  This work is free. You can redistribute it and/or modify it under the
//  terms of the Do What The Fuck You Want To Public License, Version 2,
//  as published by Sam Hocevar. See the COPYING file for more details.
// ------------------------------------------------------------------------------------------------------------

#ifndef CONSOLE_H
#define CONSOLE_H

// ... security first. not. we all LOVE buffer overflows...
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>

// used colors
enum color 
{
	COLOR_RED,
	COLOR_YELLOW,
	COLOR_GREEN
};

// sets a color for the following block
void console_set_color(enum color color);

// resets the color for the following block
void console_reset_color();

#endif