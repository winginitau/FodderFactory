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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef LINUX
#include <malloc.h>
#endif


Parser::Parser() {
	possible_list = new(TokenList);
	param_list = new(TokenList);
	// XXX context level to go here
	ResetLine();
}

Parser::~Parser() {
	possible_list->Reset();
	delete(possible_list);
}

void Parser::ResetLine(void) {

	in_buf[0] = '\0';
	in_idx = 0;

	pf.error_on_line = 0;
	//pf.unique_match = 0;
	pf.parse_result = R_NONE;
	pf.delim_reached = 0;
	pf.string_litteral = 0;
	pf.match_result = MR_NO_RESULT;
	pf.last_error = PE_NO_ERROR;
	pf.help_active = 0;
	pf.escape = 0;

	map.Reset();
	possible_list->Reset();
	param_list->Reset();
}


void Parser::ActionDispatcher(uint16_t action_asta_id) {
	// Lookup the function to call (using functions from output of lexer)
	// Call the caller with an assembled param list.
	// Assumes that public member param_list is properly populated
	// with the required params to go with the call.

	char action_ident[MAX_LABEL_LENGTH];
	uint16_t xlat_call_id;
	ParamUnion params[MAX_PARAM_COUNT];
	uint8_t param_count;
	uint8_t param_index = 0;
	//TokenNode* param_token;
	char temp_str[MAX_BUFFER_LENGTH];
	uint8_t param_type;

	// get the static xlat ID from the lexer output header
	map.GetAction(action_asta_id, action_ident);
	xlat_call_id = LookupFuncMap(action_ident);

	// set up for assembling the params
	param_count = param_list->GetSize();
	param_list->ToTop();

	// assemble the params array (of type ParamUnion)
	while (param_index < param_count) {
		param_list->GetCurrentLabel(temp_str);
		param_type = param_list->GetCurrentType();
		switch (param_type) {
			case AST_IDENTIFIER:
				uint16_t ident_xlat;
				uint16_t ident_asta_id;
				uint8_t member_id;
				ASTA temp_asta;

				// get the related asta node for its label-instance name
				ident_asta_id = param_list->GetCurrentID();
				map.GetASTAByID(ident_asta_id, &temp_asta);

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
				//XXX - do proper error reporting
				printf("ERROR: Incorrect parameter found in param_list - keyword?\n\r");
				exit(-1);
		}
		param_index++;
		param_list->Next();
	}

	// call the caller with the right function id and params
	// XXX still to sort return processing
	CallFunction(xlat_call_id, params);

}


void Parser::BufferInject(char* inject_str) {
	strcpy(in_buf, inject_str);
	in_idx = strlen(in_buf);
}

void Parser::P_ESCAPE(char ch) {
	if ( (ch == 0x1B) && (pf.escape == 0) ) {
		pf.escape = 1;
	}
	if ( (ch == '[') && (pf.escape == 1) ) {
		pf.escape = 2;
	}
	printf("DEBUG pf.escape %d\n\r", pf.escape);
}

uint8_t Parser::P_EOL() {
	// Processed when EOL reached

	// Save the last token as a parameter for action calling;
	//if (pf.help_active == 0) {
	//	SaveTokenAsParameter();
	//}

	// If help active, tuen it off, reset the map and return R_HELP so results will be displayed
	if (pf.help_active) {
		pf.help_active = 0;

		map.Reset();
		possible_list->Reset();
		return R_HELP;
	}
	if (pf.error_on_line == 0) {
		switch (pf.match_result) {
			case MR_ACTION_POSSIBLE:
				char action_ident[MAX_LABEL_LENGTH];
				// Save the last token to the param list - becuase its EOL rather then DELIM
				// 	the ParseMatch will not have been triggered to save it
				SaveTokenAsParameter();
				map.GetAction(map.GetLastMatchedID(), action_ident);
				#ifdef DEBUG
					printf("\n\rDEBUG **** Action matched and to be called: %s\n\r", action_ident);
				#endif
				ActionDispatcher(map.GetLastMatchedID());
				map.Reset();
				possible_list->Reset();
				return R_COMPLETE;
				break;
			case MR_MULTI:
				pf.last_error = PE_MULTI_NOT_PARSED;
				map.Reset();
				possible_list->Reset();
				return R_ERROR;
				break;
			default:
				if (in_idx == 0) { // blank line - do nothing
					map.Reset();
					possible_list->Reset();
					return R_COMPLETE;
				}
				pf.last_error = PE_LINE_INCOMPLETE;
				map.Reset();
				possible_list->Reset();
				return R_ERROR;
				break;
		}
	}
	if (pf.error_on_line == 1) {
		switch(pf.match_result) {
			case MR_NO_MATCH:
				pf.last_error = PE_EOL_WITH_NO_MATCH;
				map.Reset();
				possible_list->Reset();
				return R_ERROR;
				break;
			default:
				pf.last_error = PE_EOL_WITH_UKNOWN_MR;
				map.Reset();
				possible_list->Reset();
				return R_ERROR;
				break;
		}
	}
	map.Reset();
	possible_list->Reset();
	return R_ERROR;
}

