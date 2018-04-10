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

#define DEBUG

#include "common_config.h"

#ifdef ARDUINO
#define USE_PROGMEM
#endif

typedef struct ENUM_STRING_ARRAY_TYPE {
	// Array of strings relating to enums
	// Inside this struct, extended intitaliser lists are ok
	char text[MAX_BUFFER_LENGTH];
} EnumStringArray;

enum {
	MISC_ERROR = 0,
	MISC_HELP_HEADING,
	MISC_ERROR_PROMPT,
	MISC_ERROR_HEADER,
	MISC_PROMPT_BASE,
	MISC_CRNL,
	LAST_MISC_STRING
};

#ifdef USE_PROGMEM
static const EnumStringArray misc_strings[LAST_MISC_STRING] PROGMEM = {
#else
static const EnumStringArray misc_strings[LAST_MISC_STRING] = {
#endif
	"MISC_ERROR",
	"\nCommand Help:\n",
	"\n\rTry \"?\" or <command> ? for help.\n\r",
	"\n\r>>> Error:\n\r>>> ",
	"\n\r$ ",
	"\n\r",
};


#endif /* ITCH_CONFIG_H_ */
