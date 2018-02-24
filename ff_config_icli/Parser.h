/*****************************************************************
 Parser.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#ifndef PARSER_H_
#define PARSER_H_

//#include "processor_out.h"

#include "common_config.h"
#include "NodeMap.h"
#include "parser_config.h"
#include "OutputBuffer.h"
#include "TokenList.h"


typedef struct PARSER_FLAGS {
	uint8_t error_on_line;
//	uint8_t unique_match;
	uint8_t parse_result;
	uint8_t delim_reached;
	uint8_t string_litteral;
	uint8_t match_result;
	uint8_t last_error;
	uint8_t help_active;
//	uint8_t line_complete;
//	uint8_t parse_level; 			// 0-nothing yet, n-current focus, n-1 matched so far
//	uint16_t last_matched_node;
//	uint16_t possible_count;
} P_FLAGS;

class Parser {
private:
	NodeMap map;
	TokenList* possible_list;
	char in_buf[MAX_BUFFER_LENGTH];
	uint8_t in_idx;

	P_FLAGS pf;

	uint8_t ParseMatch(void);
	uint8_t MatchEvaluate(void);

public:
	Parser();
	virtual ~Parser();

	OutputBuffer output;

	void Init();
	uint8_t Parse(char ch);
	void ResetPossibleList(void);
	void GetErrorString(char* error);
	void ResetLine(void);


protected:
	uint8_t P_EOL();
	uint8_t P_DOUBLE_QUOTE();
	uint8_t P_SPACE_TAB();
	void P_ADD_TO_BUFFER(char ch);
};

#endif /* PARSER_H_ */
