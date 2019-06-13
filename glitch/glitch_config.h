/******************************************************************

 glitch - Grammar Lexer and Interactive Terminal Command sHell

 Copyright 2018, 2019, Brendan McLearie
 Distributed under MIT license - see LICENSE.txt
 See also README.txt

 File: glitch_config.h - Configurable compile time parameters
******************************************************************/

#ifndef GLITCH_CONFIG_H_
#define GLITCH_CONFIG_H_

#define VERSION "0.2a"
#define PROG_NAME "glitch - Grammar Lexer and Interactive Terminal Command sHell"

/******************************************************************
	USER CONFIGURABLE PARAMETERS
******************************************************************/

// These should all move to command line options in a future version
// They all relate to the final contents of the user code file (default: out_user_code.cpp)

//#define USER_CODE_HINTS					// Include explanation in user code

//#define USER_CODE_DEBUG					// Insert a debug output into each user function

#define USER_CODE_EXTERNAL_CALL "Reg"		// If defined call a function provided by the user
											// of the same name as the user_code_out function names
											// with this string prepended to the function name
											// passing all args plus the callback function pointer.
											// Useful during grammar development, don't have to worry
											// about any additional user code being overwritten. Put
											// all the real action code elsewhere.

//#define USER_CODE_EXTERNAL_CALL_COMMENTED	// Write the external calls but comment them out

/******************************************************************
	Don't change these unless you are deep diving
******************************************************************/

//#define DEBUG
//#define STDOUT_DEBUG

#define IDENTIFIER_REGEX "^[a-zA-Z0-9_]{1,31}$"

#include "common_config.h"

#ifndef MAX_BUFFER_LENGTH
#define MAX_BUFFER_LENGTH 200
#endif

#define MAX_TEMP_BUFFER_LENGTH 250
#define MAX_OUT_BUFFER_LENGTH 300

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