uint8_t Parser::P_DOUBLE_QUOTE() {
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

uint8_t Parser::P_SPACE_TAB() {
	if (in_idx == 0) { 			// leading space or tab
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

void Parser::P_ADD_TO_BUFFER(char ch) {
	in_buf[in_idx] = ch;
	in_buf[in_idx + 1] = '\0';
	in_idx++;
}

uint8_t Parser::Parse(char ch) {
	// Process incoming char in context of parse flags
	// set from processing the previous char and match
	// results from the node map (if called on previous).
	#ifdef DEBUG
	printf("DEBUG PARSE: Processing: \"%c\"\n\r", ch);
	#endif

	switch (ch) {
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
				printf("DEBUG PARSE Returning R_REPLAY\n\r");
				return R_REPLAY;
			}
			break;
		case '\r':   // deal with noise - ignore
			//return R_IGNORE;			break;
			return	P_EOL();			break;
		case '\n':	// handle EOL ***** doing stuff triggered from here *****
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
		printf("DEBUG: Discard: \"%c\"\n\r", ch);
		#endif
		return R_IGNORE;
	}

	// continue to token matching
	pf.match_result = ParseMatch();
	// evaluate the match results for what it means for parsing
	pf.parse_result = MatchEvaluate();
	output.SetOutputAvailable();
	return pf.parse_result;
}

uint8_t Parser::ParseMatch(void) {
	// Setup and call the nodemap token matching process

	// If last match result was unique and input char parsing detected a delimiter,
	// save the token for later action calling and advance the buffer index so that
	// the matcher is looking at the next token. (which will trigger a match dummy iteration
	// with the result being to go back to the calling routing unfinished to get the next ch)

	if (pf.delim_reached && ((pf.match_result == MR_UNIQUE) || (pf.match_result == MR_ACTION_POSSIBLE))) {
		SaveTokenAsParameter();
		// advance the nodemap past the delimiter
		map.Advance(in_idx);
		pf.delim_reached = 0;
	}

	// clear out previous matching possibilities
	possible_list->Reset();

	// preserve the in_buf for possible replay
	strcpy(replay_buf, in_buf);

	// run the next match process and return the result
	return map.Match(in_buf, possible_list);
}


uint8_t Parser::MatchEvaluate(void) {
	char out[MAX_BUFFER_LENGTH];
	char temp[MAX_LABEL_LENGTH];

	switch (pf.match_result) {
		case MR_NO_MATCH:

			#ifdef DEBUG
			printf("DEBUG PARSE: MR_NO_MATCH\n\r");
			#endif

			pf.error_on_line = 1;
			pf.parse_result = R_IGNORE; // still need to clean out incoming buffer
			break;
		case MR_UNIQUE: {

			#ifdef DEBUG
			// single unique match - will still need EOL to finsih
			printf("DEBUG PARSE: MR_UNIQUE\n\r");
			//pf.unique_match = 1;
			char temp[MAX_LABEL_LENGTH];
			possible_list->GetCurrentLabel(temp);
			printf("DEBUG PARSE: Uniquely matched: %s\n\n\r", temp);
			#endif

			pf.parse_result = R_UNFINISHED;
		}
			break;
		case MR_ACTION_POSSIBLE:

			#ifdef DEBUG
			// single unique matched with is actionalable - (if EOL is next)
			printf("DEBUG PARSE: MR_ACTION_POSSIBLE\n\r");
			#endif

			pf.parse_result = R_UNFINISHED;
			break;
		case MR_MULTI: {

			#ifdef DEBUG
			printf("DEBUG PARSE: MR_MULTI:\n\r");
			possible_list->ToTop();
			for (int i = 0; i < possible_list->GetSize(); i++) {
				possible_list->GetCurrentLabel(temp);
				sprintf(out, "\t%s\n\r", temp);
				printf("DEBUG: %s", out);
				possible_list->Next();
			}
			#endif

			pf.parse_result = R_UNFINISHED;
		}
			break;

		case MR_DELIM_SKIP:

			#ifdef DEBUG
			printf("DEBUG PARSE: MR_DELIM_SKIP\n\r");
			#endif

			pf.parse_result = R_UNFINISHED;
			break;

		case MR_HELP_ACTIVE: {

			#ifdef DEBUG
			printf("DEBUG PARSE: MR_HELP_ACTIVE\n\r");
			#endif

			possible_list->ToTop();
			for (int i = 0; i < possible_list->GetSize(); i++) {
				possible_list->GetCurrentLabel(temp);
				sprintf(out, "\t%s\n\r", temp);

				#ifdef DEBUG
				printf("DEBUG: %s", out);
				#endif

				output.EnQueue(out);
				possible_list->Next();
			}
			pf.parse_result = R_UNFINISHED;
		}
			break;

		case MR_ERROR:

			#ifdef DEBUG
			printf("DEBUG PARSE: MR_ERROR\n\r");
			#endif

			pf.last_error = map.GetErrorCode();
			map.Reset();
			pf.parse_result = R_ERROR;
			break;

		default:

			#ifdef DEBUG
			printf("DEBUG PARSE: NO MR Caught - default case\n\r");
			#endif

			pf.last_error = PE_NODEMAP_RESULT_NOT_CAUGHT;
			map.Reset();
			pf.parse_result = R_ERROR;
			break;
	}
	return pf.parse_result;
}

void Parser::SaveTokenAsParameter(void) {
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
	char temp_param[MAX_LABEL_LENGTH];
	ASTA temp_asta_node;
	uint16_t temp_asta_id;

	// get the id of the last matched node
	temp_asta_id = map.GetLastMatchedID();

	// get the matched node from the asta
	map.GetASTAByID(temp_asta_id, &temp_asta_node);

	// Add to the param list for action calling (but exclude keywords)
	if (temp_asta_node.type > AST_KEYWORD) {

		// get the last matched param as a string
		map.GetLastTargetString(temp_param);

		// get a new param node and write its details
		new_param = param_list->NewTokenNode();
		strcpy(new_param->label, temp_param);
		new_param->id = temp_asta_id;
		new_param->type = temp_asta_node.type;
		param_list->Add(new_param);
	}
}

void Parser::GetErrorString(char* error) {
	strcpy(error, parser_error_strings[pf.last_error].text);

}

