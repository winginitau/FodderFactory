/*****************************************************************
 NodeMap.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#include "NodeMap.h"
#include "TokenList.h"
#include "common_config.h"
#include "parser_errors.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef FF_ARDUINO
char* strlwr(char* s)
{
    char* tmp = s;
    for (;*tmp;++tmp) {
        *tmp = tolower((unsigned char) *tmp);
    }
    return s;
}

char* strupr(char* s)
{
    char* tmp = s;
    for (;*tmp;++tmp) {
        *tmp = toupper((unsigned char) *tmp);
    }
    return s;
}
#endif




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
	mf.error_code = PE_NO_ERROR;

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
	printf("\n\rERROR: ASTA ID Not Found - Array index going out of bounds\n\r");
	// XXX report serious error rather than crash on embedded
	return 0;
}

uint8_t NodeMap::DetermineTarget(uint8_t* target_size, char* target, char* line) {
	*target_size = strlen(line) - line_pos;	// mf.str_pos points to current token in target
	if (*target_size == 0) {
		// We've been advanced and line_pos is now pointing
		// past the space delimiter at the end of the line.
		// Don't process
		#ifdef DEBUG
		printf("DEBUG NodeMap: DELIM_SKIP\n\r");
		#endif

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
	//if (mf.keyword_match) {
	//	return 0;
	//}
	// date format is 2018-02-20
	// sufficient for filtering to just check if it starts with a digit
	if (isdigit(target[0])) {
		sprintf(temp_node->label, "<param-date>");
		return 1;
	}
	return 0;
}

uint8_t NodeMap::Compare_N_PARAM_TIME(char* target, ASTA* temp_node) {
	//if (mf.keyword_match) {
	//	return 0;
	//}
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
	//if (mf.keyword_match) {
	//	return 0;
	//}
	// sufficient for filtering to just check if it starts with a digit
	if (isdigit(target[0])) {
		sprintf(temp_node->label, "<param-float>");
		return 1;
	}
	return 0;
}

uint8_t NodeMap::Compare_N_PARAM_INTEGER(char* target, ASTA* temp_node) {
	//if (mf.keyword_match) {
	//	return 0;
	//}
	// sufficient for filtering to just check if it starts with a digit
	if (isdigit(target[0])) {
		sprintf(temp_node->label, "<param-integer>");
		return 1;
	}
	return 0;
}

uint8_t NodeMap::Compare_N_PARAM_STRING(char* target, ASTA* temp_node) {

	if (isdigit(target[0])) {
		// can't be keyword or ident or lookup
		// XXX any string, unless in "" would also likely be subject to
		// identifier regex..... eg filename.....
		return 0;
	}
	// input is a string by definition
	// we're looking for a string, so add it otherwise
	sprintf(temp_node->label, "<param-string>");
	return 1;
}

uint8_t NodeMap::Compare_N_LOOKUP(char* target, ASTA* temp_node) {

	if (isdigit(target[0])) {
		// can't be keyword or ident or lookup
		return 0;
	}

	// input is a string by definition
	// if we're looking for a lookup value, then we're looking for a string
	return 1;
}

uint8_t NodeMap::Compare_N_IDENTIFIER(char* target, ASTA* temp_node) {
	uint16_t xlat;
	uint8_t member_id;
	//char target_upr[MAX_LABEL_LENGTH];

	if (isdigit(target[0])) {
		// can't be keyword or ident or lookup
		return 0;
	}

	xlat = LookupIdentMap(temp_node->label);
	member_id = LookupIdentifierMembers(xlat, target);

#ifdef DEBUG
	printf("\n\rDEBUG LOOKUP IDENTIFIER: Target: %s  ASTALabel: %s  xlatVal: %d  block_cat_id: %d\n\n\r", target, temp_node->label, xlat, member_id);
#endif

	if (member_id > 0) {
		return member_id;
	} else {
		return 0;
	}
	return 1;
}

uint8_t NodeMap::Compare_N_KEYWORD(char* target, ASTA* temp_node) {
	char target_upr[MAX_LABEL_LENGTH];
	char label_upr[MAX_LABEL_LENGTH];

	// even as partial unique, it overrides all other types
	// check if its matching the input string
	if (isdigit(target[0])) {
		// can't be keyword or ident or lookup
		return 0;
	}
	strcpy(target_upr, target);
	strcpy(label_upr, temp_node->label);
	strupr(target_upr);
	strupr(label_upr);

	char* sub_string_start;
	sub_string_start = strstr(label_upr, target_upr);
	if (sub_string_start == label_upr) {
		mf.keyword_match = 1;
		return 1;
	}
	return 0;
}

uint8_t NodeMap::Match(char* line, TokenList* matched_list) {
	char target[MAX_BUFFER_LENGTH]; 	// for the last bit that we are interested in
	uint8_t target_size;					//

	// clear the previous match list
	matched_list->Reset();

	// if a new line advance to the first node
	if (id_current == 0) { id_current = 1; }

	id_walker = id_current;


	// Size up the target
	mf.match_result = DetermineTarget(&target_size, target, line);
	// If size == 0 assume advanced past a delim, return
	if (mf.match_result == MR_DELIM_SKIP) { return MR_DELIM_SKIP; }

	// save the target to be turned into an action call parameter later (once delim reached)
	strcpy(last_target, target);

	// by default help is off
	mf.help_active = 0;
	// except
	if (target[(target_size-1)] == '?') {
		// add sibling nodes to list based match so far
		// - if advanced past delimiter using space " " then it will be
		//	 siblings of the first child of the previously matched node
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

		if ((cr > 0) || (mf.help_active == 1)) {
			// the node is a possible match to be included
			matched_list->AddASTAToTokenList(temp_node);
			#ifdef DEBUG
			printf("DEBUG Selecting Matching: ID:%d  Label:%s  Action:%s \n\r", temp_node.id, temp_node.label, temp_node.action_identifier);
			#endif
		}

		//set up for the siblings (if any)
		id_walker = temp_node.next_sibling;
	} while (id_walker != 0); // reached end of sibling list
}

uint8_t NodeMap::EvaluateMatchedList(TokenList* matched_list) {
	ASTA temp_node;

		// filter results based on matching precedence
		// If any of these are present together in the matched
		// list (of entered data, against the available nodes)
		// then the order of precedence is:
		// 	Keyword is partially or fully matched
		// 	Member of a selected identifier list - fully matched
		//  A look up value in full
		//	A time or date param - distinguisable by its format
		//	string, int, float params
	if (mf.help_active == 1) {
		// do nothing, don't filter - keep everything for help display
		return MR_HELP_ACTIVE;
	} else {
		if (matched_list->IsEmpty()) {
			// Dont need to do any more
			return MR_NO_MATCH;
		}
		if ((matched_list->GetSize()) == 1) {
			// single entry - no filtering required
			//  its either uniquely matched and can continue to next token
			//	or its unique with an action attached (if EOL is next)
			GetASTAByID(matched_list->GetCurrentID(), &temp_node);
			mf.last_matched_id = temp_node.id; // to advance from when there's a unique match
			if (temp_node.action) {
				return MR_ACTION_POSSIBLE;
			} else {
				return MR_UNIQUE;
			}
		}
		// multi entries - either some precedent matching  to bring it
		// down to 1 or a MR_MULTI returned indicating that it can't be uniquely
		// matched without further input
		// XXX unless its a potential Look-Ahaed case......

		// Call MatchReduce and deal with the result - which is the number of
		// matching nodes left after the reduce
		switch (MatchReduce(matched_list)) {
			case 0: return MR_ERROR; break;
			case 1:
				GetASTAByID(matched_list->GetCurrentID(), &temp_node);
				mf.last_matched_id = temp_node.id; // to advance from when there's a unique match
				if (temp_node.action) {
					return MR_ACTION_POSSIBLE;
				} else {
					return MR_UNIQUE;
				}
				break;
			default: return MR_MULTI; break;
		}
	}
}

uint16_t NodeMap::MatchReduce(TokenList* list) {
	// filter results based on matching precedence
	// If any of these are present together in the matched
	// list (of entered data, against the available nodes)
	// then the order of precedence is:
	// 	Keyword is partially or fully matched
	// 	Member of a selected identifier list - fully matched
	//  A look up value in full
	//	A time or date param - distinguisable by its format
	//	A string, int, float params

	// algorithm:
	// check keywords
	// if 2 or more keywords, return multi
	// if single keyword - drop rest, return unique
	// else (0 keywords,  check identifier
	// if 2 or more identifier, return multi
	// if 1 identifier drop rest (would have been looked up in matching process), return uinique
	// if 0 identifier check lookup
	// if 2 or more lookup, return multi
	// if 1 lookup and no others - would have been caught as unique and dealt with in MatchEvaluate
	//		return error
	// if 0 lookup, check rest
	// XXX if more than 1 at this point, then it can't be parsed
	// because it requires a LOOK-RIGHT, on a user parameter to branch
	// XXX how to trigger back an error from here elegantly?
	// else check for this condition during lexer

	uint16_t count;
	uint8_t type;

	for (type = AST_KEYWORD; type <= AST_IDENTIFIER; type++) {
		count = list->CountByType(type);
		if (count > 1) {
			return list->GetSize();
		} else {
			if (count == 1) {
				for (int i = (type + 1); i < LAST_AST_TYPE; i++) {
					list->DeleteByType(i);
				}
				count = list->GetSize();
				if (count == 1) {
					return 1;
				} else {
					// reduce did not achieve a unique match

					#ifdef DEBUG
					printf("DEBUG Fatal in MatchReduce: size != 1 after iterative delete to unique node\n\r");
					#endif

					mf.error_code = PE_MATCHREDUCE_EXPECTED_1;

					return 0;
				}
			}
		}
	}

	type = AST_LOOKUP;
	count = list->CountByType(type);
	if (count > 1) {
		return list->GetSize();
	} else {
		if (count == 1) {
			if (list->GetSize() == 1) {

				#ifdef DEBUG
				printf("XXX Fatal Error in MatchReduce: unique lookup node passed to reducer\n\r");
				#endif

				mf.error_code = PE_LOOKUP_PASSED_TO_REDUCER;

				return 0;
			}
		}
	}

	#ifdef DEBUG
	printf("XXX Serious Error - Next parse step dependent on multiple user-param nodes which can't be distinguished\n\r");
	#endif

	mf.error_code = PE_MULTIPLE_PARAM_LOOKAHEAD;
	// TODO This should really best detected in the lexer....

	return 0;
}

char* NodeMap::GetLastTargetString(char* return_string) {
	return strcpy(return_string, last_target);
}

uint16_t NodeMap::Advance(uint8_t in_buf_idx) {
	// Last_matched_id is the uniquely matched node
	//
	// Move to first child so that subsequent matching
	// will occur against the child and its siblings

	ASTA temp;
	GetASTAByID(mf.last_matched_id, &temp);

	// set the current_id for the next matching iteration to
	// the first child of the last matched.
	id_current = temp.first_child;

	// advance the start pointer (line_pos) in the match
	// buffer so that the next match starts on the next token
	line_pos = in_buf_idx;

	#ifdef DEBUG
	printf("DEBUG NodeMap::Advance\n\r");
	#endif

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

uint16_t NodeMap::GetErrorCode() {
	return mf.error_code;
}

/*
int NodeMap::BuildFunctionParamList(Identifiers& idents) {
	// While on the AST node that is action-able - "current", walk via
	// parent pointers up the tree, building the function parameter
	// list that will be included in the function prototype for this action.

	KeyValuePairList params;
	ASTNode* walker;
	int param_count = 0;
	int param_index = 0;
	char param_name[MAX_BUFFER_WORD_LENGTH];
	char param_type[MAX_BUFFER_WORD_LENGTH];
	char func_name[MAX_BUFFER_WORD_LENGTH];
	char temp[MAX_BUFFER_WORD_LENGTH];

	walker = current;
	while (walker != NULL ) {
		// count the num of params that need to be included in the function
		if (walker->type > AST_KEYWORD) {
			param_count++;
		}

		// count the ones that needs to be numerically labelled
		if (walker->type >= AST_LOOKUP) {
			param_index++;
		}
		walker = walker->parent;
	}

	walker = current;
	while (walker != NULL) {
		switch (walker->type) {
			case AST_UNDEFINED:
				return E_AST_NODE_UNDEFINED_TYPE;
				break;
			case AST_KEYWORD:
				// ignore - it got us to here anyway
				break;
			case AST_IDENTIFIER:
				if (idents.Exists(walker->label)) {
					sprintf(param_type, "int");
					params.Add(walker->label, param_type);
				} else
					return E_BUILDING_ACTION_PROTO;
				break;
			case AST_LOOKUP:
				if (idents.Exists(walker->label)) {
					sprintf(param_type, "char*");
					params.Add(walker->label, param_type);
				} else
					return E_BUILDING_ACTION_PROTO;
				break;
			case AST_PARAM_STRING:
				sprintf(param_type, "char*");
				sprintf(param_name, "param%u_string", param_index);
				params.Add(param_name, param_type);
				param_index--;
				break;
			case AST_PARAM_INTEGER:
				sprintf(param_type, "int");
				sprintf(param_name, "param%u_int", param_index);
				params.Add(param_name, param_type);
				param_index--;
				break;
			case AST_PARAM_FLOAT:
				sprintf(param_type, "float");
				sprintf(param_name, "param%u_float", param_index);
				params.Add(param_name, param_type);
				param_index--;
				break;
			case AST_PARAM_TIME:
				sprintf(param_type, "char*");
				sprintf(param_name, "param%u_time", param_index);
				params.Add(param_name, param_type);
				param_index--;
				break;
			case AST_PARAM_DATE:
				sprintf(param_type, "char*");
				sprintf(param_name, "param%u_date", param_index);
				params.Add(param_name, param_type);
				param_index--;
				break;
		}
		walker = walker->parent;
	}

	// write the function
	strcpy(output_string, "char** ");
	if (idents.GetInstanceName(current->action_identifier, func_name) != E_NO_ERROR) {
		return E_BUILDING_ACTION_PROTO;
	}

	sprintf(temp, "%s(", func_name);
	strcat(output_string, temp);

	if (param_count == 0) {
		strcat(output_string, "void);\n\n");
		output.EnQueue(output_string);
	} else {
		for (int i = param_count - 1; i > 0; i--) {
			params.GetPairAtLocation(param_name, param_type, i);
			sprintf(temp, "%s %s, ", param_type, param_name);
			strcat(output_string, temp);
		}
		params.GetPairAtLocation(param_name, param_type, 0);
		sprintf(temp, "%s %s);\n\n", param_type, param_name);
		strcat(output_string, temp);
		output.EnQueue(output_string);
	}
	return E_NO_ERROR;
}
*/


