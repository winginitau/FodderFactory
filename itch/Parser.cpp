/*****************************************************************
 Parser.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "Parser.h"
#include "itch_strings.h"
#include "itch_hal.h"
#include "TokenList.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef LINUX
#include <malloc.h>
#endif


#ifdef ITCH_DEBUG
extern void M(char strn[]);
char p_debug_message[MAX_OUTPUT_LINE_SIZE];
#endif



/******************************************************************************
 * Globals
 ******************************************************************************/

char g_parser_in_buf[MAX_INPUT_LINE_SIZE];
uint8_t g_parser_in_idx;
P_FLAGS g_pflags;
TokenList* g_parser_possible_list;
TokenList* g_parser_param_list;


/******************************************************************************
 * Functions
 ******************************************************************************/

void ParserInit() {
	g_parser_possible_list = TLNewTokenList();
	g_parser_param_list = TLNewTokenList();
	// XXX context level to go here
	ParserResetLine();
}

void ParserFinish() {
	TLDeleteTokenList(g_parser_possible_list);
	TLDeleteTokenList(g_parser_param_list);
}

void ParserResetLine(void) {

	g_parser_in_buf[0] = '\0';
	g_parser_in_idx = 0;

	g_pflags.error_on_line = 0;
	g_pflags.parse_result = R_NONE;
	g_pflags.delim_reached = 0;
	g_pflags.string_litteral = 0;
	g_pflags.match_result = MR_NO_RESULT;
	g_pflags.last_error = PEME_NO_ERROR;
	g_pflags.help_active = 0;
	g_pflags.escape = 0;
	g_pflags.eol_processed = 0;

	MapReset();
	TLReset(g_parser_possible_list);
	TLReset(g_parser_param_list);
}


uint16_t ParserActionDispatcher(uint16_t action_asta_id) {
	// Lookup the function to call (using func map output from the lexer)
	// Call the caller with an assembled param list.
	// Assumes that  param_list is properly populated
	// with the required params to go with the call.

	char action_ident_str[MAX_AST_ACTION_SIZE];

	uint16_t func_xlat_call_id;
	ParamUnion param_union_array[MAX_PARAM_COUNT];
	uint8_t param_count = 0;
	uint8_t param_index = 0;

	//char temp_str[MAX_OUTPUT_LINE_SIZE];
	char* temp_str_ptr;
	uint8_t param_type;

	// get the static xlat ID from the lexer output header
	MapGetAction(action_asta_id, action_ident_str);
	func_xlat_call_id = LookupFuncMap(action_ident_str);

	// set up for assembling the params
	param_count = TLGetSize(g_parser_param_list);
	TLToTop(g_parser_param_list);

	// Assemble the param array (of type ParamUnion)
	// Iterate down g_parser_param_list to get the params
	while (param_index < param_count) {
		//TLCopyCurrentLabel(g_parser_param_list, temp_str);
		temp_str_ptr = TLGetCurrentLabelPtr(g_parser_param_list);
		param_type = TLGetCurrentType(g_parser_param_list);
		switch (param_type) {
			case AST_IDENTIFIER:
				uint16_t ident_xlat;
				uint16_t ident_asta_id;
				uint8_t member_id;

				// get the original asta node that relates to this param - for its label-instance name
				ident_asta_id = TLGetCurrentID(g_parser_param_list);
				MapGetASTAByID(ident_asta_id, &g_temp_asta);

				ident_xlat = LookupIdentMap(g_temp_asta.label);
				member_id = LookupIdentifierMembers(ident_xlat, temp_str_ptr);
				param_union_array[param_index].param_uint16_t = member_id;
				break;
			case AST_LOOKUP:
			case AST_PARAM_DATE:
			case AST_PARAM_TIME:
			case AST_PARAM_STRING:
				param_union_array[param_index].param_char_star = temp_str_ptr;
				break;
			case AST_PARAM_INTEGER:
				param_union_array[param_index].param_int16_t = atoi(temp_str_ptr);
				break;
			case AST_PARAM_FLOAT:
				param_union_array[param_index].param_float = atof(temp_str_ptr);
				break;
			default:
				return PE_BAD_PARAM_ACTION_DISPATCHER;
		}
		param_index++;
		TLNext(g_parser_param_list);
	}

	// call the caller with the right function id and params
	// Return result (of call matching - callees results come back via a callback)
	return CallFunction(func_xlat_call_id, param_union_array);

}

