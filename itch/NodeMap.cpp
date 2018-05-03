/*****************************************************************
 NodeMap.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#include "config.h"
#include "NodeMap.h"
#include "TokenList.h"
#include "itch_strings.h"
#include "itch_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/********************************************************************
 * Globals
 ********************************************************************/

#ifdef ITCH_DEBUG
extern void M(char strn[]);
char n_debug_message[MAX_OUTPUT_LINE_SIZE];
#endif

M_FLAGS g_mflags;
uint8_t g_map_line_pos;
uint16_t g_map_id_current;		// currently matched node
uint16_t g_map_id_walker;		// to walk through the asta array nodes by id

char g_map_last_target_str[MAX_IDENTIFIER_LABEL_SIZE];

extern ASTA g_temp_asta;

/********************************************************************
 * Functions
 ********************************************************************/

char* strlwr(char* s) {
	// string go lower case
    char* tmp = s;
    for (;*tmp;++tmp) {
        *tmp = tolower((unsigned char) *tmp);
    }
    return s;
}

char* strupr(char* s) {
	// string to upper case
    char* tmp = s;
    for (;*tmp;++tmp) {
        *tmp = toupper((unsigned char) *tmp);
    }
    return s;
}

void MapReset(void) {
	g_mflags.last_matched_id = 0;
	g_mflags.help_active = 0;
	g_mflags.keyword_match = 0;
	g_mflags.error_code = PEME_NO_ERROR;

	g_map_line_pos = 0;
	g_map_id_current = 0;

}

uint16_t MapGetASTAByID(uint16_t asta_id, ASTA* result) {
	uint16_t asta_idx = 0;	// index into the asta array

	// set up to walk the asta array
	//size_t node_count = sizeof(asta)/sizeof(asta[0]);

	uint16_t test;

	// First, find the id_walker node and copy to temp_node (can't guarantee the array will be in id order)
	while (asta_idx < AST_NODE_COUNT) {
		#ifdef ARDUINO
			memcpy_P(&test, &(asta[asta_idx].id), sizeof(test));
		#else
			test = asta[asta_idx].id;
		#endif
		if (test == asta_id) {
			#ifdef ARDUINO
				memcpy_P(result, &(asta[asta_idx]), sizeof(ASTA));
			#else
				memcpy(result, &(asta[asta_idx]), sizeof(ASTA));

			#endif
			return PEME_NO_ERROR;
		}
		asta_idx++;
	}
	return ME_GETASTABYID_FAIL;
}

uint8_t MapDetermineTarget(uint8_t* target_size, char* target, char* line) {
	*target_size = strlen(line) - g_map_line_pos;	// mf.str_pos points to current token in target
	if (*target_size == 0) {
		// We've been advanced and line_pos is now pointing
		// past the space delimiter at the end of the line.
		// Don't process
		#ifdef ITCH_DEBUG
			strcpy_debug(n_debug_message, ITCH_DEBUG_MAPDETERMINETARGET_DELIM_SKIP);
			//sprintf(n_debug_message, "DEBUG (MapDetermineTarget): DELIM_SKIP\n\r");
			M(n_debug_message);
		#endif

		return MR_DELIM_SKIP;
	}

	// line is \0 terminated. Copy the bit we're interested in to the target
	int i = 0;
	target[i] = line[g_map_line_pos+i];
	while (target[i] != '\0') {
		i++;
		target[i] = line[g_map_line_pos+i];
	}
	return MR_CONTINUE;
}

uint8_t Compare_N_PARAM_DATE(char* target, ASTA* temp_node) {
	//if (mf.keyword_match) {
	//	return 0;
	//}
	// date format is 2018-02-20
	// sufficient for filtering to just check if it starts with a digit
	if (isdigit(target[0])) {
		strcpy_misc(temp_node->label, MISC_PARAM_DATE);
		return 1;
	}
	return 0;
}

uint8_t Compare_N_PARAM_TIME(char* target, ASTA* temp_node) {
	//if (mf.keyword_match) {
	//	return 0;
	//}
	// time format is 23:30:45 (hh:mm:ss)
	if (isdigit(target[0])) {
		int x = target[0] - '0';
		if (x < 3) {
			// could be
			strcpy_misc(temp_node->label, MISC_PARAM_TIME);
			return 1;
		}
	}
	return 0;
}

uint8_t Compare_N_PARAM_FLOAT(char* target, ASTA* temp_node) {
	//if (mf.keyword_match) {
	//	return 0;
	//}
	// sufficient for filtering to just check if it starts with a digit
	if (isdigit(target[0])) {
		strcpy_misc(temp_node->label, MISC_PARAM_FLOAT);
		return 1;
	}
	return 0;
}

