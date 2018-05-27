/*****************************************************************
 ff_global_defs.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 26 May 2018

******************************************************************/

#ifndef FF_GLOBAL_DEFS_H_
#define FF_GLOBAL_DEFS_H_

#include <ff_sys_config.h>

#ifdef USE_PROGMEM
typedef struct SIMPLE_STRING_ARRAY_TYPE {
	// Array of strings (usually labels and messages) relating to enums
	// Inside this struct, extended intitaliser lists are ok
	char text[MAX_MESSAGE_STRING_LENGTH];
} SimpleStringArray;

#else

typedef struct SIMPLE_STRING_ARRAY_TYPE {
	// Array of strings (usually labels and messages) relating to enums
	// Inside this struct, extended intitaliser lists are ok
	const char* text;
} SimpleStringArray;
#endif





#endif /* FF_GLOBAL_DEFS_H_ */