void ParserBufferInject(char* inject_str) {
	strcpy(g_parser_in_buf, inject_str);
	g_parser_in_idx = strlen(g_parser_in_buf);
}

void P_ESCAPE(char ch) {
	if ( (ch == 0x1B) && (g_pflags.escape == 0) ) {
		g_pflags.escape = 1;
	}
	if ( (ch == '[') && (g_pflags.escape == 1) ) {
		g_pflags.escape = 2;
	}
	#ifdef ITCH_DEBUG
		strcpy_debug(g_temp_str, ITCH_DEBUG_PF_ESCAPE_IS);
		sprintf(p_debug_message, "%s %d\n", g_temp_str, g_pflags.escape);
		M(p_debug_message);
	#endif
}

uint8_t P_EOL() {
	// Processed when EOL reached

	// If help active, turn it off, reset the map and return R_HELP so results will be displayed
	if (g_pflags.help_active) {
		g_pflags.help_active = 0;

		MapReset();
		TLReset(g_parser_possible_list);
		return R_HELP;
	}
	// If no errors in matching call action or ignore blank lines, or catch other errors
	if (g_pflags.error_on_line == 0) {

		switch (g_pflags.match_result) {

			case MR_ACTION_POSSIBLE:
				// Save the last token to the param list - becuase its EOL rather then DELIM
				// 	the ParseMatch will not have been triggered to save it
				ParserSaveTokenAsParameter();

				#ifdef ITCH_DEBUG
					char action_ident[MAX_AST_ACTION_SIZE];
					MapGetAction(MapGetLastMatchedID(), action_ident);
					strcpy_debug(g_temp_str, ITCH_DEBUG_P_EOL_ACTION_TO_BE_CALLED_IS);
					sprintf(p_debug_message, "\n%s%s\n", g_temp_str, action_ident);
					M(p_debug_message);
				#endif

				// Dispatch the action and record any error
				g_pflags.last_error = ParserActionDispatcher(MapGetLastMatchedID());

				MapReset();
				TLReset(g_parser_possible_list);
				if (g_pflags.last_error == PEME_NO_ERROR) {
					return R_COMPLETE;
				} else {
					return R_ERROR;
				}
				break;
			case MR_MULTI:
				g_pflags.last_error = PE_MULTI_NOT_PARSED;
				MapReset();
				TLReset(g_parser_possible_list);
				return R_ERROR;
				break;
			default:
				if (g_parser_in_idx == 0) { // blank line - do nothing
					MapReset();
					TLReset(g_parser_possible_list);
					return R_COMPLETE;
				}
				g_pflags.last_error = PE_LINE_INCOMPLETE;
				MapReset();
				TLReset(g_parser_possible_list);
				return R_ERROR;
				break;
		}
	}

	// else - must be an error on the line

	if (g_pflags.error_on_line == 1) {
		switch(g_pflags.match_result) {
			case MR_NO_MATCH:
				g_pflags.last_error = PE_EOL_WITH_NO_MATCH;
				MapReset();
				TLReset(g_parser_possible_list);
				return R_ERROR;
				break;
			default:
				g_pflags.last_error = PE_EOL_WITH_UKNOWN_MR;
				MapReset();
				TLReset(g_parser_possible_list);
				return R_ERROR;
				break;
		}
	}
	MapReset();
	TLReset(g_parser_possible_list);
	return R_ERROR;
}

