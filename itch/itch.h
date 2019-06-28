/*****************************************************************
 ITCH.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#ifndef ITCH_H_
#define ITCH_H_

#define NEW_MODEL

#include <config.h>
#include <LineBuffer.h>
#include <stdio.h>

#ifdef NEW_MODEL
#include <new_Parser.h>
#else
#include <Parser.h>
#endif

#ifdef PLATFORM_ARDUINO
#include <Arduino.h>
#endif

#ifdef PLATFORM_LINUX
#include <termios.h>
#endif //PLATFORM_LINUX

enum ITCH_SESSION_FLAGS {
	ITCH_INIT,
	ITCH_TEXT_DATA,		// Normal operating - just sending log data
	ITCH_TEXT_CCC,		// Command Configure Control
	ITCH_TERMINAL,		// Interactive terminal with basic emulation
	ITCH_BUFFER_STUFF,	// Parsing from the stuff buffer
};


void WriteLineCallback(const char* string);
void ITCHWriteLine(const char* string);
void ITCHWrite(const char* string);
void ITCHWriteChar(const char ch);

void PreserveReplay(void);
void TrimReStuffBuffer(void);
void TrimBuffer(void);

#ifdef PLATFORM_ARDUINO
	size_t ITCHWrite_P(const char *string);
	void ITCHWriteLine_P(const char *string);
	void ITCHWriteLine(const __FlashStringHelper *string);
	void ITCHWrite(const __FlashStringHelper *string);
#endif //PLATFORM_ARDUINO

class ITCH {
public:
	ITCH();
	//virtual ~ITCH();

	#ifdef PLATFORM_ARDUINO
		void Begin();
	#endif //PLATFORM_ARDUINO
	#ifdef PLATFORM_LINUX
		void Begin(FILE* input_stream, FILE* output_stream);
		void RestoreTermAndExit();
	#endif//PLATFORM_LINUX

	void SetMode(uint8_t mode);
	uint8_t StuffAndProcess(char* str);
	void Poll();

private:
	uint8_t i_mode;				// Itch Mode - Data, terminal, command etc
	uint8_t i_replay;			// Replaying on or off
	uint8_t i_term_echo;		// Echo received chars back to the terminal?

	char term_esc_seq[4];
	uint8_t term_esc_idx;
	char ccc_esc_seq[4];
	uint8_t ccc_esc_idx;

	#ifdef PLATFORM_LINUX
	// Start up terminal settings
	struct termios old_tio;
	#endif //PLATFORM_LINUX
};



#endif /* ITCH_H_ */

