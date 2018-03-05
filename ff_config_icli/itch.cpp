/*****************************************************************
 ITCH.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#include "out.h"
#include <stdlib.h>
#include "itch.h"



ITCH::ITCH() {
	mode = 0;
	isp = NULL;
	osp= NULL;
}

ITCH::~ITCH() {
	// Auto-generated destructor stub
}

void ITCH::Begin(FILE* input_stream, FILE* output_stream, int icli_mode) {
	isp = input_stream;
	osp = output_stream;
	mode = icli_mode;
}


void ITCH::Poll(void) {
	char temp[MAX_BUFFER_LENGTH];

	// XXX this needs to become non-blocking portable
	// for arduino Serial.available should work but more
	// complex on linux.
	// Consider using a multi-thread or signal model on linux

	char ch;

	uint8_t result = R_NONE;

	char output_line[MAX_BUFFER_LENGTH];

	sprintf(prompt_base, "\n\r$ ");
	if (mode == ITCH_INTERACTIVE) {
		fputs(prompt_base, osp);
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
						strcat(output_line, "\r");
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
						strcat(output_line, "\r");
						fputs(output_line, osp);
					}
					break; //continue
				case R_ERROR:
					sprintf(output_line, "\n\r>>> Error:\n\r>>> ");
					fputs(output_line, osp);
					parser.GetErrorString(output_line);
					strcat(output_line, "\n\rTry \"?\" or <command> ? for help.\n\r");
					fputs(output_line, osp);
					fputs(prompt, osp);
					parser.ResetLine();
					break;
				case R_NONE:
					// parser should always return a meaningful result
					sprintf(output_line, ">>> ERROR: Parser did not return a result at all. Error in Parser?\n\r>> ");
					fputs(output_line, osp);
					fputs(prompt, osp);
					parser.ResetLine();
					//exit(-1);
					break; //continue
				case R_UNFINISHED:
					break; //continue
				case R_COMPLETE: {
					while (parser.output.OutputAvailable()) {
						parser.output.GetOutputAsString(output_line);
						strcat(output_line, "\r");
						fputs(output_line, osp);
					}
					// XXX parser.get / resolve action to call
					// call the action
					// stream the result
					while (parser.output.OutputAvailable()) {
						parser.output.GetOutputAsString(output_line);
						strcat(output_line, "\r");
						fputs(output_line, osp);
					}
					parser.ResetLine();
					fputs(prompt_base, osp);
					break;
				}
				case R_REPLAY:
					while (parser.output.OutputAvailable()) {
						parser.output.GetOutputAsString(output_line);
						strcat(output_line, "\r");
						fputs(output_line, osp);
					}

					parser.ResetLine();
					strcpy(temp, parser.replay_buf);
					printf("DEBUG ICLI: case R_REPLAY. replay_buf: %s\n\r", temp);
					parser.BufferInject(temp);
					// XXX unfinshied as to how to restuff the buffer
					// without using a buffer!
					strcpy(prompt, prompt_base);
					strcat(prompt, parser.replay_buf);
					fputs(prompt, osp);
					break;

		}
		ch = fgetc(isp);
		//ch = getchar();
	}
}

void ITCH::WriteLine(char* string) {
	parser.output.AddString(string);
	parser.output.SetOutputAvailable();
}








