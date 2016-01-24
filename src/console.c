// ------------------------------------------------------------------------------------------------------------
//	Copyright © 2016 Simon Friedrich	<simon.friedrich@iem.thm.de>
//					 Sergej Zuyev		<sergej.zuyev@zz-systems.net>
//
//  This work is free. You can redistribute it and/or modify it under the
//  terms of the Do What The Fuck You Want To Public License, Version 2,
//  as published by Sam Hocevar. See the COPYING file for more details.
// ------------------------------------------------------------------------------------------------------------

#include "console.h"

#ifdef WIN32
#include <windows.h>
static HANDLE h_console = NULL;
static WORD saved_attr;
#endif

// public -----------------------------------------------------------------------------------------------------

void console_set_color(enum color color)
{
#ifdef WIN32
	CONSOLE_SCREEN_BUFFER_INFO console_info;	
	WORD new_attr;

	if(h_console == NULL) 
		h_console = GetStdHandle(STD_OUTPUT_HANDLE);	

	GetConsoleScreenBufferInfo(h_console, &console_info);
	saved_attr = console_info.wAttributes;	

	switch(color)
	{
	case COLOR_RED:		new_attr = FOREGROUND_RED | FOREGROUND_INTENSITY;						break;
	case COLOR_YELLOW:	new_attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;	break;
	case COLOR_GREEN:	new_attr = FOREGROUND_GREEN | FOREGROUND_INTENSITY;						break;
	}
	SetConsoleTextAttribute(h_console, new_attr);
#else
	switch(color)
	{
	case COLOR_RED:		printf("\x1b[31m"); break;
	case COLOR_GREEN:	printf("\x1b[32m"); break;
	case COLOR_YELLOW:	printf("\x1b[33m"); break;
	}
#endif
}

void console_reset_color()
{
#ifdef WIN32
	if(h_console == NULL)
		return;

	SetConsoleTextAttribute(h_console, saved_attr);
#else
	printf("\x1b[0m");
#endif
}

// ------------------------------------------------------------------------------------------------------------