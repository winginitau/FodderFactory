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
	str_pos = 0;
	id_current = 0;
	last_matched_id = 0;
	help_active = 0;
	keyword_match = 0;
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

uint8_t NodeMap::Match(char* target, TokenList* match_list) {
	// holders
	ASTA temp_node; 	// holder for the node of interest
	char match_buffer[MAX_BUFFER_LENGTH]; 	// for the last bit that we are interested in

	if (id_current == 0) {  // starting to match a new line
		//advance to the first node
		id_current = 1;
	}
	uint16_t id_walker = id_current;	// to walk through the asta array nodes by id

	uint8_t target_size = strlen(target) - str_pos;		// str_pos points to current token in target
	if (target_size == 0) {
		// We've been advanced and the str_pos is now pointing
		// past the space delimiter at the end of the target.
		// Don't process
		return MR_DELIM_SKIP;
	}

	// target is \0 terminated. Copy the bit we're interested in to match_buffer
	int i = 0;
	match_buffer[i] = target[str_pos+i];
	while (match_buffer[i] != '\0') {
		i++;
		match_buffer[i] = target[str_pos+i];
	}

	// by default help is off
	help_active = 0;
	// except
	if (match_buffer[(target_size-1)] == '?') {
		// add all available sibling nodes to list for help display
		help_active = 1;
	}
	// reset keyword exclusive if set previously
	keyword_match = 0 ;


	do {
		// Iteratively short list possible sibling node matches
		// by comparing the top node at this level and each of its siblings
		// to what we have in the match_buffer

		GetASTAByID(id_walker, &temp_node);

		// type will determine how we compare the node
		switch (temp_node.type) {
			case AST_PARAM_DATE:
				if (keyword_match) break;
				// date format is 2018-02-20
				// sufficient for filtering to just check if it starts with a digit
				if (isdigit(match_buffer[0])) {
					sprintf(temp_node.label, "<param-date>");
					match_list->AddASTAToTokenList(temp_node);
					last_matched_id = temp_node.id; // to advance from when there's a unique match
				}
				break;
			case AST_PARAM_TIME:
				if (keyword_match) break;
				// time format is 23:30:45 (hh:mm:ss)
				if (isdigit(match_buffer[0])) {
					int x = match_buffer[0] - '0';
					if (x < 3) {
						// could be
						sprintf(temp_node.label, "<param-time>");
						match_list->AddASTAToTokenList(temp_node);
						last_matched_id = temp_node.id; // to advance from when there's a unique match
					}
				}
				break;
			case AST_PARAM_FLOAT:
				if (keyword_match) break;
				// sufficient for filtering to just check if it starts with a digit
				if (isdigit(match_buffer[0])) {
					sprintf(temp_node.label, "<param-float>");
					match_list->AddASTAToTokenList(temp_node);
					last_matched_id = temp_node.id; // to advance from when there's a unique match
				}
				break;
			case AST_PARAM_INTEGER:
				if (keyword_match) break;
				// sufficient for filtering to just check if it starts with a digit
				if (isdigit(match_buffer[0])) {
					sprintf(temp_node.label, "<param-integer>");
					match_list->AddASTAToTokenList(temp_node);
					last_matched_id = temp_node.id; // to advance from when there's a unique match
				}
				break;
			case AST_PARAM_STRING:
				if (keyword_match) break;
				// input is a string by definition
				// we're looking for a string, so add it regardless
				sprintf(temp_node.label, "<param-string>");
				match_list->AddASTAToTokenList(temp_node);
				last_matched_id = temp_node.id; // to advance from when there's a unique match
				break;
			case AST_LOOKUP:
				if (keyword_match) break;
				// input is a string by definition
				// if we're looking for a lookup value, then we're looking for a string
				match_list->AddASTAToTokenList(temp_node);
				last_matched_id = temp_node.id; // to advance from when there's a unique match
				break;
			case AST_IDENTIFIER:
				if (keyword_match) break;
				// if we're looking for an ident then this could be it
				// XXX need identifier lookup mapping code to be produced by processor
				// before proceeding with this bit.
				match_list->AddASTAToTokenList(temp_node);
				last_matched_id = temp_node.id; // to advance from when there's a unique match
				break;
			case AST_KEYWORD:
				// if help active include it regardless of match
				// and without dominating the other nodes
				if (help_active) {
					match_list->AddASTAToTokenList(temp_node);
				} else {
					// even as partial unique, it overrides all other types
					// check if its matching the input string
					char* sub_string_start;
					sub_string_start = strstr(temp_node.label, match_buffer);
					if(sub_string_start == temp_node.label) {
						// bump out the other less dominant and stop further matching
						match_list->Reset();
						match_list->AddASTAToTokenList(temp_node);
						keyword_match = 1;
						last_matched_id = temp_node.id; // to advance from when there's a unique match
					}
				}
				break;
		};
		//set up for the siblings (if any)
		id_walker = temp_node.next_sibling;
	} while (id_walker != 0); // reached end of sibling list


	if (match_list->IsEmpty()) {
		return MR_NO_MATCH;
	} else {
		if ((match_list->GetSize()) == 1) {
			// single unique match
			// is it an action block?
			GetASTAByID(match_list->GetCurrentID(), &temp_node);
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

	GetASTAByID(last_matched_id, &temp);

	// set the current_id for the next matching iteration to
	// the first child of the last matched.

	id_current = temp.first_child;
	str_pos = in_buf_idx;

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
	return last_matched_id;
}
