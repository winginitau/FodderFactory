/*****************************************************************
 ITCH.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

//#include "config.h"
//#include "out.h"
#include "itch.h"
#include "parser_errors.h"
#include <stdlib.h>


#ifdef ARDUINO
#define D(x) Serial.write(x)
#endif


/******************************************************************************
 * Globals
 ******************************************************************************/

#ifdef DEBUG

//extern void M(char strn[]);

void M(char strn[]) {
	char str[MAX_OUTPUT_LINE_SIZE];
	strncpy(str, strn, MAX_OUTPUT_LINE_SIZE);
	#ifdef ARDUINO
		Serial.write(str);
	#else
		printf("%s", str);
	#endif
}

char i_debug_message[MAX_OUTPUT_LINE_SIZE];

#endif

// Lots of routines dealing with input and output buffers
// strings, param lists, tokens etc.... although ugly coding
// practice, for cleaner memory management on embedded systems
// with limited ram, better to have some preallocated variables
// that can be have their space measured at compile / link time
// and allocated at the start rather than dynamically creating them
// on the stack inside functions, potentially bumping into the heap.

char g_itch_replay_buff[MAX_INPUT_LINE_SIZE] = "\0";
char* g_itch_replay_buff_ptr = (char *)g_itch_replay_buff;

OutputBuffer g_itch_output_buff;		// General output buffer
char g_out_str[MAX_OUTPUT_LINE_SIZE];	// Strings being assembled for output
char g_temp_str[MAX_OUTPUT_LINE_SIZE];	// Generl string temp
ASTA g_temp_asta;						// Temp asta node (and for the Progmem working copy)

/******************************************************************************
 * Class Methods
 ******************************************************************************/

ITCH::ITCH() {
	iflags.mode = 0;
	iflags.replay = 0;
	#ifndef ARDUINO
	isp = NULL;
	osp= NULL;
	#endif
}

ITCH::~ITCH() {
	// Auto-generated destructor stub
}


#ifdef ARDUINO
void ITCH::Begin(int mode) {
	iflags.mode = mode;

	ParserInit();

	if (iflags.mode == ITCH_INTERACTIVE) {
		//sprintf(prompt_base, "\n\r$ ");
		strcpy_P(prompt_base, misc_strings[MISC_PROMPT_BASE].text);
		strcpy(prompt, prompt_base);
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
	//char output_line[MAX_OUTPUT_LINE_SIZE];

	char ch;

	uint8_t result = R_NONE;

	// if no replay editing is active then get the next ch from the input stream
	if (iflags.replay == 0) {
		#ifndef ARDUINO
			ch = getc(isp);
		#endif

		#ifdef ARDUINO
			if (Serial.available()) {
				ch = Serial.read();
				Serial.write(ch);
			} else {
				return;
			}
		#endif
	} else {
		// re-stuff the replay buffer into the stream
		ch = *g_itch_replay_buff_ptr;
		g_itch_replay_buff_ptr++;
		// If finished re-stuffing, reset the replay flag and buffer
		if (*g_itch_replay_buff_ptr == '\0') {
			iflags.replay = 0;
			g_itch_replay_buff_ptr = (char *)g_itch_replay_buff;
		}
	}

	if (ch != EOF) {

		result = Parse(ch);

		switch (result) {
			case R_HELP:
				#ifdef ARDUINO
				strcpy_P(g_out_str, misc_strings[MISC_HELP_HEADING].text);
				Serial.write(g_out_str);
				#else
				strcpy(g_out_str, misc_strings[MISC_HELP_HEADING].text);
				fputs(g_out_str, osp);
				#endif
				while (g_itch_output_buff.OutputAvailable()) {
					g_itch_output_buff.GetOutputAsString(g_out_str);
					strcat(g_out_str, "\r");

					#ifdef ARDUINO
					Serial.write(g_out_str);
					Serial.flush();
					#else
					fputs(g_out_str, osp);
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
				while (g_itch_output_buff.OutputAvailable()) {
					g_itch_output_buff.GetOutputAsString(g_out_str);
					strcat(g_out_str, "\r");
					#ifdef ARDUINO
					Serial.write(g_out_str);
					Serial.flush();
					#else
					fputs(g_out_str, osp);
					#endif
				}
				break; //continue
			case R_ERROR:
				#ifdef ARDUINO
				strcpy_P(g_out_str, misc_strings[MISC_ERROR_HEADER].text);
				Serial.write(g_out_str);
				#else
				strcpy(g_out_str, misc_strings[MISC_ERROR_HEADER].text);
				fputs(g_out_str, osp);
				#endif

				//Serial.write("Error caught back to ITCH R_ERROR - now after Error Header Write\n\r");
				//char temp[MAX_OUTPUT_LINE_SIZE];
				//sprintf(temp, "result: %d \n\r", result);
				//Serial.write(temp);

				ParserGetErrorString(g_out_str);
				strcat(g_out_str, misc_strings[MISC_ERROR_PROMPT].text);
				#ifdef ARDUINO
				Serial.write(g_out_str);
				#else
				fputs(g_out_str, osp);
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
				ParserGetErrorString(g_out_str);
				strcat_P(g_out_str, misc_strings[MISC_CRNL].text);
				#ifdef ARDUINO
				Serial.write(g_out_str);
				#else
				fputs(g_out_str, osp);
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
				strcpy(i_debug_message, "R_COMPLETE\n");
				M(i_debug_message);
				#endif
				while (g_itch_output_buff.OutputAvailable()) {
					g_itch_output_buff.GetOutputAsString(g_out_str);
					strcat(g_out_str, "\r");
					#ifdef ARDUINO
					Serial.write(g_out_str);
					Serial.flush();
					#else
					fputs(g_out_str, osp);
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
				while (g_itch_output_buff.OutputAvailable()) {
					g_itch_output_buff.GetOutputAsString(g_out_str);
					strcat(g_out_str, "\r");
					#ifdef ARDUINO
					Serial.write(prompt);
					Serial.flush();
					#else
					fputs(g_out_str, osp);
					#endif
				}
				ParserResetLine();
				#ifdef DEBUG
				sprintf(i_debug_message, "DEBUG (Poll) case R_REPLAY. replay_buf: %s\n\r", g_itch_replay_buff_ptr);
				M(i_debug_message);
				#endif
				// Set the replay flag on
				iflags.replay = 1;
				//ParserBufferInject(parser_replay_buff);
				// Write out the prompt and the replay buffer to indicate to the user
				// recall success
				#ifdef ARDUINO
				Serial.write(prompt);
				Serial.write(g_itch_replay_buff);
				Serial.flush();
				#else
				fputs(prompt, osp);
				fputs(g_itch_replay_buff, osp);
				#endif
				break;
		}
	}
}

void ITCH::WriteLine(char* string) {
	g_itch_output_buff.AddString(string);
	g_itch_output_buff.SetOutputAvailable();
}

void ITCH::WriteImmediate(char* string) {
	#ifdef ARDUINO
		Serial.write(string);
		Serial.flush();
	#endif
}







