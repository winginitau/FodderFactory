/*****************************************************************
 NodeMap.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#ifndef NODEMAP_H_
#define NODEMAP_H_


#include "TokenList.h"
#include "common_config.h"
#include "out.h"
#include <stdint.h>

// Possible Match Results
enum {
	MR_ERROR = 0,
	MR_NO_RESULT,
	MR_NO_MATCH,
	MR_MULTI,
	MR_UNIQUE_KEYWORD,
	MR_UNIQUE_IDENT,
	MR_PARAM,
	MR_LOOK_RIGHT,
	MR_INCOMPLETE,
	MR_ACTION_POSSIBLE,
	MR_DELIM_SKIP,
	MR_CONTINUE,
	MR_UNIQUE,
	MR_HELP_ACTIVE,
};

typedef struct MATCH_FLAGS {
	uint8_t help_active;
	uint8_t keyword_match;
	uint8_t last_matched_id;
	uint8_t match_result;
	uint16_t error_code;
} M_FLAGS;


	void MapReset(void);
	uint16_t MapGetASTAByID(uint16_t ASTA_ID, ASTA* result);
	uint16_t MapGetLastMatchedID();
	uint8_t MapMatch(char* target, TokenList* match_list);
	char* MapGetLastTargetString(char* return_string);
	uint16_t MapAdvance(uint8_t in_buf_idx);
	uint8_t MapGetAction(uint16_t asta_id, char* action_str);
	uint16_t MapMatchReduce(TokenList* list);
	uint16_t MapGetErrorCode();

	uint8_t MapDetermineTarget(uint8_t* target_size, char* target, char* line);
	void MapSelectMatchingNodes(char* target, TokenList* matched_list);
	uint8_t MapEvaluateMatchedList(TokenList* matched_list);
	uint8_t Compare_N_PARAM_DATE(char* target, ASTA* temp_node);
	uint8_t Compare_N_PARAM_TIME(char* target, ASTA* temp_node);
	uint8_t Compare_N_PARAM_FLOAT(char* target, ASTA* temp_node);
	uint8_t Compare_N_PARAM_INTEGER(char* target, ASTA* temp_node);
	uint8_t Compare_N_PARAM_STRING(char* target, ASTA* temp_node);
	uint8_t Compare_N_LOOKUP(char* target, ASTA* temp_node);
	uint8_t Compare_N_IDENTIFIER(char* target, ASTA* temp_node);
	uint8_t Compare_N_KEYWORD(char* target, ASTA* temp_node);

#endif /* NODEMAP_H_ */