uint8_t Compare_N_PARAM_INTEGER(char* target, ASTA* temp_node) {
	//if (mf.keyword_match) {
	//	return 0;
	//}
	// sufficient for filtering to just check if it starts with a digit
	if (isdigit(target[0])) {
		strcpy_misc(temp_node->label, MISC_PARAM_INTEGER);
		return 1;
	}
	return 0;
}

uint8_t Compare_N_PARAM_STRING(char* target, ASTA* temp_node) {

	if (isdigit(target[0])) {
		// can't be keyword or ident or lookup
		// XXX any string, unless in "" would also likely be subject to
		// identifier regex..... eg filename.....
		return 0;
	}
	// input is a string by definition
	// we're looking for a string, so add it otherwise
	strcpy_misc(temp_node->label, MISC_PARAM_STRING);
	return 1;
}

uint8_t Compare_N_LOOKUP(char* target, ASTA* temp_node) {
	// TODO temp note will throw warning unused
	// When lookup fully implemented it will be used
	if (isdigit(target[0])) {
		// can't be keyword or ident or lookup
		return 0;
	}

	// XXX although a lookup is a string it is actually
	// a special case of a reserved word (ie identifier)
	// So lookup needs to be called here or the idea of
	// lookups needs to be expunged from the whole code base.

	// Currently - lookups not matched at all
	// Cant just treat as a string or matchreduce will error
	// because it can't distinguish a string from a lookup
	// in the case where an AST node has valid branching to
	// both a param-string node and a lookup node.
	return 0;
}

uint8_t Compare_N_IDENTIFIER(char* target, ASTA* temp_node) {
	uint16_t xlat;
	uint8_t member_id;
	//char target_upr[MAX_LABEL_LENGTH];

	if (isdigit(target[0])) {
		// can't be keyword or ident or lookup
		return 0;
	}

	xlat = LookupIdentMap(temp_node->label);
	member_id = LookupIdentifierMembers(xlat, target);

#ifdef ITCH_DEBUG
	//sprintf(n_debug_message, "DEBUG (Compare_N_IDENTIFIER): Target: %s  ASTALabel: %s  xlatVal: %d  block_cat_id: %d\n\n\r", target, temp_node->label, xlat, member_id);
	//M(n_debug_message);
#endif

	if (member_id > 0) {
		return member_id;
	} else {
		return 0;
	}
	return 1;
}

uint8_t Compare_N_KEYWORD(char* target, ASTA* temp_node) {
	char target_upr[MAX_IDENTIFIER_LABEL_SIZE];
	char label_upr[MAX_IDENTIFIER_LABEL_SIZE];

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
		g_mflags.keyword_match = 1;
		return 1;
	}
	return 0;
}

uint8_t MapMatch(char* line, TokenList* matched_list) {
	// working from the currently matched node, isolate the last token
	// in the line buffer (complete or partial), match it against
	// possible nodes, evaluate the results and return result
	// and a list of matched nodes to the parser

	char target[MAX_IDENTIFIER_LABEL_SIZE]; 	// for the last bit that we are interested in
	uint8_t target_size;					//

	// clear the previous match list
	TLReset(matched_list);

	// if a new line advance to the first node in the ASTA
	if (g_map_id_current == 0) { g_map_id_current = 1; }

	// set the ASTA walker to the current node
	g_map_id_walker = g_map_id_current;

	// Determine the target part of the line buffer
	// ie the last token on the line - complete or partial and get its size
	g_mflags.match_result = MapDetermineTarget(&target_size, target, line);

	// If size == 0 assume advanced past a delim, return
	// size will only be 0 if the previous pass detected a delim and advanced the buffer
	// therefore code now just checks for the MR_DELIM_SKIP flag
	if (g_mflags.match_result == MR_DELIM_SKIP) { return MR_DELIM_SKIP; }

	// save the target to be turned into an action call parameter later (once delim reached)
	strcpy(g_map_last_target_str, target);

	// by default help is off
	g_mflags.help_active = 0;
	// except if '?' is the last char (ie target/token of interest)
	if (target[(target_size-1)] == '?') {
		// add sibling nodes to list based match so far
		// - if advanced past delimiter using space " " then it will be
		//	 siblings of the first child of the previously matched node
		g_mflags.help_active = 1;
	}
	// reset keyword match if set previously
	g_mflags.keyword_match = 0 ;

	// Scan the current node and its siblings for potential or exact matches
	// Returning the result in matched_list
	MapSelectMatchingNodes(target, matched_list);

	// Evaluate the matched_list and return the result
	return MapEvaluateMatchedList(matched_list);

}

