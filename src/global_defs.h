/*****************************************************************
 ff_global_defs.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 26 May 2018

******************************************************************/

#ifndef GLOBAL_DEFS_H_
#define GLOBAL_DEFS_H_

#include <build_config.h>

#ifdef USE_PROGMEM
typedef struct SIMPLE_STRING_ARRAY_TYPE {
	// Array of strings (usually labels and messages) relating to enums
	// Inside this struct, extended intitaliser lists are ok
	char text[MAX_LABEL_LENGTH];
} SimpleStringArray;

#else

typedef struct SIMPLE_STRING_ARRAY_TYPE {
	// Array of strings (usually labels and messages) relating to enums
	// Inside this struct, extended intitaliser lists are ok
	const char* text;
} SimpleStringArray;
#endif





#endif /* GLOBAL_DEFS_H_ */
