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
#include "processor_out.h"
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
};

class NodeMap {
private:
	uint8_t id_current;
	uint8_t last_matched_id;
	uint8_t str_pos;

	uint8_t help_active;
	uint8_t keyword_match;

public:
	NodeMap();
	virtual ~NodeMap();


	void Reset(void);
	uint16_t GetASTAByID(uint16_t ASTA_ID, ASTA* result);
	uint16_t GetLastMatchedID();
	uint8_t Match(char* target, TokenList* match_list);
	uint16_t Advance(uint8_t in_buf_idx);
	uint8_t GetAction(uint16_t asta_id, char* action_str);


protected:

};

#endif /* NODEMAP_H_ */
