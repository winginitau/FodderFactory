/*****************************************************************
 ITCH.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#ifndef ITCH_H_
#define ITCH_H_

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "config.h"
#include "LineBuffer.h"
#include "OutputBuffer.h"
#include "Parser.h"

#include <stdio.h>

enum ITCH_SESSION_FLAGS {
	ITCH_INIT,
	ITCH_TEXT_DATA ,
	ITCH_TERMINAL,
	ITCH_BUFFER_STUFF,
};

typedef struct ITCH_FLAGS {
	uint8_t mode;
	uint8_t replay;
	// XXX
	char esc_seq[4];
	uint8_t esc_idx;
	uint8_t echo_received;
} I_FLAGS;


class ITCH {
private:
	I_FLAGS iflags;
	// XXX Dynamically allocate prompt? Takes up a lot of static space for most of the time
	//char prompt[MAX_OUTPUT_LINE_SIZE];
	char prompt[20];
	char out_str[MAX_OUTPUT_LINE_SIZE];

	void WriteOutputBuffer(void);
	void PreserveReplay(void);
	void TrimReStuffBuffer(void);

public:
	ITCH();
	virtual ~ITCH();

	#ifdef ARDUINO
	void Begin();
	#else
	void Begin(FILE* input_stream, FILE* output_stream);
	#endif
	void SetMode(uint8_t mode);
	uint8_t StuffAndProcess(char* str);
	void Poll();
	static void WriteLine(char* string);
	static void WriteLineCallback(char* string);
	static void WriteLineDirect(char* string);
	static void WriteDirect(char* string);
	static void WriteDirectCh(char ch);
};


#endif /* ITCH_H_ */

