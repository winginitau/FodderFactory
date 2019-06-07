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

#ifdef FF_SIMULATOR
#include <unistd.h>
#include <termios.h>
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

char g_itch_replay_buf[MAX_INPUT_LINE_SIZE] = "\0";
char g_itch_restuff_buf[MAX_INPUT_LINE_SIZE] = "\0";
char *g_itch_restuff_buf_ptr = (char *)g_itch_restuff_buf;

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
	iflags.echo_received = 0;
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

	struct termios old_tio, new_tio;
	/* get the terminal settings for stdin */
	tcgetattr(STDIN_FILENO,&old_tio);
	/* we want to keep the old setting to restore them a the end */
	new_tio=old_tio;
	/* disable canonical mode (buffered i/o) and local echo */
	new_tio.c_lflag &=(~ICANON & ~ECHO);
	/* set the new settings immediately */
	tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

	/* restore the former settings */
	//tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);

	isp = input_stream;
	osp = output_stream;
	iflags.mode = ITCH_TEXT_DATA;
	ParserInit();
}
#endif //else ARDUINO

void ITCH::SetMode(uint8_t new_mode) {
	//char out_str[MAX_OUTPUT_LINE_SIZE];
	switch (new_mode) {
		case ITCH_TEXT_DATA:
			if (iflags.mode == ITCH_TERMINAL) {
				strcpy_itch_hal(out_str, misc_itch_strings[ITCH_TERMINAL_GOODBYE].text);
				WriteDirectCh('\n');
				WriteLineDirect(out_str);
			}
			// clean up
			ParserResetLine();
			iflags.mode = new_mode;
			break;
		case ITCH_TERMINAL:
			// ITCH Welcome
			WriteDirectCh('\n');
			strcpy_itch_hal(out_str, misc_itch_strings[ITCH_TERMINAL_WELCOME].text);
			WriteLineDirect(out_str);
			// itch prompt
			strcpy_itch_hal(prompt, misc_itch_strings[ITCH_MISC_PROMPT_BASE].text);
			WriteDirect(prompt);
			// set terminal echo-back to on
			iflags.echo_received = 1;
			ParserResetLine();
			iflags.mode = new_mode;
			break;
		case ITCH_BUFFER_STUFF:
			// Processing commands being stuffed into the stuff buffer
			WriteDirectCh('\n');
			strcpy_itch_hal(out_str, misc_itch_strings[ITCH_BUFFER_STUFFING_MODE].text);
			WriteLineDirect(out_str);
			// set terminal echo-back to off
			iflags.echo_received = 0;
			// Reset the parser - we may have got here from a parser triggered command.
			ParserResetLine();
			iflags.mode = new_mode;
			break;
		default:
			break;
			// ERROR
	}
}

void ITCH::WriteOutputBuffer(void) {
	// Iterate the output list and write contents till empty
	while (g_itch_output_buff.OutputAvailable()) {
		g_itch_output_buff.GetOutputAsString(out_str);
		WriteLineDirect(out_str);
	}
}

void ITCH::PreserveReplay(void) {
	// Preserve the replay buffer into the restuff buffer
	// so the replay buffer can reset and cleanly capture the restuffing too!
	strcpy(g_itch_restuff_buf, g_itch_replay_buf);
	g_itch_replay_buf[0] = '\0';		// redundant? handled in ParserReset()
	g_itch_restuff_buf_ptr = (char *)g_itch_restuff_buf;
}

void ITCH::TrimReStuffBuffer(void) {
	// Trim the restuff buffer of the last char
	// to support BackSpace and trimming of "?" after help requested
	uint8_t i = 0;
	while(g_itch_restuff_buf[i] != '\0') i++;
	if (i > 0) g_itch_restuff_buf[i-1] = '\0';
}

uint8_t ITCH::StuffAndProcess(char* str) {
	ParserResetLine();
	//SetMode(ITCH_BUFFER_STUFF);
	strcpy(g_itch_restuff_buf, str);
	iflags.replay = 1;
	while (iflags.replay == 1) {
		Poll();
	}
	return 1;
}