void MapSelectMatchingNodes(char* token, TokenList* matched_list) {
	//ASTA g_temp_asta;
	uint8_t cr = 0;		// compare result

	do {
		// Iteratively short list possible sibling node matches
		// by comparing the top node at this level and each of its siblings
		// to what we have in the token

		// Get the first candidate node
		MapGetASTAByID(g_map_id_walker, &g_temp_asta);

		// Node type will determine how we compare it to the token
		switch (g_temp_asta.type) {
			case AST_PARAM_DATE:	cr = Compare_N_PARAM_DATE(token, &g_temp_asta);		break;
			case AST_PARAM_TIME:	cr = Compare_N_PARAM_TIME(token, &g_temp_asta);		break;
			case AST_PARAM_FLOAT: 	cr = Compare_N_PARAM_FLOAT(token, &g_temp_asta);		break;
			case AST_PARAM_INTEGER:	cr = Compare_N_PARAM_INTEGER(token, &g_temp_asta);	break;
			case AST_PARAM_STRING:	cr = Compare_N_PARAM_STRING(token, &g_temp_asta);	break;
			case AST_LOOKUP:		cr = Compare_N_LOOKUP(token, &g_temp_asta);			break;
			case AST_IDENTIFIER:	cr = Compare_N_IDENTIFIER(token, &g_temp_asta);		break;
			case AST_KEYWORD:		cr = Compare_N_KEYWORD(token, &g_temp_asta);			break;
		};

		if ((cr > 0) || (g_mflags.help_active == 1)) {
			// the node is a possible match to be included
			TLAddASTAToTokenList(matched_list, g_temp_asta);
			#ifdef ITCH_DEBUG
			// XXX
			// XXX This causes an lto_wrapper / ld seg fault at compile time
			// XXX TRIAGE

			//ORIGINAL - FAILS
			//sprintf(n_debug_message, "DEBUG (MapSelectMatchingNodes) Adding Possible Match: ID:%d  Label:%s  which has Action:%s \n\r", g_temp_asta.id, g_temp_asta.label, g_temp_asta.action_identifier);

			// WORKS
			//sprintf(n_debug_message, "ID:%d \n\r", g_temp_asta.id);

			// FAILS
			//sprintf(n_debug_message, "Label:%s\n\r", g_temp_asta.label);

			// WORKS
			//strcpy(n_debug_message, g_temp_asta.label);

			// WORKS
			//sprintf(g_temp_asta.label, "Triage Label\n");
			//sprintf(n_debug_message, "Label:%s\n\r", g_temp_asta.label);

			// WORKS
			//strcpy(n_debug_message, g_temp_asta.label);
			//strcpy(n_debug_message, g_temp_asta.action_identifier);
			//sprintf(n_debug_message, "DEBUG (MapSelectMatchingNodes) Adding Possible Match: ID:%d  Label:%s  which has Action:%s \n\r", g_temp_asta.id, g_temp_asta.label, g_temp_asta.action_identifier);

			//NEW
			strcpy(n_debug_message, g_temp_asta.label);
			strcpy(n_debug_message, g_temp_asta.action_identifier);
			strcpy_debug(g_temp_str, ITCH_DEBUG_MAPSELECTMATCHINGNODES_ID_LABEL_ACTION);
			sprintf(n_debug_message, "%s%d/%s/%s\n", g_temp_str, g_temp_asta.id, g_temp_asta.label, g_temp_asta.action_identifier);

			M(n_debug_message);
			#endif
		}

		//set up for the siblings (if any)
		g_map_id_walker = g_temp_asta.next_sibling;
	} while (g_map_id_walker != 0); // reached end of sibling list
}

