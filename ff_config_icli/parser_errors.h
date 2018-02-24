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
	PE_MULTI_NOT_PARSED,
	PE_EOL_WITH_UKNOWN_MR,
	PE_EOL_WITH_NO_MATCH,
	PE_NODEMAP_RESULT_NOT_CAUGHT,
	PE_MATCHREDUCE_EXPECTED_1,
	PE_LOOKUP_PASSED_TO_REDUCER,
	PE_MULTIPLE_PARAM_LOOKAHEAD,


	LAST_PARSE_ERROR
};

static const EnumStringArray parser_error_strings[LAST_PARSE_ERROR] = {
		"Processing error type",
		"Unknown error",
		"Line valid up to EOL but incomplete",
		"Multiple non-unique matches were found for some or all terms and could not be parsed",
		"EOL reached and the MR result from NodeMap is unknown",
		"Term or terms could not be matched",
		"NodeMap result was not caught by the parser",
		"Fatal error in MatchReduce: size != 1 after iterative delete to unique node",
		"Fatal error in MatchReduce: unique node of type AST_LOOKUP passed to reducer",
		"In MatchReduce - Next parse step dependent on multiple user-param nodes which can't be distinguished",

};



#endif /* PARSER_ERRORS_H_ */
