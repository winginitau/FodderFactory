/*****************************************************************
 NodeMap.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#include "NodeMap.h"
#include "TokenList.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

NodeMap::NodeMap() {
	Reset();
}

NodeMap::~NodeMap() {
	// Auto-generated destructor stub
}

void NodeMap::Reset(void) {
	mf.last_matched_id = 0;
	mf.help_active = 0;
	mf.keyword_match = 0;

	line_pos = 0;
	id_current = 0;

}

uint16_t NodeMap::GetASTAByID(uint16_t ASTA_ID, ASTA* result) {
	uint16_t arr_idx = 0;	// index into the asta array

	// set up to walk the asta array
	size_t node_count = sizeof(asta)/sizeof(asta[0]);

	// First, find the id_walker node and copy to temp_node (can't guarantee the array will be in id order)
	while (arr_idx < node_count) {
		if (asta[arr_idx].id == ASTA_ID) {
			result->action = asta[arr_idx].action;
			strcpy(result->action_identifier, asta[arr_idx].action_identifier);
			result->first_child = asta[arr_idx].first_child;
			result->id = asta[arr_idx].id;
			strcpy(result->label, asta[arr_idx].label);
			result->next_sibling = asta[arr_idx].next_sibling;
			result->parent = asta[arr_idx].parent;
			result->type = asta[arr_idx].type;
			return 1;
		}
		arr_idx++;
	}
	printf("ASTA ID Not Found - Array index going out of bounds - Serious error\n");
	// XXX report serious error rather than crash on embedded
	return 0;
}

uint8_t NodeMap::DetermineTarget(uint8_t* target_size, char* target, char* line) {
	*target_size = strlen(line) - line_pos;	// mf.str_pos points to current token in target
	if (*target_size == 0) {
		// We've been advanced and line_pos is now pointing
		// past the space delimiter at the end of the line.
		// Don't process
		printf("DEBUG NodeMap: DELIM_SKIP\n");
		return MR_DELIM_SKIP;
	}

	// line is \0 terminated. Copy the bit we're interested in to the target
	int i = 0;
	target[i] = line[line_pos+i];
	while (target[i] != '\0') {
		i++;
		target[i] = line[line_pos+i];
	}
	return MR_CONTINUE;
}

uint8_t NodeMap::Compare_N_PARAM_DATE(char* target, ASTA* temp_node) {
	if (mf.keyword_match) {
		return 0;
	}
	// date format is 2018-02-20
	// sufficient for filtering to just check if it starts with a digit
	if (isdigit(target[0])) {
		sprintf(temp_node->label, "<param-date>");
		return 1;
	}
	return 0;
}

uint8_t NodeMap::Compare_N_PARAM_TIME(char* target, ASTA* temp_node) {
	if (mf.keyword_match) {
		return 0;
	}
	// time format is 23:30:45 (hh:mm:ss)
	if (isdigit(target[0])) {
		int x = target[0] - '0';
		if (x < 3) {
			// could be
			sprintf(temp_node->label, "<param-time>");
			return 1;
		}
	}
	return 0;
}

uint8_t NodeMap::Compare_N_PARAM_FLOAT(char* target, ASTA* temp_node) {
	if (mf.keyword_match) {
		return 0;
	}
	// sufficient for filtering to just check if it starts with a digit
	if (isdigit(target[0])) {
		sprintf(temp_node->label, "<param-float>");
		return 1;
	}
	return 0;
}

uint8_t NodeMap::Compare_N_PARAM_INTEGER(char* target, ASTA* temp_node) {
	if (mf.keyword_match) {
		return 0;
	}
	// sufficient for filtering to just check if it starts with a digit
	if (isdigit(target[0])) {
		sprintf(temp_node->label, "<param-integer>");
		return 1;
	}
	return 0;
}

uint8_t NodeMap::Compare_N_PARAM_STRING(char* target, ASTA* temp_node) {
	if (mf.keyword_match) {
		return 0;
	}
	// input is a string by definition
	// we're looking for a string, so add it regardless
	sprintf(temp_node->label, "<param-string>");
	return 1;
}

uint8_t NodeMap::Compare_N_LOOKUP(char* target, ASTA* temp_node) {
	if (mf.keyword_match) {
		return 0;
	}
	// input is a string by definition
	// if we're looking for a lookup value, then we're looking for a string
	return 1;
}

uint8_t NodeMap::Compare_N_IDENTIFIER(char* target, ASTA* temp_node) {
	if (mf.keyword_match) {
		return 0;
	}
	// if we're looking for an ident then this could be it
	// XXX need identifier lookup mapping code to be produced by processor
	// before proceeding with this bit.
	return 1;
}

uint8_t NodeMap::Compare_N_KEYWORD(char* target, ASTA* temp_node) {
	// if help actived include it regardless of match
	// and without dominating the other nodes
	if (mf.help_active) {
		return 1;
	} else {
		// even as partial unique, it overrides all other types
		// check if its matching the input string
		char* sub_string_start;
		sub_string_start = strstr(temp_node->label, target);
		if (sub_string_start == temp_node->label) {
			mf.keyword_match = 1;
			return 1;
		}
	}
	return 0;
}

uint8_t NodeMap::Match(char* line, TokenList* matched_list) {
	char target[MAX_BUFFER_LENGTH]; 	// for the last bit that we are interested in
	uint8_t target_size;					//

	// if a new line advance to the first node
	if (id_current == 0) { id_current = 1; }

	id_walker = id_current;

	mf.match_result = DetermineTarget(&target_size, target, line);
	if (mf.match_result == MR_DELIM_SKIP) { return MR_DELIM_SKIP; }

	// by default help is off
	mf.help_active = 0;
	// except
	if (target[(target_size-1)] == '?') {
		// add all available sibling nodes to list for help display
		mf.help_active = 1;
	}
	// reset keyword match if set previously
	mf.keyword_match = 0 ;

	SelectMatchingNodes(target, matched_list);
	return EvaluateMatchedList(matched_list);
}

void NodeMap::SelectMatchingNodes(char* target, TokenList* matched_list) {
	ASTA temp_node;
	uint8_t cr;		// compare result

	do {
		// Iteratively short list possible sibling node matches
		// by comparing the top node at this level and each of its siblings
		// to what we have in the target

		GetASTAByID(id_walker, &temp_node);

		// type will determine how we compare the node
		switch (temp_node.type) {
			case AST_PARAM_DATE:	cr = Compare_N_PARAM_DATE(target, &temp_node);		break;
			case AST_PARAM_TIME:	cr = Compare_N_PARAM_TIME(target, &temp_node);		break;
			case AST_PARAM_FLOAT: 	cr = Compare_N_PARAM_FLOAT(target, &temp_node);		break;
			case AST_PARAM_INTEGER:	cr = Compare_N_PARAM_INTEGER(target, &temp_node);	break;
			case AST_PARAM_STRING:	cr = Compare_N_PARAM_STRING(target, &temp_node);	break;
			case AST_LOOKUP:		cr = Compare_N_LOOKUP(target, &temp_node);			break;
			case AST_IDENTIFIER:	cr = Compare_N_IDENTIFIER(target, &temp_node);		break;
			case AST_KEYWORD:		cr = Compare_N_KEYWORD(target, &temp_node);			break;
		};

		if (cr == 1) {	// the node is a possible match
			if (mf.keyword_match ==1) {
				// bump out the other less dominant and stop the rest from matching
				matched_list->Reset();
			}
			matched_list->AddASTAToTokenList(temp_node);
			mf.last_matched_id = temp_node.id; // to advance from when there's a unique match
		}

		//set up for the siblings (if any)
		id_walker = temp_node.next_sibling;
	} while (id_walker != 0); // reached end of sibling list
}

uint8_t NodeMap::EvaluateMatchedList(TokenList* matched_list) {
	ASTA temp_node;

	if (matched_list->IsEmpty()) {
		return MR_NO_MATCH;
	} else {
		if ((matched_list->GetSize()) == 1) {
			// single unique match
			// is it an action block?
			GetASTAByID(matched_list->GetCurrentID(), &temp_node);
			if (temp_node.action) {
				return MR_ACTION_POSSIBLE;
			} else {
				return MR_UNIQUE_KEYWORD;
			}
		} else {
			return MR_MULTI;
		}
	}
}



uint16_t NodeMap::Advance(uint8_t in_buf_idx) {
	// last_matched_id is the uniquely matched node
	// move to first child so that subsequent mactching
	// will occur against the child and its siblings

	ASTA temp;

	GetASTAByID(mf.last_matched_id, &temp);

	// set the current_id for the next matching iteration to
	// the first child of the last matched.

	id_current = temp.first_child;
	line_pos = in_buf_idx;

	printf("DEBUG NodeMap::Advance\n");
	return id_current;
}

uint8_t NodeMap::GetAction(uint16_t asta_id, char* action_str) {
	ASTA temp_node;
	uint8_t result;
	result = GetASTAByID(asta_id, &temp_node);
	if (result) {
		strcpy(action_str, temp_node.action_identifier);
	}
	return result;
}

uint16_t NodeMap::GetLastMatchedID() {
	return mf.last_matched_id;
}
