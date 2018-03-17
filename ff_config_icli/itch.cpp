/*****************************************************************
 ITCH.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#include "out.h"
#include "itch.h"
#include "parser_errors.h"
#include "parser_config.h"
#include <stdlib.h>


#ifdef ARDUINO
#define D(x) Serial.write(x)
#endif



/******************************************************************************
 * Globals
 ******************************************************************************/

#ifdef DEBUG
	extern void M(char strn[]);
	char i_debug_message[MAX_OUTPUT_LINE_SIZE];
#endif

OutputBuffer global_output;
char parser_replay_buff[MAX_INPUT_LINE_SIZE] = "\0";
char* parser_replay_buff_ptr = (char *)parser_replay_buff;


/******************************************************************************
 * Class Methods
 ******************************************************************************/

ITCH::ITCH() {
	iflags.mode = 0;
	iflags.replay = 0;
	isp = NULL;
	osp= NULL;
}

ITCH::~ITCH() {
	// Auto-generated destructor stub
}


#ifdef ARDUINO
void ITCH::Begin(int mode) {
	itch_mode = mode;

	if (itch_mode == ITCH_INTERACTIVE) {
		sprintf(prompt_base, "\n\r$ ");
		delay(100);
		Serial.write(prompt_base);
	};

}

#else

void ITCH::Begin(FILE* input_stream, FILE* output_stream, int mode) {
	isp = input_stream;
	osp = output_stream;
	iflags.mode = mode;
	ParserInit();

	if (iflags.mode == ITCH_INTERACTIVE) {
		sprintf(prompt_base, "\n\r$ ");
		fputs(prompt_base, osp);
	};
}

#endif


void ITCH::Poll(void) {
	char output_line[MAX_OUTPUT_LINE_SIZE];

	char ch;

	uint8_t result = R_NONE;

	// if no replay editing is active then get the next ch from the input stream
	if (iflags.replay == 0) {
		#ifndef ARDUINO
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
	} else {
		// re-stuff the replay buffer into the stream
		ch = *parser_replay_buff_ptr;
		parser_replay_buff_ptr++;
		// If finished re-stuffing, reset the replay flag and buffer
		if (*parser_replay_buff_ptr == '\0') {
			iflags.replay = 0;
			parser_replay_buff_ptr = (char *)parser_replay_buff;
		}
	}



	#ifdef ARDUINO
	//delay(1000);
	//D("Got ch, about to check EOF\n\r");
	#endif

	if (ch != EOF) {

		result = Parse(ch);

		switch (result) {
			case R_HELP:
				strcpy(output_line, misc_strings[MISC_HELP_HEADING].text);
				fputs(output_line, osp);
				while (global_output.OutputAvailable()) {
					global_output.GetOutputAsString(output_line);
					strcat(output_line, "\r");

					#ifdef ARDUINO
					Serial.write(output_line);
					#else
					fputs(output_line, osp);
					#endif
				}
				ParserResetLine();
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
				while (global_output.OutputAvailable()) {
					global_output.GetOutputAsString(output_line);
					strcat(output_line, "\r");
					#ifdef ARDUINO
					Serial.write(output_line);
					#else
					fputs(output_line, osp);
					#endif
				}
				break; //continue
			case R_ERROR:
				strcpy(output_line, misc_strings[MISC_ERROR_HEADER].text);
				fputs(output_line, osp);
				ParserGetErrorString(output_line);
				strcat(output_line, misc_strings[MISC_ERROR_PROMPT].text);
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
				ParserResetLine();
				break;
			case R_NONE:
				// parser should always return a meaningful result
				ParserSetError(PE_NO_PARSE_RESULT);
				ParserGetErrorString(output_line);
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
				ParserResetLine();
				break; //continue
			case R_UNFINISHED:
				break; //continue
			case R_COMPLETE: {
				#ifdef DEBUG
				strcpy(i_debug_message, "R_COMPLETE Case");
				M(i_debug_message);
				#endif
				while (global_output.OutputAvailable()) {
					global_output.GetOutputAsString(output_line);
					strcat(output_line, "\r");
					#ifdef ARDUINO
					Serial.write(output_line);
					#else
					fputs(output_line, osp);
					#endif
				}
				ParserResetLine();
				#ifdef ARDUINO
				Serial.write(prompt);
				#else
				fputs(prompt, osp);
				#endif
				break;
			}
			case R_REPLAY:
				while (global_output.OutputAvailable()) {
					global_output.GetOutputAsString(output_line);
					strcat(output_line, "\r");
					#ifdef ARDUINO
					Serial.write(prompt);
					#else
					fputs(output_line, osp);
					#endif
				}
				ParserResetLine();
				#ifdef DEBUG
				sprintf(i_debug_message, "DEBUG (Poll) case R_REPLAY. replay_buf: %s\n\r", parser_replay_buff_ptr);
				M(i_debug_message);
				#endif
				// Set the replay flag on
				iflags.replay = 1;
				//ParserBufferInject(parser_replay_buff);
				// Write out the prompt and the replay buffer to indicate to the user
				// recall success
				#ifdef ARDUINO
				Serial.write(prompt);
				Serial.write(parser_replay_buff);
				#else
				fputs(prompt, osp);
				fputs(parser_replay_buff, osp);
				#endif
				break;
		}
	}
}

void ITCH::WriteLine(char* string) {
	global_output.AddString(string);
	global_output.SetOutputAvailable();
}








