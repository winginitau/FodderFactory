/*
 * glitch_config.h
 *
 *  Created on: 18 Mar. 2018
 *      Author: brendan
 */

#ifndef GLITCH_CONFIG_H_
#define GLITCH_CONFIG_H_

#define VERSION "0.1a"
#define PROG_NAME "glitch - Grammar Lexer and Interactive Terminal Command sHell"

#define IDENTIFIER_REGEX "^[a-zA-Z0-9_]{1,31}$"

//#define USER_CODE_HINTS					// Include explanation in user code //XXX move to cli opt
//#define USER_CODE_DEBUG					// Write out the function being called and its params
#define USER_CODE_EXTERNAL_CALL "Reg"		// If defined call a function provided by the user
											// 	of the same name as the user_code_out function names
											// 	with this string prepended to the function name
											// 	passing all args plus the callback function pointer
#define USER_CODE_EXTERNAL_CALL_COMMENTED	// Write the external calls but comment them out

//#define DEBUG

#include "common_config.h"

#ifndef MAX_BUFFER_LENGTH
#define MAX_BUFFER_LENGTH 200
#endif

#ifndef MAX_BUFFER_WORD_LENGTH
#define MAX_BUFFER_WORD_LENGTH 200
#endif

#ifndef MAX_BUFFER_WORDS_PER_LINE
#define MAX_BUFFER_WORDS_PER_LINE 50
#endif

#define MAX_IDENTIFIERS 300

#ifndef MAX_LABEL_LENGTH
#define MAX_LABEL_LENGTH 200
#endif

#ifdef ARDUINO
#define USE_PROGMEM
#endif

typedef struct ENUM_STRING_ARRAY_TYPE {
	// Array of strings relating to enums
	// Inside this struct, extended intitaliser lists are ok
	char text[MAX_BUFFER_LENGTH];
} EnumStringArray;


#endif /* GLITCH_CONFIG_H_ */
