/*****************************************************************
 ITCH.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#include <itch_strings.h>
#include <itch.h>
#include <itch_hal.h>
#include <stdlib.h>

#ifdef PLATFORM_ARDUINO
#define D(x) Serial.write(x)
#endif

#ifdef PLATFORM_LINUX
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

extern char g_parser_parse_buf[MAX_INPUT_LINE_SIZE]; // main parsing buf declared in the parser
extern uint8_t g_parser_buf_idx;	// g_parser_parse_buf not \0 terminated
									// g_parser_buf_idx always points to next free and is size

char *g_itch_buf_inject_ptr = NULL;	// XXX might be able to move these local
uint8_t g_itch_buf_inject_size = 0;

#ifdef PLATFORM_LINUX
static FILE* isp = NULL;	// input stream pointer
static FILE* osp = NULL;	// output stream pointer
#endif //PLATFORM_LINUX

/******************************************************************************
 * Class Methods
 ******************************************************************************/

ITCH::ITCH() {
	i_mode = ITCH_INIT;
	i_term_echo = 0;
	i_replay = 0;
	term_esc_idx = 0;
	ccc_esc_idx = 0;
	strcpy(term_esc_seq, ITCH_TERM_ESC_SEQ);
	strcpy(ccc_esc_seq, ITCH_CCC_ESC_SEQ);
}


#ifdef PLATFORM_LINUX
void ITCH::RestoreTermAndExit() {
	// Restore original terminal settings
	tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
}
#endif //PLATFORM_LINUX

#ifdef PLATFORM_ARDUINO
void ITCH::Begin() {
	i_mode = ITCH_TEXT_DATA;
	ParserInit();
}
#endif //PLATFORM_ARDUINO

#ifdef PLATFORM_LINUX
void ITCH::Begin(FILE* input_stream, FILE* output_stream) {

	struct termios new_tio;
	// Get the terminal settings for stdin
	tcgetattr(STDIN_FILENO,&old_tio);
	// Keep the old setting to restore at end
	new_tio=old_tio;
	// Disable canonical mode (buffered i/o) and local echo
	new_tio.c_lflag &=(~ICANON & ~ECHO);
	// Set the new settings
	tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

	isp = input_stream;
	osp = output_stream;
	i_mode = ITCH_TEXT_DATA;
	ParserInit();
}
#endif //PLATFORM_LINUX

void ITCH::SetMode(uint8_t new_mode) {
	switch (new_mode) {
		case ITCH_TEXT_DATA:
			if (i_mode == ITCH_TERMINAL) {
				ITCHWriteChar('\n');
				ITCHWriteLineMisc(ITCH_TERMINAL_GOODBYE);
			} else if (i_mode == ITCH_TEXT_CCC) {
				ITCHWriteLineMisc(ITCH_CCC_OK);
			}
			// clean up
			ParserResetLine();
			i_mode = new_mode;
			break;
		case ITCH_TEXT_CCC:
			ITCHWriteLineMisc(ITCH_CCC_OK);
			// set terminal echo-back to off
			i_term_echo = 0;
			ParserResetLine();
			i_mode = new_mode;
			break;
		case ITCH_TERMINAL:
			// ITCH Welcome
			ITCHWriteChar('\n');
			ITCHWriteLineMisc(ITCH_TERMINAL_WELCOME);
			// itch prompt
			ITCHWriteMisc(ITCH_MISC_PROMPT);
			// set terminal echo-back to on
			i_term_echo = 1;
			ParserResetLine();
			i_mode = new_mode;
			break;
		case ITCH_BUFFER_STUFF:
			// Processing commands being stuffed into the stuff buffer
			ITCHWriteChar('\n');
			ITCHWriteLineMisc(ITCH_BUFFER_STUFFING_MODE);
			// set terminal echo-back to off
			i_term_echo = 0;
			// Reset the parser - we may have got here from a parser triggered command.
			ParserResetLine();
			i_mode = new_mode;
			break;
		default:
			break;
			// TODO ERROR
	}
}

void TrimBuffer(void) {
	// Trim the buffer of the last char
	// to support BackSpace and trimming of "?" after help requested
	if (g_parser_buf_idx > 0) { g_parser_buf_idx--; }
}

uint8_t ITCH::StuffAndProcess(char* str) {
	// Expects NULL terminated str
	ParserResetLine();
	strcpy(g_parser_parse_buf, str);
	g_itch_buf_inject_ptr = g_parser_parse_buf;
	g_parser_buf_idx = strlen(str);
	i_replay = 1;
	while (i_replay == 1) {
		Poll();
	}
	return 1;
}

