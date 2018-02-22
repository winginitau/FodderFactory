/*****************************************************************
 defines.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 9 Feb. 2018

******************************************************************/

#ifndef COMMON_CONFIG_H_
#define COMMON_CONFIG_H_

#define DEBUG


#define MAX_BUFFER_LENGTH 150
#define MAX_BUFFER_WORD_LENGTH 150
#define MAX_BUFFER_WORDS_PER_LINE 30
#define MAX_IDENTIFIERS 50

typedef struct ENUM_STRING_ARRAY_TYPE {
	// Array of strings (usually labels and messages) relating to enums
	// Inside this struct, extended intitaliser lists are ok
	char text[MAX_BUFFER_LENGTH];
} EnumStringArray;


// Visitor pattern result codes

enum {
	R_ERROR = 0,
	R_NONE,
	R_UNFINISHED,
	R_COMPLETE,
	R_IGNORE,
	R_HELP,
	R_CONTINUE,
};

// AST types shared across grammar processor and parser
enum {
	AST_UNDEFINED = 0,
	AST_KEYWORD,
	AST_IDENTIFIER,
	AST_LOOKUP,
	AST_PARAM_STRING,
	AST_PARAM_INTEGER,
	AST_PARAM_FLOAT,
	AST_PARAM_TIME,
	AST_PARAM_DATE,
	LAST_AST_TYPE,
};


#endif /* COMMON_CONFIG_H_ */
