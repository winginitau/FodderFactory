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

//#define ITCH_DEBUG

#include "common_config.h"

#ifdef ARDUINO
#define USE_PROGMEM
#endif

#ifdef MAX_LABEL_LENGTH						// As defined in src/build_config.h
#define MAX_TOKEN_SIZE MAX_LABEL_LENGTH		// MAX_TOKEN_SIZE only used in itch parser
#else
#define MAX_TOKEN_SIZE 48
#endif


typedef struct ENUM_STRING_ARRAY_TYPE {
	// Array of strings relating to enums
	// Inside this struct, extended intitaliser lists are ok
	char text[200];
} EnumStringArray;



#endif /* ITCH_CONFIG_H_ */
