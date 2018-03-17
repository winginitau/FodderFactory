/*****************************************************************
 parser_config.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 18 Feb. 2018

******************************************************************/

#ifndef PARSER_CONFIG_H_
#define PARSER_CONFIG_H_

#include "common_config.h"


/*
#ifndef MAX_BUFFER_WORD_LENGTH
#define MAX_BUFFER_WORD_LENGTH 50
#endif

#ifndef MAX_BUFFER_LENGTH
#define MAX_BUFFER_LENGTH 150
#endif

#ifndef MAX_BUFFER_LINE_LENGTH
#define MAX_BUFFER_LINE_LENGTH 150
#endif

*/


enum {
	MISC_ERROR = 0,
	MISC_HELP_HEADING,
	MISC_ERROR_PROMPT,
	MISC_ERROR_HEADER,
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
};




#endif /* PARSER_CONFIG_H_ */
