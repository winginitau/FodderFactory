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
	parse_level = 0;
	possible_count = 0;
	last_matched_node = 0;
	in_buf[0] = '\0';
	in_idx = 0;
	error_on_line = 0;
	unique_match = 0;
	result = R_NONE;
	line_complete = 0;
	delim_reached = 0;
	string_litteral = 0;
	match_result = MR_NO_RESULT;
	last_error = PE_NO_ERROR;
	help_active = 0;
	map.Reset();
}


uint8_t Parser::Parse(char ch) {
	// char out[MAX_BUFFER_LENGTH];

	printf("DEBUG PARSE: Matching: \"%c\"\n", ch);
		// deal with noise
	if (ch == '\r') {    // ignore \r
		return R_IGNORE;
	}
	// handle EOL ***** doing stuff triggered from here
	if (ch == '\n') {
		if (help_active) {
			help_active = 0;
			map.Reset();
			return R_HELP;
		}
		if (error_on_line == 0) {
			line_complete = 1;  // not yet used
			if (match_result == MR_ACTION_POSSIBLE) {
				char action_ident[MAX_LABEL_LENGTH];
				map.GetAction(map.GetLastMatchedID(), action_ident);
				printf("DEBUG Action matched and to be called: %s\n", action_ident);
				map.Reset();
				return R_COMPLETE;
			} else {
				last_error = PE_LINE_INCOMPLETE;
				map.Reset();
				return R_ERROR;
			}
		} else {
			map.Reset();
			return R_ERROR;
		}
	}

	// handle string literals
	if (string_litteral == 1) { // already in one
		// XXX stuff into string literal param buffer as well as
		// 	fall through to main buffer
	}
	if (ch == '\"') {
		if (string_litteral == 0) {
			string_litteral = 1;
		} else {
			string_litteral = 0;
		}
	}

	// handle space and tab delimiters (and extras of them)
	if (ch == ' ' || ch == '\t') {
		if (in_idx == 0) { 			// leading space or tab
			// just ignore it
			return R_IGNORE;
		} else {
			if (string_litteral == 1) {
				// let it fall through into the buffer
			} else {
				if (delim_reached == 1) { // already had one - extra space or tabs between tokens
					return R_IGNORE;
				} else {
					if (error_on_line == 0) { // must be a token delim
						delim_reached = 1;
					} else {
						// already error on line - just fall through
						// for error line play back
					}
				}
			}
		}
	}
	// Handle "?"
	if (ch == '?') {
		help_active = 1;
	}

	// whatever it is now, throw it in the buffer and advance the terminator
	in_buf[in_idx] = ch;
	in_buf[in_idx + 1] = '\0';
	in_idx++;
	if (error_on_line == 0) {
		return ParseStage2();
	} else {
		// Error already flagged but need to discard buffered ch still coming before return
		// so just ignore until EOL. Next iteration will return Error or Complete accordingly
		// once EOL encountered
		printf("DEBUG: Discard: \"%c\"\n", ch);
		return R_IGNORE;
	}
}

uint8_t Parser::ParseStage2(void) {
	char out[MAX_BUFFER_LENGTH];

	possible_list->Reset();

	if (delim_reached && (match_result == MR_UNIQUE_KEYWORD)) {
		map.Advance(in_idx);
	}

	match_result = map.Match(in_buf, possible_list);

	switch (match_result) {
		case MR_NO_MATCH:
			printf("DEBUG PARSE: No possible match\n\n");
			error_on_line = 1;
			result = R_IGNORE; // still need to clean out incoming buffer
			break;
		case MR_UNIQUE_KEYWORD: {
			// single unique match
			unique_match = 1;
			char temp[MAX_LABEL_LENGTH];
			possible_list->GetCurrentLabel(temp);
			printf("DEBUG PARSE: Uniquely matched:\n\t%s\n\n", temp);
			result = R_UNFINISHED;
		}
			break;
		case MR_MULTI: {
			printf("DEBUG PARSE: Multiple matches - Queuing:\n");
			possible_list->ToTop();
			char temp[MAX_LABEL_LENGTH];
			do {
				possible_list->GetCurrentLabel(temp);
				sprintf(out, "\t%s\n", temp);
				printf("DEBUG: %s", out);
				output.EnQueue(out);
				possible_list->Next();
			} while (!possible_list->AtEnd());
			result = R_UNFINISHED;
		}
			break;
		case MR_DELIM_SKIP:
			result = R_UNFINISHED;
			break;
		case MR_ACTION_POSSIBLE:
			result = R_UNFINISHED;
			break;
		default:
			result = R_NONE;
			break;
	}

	output.SetOutputAvailable();
	return result;
}



void Parser::GetErrorString(char* error) {
	strcpy(error, parser_error_strings[last_error].text);

}