uint8_t P_DOUBLE_QUOTE() {
	if (g_pflags.string_litteral == 0) {
		g_pflags.string_litteral = 1;
	} else {
		g_pflags.string_litteral = 0;
	}
	// XXX send " to match buffer?
	// map should probably handle string capture.....
	// handle string literals
	//if (pf.string_litteral == 1) { // already in one
		// XXX stuff into a string literal param buffer as well as
		// 	fall through to main buffer
	//}

	return R_CONTINUE;
}

uint8_t P_SPACE_TAB() {
	if (g_parser_in_idx == 0) { 			// leading space or tab
		// just ignore it
		return R_IGNORE;
	} else {
		if (g_pflags.string_litteral == 1) {
			// let it fall through into the buffer
			return R_CONTINUE;
		} else {
			if (g_pflags.delim_reached == 1) { // already had one - extra space or tabs between tokens
				return R_IGNORE;
			} else {
				if (g_pflags.error_on_line == 0) { // must be a token delim
					g_pflags.delim_reached = 1;
				} else {
					// already error on line - just fall through
					// for error line play back
				}
				return R_CONTINUE;
			}
		}
	}
}

void P_ADD_TO_BUFFER(char ch) {
	g_parser_in_buf[g_parser_in_idx] = ch;
	g_parser_in_buf[g_parser_in_idx + 1] = '\0';
	g_parser_in_idx++;
}

uint8_t Parse(char ch) {
	// Process incoming char in context of parse flags
	// set from processing the previous char and match
	// and results from the node map (if called on previous).
	#ifdef ITCH_DEBUG
		strcpy_debug(g_temp_str, ITCH_DEBUG_PARSE_PROCESSING_CHAR);
		sprintf(p_debug_message, "%s%c\n", g_temp_str, ch);
		M(p_debug_message);
	#endif

	switch (ch) {
		// Process escape sequences
		case 0x1B:
			P_ESCAPE(ch);
			return R_IGNORE;
			break;
		case '[':
			if(g_pflags.escape == 1) {
				P_ESCAPE(ch);
				return R_IGNORE;
			}
			break;
		case 'A':
		case 'D':
			if(g_pflags.escape == 2) {
				g_pflags.escape = 0;
				#ifdef ITCH_DEBUG
					strcpy_debug(p_debug_message, ITCH_DEBUG_PARSE_R_REPLAY);
					M(p_debug_message);
				#endif
				return R_REPLAY;
			}
			break;
		case '\r':   // deal with \r\n combinations and trigger actions
			if(g_pflags.eol_processed == 0) {
				g_pflags.eol_processed = 1;
				return	P_EOL();
			} else {
				return R_IGNORE;
			}
			break;
		case '\n':	// deal with \r\n combinations and trigger actions
			if(g_pflags.eol_processed == 0) {
				g_pflags.eol_processed = 1;
				return	P_EOL();
			} else {
				return R_IGNORE;
			}
			break;
		case '\"':	// Toggle string_litteral flag, discard the char
			return P_DOUBLE_QUOTE();	break;
		case ' ':
		case '\t':	// handle space and tab delimiters (and extras of them)
			if (P_SPACE_TAB() == R_IGNORE) { return R_IGNORE; }
			else						break; // R_CONTINUE
		case '?':	// turn on help flag
			g_pflags.help_active = 1;			break;
		default:
			break;
	}


	// whatever it is now, throw it in the buffer and continue
	P_ADD_TO_BUFFER(ch);

	if (g_pflags.error_on_line == 1) {
		// Error already flagged but need to discard buffered chars still coming before finishing
		// so just ignore until EOL. Next iteration will return Error or Complete accordingly
		// once EOL encountered
		#ifdef ITCH_DEBUG
			strcpy_debug(g_temp_str, ITCH_DEBUG_PARSE_DISCARD_CHAR);
			sprintf(p_debug_message, "%s%c\n", g_temp_str, ch);
			M(p_debug_message);
		#endif
		return R_IGNORE;
	}


	// if we get to here its just another ordinary char - continue to matching
	g_pflags.match_result = ParserMatch();

	// evaluate the match results for what it means for parsing
	g_pflags.parse_result = ParserMatchEvaluate();

	g_itch_output_buff.SetOutputAvailable();
	return g_pflags.parse_result;

}

