/*****************************************************************
 Parser.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "Parser.h"
#include "parser_errors.h"
#include "out.h"				// file written by lexer
								// XXX need proxy that gets written by lexer
#include "TokenList.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef LINUX
#include <malloc.h>
#endif



#ifdef DEBUG
extern void M(char strn[]);
char p_debug_message[MAX_OUTPUT_LINE_SIZE];
#endif




/******************************************************************************
 * Globals
 ******************************************************************************/

extern void M(char * strn);
extern OutputBuffer global_output;
extern char parser_replay_buff[MAX_OUTPUT_LINE_SIZE];


TokenList* parser_possible_list;

char parser_in_buf[MAX_INPUT_LINE_SIZE];

uint8_t parser_in_idx;

P_FLAGS pf;

TokenList* parser_param_list;


/******************************************************************************
 * Functions
 ******************************************************************************/

void ParserInit() {
	parser_possible_list = TLNewTokenList();
	parser_param_list = TLNewTokenList();
	// XXX context level to go here
	MapReset();
	ParserResetLine();
}

void ParserFinish() {
	TLDeleteTokenList(parser_possible_list);
}

void ParserResetLine(void) {

	parser_in_buf[0] = '\0';
	parser_in_idx = 0;

	pf.error_on_line = 0;
	//pf.unique_match = 0;
	pf.parse_result = R_NONE;
	pf.delim_reached = 0;
	pf.string_litteral = 0;
	pf.match_result = MR_NO_RESULT;
	pf.last_error = PEME_NO_ERROR;
	pf.help_active = 0;
	pf.escape = 0;

	MapReset();
	TLReset(parser_possible_list);
	TLReset(parser_possible_list);
}


uint16_t ParserActionDispatcher(uint16_t action_asta_id) {
	// Lookup the function to call (using functions from output of lexer)
	// Call the caller with an assembled param list.
	// Assumes that public member param_list is properly populated
	// with the required params to go with the call.

	char action_ident[MAX_AST_ACTION_SIZE];
	uint16_t xlat_call_id;
	ParamUnion params[MAX_PARAM_COUNT];
	uint8_t param_count;
	uint8_t param_index = 0;
	//TokenNode* param_token;
	char temp_str[MAX_OUTPUT_LINE_SIZE];
	uint8_t param_type;

	// get the static xlat ID from the lexer output header
	MapGetAction(action_asta_id, action_ident);
	xlat_call_id = LookupFuncMap(action_ident);

	// set up for assembling the params
	param_count = TLGetSize(parser_param_list);
	TLToTop(parser_param_list);

	// assemble the params array (of type ParamUnion)
	while (param_index < param_count) {
		TLGetCurrentLabel(parser_param_list, temp_str);
		param_type = TLGetCurrentType(parser_param_list);
		switch (param_type) {
			case AST_IDENTIFIER:
				uint16_t ident_xlat;
				uint16_t ident_asta_id;
				uint8_t member_id;
				ASTA temp_asta;

				// get the related asta node for its label-instance name
				ident_asta_id = TLGetCurrentID(parser_param_list);
				MapGetASTAByID(ident_asta_id, &temp_asta);

				ident_xlat = LookupIdentMap(temp_asta.label);
				member_id = LookupIdentifierMembers(ident_xlat, temp_str);
				params[param_index].param_uint16_t = member_id;
				break;
			case AST_LOOKUP:
			case AST_PARAM_DATE:
			case AST_PARAM_TIME:
			case AST_PARAM_STRING:
				params[param_index].param_char_star = temp_str;
				break;
			case AST_PARAM_INTEGER:
				params[param_index].param_int16_t = atoi(temp_str);
				break;
			case AST_PARAM_FLOAT:
				params[param_index].param_float = atof(temp_str);
				break;
			default:
				return PE_BAD_PARAM_ACTION_DISPATCHER;
		}
		param_index++;
		TLNext(parser_param_list);
	}

	// call the caller with the right function id and params
	// Return result (of call matching - callees results come back via a callback)
	return CallFunction(xlat_call_id, params);

}