uint8_t MapEvaluateMatchedList(TokenList* matched_list) {
	//ASTA temp_node;

		// filter results based on matching precedence
		// If any of these are present together in the matched
		// list (of entered data, against the available nodes)
		// then the order of precedence is:
		// 	Keyword is partially or fully matched
		// 	Member of a selected identifier list - fully matched
		//  A look up value in full
		//	A time or date param - distinguisable by its format
		//	string, int, float params
	if (g_mflags.help_active == 1) {
		// do nothing, don't filter - keep everything for help display
		return MR_HELP_ACTIVE;
	} else {
		if (TLIsEmpty(matched_list)) {
			// Dont need to do any more
			return MR_NO_MATCH;
		}
		if ((TLGetSize(matched_list)) == 1) {
			// single entry - no filtering required
			//  its either uniquely matched and can continue to next token
			//	or its unique with an action attached (if EOL is next)
			MapGetASTAByID(TLGetCurrentID(matched_list), &g_temp_asta);
			g_mflags.last_matched_id = g_temp_asta.id; // to advance from when there's a unique match
			if (g_temp_asta.action) {
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
		switch (MapMatchReduce(matched_list)) {
			case 0: return MR_ERROR; break;
			case 1:
				MapGetASTAByID(TLGetCurrentID(matched_list), &g_temp_asta);
				g_mflags.last_matched_id = g_temp_asta.id; // to advance from when there's a unique match
				if (g_temp_asta.action) {
					return MR_ACTION_POSSIBLE;
				} else {
					return MR_UNIQUE;
				}
				break;
			default: return MR_MULTI; break;
		}
	}
}

uint16_t MapMatchReduce(TokenList* list) {
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
	//		return error XXX (when implemented)
	// if 0 lookup, check rest
	// XXX if more than 1 at this point, then it can't be parsed
	// because it requires a LOOK-RIGHT, on a user parameter to branch
	// TODO how to trigger back an error from here more elegantly?
	// else check for this condition during lexer

	uint16_t count;
	uint8_t type;

	// Work through the types in order of precedence - one by one
	// Starting with the two that have values known to the parser
	for (type = AST_KEYWORD; type <= AST_IDENTIFIER; type++) {
		count = TLCountByType(list, type);
		if (count > 1) {
			// return > 1 - still need more chars to determine uniqueness
			return TLGetSize(list);
		} else {
			// count is 0 or 1
			if (count == 1) {
				// count of this type unique
				// drop the rest of list
				for (int i = (type + 1); i < LAST_AST_TYPE; i++) {
					TLDeleteByType(list, i);
				}
				// double check - should now definitely be 1
				// TODO: redundant?
				count = TLGetSize(list);
				if (count == 1) {
					return 1;
				} else {
					// error - count  expected to be 1 but did not achieve a unique match
					#ifdef ITCH_DEBUG
						strcpy_debug(n_debug_message, ITCH_DEBUG_MATCH_REDUCE_FATAL_SIZE_NOT_1);
						M(n_debug_message);
					#endif

					g_mflags.error_code = ME_MATCHREDUCE_EXPECTED_1;

					return 0;
				}
			}
		}
	}

	// TODO: not sure why LOOKUPs can be treated in the previous loop??
	type = AST_LOOKUP;
	count = TLCountByType(list, type);
	if (count > 1) {
		return TLGetSize(list);
	} else {
		if (count == 1) {
			if (TLGetSize(list) == 1) {

				#ifdef ITCH_DEBUG
					strcpy_debug(n_debug_message, ITCH_DEBUG_MATCH_REDUCE_UNIQUE_NODE_PASSED_TO_REDUCER);
					M(n_debug_message);
				#endif

				g_mflags.error_code = PE_LOOKUP_PASSED_TO_REDUCER;

				return 0;
			}
		}
	}

	#ifdef ITCH_DEBUG
		strcpy_debug(n_debug_message, ITCH_DEBUG_MATCH_REDUCE_NEXT_DEP_MULTIPLE_USER_PARAM_NODES);
		M(n_debug_message);
	#endif

	g_mflags.error_code = PE_MULTIPLE_PARAM_LOOKAHEAD;
	//TODO This should really best detected in the lexer....

	return 0;
}

char* MapGetLastTargetString(char* return_string) {
	return strcpy(return_string, g_map_last_target_str);
}

uint16_t MapAdvance(uint8_t in_buf_idx) {
	// Last_matched_id is the uniquely matched node
	//
	// Move to first child so that subsequent matching
	// will occur against the child and its siblings

	//ASTA temp;
	MapGetASTAByID(g_mflags.last_matched_id, &g_temp_asta);

	// set the current_id for the next matching iteration to
	// the first child of the last matched.
	g_map_id_current = g_temp_asta.first_child;

	// advance the start pointer (line_pos) in the match
	// buffer so that the next match starts on the next token
	g_map_line_pos = in_buf_idx;

	#ifdef ITCH_DEBUG
		strcpy_debug(n_debug_message, ITCH_DEBUG_MAP_ADVANCE);
		M(n_debug_message);
	#endif

	return g_map_id_current;
}

uint8_t MapGetAction(uint16_t asta_id, char* action_str) {
	//ASTA temp_node;
	uint8_t result;
	result = MapGetASTAByID(asta_id, &g_temp_asta);
	if (result == PEME_NO_ERROR) {
		strcpy(action_str, g_temp_asta.action_identifier);
	}
	return result;
}

uint16_t MapGetLastMatchedID() {
	return g_mflags.last_matched_id;
}

uint16_t MapGetErrorCode() {
	return g_mflags.error_code;
}

