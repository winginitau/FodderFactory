/*****************************************************************
 ITCH.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#include "out.h"
#include "itch.h"
#include "parser_errors.h"
#include <stdlib.h>


#ifdef ARDUINO
#define D(x) Serial.write(x)
#endif

#ifdef ARDUINO
#ifdef DEBUG
extern void M(char strn[]);
char i_debug_message[MAX_OUTPUT_LINE_SIZE];
#endif
#endif


ITCH::ITCH() {
	mode = 0;
	isp = NULL;
	osp= NULL;
}

ITCH::~ITCH() {
	// Auto-generated destructor stub
}


#ifdef ARDUINO
void ITCH::Begin(int icli_mode) {
	mode = icli_mode;

	if (mode == ITCH_INTERACTIVE) {
		sprintf(prompt_base, "\n\r$ ");
		delay(100);
		Serial.write(prompt_base);
	};

}

#else

void ITCH::Begin(FILE* input_stream, FILE* output_stream, int icli_mode) {
	isp = input_stream;
	osp = output_stream;
	mode = icli_mode;
}

#endif


void ITCH::Poll(void) {
	char temp[MAX_OUTPUT_LINE_SIZE];

	char ch;
	uint8_t result = R_NONE;
	char output_line[MAX_OUTPUT_LINE_SIZE];

#ifndef ARDUINO
	sprintf(prompt_base, "\n\r$ ");
	if (mode == ITCH_INTERACTIVE) {
		fputs(prompt_base, osp);
	};
	ch = fgetc(isp);
#endif

#ifdef ARDUINO
	if (Serial.available()) {
		//delay(100);
		ch = Serial.read();
		//delay(100);
		Serial.write(ch);
	} else {
		return;
	}
#endif

#ifdef ARDUINO
	//delay(1000);
	//D("Got ch, about to check EOF\n\r");
#endif

	if (ch != EOF) {

#ifdef DEBUG
		strcpy(i_debug_message, "Pre Parse");
		M(i_debug_message);
#endif

		result = parser.Parse(ch);

#ifdef DEBUG
		sprintf(i_debug_message, "Parse Result: %d\t", result);
		M(i_debug_message);
#endif

		switch (result) {
			case R_HELP:
				sprintf(output_line, "\nCommand Help:\n");
				fputs(output_line, osp);
				while (parser.output.OutputAvailable()) {
					parser.output.GetOutputAsString(output_line);
					strcat(output_line, "\r");

#ifdef ARDUINO
					Serial.write(output_line);
#else
					fputs(output_line, osp);
#endif
				}
				parser.ResetLine();
#ifdef ARDUINO
				Serial.write(prompt);
#else
				fputs(prompt, osp);
#endif
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

#ifdef ARDUINO
					Serial.write(output_line);
#else
					fputs(output_line, osp);
#endif
				}
				break; //continue
			case R_ERROR:
				sprintf(output_line, "\n\r>>> Error:\n\r>>> ");
				fputs(output_line, osp);
				parser.GetErrorString(output_line);
				strcat(output_line, "\n\rTry \"?\" or <command> ? for help.\n\r");

#ifdef ARDUINO
				Serial.write(output_line);
#else
				fputs(output_line, osp);
#endif

#ifdef ARDUINO
				Serial.write(prompt);
#else
				fputs(prompt, osp);
#endif

				parser.ResetLine();
				break;
			case R_NONE:
				// parser should always return a meaningful result
				parser.SetError(PE_NO_PARSE_RESULT);
				parser.GetErrorString(output_line);
				strcat(output_line, "\n\r");

#ifdef ARDUINO
				Serial.write(output_line);
#else
				fputs(output_line, osp);
#endif

#ifdef ARDUINO
				Serial.write(prompt);
#else
				fputs(prompt, osp);
#endif

				parser.ResetLine();
				//exit(-1);
				break; //continue
			case R_UNFINISHED:
				break; //continue
			case R_COMPLETE: {

				strcpy(i_debug_message, "R_COMPLETE Case");
				M(i_debug_message);

				while (parser.output.OutputAvailable()) {
					parser.output.GetOutputAsString(output_line);
					strcat(output_line, "\r");

#ifdef ARDUINO
					Serial.write(output_line);
#else
					fputs(output_line, osp);
#endif

				}
				while (parser.output.OutputAvailable()) {
					parser.output.GetOutputAsString(output_line);
					strcat(output_line, "\r");

#ifdef ARDUINO
					Serial.write(output_line);
#else
					fputs(output_line, osp);
#endif

				}
				parser.ResetLine();

#ifdef ARDUINO
				Serial.write(prompt);
#else
				fputs(prompt, osp);
#endif

				break;
			}
			case R_REPLAY:
				while (parser.output.OutputAvailable()) {
					parser.output.GetOutputAsString(output_line);
					strcat(output_line, "\r");

#ifdef ARDUINO
					Serial.write(prompt);
#else
					fputs(output_line, osp);
#endif

				}

				parser.ResetLine();
				strcpy(temp, parser.replay_buf);
#ifdef DEBUG
				sprintf(i_debug_message, "DEBUG ICLI: case R_REPLAY. replay_buf: %s\n\r", temp);
				M(i_debug_message);
#endif
				parser.BufferInject(temp);
				// XXX unfinshied as to how to restuff the buffer
				// without using a buffer!
				strcpy(prompt, prompt_base);
				strcat(prompt, parser.replay_buf);

#ifdef ARDUINO
				Serial.write(prompt);
#else
				fputs(prompt, osp);
#endif

				break;

		}
		//ch = fgetc(isp);
		//ch = getchar();
	}
}

void ITCH::WriteLine(char* string) {
	parser.output.AddString(string);
	parser.output.SetOutputAvailable();
}