void ParserBufferInject(char* inject_str) {
	strcpy(parser_in_buf, inject_str);
	parser_in_idx = strlen(parser_in_buf);
}

void P_ESCAPE(char ch) {
	if ( (ch == 0x1B) && (pf.escape == 0) ) {
		pf.escape = 1;
	}
	if ( (ch == '[') && (pf.escape == 1) ) {
		pf.escape = 2;
	}
	#ifdef DEBUG
	sprintf(p_debug_message, "DEBUG pf.escape %d\n\r", pf.escape);
	M(p_debug_message);
	#endif
}

uint8_t P_EOL() {
	// Processed when EOL reached

	// If help active, turn it off, reset the map and return R_HELP so results will be displayed
	if (pf.help_active) {
		pf.help_active = 0;

		MapReset();
		TLReset(parser_possible_list);
		return R_HELP;
	}
	// If no errors in matching call action or ignore blank lines, or catch other errors
	if (pf.error_on_line == 0) {
		switch (pf.match_result) {
			case MR_ACTION_POSSIBLE:
				char action_ident[MAX_AST_ACTION_SIZE];
				// Save the last token to the param list - becuase its EOL rather then DELIM
				// 	the ParseMatch will not have been triggered to save it
				ParserSaveTokenAsParameter();
				MapGetAction(MapGetLastMatchedID(), action_ident);
				#ifdef DEBUG
					sprintf(p_debug_message, "\n\rDEBUG **** Action matched and to be called: %s\n\r", action_ident);
					M(p_debug_message);
				#endif
				pf.last_error = ParserActionDispatcher(MapGetLastMatchedID());
				MapReset();
				TLReset(parser_possible_list);
				if (pf.last_error == PEME_NO_ERROR) {
					return R_COMPLETE;
				} else {
					return R_ERROR;
				}
				break;
			case MR_MULTI:
				pf.last_error = PE_MULTI_NOT_PARSED;
				MapReset();
				TLReset(parser_possible_list);
				return R_ERROR;
				break;
			default:
				if (parser_in_idx == 0) { // blank line - do nothing
					MapReset();
					TLReset(parser_possible_list);
					return R_COMPLETE;
				}
				pf.last_error = PE_LINE_INCOMPLETE;
				MapReset();
				TLReset(parser_possible_list);
				return R_ERROR;
				break;
		}
	}
	if (pf.error_on_line == 1) {
		switch(pf.match_result) {
			case MR_NO_MATCH:
				pf.last_error = PE_EOL_WITH_NO_MATCH;
				MapReset();
				TLReset(parser_possible_list);
				return R_ERROR;
				break;
			default:
				pf.last_error = PE_EOL_WITH_UKNOWN_MR;
				MapReset();
				TLReset(parser_possible_list);
				return R_ERROR;
				break;
		}
	}
	MapReset();
	TLReset(parser_possible_list);
	return R_ERROR;
}

