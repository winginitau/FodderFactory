/*****************************************************************
 NodeMap.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#ifndef NODEMAP_H_
#define NODEMAP_H_

#include "MapNode.h"
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


class NodeMap {
private:
	M_FLAGS mf;
	uint8_t line_pos;
	uint16_t id_current;	// currently matched node
	uint16_t id_walker;		// to walk through the asta array nodes by id

public:
	NodeMap();
	virtual ~NodeMap();

	char last_target[MAX_BUFFER_LENGTH];

	void Reset(void);
	uint16_t GetASTAByID(uint16_t ASTA_ID, ASTA* result);
	uint16_t GetLastMatchedID();
	uint8_t Match(char* target, TokenList* match_list);
	char* GetLastTargetString(char* return_string);
	uint16_t Advance(uint8_t in_buf_idx);
	uint8_t GetAction(uint16_t asta_id, char* action_str);
	uint16_t MatchReduce(TokenList* list);
	uint16_t GetErrorCode();

protected:
	uint8_t DetermineTarget(uint8_t* target_size, char* target, char* line);
	void SelectMatchingNodes(char* target, TokenList* matched_list);
	uint8_t EvaluateMatchedList(TokenList* matched_list);
	uint8_t Compare_N_PARAM_DATE(char* target, ASTA* temp_node);
	uint8_t Compare_N_PARAM_TIME(char* target, ASTA* temp_node);
	uint8_t Compare_N_PARAM_FLOAT(char* target, ASTA* temp_node);
	uint8_t Compare_N_PARAM_INTEGER(char* target, ASTA* temp_node);
	uint8_t Compare_N_PARAM_STRING(char* target, ASTA* temp_node);
	uint8_t Compare_N_LOOKUP(char* target, ASTA* temp_node);
	uint8_t Compare_N_IDENTIFIER(char* target, ASTA* temp_node);
	uint8_t Compare_N_KEYWORD(char* target, ASTA* temp_node);
};

#endif /* NODEMAP_H_ */
