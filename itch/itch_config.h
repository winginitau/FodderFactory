/*
 * glitch_config.h
 *
 *  Created on: 18 Mar. 2018
 *      Author: brendan
 */

#ifndef ITCH_CONFIG_H_
#define ITCH_CONFIG_H_

#define VERSION "0.1a"
#define PROG_NAME "itch - Interactive Terminal Command sHell"

//#define IDENTIFIER_REGEX "^[a-zA-Z0-9_]{1,31}$"

#define ITCH_DEBUG

#include "common_config.h"

#ifdef ARDUINO
#define USE_PROGMEM
#endif

typedef struct ENUM_STRING_ARRAY_TYPE {
	// Array of strings relating to enums
	// Inside this struct, extended intitaliser lists are ok
	char text[MAX_BUFFER_LENGTH];
} EnumStringArray;



#endif /* ITCH_CONFIG_H_ */