uint8_t P_DOUBLE_QUOTE() {
	if (pf.string_litteral == 0) {
		pf.string_litteral = 1;
	} else {
		pf.string_litteral = 0;
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
	if (parser_in_idx == 0) { 			// leading space or tab
		// just ignore it
		return R_IGNORE;
	} else {
		if (pf.string_litteral == 1) {
			// let it fall through into the buffer
			return R_CONTINUE;
		} else {
			if (pf.delim_reached == 1) { // already had one - extra space or tabs between tokens
				return R_IGNORE;
			} else {
				if (pf.error_on_line == 0) { // must be a token delim
					pf.delim_reached = 1;
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
	parser_in_buf[parser_in_idx] = ch;
	parser_in_buf[parser_in_idx + 1] = '\0';
	parser_in_idx++;
}

uint8_t Parse(char ch) {
	// Process incoming char in context of parse flags
	// set from processing the previous char and match
	// and results from the node map (if called on previous).
	#ifdef DEBUG
		sprintf(p_debug_message, "*************** DEBUG (Parse) Processing: \"%c\"  ************\n\r", ch);
		M(p_debug_message);
	#endif

	switch (ch) {
		// Process escape sequences
		case 0x1B:
			P_ESCAPE(ch);
			return R_IGNORE;
			break;
		case '[':
			if(pf.escape == 1) {
				P_ESCAPE(ch);
				return R_IGNORE;
			}
			break;
		case 'A':
		case 'D':
			if(pf.escape == 2) {
				pf.escape = 0;
				#ifdef DEBUG
				sprintf(p_debug_message, "DEBUG PARSE Returning R_REPLAY\n\r");
				M(p_debug_message);
				#endif
				return R_REPLAY;
			}
			break;
		case '\r':   // deal with \r\n combinations and trigger actions
			return	P_EOL();			break;
		case '\n':	// deal with \r\n combinations and trigger actions
			return	P_EOL();			break;
		case '\"':	// Toggle string_litteral flag, discard the char
			return P_DOUBLE_QUOTE();	break;
		case ' ':
		case '\t':	// handle space and tab delimiters (and extras of them)
			if (P_SPACE_TAB() == R_IGNORE) { return R_IGNORE; }
			else						break; // R_CONTINUE
		case '?':	// turn on help flag
			pf.help_active = 1;			break;
		default:
			break;
	}
	// whatever it is now, throw it in the buffer and continue
	P_ADD_TO_BUFFER(ch);

	if (pf.error_on_line == 1) {
		// Error already flagged but need to discard buffered chars still coming before finishing
		// so just ignore until EOL. Next iteration will return Error or Complete accordingly
		// once EOL encountered
		#ifdef DEBUG
			sprintf(p_debug_message, "DEBUG: Discard: \"%c\"\n\r", ch);
			M(p_debug_message);
		#endif
		return R_IGNORE;
	}

	// continue to token matching
	pf.match_result = ParserMatch();
	// evaluate the match results for what it means for parsing
	pf.parse_result = ParserMatchEvaluate();
	global_output.SetOutputAvailable();
	return pf.parse_result;
}

uint8_t ParserMatch(void) {
	// Setup and call the nodemap token matching process

	// If last match result was unique and input char parsing detected a delimiter,
	// save the token for later action calling and advance the buffer index so that
	// the matcher is looking at the next token. (which will trigger a match dummy iteration
	// with the result being to go back to the calling routing unfinished to get the next ch)

	if (pf.delim_reached && ((pf.match_result == MR_UNIQUE) || (pf.match_result == MR_ACTION_POSSIBLE))) {
		ParserSaveTokenAsParameter();
		// advance the nodemap past the delimiter
		MapAdvance(parser_in_idx);
		pf.delim_reached = 0;
	}

	// clear out previous matching possibilities
	TLReset(parser_possible_list);

	// preserve the in_buf for possible replay
	strcpy(parser_replay_buff, parser_in_buf);

	// run the next match process and return the result
	return MapMatch(parser_in_buf, parser_possible_list);
}


uint8_t ParserMatchEvaluate(void) {
	char out[MAX_OUTPUT_LINE_SIZE];
	char temp[MAX_OUTPUT_LINE_SIZE];

	switch (pf.match_result) {
		case MR_NO_MATCH:

			#ifdef DEBUG
				sprintf(p_debug_message, "DEBUG (MatchEvaluate): MR_NO_MATCH\n\r");
				M(p_debug_message);
			#endif

			pf.error_on_line = 1;
			pf.parse_result = R_IGNORE; // still need to clean out incoming buffer
			break;
		case MR_UNIQUE: {

			#ifdef DEBUG
			// single unique match - will still need EOL to finsih
			sprintf(p_debug_message, "DEBUG PARSE: MR_UNIQUE\n\r");
			M(p_debug_message);
			//pf.unique_match = 1;
			char temp[MAX_OUTPUT_LINE_SIZE];
			TLGetCurrentLabel(parser_possible_list, temp);
			sprintf(p_debug_message, "DEBUG PARSE: Uniquely matched: %s\n\n\r", temp);
			M(p_debug_message);
			#endif

			pf.parse_result = R_UNFINISHED;
		}
			break;
		case MR_ACTION_POSSIBLE:

			#ifdef DEBUG
			// single unique matched with is actionalable - (if EOL is next)
			sprintf(p_debug_message, "DEBUG PARSE: MR_ACTION_POSSIBLE\n\r");
			M(p_debug_message);
			#endif

			pf.parse_result = R_UNFINISHED;
			break;
		case MR_MULTI: {

			#ifdef DEBUG
			sprintf(p_debug_message, "DEBUG PARSE: MR_MULTI:\n\r");
			M(p_debug_message);
			TLToTop(parser_possible_list);
			for (uint16_t i = 0; i < TLGetSize(parser_possible_list); i++) {
				TLGetCurrentLabel(parser_possible_list, temp);
				sprintf(out, "\t%s\n\r", temp);
				sprintf(p_debug_message, "DEBUG: %s", out);
				M(p_debug_message);
				TLNext(parser_possible_list);
			}
			#endif

			pf.parse_result = R_UNFINISHED;
		}
			break;

		case MR_DELIM_SKIP:

			#ifdef DEBUG
			sprintf(p_debug_message, "DEBUG PARSE: MR_DELIM_SKIP\n\r");
			M(p_debug_message);
			#endif

			pf.parse_result = R_UNFINISHED;
			break;

		case MR_HELP_ACTIVE: {

			#ifdef DEBUG
			sprintf(p_debug_message, "DEBUG PARSE: MR_HELP_ACTIVE\n\r");
			M(p_debug_message);
			#endif

			TLToTop(parser_possible_list);
			for (uint16_t i = 0; i < TLGetSize(parser_possible_list); i++) {
				TLGetCurrentLabel(parser_possible_list, temp);
				sprintf(out, "\t%s\n\r", temp);

				#ifdef DEBUG
				sprintf(p_debug_message, "DEBUG: %s", out);
				M(p_debug_message);
				#endif

				global_output.EnQueue(out);
				TLNext(parser_possible_list);
			}
			pf.parse_result = R_UNFINISHED;
		}
			break;

		case MR_ERROR:

			#ifdef DEBUG
			sprintf(p_debug_message, "DEBUG PARSE: MR_ERROR\n\r");
			M(p_debug_message);
			#endif

			pf.last_error = MapGetErrorCode();
			MapReset();
			pf.parse_result = R_ERROR;
			break;

		default:

			#ifdef DEBUG
			sprintf(p_debug_message, "DEBUG PARSE: NO MR Caught - default case\n\r");
			M(p_debug_message);
			#endif

			pf.last_error = PE_NODEMAP_RESULT_NOT_CAUGHT;
			MapReset();
			pf.parse_result = R_ERROR;
			break;
	}
	return pf.parse_result;
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
	char temp_param[MAX_IDENTIFIER_LABEL_SIZE];
	ASTA temp_asta_node;
	uint16_t temp_asta_id;

	// get the id of the last matched node
	temp_asta_id = MapGetLastMatchedID();

	// get the matched node from the asta
	MapGetASTAByID(temp_asta_id, &temp_asta_node);

	// Add to the param list for action calling (but exclude keywords)
	if (temp_asta_node.type > AST_KEYWORD) {

		// get the last matched param as a string
		MapGetLastTargetString(temp_param);

		// get a new param node and write its details
		new_param = TLNewTokenNode();
		new_param->label = (char *)malloc((strlen(temp_param) + 1) * sizeof(char));
		strcpy(new_param->label, temp_param);
		new_param->id = temp_asta_id;
		new_param->type = temp_asta_node.type;
		TLAdd(parser_param_list, new_param);
	}
}

void ParserGetErrorString(char* error) {
#ifdef ARDUINO
	memcpy_P(error, &(parser_error_strings[pf.last_error].text), sizeof(SimpleStringArray));
#else
	strcpy(error, parser_error_strings[pf.last_error].text);
#endif
}

void ParserSetError(uint16_t err) {
	pf.last_error = err;
}

