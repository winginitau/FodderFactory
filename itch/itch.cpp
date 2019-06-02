/*****************************************************************
 ITCH.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

//#include "config.h"
//#include "out.h"
#include "itch_strings.h"
#include "itch.h"
#include "itch_hal.h"
#include <stdlib.h>

#ifdef ARDUINO
#define D(x) Serial.write(x)
#endif


/******************************************************************************
 * Globals
 ******************************************************************************/

#ifdef ITCH_DEBUG

void M(char strn[]) {
//void M(char *strn) {
	char str[MAX_OUTPUT_LINE_SIZE];
	strncpy(str, strn, MAX_OUTPUT_LINE_SIZE);
	#ifdef ARDUINO
		Serial.write(str);
	#else
		printf("%s", str);
	#endif
}

char g_debug_message[MAX_OUTPUT_LINE_SIZE];

#endif //ITCH_DEBUG

// Lots of routines dealing with input and output buffers
// strings, param lists, tokens etc.... although ugly coding
// practice, for cleaner memory management on embedded systems
// with limited ram, better to have some preallocated variables
// that can have their space measured at compile / link time
// and allocated at the start rather than dynamically creating them
// on the stack inside functions, potentially bumping into the heap.

// 2018-05-23 Changed my mind. They're taking up too much space.
// Mostly pruned, except a debug message and some parser globals
// that were originally members of a class (before reverting to)
// mostly ANSI C and the oo code structure still thinks of them as
// a class variable - which needs to be a global without oo constructs.

char g_itch_replay_buff[MAX_INPUT_LINE_SIZE] = "\0";
char *g_itch_replay_buff_ptr = (char *)g_itch_replay_buff;

OutputBuffer g_itch_output_buff;		// General output buffer
//char g_out_str[MAX_OUTPUT_LINE_SIZE];	// Strings being assembled for output
//char g_temp_str[MAX_OUTPUT_LINE_SIZE];	// General string temp
//ASTA g_temp_asta;						// Temp asta node (and for the Progmem working copy)

#ifndef ARDUINO
FILE* isp;	// input stream pointer
FILE* osp;	// output stream pointer
#endif

/******************************************************************************
 * Class Methods
 ******************************************************************************/

ITCH::ITCH() {
	iflags.mode = ITCH_INIT;
	iflags.replay = 0;
	iflags.esc_idx = 0;
	strcpy(iflags.esc_seq, ITCH_ESCAPE_SEQUENCE);

	#ifndef ARDUINO
	isp = NULL;
	osp= NULL;
	#endif
}

ITCH::~ITCH() {
	// Auto-generated destructor stub
}


#ifdef ARDUINO
void ITCH::Begin() {
	iflags.mode = ITCH_TEXT_DATA;
	ParserInit();
}

#else

void ITCH::Begin(FILE* input_stream, FILE* output_stream) {
	isp = input_stream;
	osp = output_stream;
	iflags.mode = ITCH_TEXT_DATA;
	ParserInit();
}
#endif //else ARDUINO

void ITCH::SetMode(uint8_t mode) {
	char out_str[MAX_OUTPUT_LINE_SIZE];
	iflags.mode = mode;
	switch (mode) {
		case ITCH_TEXT_DATA:
			break;
		case ITCH_TERMINAL:
			// ITCH Welcome
			strcpy_itch_hal(out_str, misc_itch_strings[ITCH_TERMINAL_WELCOME].text);
			WriteDirect(out_str);
			// itch prompt
			strcpy_itch_hal(prompt, misc_itch_strings[ITCH_MISC_PROMPT_BASE].text);
			WriteDirect(prompt);
			ParserResetLine();
			break;
		default:
			break;
			// ERROR
	}
}

