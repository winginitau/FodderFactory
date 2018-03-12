/*****************************************************************
 parser_errors.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 19 Feb. 2018

******************************************************************/

#ifndef PARSER_ERRORS_H_
#define PARSER_ERRORS_H_

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "common_config.h"

enum {
	PE_ERROR = 0,
	PEME_NO_ERROR,
	PE_LINE_INCOMPLETE,
	PE_MULTI_NOT_PARSED,
	PE_EOL_WITH_UKNOWN_MR,
	PE_EOL_WITH_NO_MATCH,
	PE_NODEMAP_RESULT_NOT_CAUGHT,
	ME_MATCHREDUCE_EXPECTED_1,
	PE_LOOKUP_PASSED_TO_REDUCER,
	PE_MULTIPLE_PARAM_LOOKAHEAD,
	PE_FUNC_XLAT_NOT_MATCHED_IN_CALLFUNCTION,
	PE_BAD_PARAM_ACTION_DISPATCHER,
	ME_GETASTABYID_FAIL,
	PE_NO_PARSE_RESULT,


	LAST_PARSE_ERROR
};

#ifdef USE_PROGMEM
static const EnumStringArray parser_error_strings[LAST_PARSE_ERROR] PROGMEM = {
#else
static const EnumStringArray parser_error_strings[LAST_PARSE_ERROR] = {
#endif
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
		"func_xlat not matched in CallFunction - no matching function?",
		"ActionDispatcher: Unknown or incorrect parameter type found in param_list while assembling ParamUnion- keyword?",
		"GetASTAByID: ASTA ID Not Found or array index going out of bounds",
		"Parser did not return a result at all. Error in Parser?",

};



#endif /* PARSER_ERRORS_H_ */
