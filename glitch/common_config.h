/*****************************************************************
 common_config.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 9 Feb. 2018

******************************************************************/

#ifndef COMMON_CONFIG_H_
#define COMMON_CONFIG_H_


// Visitor pattern result codes

enum {
	R_ERROR = 0,
	R_NONE,
	R_UNFINISHED,
	R_COMPLETE,
	R_IGNORE,
	R_DISCARD,
	R_HELP,
	R_CONTINUE,
	R_REPLAY,
	R_BACKSPACE,
};

// AST types shared across grammar processor and parser
enum {
	AST_UNDEFINED = 0,
	AST_KEYWORD,
	AST_ENUM_ARRAY,
	AST_LOOKUP,
	AST_PARAM_DATE,
	AST_PARAM_TIME,
	AST_PARAM_INTEGER,
	AST_PARAM_UINT8,
	AST_PARAM_UINT16,
	AST_PARAM_UINT32,
	AST_PARAM_INT16,
	AST_PARAM_INT32,
	AST_PARAM_FLOAT,
	AST_PARAM_STRING,
	LAST_AST_TYPE,
};


#endif /* COMMON_CONFIG_H_ */
