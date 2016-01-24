#include "console.h"

#ifdef WIN32
#include <Windows.h>
#endif

void console_set_color(enum color color)
{
#ifdef WIN32
	// der kram. mapping von color -> windoofs color
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
	// der kram. windoofs reset
#else
	printf("\x1b[0m");
#endif
}