void ITCH::Poll(void) {
	char ch;							// the object of attention
	uint8_t parse_result = R_NONE;		// captures the ch by char parsing result

	// if no replay editing is active then get the next ch from the input stream
	if (i_replay == 0) {
		#ifdef PLATFORM_LINUX
		ch = getc(isp);					// XXX convert to non-blocking for linux
		#endif //PLATFORM_LINUX
		#ifdef PLATFORM_ARDUINO
		if (Serial.available()) {
			ch = Serial.read();
		} else return;					// nothing available, return
		#endif //PLATFORM_ARDUINO
	} else { 							// replay is active - replay != 0
		// Assumes parse buffer already loaded
		// Doll out the chars
		if (g_itch_buf_inject_ptr != ((char* )g_parser_parse_buf + g_itch_buf_inject_size)) {
			ch = *g_itch_buf_inject_ptr;
			g_itch_buf_inject_ptr++;
		} else {
			// finished re-stuffing, turn off the replay flag
			i_replay = 0;
			g_itch_buf_inject_ptr = NULL;
			g_itch_buf_inject_size = 0;
			return;
		}
	}
	// Process the ch - user entered or re-stuffed
	if (ch != EOF) {
		// In non-terminal mode? - Test for escape sequences only
		if (i_mode < ITCH_TEXT_CCC) {
			// Test if ch is the first/next char the "enter terminal mode" escape sequence
			if (ch == term_esc_seq[term_esc_idx]) {
				// matched - advance matching to next
				term_esc_idx++;
			} else {
				// sequence broken, reset matching
				term_esc_idx = 0;
			}
			// Test if ch is the first/next char the "enter ccc mode" escape sequence
			if (ch == ccc_esc_seq[ccc_esc_idx]) {
				// matched - advance matching to next
				ccc_esc_idx++;
			} else {
				// sequence broken, reset matching
				ccc_esc_idx = 0;
			}
			// Terminal mode escape sequence fully matched?
			if (term_esc_idx == ITCH_TERM_ESC_SEQ_SIZE) {
				// Enter terminal mode
				SetMode(ITCH_TERMINAL);
				term_esc_idx = 0;
				ParserResetLine();
				return;
			}
			// CCC mode escape sequence fully matched?
			if (ccc_esc_idx == ITCH_CCC_ESC_SEQ_SIZE) {
				// Enter terminal mode
				SetMode(ITCH_TEXT_CCC);
				ccc_esc_idx = 0;
				ParserResetLine();
				return;
			}
			// not in terminal or ccc modes - ignore input
			return;
		}
		// if here, we have a ch to start or continue parsing
		parse_result = ParseProcess(ch);

		switch (parse_result) {
			case R_HELP:
				// Help contents has been displayed form the parser / nodemap
				// Re-present the prompt and the line so far without the '?'
				TrimBuffer();		// to point before '?'
				//write out the prompt
				ITCHWriteMisc(ITCH_MISC_PROMPT);
				// Is there content still in the buffer?
				// ie. '?' was following a command
				if(g_parser_buf_idx > 0) {
					// replay it
					i_replay = 1;
					g_itch_buf_inject_ptr = g_parser_parse_buf;
					g_itch_buf_inject_size = g_parser_buf_idx;
				} else { // now blank line
					// Set the inject size to 1 in case the next command is a recall
					// 	which will just recall the '?'
					g_itch_buf_inject_size = 1;
				}
				ParserResetPreserve();		// reset parsing preserving the buffer
				break;
			case R_IGNORE:
				// Parser ignoring something irrelevant. Egs extra whitespace,
				//  or the rest of anything that arrives after an error has been detected
				// But - echo it back if echo_received is on
				//WriteOutputBuffer();
				if (i_term_echo == 1) {
					ITCHWriteChar(ch);		// echo the received char
				}
				break; //continue
			case R_DISCARD:
				// Parser ignoring an escape sequence or something that should not be
				//  echoed back even if echo_received is on
				break; //continue
			case R_CONTINUE:
				// Parser processed a char - all good, get next
				// Potentially same as R_IGNORE???
				if (i_term_echo == 1) {
					ITCHWriteChar(ch);		// echo the received char
				}
				break; //continue
			case R_ERROR:
				if (i_mode == ITCH_TERMINAL) {
					//ITCHWriteChar('\n');
					ITCHWriteMisc(ITCH_MISC_ERROR_HEADER);
					ParserWriteLineErrorString();
				} else if (i_mode == ITCH_BUFFER_STUFF) {
					ITCHWriteLineMisc(ITCH_BUFFER_STUFFING_ERROR);
					ITCHWriteLine(g_itch_buf_inject_ptr);
				} else if (i_mode == ITCH_TEXT_CCC) {
					ITCHWriteLineMisc(ITCH_CCC_ERROR);
					ITCHWriteLine(g_itch_buf_inject_ptr);
				} else {
					ITCHWriteLineMisc(ITCH_MISC_ERROR_PROMPT);
				}
				ITCHWriteMisc(ITCH_MISC_PROMPT);
				// set the inject size in case the next command is a recall
				g_itch_buf_inject_size = g_parser_buf_idx;
				ParserResetPreserve();
				break;
			case R_NONE:
				if ((i_mode == ITCH_TERMINAL) || \
					(i_mode == ITCH_BUFFER_STUFF)	) {
						// parser should always return a meaningful result - report error
						ParserSetError(PE_NO_PARSE_RESULT);
						ParserWriteLineErrorString();
						ITCHWriteMisc(ITCH_MISC_PROMPT);
				} else if (i_mode == ITCH_TEXT_CCC) {
					ITCHWriteLineMisc(ITCH_CCC_ERROR);
					ITCHWriteLine(g_itch_buf_inject_ptr);
				}
				ParserResetLine();
				break; //continue
			case R_UNFINISHED:
				//char consumed, ready for next, nothing to report
				if (i_term_echo == 1) {
					ITCHWriteChar(ch);		// echo the received char
				}
				break;
			case R_COMPLETE: {
				if (i_mode == ITCH_TERMINAL) {
					ITCHWriteMisc(ITCH_MISC_PROMPT);
				} else if (i_mode == ITCH_TEXT_CCC) {
					ITCHWriteLineMisc(ITCH_CCC_OK);
				}
				// set the inject size in case the next command is a recall
				g_itch_buf_inject_size = g_parser_buf_idx;
				ParserResetPreserve();
				break;
			}
			case R_REPLAY:
				// Assumes previous process (if any) has left g_itch_buf_inject_size
				//	set to the size of the previously processed command.
				// For dumb terminals with only ASCII emulation, clear the line
				ITCHWriteChar(0x0D); 	//CR
				//write spaces * length of replay string (ie whats already in the buffer) + prompt
				for (uint8_t i = 0; i < (g_parser_buf_idx + ITCH_PROMPT_SIZE); i++) {
					ITCHWriteChar(' ');
				}
				ITCHWriteChar(0x0D); 	// CR
				//write out the prompt
				ITCHWriteMisc(ITCH_MISC_PROMPT);
				// Content in the buffer?
				if(g_itch_buf_inject_size > 0) {
					i_replay = 1;
					g_itch_buf_inject_ptr = g_parser_parse_buf;
					//g_itch_buf_inject_size = g_parser_buf_idx;
				}
				ParserResetPreserve();		// reset parsing preserving the buffer
				break;
			case R_BACKSPACE:
				// for dumb terminals with only ASCII emulation, clear the line
				ITCHWriteChar(0x0D); 	//CR
				//write spaces * length of replay string (ie whats already in the buffer) + prompt
				for (uint8_t i = 0; i < (g_parser_buf_idx + ITCH_PROMPT_SIZE); i++) {
					ITCHWriteChar(' ');
				}
				ITCHWriteChar(0x0D); 	// CR
				TrimBuffer();			// Remove the deleted ch from restuff
				//write out the prompt
				ITCHWriteMisc(ITCH_MISC_PROMPT);
				// Content stll in the buffer?
				if(g_parser_buf_idx > 0) {
					i_replay = 1;
					g_itch_buf_inject_ptr = g_parser_parse_buf;
					g_itch_buf_inject_size = g_parser_buf_idx;
				}
				ParserResetPreserve();		// reset parsing preserving the buffer
				break;
			case R_EXIT:
				SetMode(ITCH_TEXT_DATA);
				break;
			default: break;
		}
	}
}