void ITCH::Poll(void) {
	char ch;							// the object of attention
	uint8_t parse_result = R_NONE;		// captures the ch by char parsing result

	// if no replay editing is active then get the next ch from the input stream
	if (iflags.replay == 0) {
		#ifndef ARDUINO
		ch = getc(isp);					// XXX convert to non-blocking for linux
		#endif
		#ifdef ARDUINO
		if (Serial.available()) {
			ch = Serial.read();
		} else return;					// nothing available, return
		#endif
	} else { 							// replay is active - iflags.replay != 0
		// re-stuff the restuff buffer into the stream (having already reset the replay buf)
		ch = *g_itch_restuff_buf_ptr;
		g_itch_restuff_buf_ptr++;
		// If finished re-stuffing, turn off  the replay flag and buffer
		if (*g_itch_restuff_buf_ptr == '\0') {
			iflags.replay = 0;
			g_itch_restuff_buf_ptr = (char *)g_itch_restuff_buf;
		}
	}
	// Process the ch - user entered or re-stuffed
	if (ch != EOF) {
		// In non-terminal mode? - Test for escape sequence only
		if (iflags.mode < ITCH_TERMINAL) {
			// Test if ch is the first/next char the "enter terminal mode" escape sequence
			if (ch == iflags.esc_seq[iflags.esc_idx]) {
				// matched - advance matching to next
				iflags.esc_idx++;
			} else {
				// sequence broken, reset matching
				iflags.esc_idx = 0;
			}

			// Terminal mode escape sequence fully matched?
			if (iflags.esc_idx == ITCH_ESCAPE_SEQUENCE_SIZE) {
				// Enter terminal mode
				SetMode(ITCH_TERMINAL);
				iflags.esc_idx = 0;
				ParserResetLine();
				return;
			}
			// not in terminal mode - ignore input
			//ParserResetLine();
			return;
		}
		// if here, we have a ch to start or continue parsing
		parse_result = Parse(ch);

		switch (parse_result) {
			case R_HELP:
				WriteDirectCh('\n');
				strcpy_itch_misc(out_str, ITCH_MISC_HELP_HEADING);
				WriteLineDirect(out_str);
				WriteOutputBuffer();		// show the help populated by the parser
				PreserveReplay();
				TrimReStuffBuffer();		// to remove ?

				//write out the prompt and modified restuff buf to the user
				WriteDirect(prompt);
				if (g_itch_restuff_buf[0] != '\0') {	// it wasn't on an empty line
					//WriteDirect(g_itch_restuff_buf);
					// Set the replay flag on to tell the loop to restuff rather than process new chars
					iflags.replay = 1;
				}
				ParserResetLine();		// reset parsing, start from beginning

				// re command prompts:
				// basic 	$ _
				// auth		host$
				// enable	host#
				// context	host(context)#
				// context2 host(context/context2)#
				break;
			case R_IGNORE:
				// Parser ignoring something irrelevant. Egs extra whitespace,
				//  or the rest of anything that arrives after an error has been detected
				// But - echo it back if echo_received is on
				WriteOutputBuffer();
				if (iflags.echo_received == 1) {
					WriteDirectCh(ch);		// echo the received char
				}
				break; //continue
			case R_DISCARD:
				// Parser ignoring an escape sequence or something that should not be
				//  echoed back even if echo_received is on
				WriteOutputBuffer();
				break; //continue
			case R_CONTINUE:
				// Parser processed something (eg toggle double quote) but
				//	did not advance to the node mapping stage
				// Potentially same as R_IGNORE???
				WriteOutputBuffer();
				if (iflags.echo_received == 1) {
					WriteDirectCh(ch);		// echo the received char
				}
				break; //continue
			case R_ERROR:
				WriteDirectCh('\n');
				strcpy_itch_misc(out_str, ITCH_MISC_ERROR_HEADER);
				WriteDirect(out_str);
				ParserGetErrorString(out_str);
				WriteLineDirect(out_str);
				if (iflags.mode == ITCH_BUFFER_STUFF) {
					strcpy_itch_misc(out_str, ITCH_BUFFER_STUFFING_ERROR);
					WriteLineDirect(out_str);
					WriteLineDirect(g_itch_restuff_buf);
				} else {
					strcpy_itch_misc(out_str, ITCH_MISC_ERROR_PROMPT);
					WriteLineDirect(out_str);
					WriteDirect(prompt);
				}
				PreserveReplay();
				ParserResetLine();
				break;
			case R_NONE:
				// parser should always return a meaningful result - report error
				ParserSetError(PE_NO_PARSE_RESULT);
				ParserGetErrorString(out_str);
				strcat_itch_misc(out_str, ITCH_MISC_CRNL);
				WriteLineDirect(out_str);
				WriteDirect(prompt);
				PreserveReplay();
				ParserResetLine();
				break; //continue
			case R_UNFINISHED:
				//char consumed, ready for next, nothing to report
				if (iflags.echo_received == 1) {
					WriteDirectCh(ch);		// echo the received char
				}
				break;
			case R_COMPLETE: {
				WriteOutputBuffer();
				if (iflags.mode != ITCH_BUFFER_STUFF) {
					WriteDirectCh('\n');
					WriteDirect(prompt);
				}
				PreserveReplay();
				ParserResetLine();
				break;
			}
			case R_REPLAY:
				// for dumb diag terms with no emulation, clear the line
				WriteDirectCh(0x0D); 	//CR
				//write spaces * length of replay string (ie whats already in the buffer) + prompt
				for (uint8_t i = 0; i < (strlen(g_itch_replay_buf) + strlen(prompt)); i++) {
					WriteDirectCh(' ');
				}
				WriteDirectCh(0x0D); 	// CR
				//write out the prompt
				WriteDirect(prompt);
				if (g_itch_restuff_buf[0] != '\0') {	// restuff is not empty
					// WriteDirect(g_itch_restuff_buf);
					// Set the replay flag on to tell the loop to restuff rather than process new chars
					iflags.replay = 1;
				}
				ParserResetLine();		// reset parsing, start from beginning
				break;
			case R_BACKSPACE:
				// for dumb diag terms with no emulation, simulate a backspace
				WriteDirectCh(0x0D); 	//CR
				//write spaces * length of replay string (ie whats already in the buffer) + prompt
				for (uint8_t i = 0; i < (strlen(g_itch_replay_buf) + strlen(prompt)); i++) {
					WriteDirectCh(' ');
				}
				WriteDirectCh(0x0D); 	// CR
				PreserveReplay();		// Copy replay to restuff
				TrimReStuffBuffer();	// Remove the deleted ch from restuff
				//write out the prompt
				WriteDirect(prompt);
				if (g_itch_restuff_buf[0] != '\0') {	// it wasn't on a single ch or empty line
					//WriteDirect(g_itch_restuff_buf);
					// Set the replay flag on to tell the loop to restuff rather than process new chars
					iflags.replay = 1;
				}
				ParserResetLine();		// reset parsing, start from beginning
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

void ITCH::WriteLineCallback(char* string) {
	if(string != NULL) {
		g_itch_output_buff.AddString("\n");
		g_itch_output_buff.AddString(string);
		g_itch_output_buff.SetOutputAvailable();
	}
}


void ITCH::WriteLineDirect(char* string) {
	// Add a newline and send the string to the output
	strcat(string, "\n");
	#ifdef ARDUINO
		Serial.flush();
		//delay(500);
		Serial.write(string);
		//delay(500);
		Serial.flush();
	#else
		fputs(string, osp);
		fflush(stdout);
	#endif
}

void ITCH::WriteDirect(char* string) {
	// Send the string to the output
	#ifdef ARDUINO
		Serial.flush();
		//delay(500);
		Serial.write(string);
		//delay(500);
		Serial.flush();
		//delay(500);
	#else
		fputs(string, osp);
		fflush(stdout);
	#endif
}

void ITCH::WriteDirectCh(char ch) {
	#ifdef ARDUINO
		Serial.flush();
		//delay(500);
		Serial.write(ch);
		//delay(500);
		Serial.flush();
		//delay(500);
	#else
		fputc(ch, osp);
		fflush(stdout);
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


