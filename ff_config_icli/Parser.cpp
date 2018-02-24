/*****************************************************************
 Parser.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#include "Parser.h"
#include "parser_errors.h"
#include <string.h>
#include <stdio.h>
#include <malloc.h>


Parser::Parser() {
	possible_list = new(TokenList);
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

	//parse_level = 0;
	//possible_count = 0;
	//last_matched_node = 0;
	//line_complete = 0;

	map.Reset();
	possible_list->Reset();

}

uint8_t Parser::P_EOL() {
	// Processed when EOL reached

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
				map.GetAction(map.GetLastMatchedID(), action_ident);
				printf("!!!!!!!*********!!!!!!!!**** Action matched and to be called: %s\n", action_ident);
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
	printf("DEBUG PARSE: Processing: \"%c\"\n", ch);
	#endif

	switch (ch) {

		case '\r':   // deal with noise - ignore
			return R_IGNORE;			break;
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
		printf("DEBUG: Discard: \"%c\"\n", ch);
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
	// advance the buffer index so that the matcher is looking at the next token.
	if (pf.delim_reached && (pf.match_result == MR_UNIQUE)) {
		map.Advance(in_idx);
		pf.delim_reached = 0;
	}

	// clear out previous matching possibilities
	possible_list->Reset();
	// run the match process and return the result
	return map.Match(in_buf, possible_list);
}


uint8_t Parser::MatchEvaluate(void) {
	char out[MAX_BUFFER_LENGTH];
	char temp[MAX_LABEL_LENGTH];

	switch (pf.match_result) {
		case MR_NO_MATCH:

			#ifdef DEBUG
			printf("DEBUG PARSE: MR_NO_MATCH\n");
			#endif

			pf.error_on_line = 1;
			pf.parse_result = R_IGNORE; // still need to clean out incoming buffer
			break;
		case MR_UNIQUE: {

			#ifdef DEBUG
			// single unique match - will still need EOL to finsih
			printf("DEBUG PARSE: MR_UNIQUE\n");
			//pf.unique_match = 1;
			char temp[MAX_LABEL_LENGTH];
			possible_list->GetCurrentLabel(temp);
			printf("DEBUG PARSE: Uniquely matched: %s\n\n", temp);
			#endif

			pf.parse_result = R_UNFINISHED;
		}
			break;
		case MR_ACTION_POSSIBLE:

			#ifdef DEBUG
			// single unique matched with is actionalable - (if EOL is next)
			printf("DEBUG PARSE: MR_ACTION_POSSIBLE\n");
			#endif

			pf.parse_result = R_UNFINISHED;
			break;
		case MR_MULTI: {

			#ifdef DEBUG
			printf("DEBUG PARSE: MR_MULTI:\n");
			possible_list->ToTop();
			for (int i = 0; i < possible_list->GetSize(); i++) {
				possible_list->GetCurrentLabel(temp);
				sprintf(out, "\t%s\n", temp);
				printf("DEBUG: %s", out);
				possible_list->Next();
			}
			#endif

			pf.parse_result = R_UNFINISHED;
		}
			break;

		case MR_DELIM_SKIP:

			#ifdef DEBUG
			printf("DEBUG PARSE: MR_DELIM_SKIP\n");
			#endif

			pf.parse_result = R_UNFINISHED;
			break;

		case MR_HELP_ACTIVE: {

			#ifdef DEBUG
			printf("DEBUG PARSE: MR_HELP_ACTIVE\n");
			#endif

			possible_list->ToTop();
			for (int i = 0; i < possible_list->GetSize(); i++) {
				possible_list->GetCurrentLabel(temp);
				sprintf(out, "\t%s\n", temp);

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
			printf("DEBUG PARSE: MR_ERROR\n");
			#endif

			pf.last_error = map.GetErrorCode();
			map.Reset();
			pf.parse_result = R_ERROR;
			break;

		default:

			#ifdef DEBUG
			printf("DEBUG PARSE: NO MR Caught - default case\n");
			#endif

			pf.last_error = PE_NODEMAP_RESULT_NOT_CAUGHT;
			map.Reset();
			pf.parse_result = R_ERROR;
			break;
	}
	return pf.parse_result;
}

void Parser::GetErrorString(char* error) {
	strcpy(error, parser_error_strings[pf.last_error].text);

}