/***********************************************************
 * Static functions outside the ITCH class that
 * for output writing
 ***********************************************************/

void WriteLineCallback(const char* string) {
	ITCHWriteLine(string);
}

void ITCHWriteLine(const char* string) {
	// Add a newline and send the string to the output
	#ifdef PLATFORM_ARDUINO
		Serial.flush();
		Serial.write(string);
		Serial.write('\n');
		Serial.flush();
	#endif //PLATFORM_ARDUINO
	#ifdef PLATFORM_LINUX
		fputs(string, osp);
		fputs("\n", osp);
		fflush(stdout);
	#endif //PLATFORM_LINUX
}

void ITCHWrite(const char* string) {
	// Send the string to the output
	#ifdef PLATFORM_ARDUINO
		Serial.flush();
		Serial.write(string);
		Serial.flush();
	#endif //PLATFORM_ARDUINO
	#ifdef PLATFORM_LINUX
		fputs(string, osp);
		fflush(stdout);
	#endif //PLATFORM_LINUX
}

void ITCHWriteChar(const char ch) {
	#ifdef PLATFORM_ARDUINO
		Serial.flush();
		Serial.write(ch);
		Serial.flush();
	#endif //PLATFORM_ARDUINO
	#ifdef PLATFORM_LINUX
		fputc(ch, osp);
		fflush(stdout);
	#endif //PLATFORM_LINUX
}

#ifdef PLATFORM_ARDUINO
void ITCHWriteLine(const __FlashStringHelper *string) {
	// Write PROGMEM strings directly from PROGMEM
		Serial.flush();
		Serial.println(string); //will link to appropriate overloaded println
		Serial.flush();
}

void ITCHWrite(const __FlashStringHelper *string) {
	// Send the string to the output
	Serial.flush();
	Serial.print(string);
	Serial.flush();
}

size_t ITCHWrite_P(const char *string) {
  PGM_P p = reinterpret_cast<PGM_P>(string);
  size_t n = 0;
  while (1) {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) break;
    if (Serial.write(c)) n++;
    else break;
  }
  return n;
}

void ITCHWriteLine_P(const char *string) {
	ITCHWrite_P(string);
	ITCHWriteChar('\n');
}

#endif //PLATFORM_ARDUINO