uint8_t ParserMatch(void) {
	// Setup and call the nodemap token matching process

	// If last match result was unique and input char parsing detected a delimiter,
	// save the token for later action calling and advance the buffer index so that
	// the matcher is looking at the next token. (which will trigger a match dummy iteration
	// with the result being to go back to the calling routing unfinished to get the next ch)

	if (g_pflags.delim_reached && ((g_pflags.match_result == MR_UNIQUE) || (g_pflags.match_result == MR_ACTION_POSSIBLE))) {
		ParserSaveTokenAsParameter();
		// advance the nodemap past the delimiter
		MapAdvance(g_parser_in_idx);
		g_pflags.delim_reached = 0;
	}

	// clear out previous matching possibilities
	TLReset(g_parser_possible_list);

	// preserve the in_buf for possible replay
	strcpy(g_itch_replay_buff, g_parser_in_buf);

	// run the next match process and return the result
	return MapMatch(g_parser_in_buf, g_parser_possible_list);

}


uint8_t ParserMatchEvaluate(void) {
	//char out[MAX_OUTPUT_LINE_SIZE];
	//char temp[MAX_OUTPUT_LINE_SIZE];

	switch (g_pflags.match_result) {
		case MR_NO_MATCH:

			#ifdef ITCH_DEBUG
				strcpy_debug(p_debug_message, ITCH_DEBUG_PARSERMATCHEVALUATE_MR_NO_MATCH);
				M(p_debug_message);
			#endif

			g_pflags.error_on_line = 1;
			g_pflags.parse_result = R_IGNORE; // still need to clean out incoming buffer
			break;
		case MR_UNIQUE: {

			#ifdef ITCH_DEBUG
				// single unique match - will still need EOL to finsih
				strcpy_debug(p_debug_message, ITCH_DEBUG_PARSERMATCHEVALUATE_MR_UNIQUE);
				M(p_debug_message);
				//pf.unique_match = 1;
				char temp[MAX_OUTPUT_LINE_SIZE];
				TLCopyCurrentLabel(g_parser_possible_list, temp);
				strcpy_debug(g_temp_str, ITCH_DEBUG_PARSERMATCHEVALUATE_UNIQUE_MATCH_IS);
				sprintf(p_debug_message, "%s%s\n", g_temp_str, temp);
				M(p_debug_message);
			#endif

			// All good - unique match to node on this token,
			// return to continue getting more chars
			g_pflags.parse_result = R_UNFINISHED;
		}
			break;
		case MR_ACTION_POSSIBLE:

			#ifdef ITCH_DEBUG
				// single unique matched with is actionalable - (if EOL is next)
				strcpy_debug(p_debug_message, ITCH_DEBUG_PARSERMATCHEVALUATE_MR_ACTION_POSSIBLE);
				M(p_debug_message);
			#endif

			// All good - unique match to node on this token,
			// and it is actionable at this point (if EOL is next)
			// return to continue getting more chars
			g_pflags.parse_result = R_UNFINISHED;
			break;
		case MR_MULTI: {

			#ifdef ITCH_DEBUG
				strcpy_debug(p_debug_message, ITCH_DEBUG_PARSERMATCHEVALUATE_MR_MULTI);
				M(p_debug_message);
				TLToTop(g_parser_possible_list);
				for (uint16_t i = 0; i < TLGetSize(g_parser_possible_list); i++) {
					TLCopyCurrentLabel(g_parser_possible_list, g_temp_str);
					sprintf(g_out_str, "\t%s\n", g_temp_str);
					sprintf(p_debug_message, "DEBUG: %s", g_out_str);
					M(p_debug_message);
					TLNext(g_parser_possible_list);
				}
			#endif

			// no errors so far, but the entered token could match
			// multiple nodes... continue to get more chars
			g_pflags.parse_result = R_UNFINISHED;
		}
			break;

		case MR_DELIM_SKIP:

			#ifdef ITCH_DEBUG
				strcpy_debug(p_debug_message, ITCH_DEBUG_PARSERMATCHEVALUATE_MR_DELIM_SKIP);
				M(p_debug_message);
			#endif

			// good-oh, carry on
			g_pflags.parse_result = R_UNFINISHED;
			break;

		case MR_HELP_ACTIVE: {

			#ifdef ITCH_DEBUG
				strcpy_debug(p_debug_message, ITCH_DEBUG_PARSERMATCHEVALUATE_MR_HELP_ACTIVE);
				M(p_debug_message);
			#endif

			TLToTop(g_parser_possible_list);
			for (uint16_t i = 0; i < TLGetSize(g_parser_possible_list); i++) {
				TLCopyCurrentLabel(g_parser_possible_list, g_temp_str);
				sprintf(g_out_str, "\t%s", g_temp_str);

				#ifdef ITCH_DEBUG
					sprintf(p_debug_message, "DEBUG: %s", g_out_str);
					M(p_debug_message);
				#endif

				g_itch_output_buff.EnQueue(g_out_str);
				TLNext(g_parser_possible_list);
			}
			g_pflags.parse_result = R_UNFINISHED;
		}
			break;

		case MR_ERROR:

			#ifdef ITCH_DEBUG
				strcpy_debug(p_debug_message, ITCH_DEBUG_PARSERMATCHEVALUATE_MR_ERROR);
				M(p_debug_message);
			#endif

			g_pflags.last_error = MapGetErrorCode();
			MapReset();
			g_pflags.parse_result = R_ERROR;
			break;

		default:

			#ifdef ITCH_DEBUG
				strcpy_debug(p_debug_message, ITCH_DEBUG_PARSERMATCHEVALUATE_NO_MR_CAUGHT_DEFUALT);
				M(p_debug_message);
			#endif

			g_pflags.last_error = PE_NODEMAP_RESULT_NOT_CAUGHT;
			MapReset();
			g_pflags.parse_result = R_ERROR;
			break;
	}
	return g_pflags.parse_result;
}

