/*****************************************************************
 ICLI.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#include "ICLI.h"

#include "processor_out.h"
#include <stdlib.h>

ICLI::ICLI() {
	mode = 0;
	isp = NULL;
	osp= NULL;
}

ICLI::~ICLI() {
	// Auto-generated destructor stub
}

void ICLI::Begin(FILE* input_stream, FILE* output_stream, int icli_mode) {
	isp = input_stream;
	osp = output_stream;
	mode = icli_mode;
}


void ICLI::Poll(void) {

	// XXX this needs to become non-blocking portable
	// for arduino Serial.available should work but more
	// complex on linux.
	// Consider using a multi-thread or signal model on linux

	char ch;

	uint8_t result = R_NONE;

	char output_line[MAX_BUFFER_LENGTH];

	sprintf(prompt, "\n$ ");
	if (mode == ICLI_INTERACTIVE) {
		fputs(prompt, osp);
	};

	ch = fgetc(isp);

	while (ch != EOF) {

			result = parser.Parse(ch);

			switch (result) {
				case R_HELP:
					sprintf(output_line, "\nCommand Help:\n");
					fputs(output_line, osp);
					while (parser.output.OutputAvailable()) {
						parser.output.GetOutputAsString(output_line);
						fputs(output_line, osp);
					}
					parser.ResetLine();
					fputs(prompt, osp);
					// logic to repost up to
					// but not including the ? that triggered help
					// parser.RePost????();
					//
					// re command prompts:
					// basic 	$ _
					// auth		host$
					// enable	host#
					// context	host(context)#
					// context2 host(context/context2)#
					break;
				case R_IGNORE:
					// parser discarding something irrelevant (eg extra whitespace)
					// 	or the rest of the input buffer after error detected
					while (parser.output.OutputAvailable()) {
						parser.output.GetOutputAsString(output_line);
						fputs(output_line, osp);
					}
					break; //continue
				case R_ERROR:
					sprintf(output_line, "\n>>> Error:\n>>> ");
					fputs(output_line, osp);
					parser.GetErrorString(output_line);
					strcat(output_line, "\nTry \"?\" or <command> ? for help.\n");
					fputs(output_line, osp);
					fputs(prompt, osp);
					parser.ResetLine();
					break;
				case R_NONE:
					// parser should always return a meaningful result
					sprintf(output_line, ">>> Error: Parser did not return a result at all. Error in Parser?\n>> ");
					fputs(output_line, osp);
					exit(-1);
					break; //continue
				case R_UNFINISHED:
//					while (parser.output.OutputAvailable()) {
//						parser.output.GetOutputAsString(output_line);
//						fputs(output_line, osp);
//					}
					break; //continue
				case R_COMPLETE: {
					while (parser.output.OutputAvailable()) {
						parser.output.GetOutputAsString(output_line);
						fputs(output_line, osp);
					}
					// XXX parser.get / resolve action to call
					// call the action
					// stream the result
					while (parser.output.OutputAvailable()) {
						parser.output.GetOutputAsString(output_line);
						fputs(output_line, osp);
					}
					parser.ResetLine();
					fputs(prompt, osp);
					break;
				}

		}
		ch = fgetc(isp);
	}
}


			//
			// get first token

			// Get first char

			// Call the parser with first char


				// parser has array
				// parser has "level" - token cardinal (from context cardinal base)
				// parser knows how far down the match chain it is

				// parser grabs all sibling nodes of the matched parent

				// compares the char to the first char of the siblings
				// filters out non-matches, reducing the set
					// keywords - straight compare match
					// identifiers "remain open" but for performance are not iterated
					// lookups remain open
					// user entry params remain open

			// at end of token chars (either by \0 or \n)
				// set reduce based on
					// match to key words
					// iterate idents
					// look up external lists
					// if possible in parse path then fall through of user param
			// if last char (on token or line) is ?
				// return all possible sibling matches fir display
				// display raw buffer entered so far as prompt

			// continue in loop until action is reached with \n
				// node number with action - how best to relate function call
				//	and parent param list? - Build simple map reduced dynamic tree / parent
				//	structure in memory - just the pointers and the ref to node#
				// 	- pull node detail from the array.








