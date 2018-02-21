/*****************************************************************
 parser_errors.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 19 Feb. 2018

******************************************************************/

#ifndef PARSER_ERRORS_H_
#define PARSER_ERRORS_H_

#include "common_config.h"

enum {
	PE_ERROR_ERROR = 0,
	PE_NO_ERROR,
	PE_LINE_INCOMPLETE,

	LAST_PARSE_ERROR
};

static const EnumStringArray parser_error_strings[LAST_PARSE_ERROR] = {
		"Processing error type",
		"Unknown error",
		"Command valid but line incomplete",

};



#endif /* PARSER_ERRORS_H_ */