void ITCH::Poll(void) {
	char out_str[MAX_OUTPUT_LINE_SIZE];	// Strings being assembled for output
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
			// XXX deal with terminal modes - local echo?
			// Moving "write" to be conditional on mode
			//Serial.write(ch);
		} else return;
		#endif

	} else { //iflags.relay != 0
		// re-stuff the replay buffer into the stream
		ch = *g_itch_replay_buff_ptr;
		g_itch_replay_buff_ptr++;
		// If finished re-stuffing, reset the replay flag and buffer
		if (*g_itch_replay_buff_ptr == '\0') {
			iflags.replay = 0;
			g_itch_replay_buff_ptr = (char *)g_itch_replay_buff;
		}
	}

	// Process the ch
	if (ch != EOF) {

		// In non-terminal modes ? - Test for escape sequence
		if (iflags.mode < ITCH_TERMINAL) {

			// Test if ch is next in escape sequence
			if (ch == iflags.esc_seq[iflags.esc_idx]) {
				// matched - advance matching to next
				iflags.esc_idx++;
			} else {
				// sequence broken, reset matching
				iflags.esc_idx = 0;
			}

			// Fully matched?
			if (iflags.esc_idx == ITCH_ESCAPE_SEQUENCE_SIZE) {
				// Enter terminal mode
				SetMode(ITCH_TERMINAL);
				iflags.esc_idx = 0;
				return;
			}
			// not in terminal mode - ignore input
			//ParserResetLine();
			return;
		}

		result = Parse(ch);

		switch (result) {
			case R_HELP:
				strcpy_itch_misc(out_str, ITCH_MISC_HELP_HEADING);
				WriteLineDirect(out_str);
				while (g_itch_output_buff.OutputAvailable()) {
					g_itch_output_buff.GetOutputAsString(out_str);
					WriteLineDirect(out_str);
				}
				ParserResetLine();
				WriteDirect(prompt);
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
					g_itch_output_buff.GetOutputAsString(out_str);
					WriteLineDirect(out_str);
				}
				break; //continue
			case R_ERROR:
				strcpy_itch_misc(out_str, ITCH_MISC_ERROR_HEADER);
				WriteDirect(out_str);
				ParserGetErrorString(out_str);
				WriteLineDirect(out_str);
				strcpy_itch_misc(out_str, ITCH_MISC_ERROR_PROMPT);
				WriteLineDirect(out_str);
				WriteDirect(prompt);
				ParserResetLine();
				break;
			case R_NONE:
				// parser should always return a meaningful result - report error
				ParserSetError(PE_NO_PARSE_RESULT);
				ParserGetErrorString(out_str);
				strcat_itch_misc(out_str, ITCH_MISC_CRNL);
				WriteLineDirect(out_str);
				WriteDirect(prompt);
				ParserResetLine();
				break; //continue
			case R_UNFINISHED:
				//char consumed, ready for next, nothing to report
				break;
			case R_COMPLETE: {
				#ifdef ITCH_DEBUG
					strcpy_itch_debug(g_debug_message, ITCH_DEBUG_R_COMPLETE);
					M(g_debug_message);
				#endif
				while (g_itch_output_buff.OutputAvailable()) {
					g_itch_output_buff.GetOutputAsString(out_str);
					WriteLineDirect(out_str);
				}
				ParserResetLine();
				WriteDirect(prompt);
				break;
			}
			case R_REPLAY:
				while (g_itch_output_buff.OutputAvailable()) {
					g_itch_output_buff.GetOutputAsString(out_str);
					//strcat(out_str, "\r");
					WriteLineDirect(out_str);
				}
				ParserResetLine();
				#ifdef ITCH_DEBUG
					char temp_str[MAX_OUTPUT_LINE_SIZE];
					strcpy_itch_debug(temp_str, ITCH_DEBUG_POLL_CASE_R_REPLAY);
					sprintf(g_debug_message, "%s%s\n", temp_str, g_itch_replay_buff_ptr);
					M(g_debug_message);
				#endif
				// Set the replay flag on
				iflags.replay = 1;
				//ParserBufferInject(parser_replay_buff);
				// Write out the prompt and the replay buffer to indicate to the user
				// recall success
				WriteDirect(prompt);
				WriteDirect(g_itch_replay_buff);
				break;
		}
	}
}

void ITCH::WriteLine(char* string) {
	if(string != NULL) {
		g_itch_output_buff.AddString(string);
		g_itch_output_buff.SetOutputAvailable();
	}
}

void ITCH::WriteLineDirect(char* string) {
	strcat(string, "\n");
	#ifdef ARDUINO
		Serial.flush();
		//delay(5);
		Serial.write(string);
		//delay(5);
		Serial.flush();
	#else
		fputs(string, osp);
	#endif
}

void ITCH::WriteDirect(char* string) {
	#ifdef ARDUINO
		Serial.flush();
		//delay(5);
		Serial.write(string);
		//delay(5);
		Serial.flush();
		//delay(5);
	#else
		fputs(string, osp);
	#endif
}



/*************************************************************************
 * Example Terminal Program for picking over
 */