void ParserSaveTokenAsParameter(void) {
	// Save the last matched node/token details into the param list
	//
	// Called by ParseMatch when a delimiter is reached and there is
	//	a unique match or an action match.
	//
	// Also called by P_EOL to capture the last token on the line before
	// 	action processing, as this token will not have been isolated in
	// 	the match loop - because the EOL has been reached rather than a
	// 	delimiter.

	TokenNode* new_param;
	//char temp_param[MAX_IDENTIFIER_LABEL_SIZE];
	//ASTA temp_asta_node;
	uint16_t temp_asta_id;

	// get the id of the last matched node
	temp_asta_id = MapGetLastMatchedID();

	// get the matched node from the asta
	MapGetASTAByID(temp_asta_id, &g_temp_asta);

	// Add to the param list for action calling (but exclude keywords)
	if (g_temp_asta.type > AST_KEYWORD) {

		// get the last matched param as a string
		MapGetLastTargetString(g_temp_str);

		// get a new param node and write its details
		new_param = TLNewTokenNode();
		new_param->label = (char *)malloc((strlen(g_temp_str) + 1) * sizeof(char));
		strcpy(new_param->label, g_temp_str);
		new_param->id = temp_asta_id;
		new_param->type = g_temp_asta.type;
		TLAdd(g_parser_param_list, new_param);
	}
}

void ParserGetErrorString(char* error) {
#ifdef ARDUINO
	//memcpy_P(error, &(parser_error_strings[g_pflags.last_error].text), sizeof(EnumStringArray));
	strcpy_P(error, parser_error_strings[g_pflags.last_error].text);
#else
	strcpy(error, parser_error_strings[g_pflags.last_error].text);
#endif
}

void ParserSetError(uint16_t err) {
	g_pflags.last_error = err;
}

