// ------------------------------------------------------------------------------------------------------------
//	Copyright © 2016 Simon Friedrich	<simon.friedrich@iem.thm.de>
//					 Sergej Zuyev		<sergej.zuyev@zz-systems.net>
//
//  This work is free. You can redistribute it and/or modify it under the
//  terms of the Do What The Fuck You Want To Public License, Version 2,
//  as published by Sam Hocevar. See the COPYING file for more details.
// ------------------------------------------------------------------------------------------------------------

#ifndef PREDEFINED_SYMBOLS
#define PREDEFINED_SYMBOLS

// ... security first. not. we all LOVE buffer overflows...
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// fills the symbol table with predefined functions and constants
void predefined_symbols_init();

#endif