/*
typedef struct command {
	char	cmd[10];
	void	(*func)	(void);
};

command commands[] = {
	{"rd",		Cmd_RD},
	{"rdw",		Cmd_RDW},
	{"wr",		Cmd_WR},
	{"l",		Cmd_L},
	{"p",		Cmd_P},
	{"regs",	Cmd_REGS},
	{"io",		Cmd_IO},
	{"f",		Cmd_F},
	{"sp",		Cmd_SP},
	{"per",		Cmd_PER},
	{"watch",	Cmd_WATCH},
	{"rst",		Cmd_RST},
	{"ipc",		Cmd_IPC},
	{"set",		Cmd_SET},
	{"dig",		Cmd_DIG},
	{"an",		Cmd_AN},
	{"pin",		Cmd_PIN},
	{"pc",		Cmd_PC},
	{"info",	Cmd_INFO},
	{"/",		Cmd_HELP}
};
...
void	Cmd_WR () {  // example of one of the commands
	/////////////////////////////////////////////////////////
	// write a value to target RAM
	int addr = GetAddrParm(0);
	byte val = GetValParm(1, HEX);
	if (SetTargetByte (addr, val) != val)
		Serial << "Write did not verify" << endl;
}
...
void	loop() {
	char c;

	if (Serial.available() > 0) {
		c = Serial.read();
		if (ESC == c) {
			while (Serial.available() < 2) {};
			c = Serial.read();
			c = Serial.read();
			switch (c) {
				case 'A':  // up arrow
					// copy the last command into the command buffer
					// then echo it to the terminal and set the
					// the buffer's index pointer to the end
					memcpy(cmd_buffer, last_cmd, sizeof(last_cmd));
					cmd_buffer_index = strlen (cmd_buffer);
					Serial << cmd_buffer;
					break;
			}
		} else {
			c = tolower(c);
			switch (c) {

				case TAB:   // retrieve and execute last command
					memcpy(cmd_buffer, last_cmd, sizeof(cmd_buffer));
					ProcessCommand ();
					break;

				case BACKSPACE:  // delete last char
					if (cmd_buffer_index > 0) {
						cmd_buffer[--cmd_buffer_index] = NULLCHAR;
						Serial << _BYTE(BACKSPACE) << SPACE << _BYTE(BACKSPACE);
					}
					break;

				case LF:
					ProcessCommand ();
					Serial.read();		// remove any following CR
					break;

				case CR:
					ProcessCommand ();
					Serial.read();		// remove any following LF
					break;

				default:
					cmd_buffer[cmd_buffer_index++] = c;
					cmd_buffer[cmd_buffer_index] = NULLCHAR;
					Serial.print (c);
			}
		}
	}
}

void	ProcessCommand () {
	int cmd, per;
	peripheral p;

	/////////////////////////////////////////////////////
	// any command will kill the watching feature
	watching = false;

	Serial.println("");

	/////////////////////////////////////////////////////
	// trap just a CRLF
	if (cmd_buffer[0] == NULLCHAR) {
		Serial.print("AVR> ");
		return;
	}

	/////////////////////////////////////////////////////
	// save this command for later use with TAB or UP arrow
	memcpy(last_cmd, cmd_buffer, sizeof(last_cmd));

	/////////////////////////////////////////////////////
	// Chop the command line into substrings by
	// replacing ' ' with '\0'
	// Also adds pointers to the substrings
	SplitCommand();

	/////////////////////////////////////////////////////
	// Scan the command table looking for a match
	for (cmd = 0; cmd < N_COMMANDS; cmd++) {
		if (strcmp (commands[cmd].cmd, (char *)cmd_buffer) == 0) {
			commands[cmd].func();
			goto done;
		}
	}

	/////////////////////////////////////////////////////
	// If we didn't find a valid command scan the peripherals
	// table looking for a match with the entered command
	for (per = 0; per < N_PERIPHERALS; per++) {
		memcpy_P(&p, &peripherals[per], sizeof (p));
		if (strcmp (UCASE_STR(cmd_buffer), p.name) == 0) {
			// we found a match, point the first parm pointer
			// to the command and call Cmd_PER to do the work
			// Cmd_PER will use parms[0] to find the peripheral
			// to dump
			parms[0] = cmd_buffer;
			n_parms = 1;
			Cmd_PER();
			goto done;
		}
	}

	/////////////////////////////////////////////////////
	// if we get here no valid command was found
	Serial << "wtf?" << endl;

	done:
	cmd_buffer_index = 0;
	cmd_buffer[0] = NULLCHAR;
	Serial << "AVR> ";
}
*/


