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


class Parser {
private:
	NodeMap map;

	uint8_t parse_level; 			// 0-nothing yet, n-current focus, n-1 matched so far
	uint16_t last_matched_node;
	TokenList* possible_list;
	uint16_t possible_count;
	uint8_t error_on_line;
	uint8_t unique_match;
	uint8_t result;
	uint8_t line_complete;
	uint8_t delim_reached;
	uint8_t string_litteral;
	uint8_t match_result;
	uint8_t last_error;
	uint8_t help_active;
	char in_buf[MAX_BUFFER_LENGTH];
	uint8_t in_idx;

	uint8_t ParseStage2(void);

public:
	Parser();
	virtual ~Parser();

	OutputBuffer output;

	void Init();
	uint8_t Parse(char ch);
	void ResetPossibleList(void);
	void GetErrorString(char* error);
	void ResetLine(void);

};

#endif /* PARSER_H_ */
