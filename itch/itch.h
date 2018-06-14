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
//XXX hard coded:
//#include "out.h"

#include <stdio.h>

enum ITCH_SESSION_FLAGS {
	ITCH_INIT,
	ITCH_TEXT_DATA ,
	ITCH_TEXT_PROTOCOL,
	ITCH_TERMINAL,
	ITCH_TERMINAL_CONFIG,
	ITCH_FILE_CONFIG,
};

typedef struct ITCH_FLAGS {
	uint8_t mode;
	uint8_t replay;
	// XXX
	char esc_seq[4];
	uint8_t esc_idx;
} I_FLAGS;

//#ifndef ARDUINO
//#define strcpy_P(d, s) strcpy(d, s)
//#define strcat_P(d, c) strcat(d, c)
//#endif



class ITCH {
private:
	I_FLAGS iflags;

	// XXX Dynamically allocate prompt? Takes up a lot of static space for most of the time
	//char prompt[MAX_OUTPUT_LINE_SIZE];
	char prompt[20];

public:
	ITCH();
	virtual ~ITCH();

	#ifdef ARDUINO
	void Begin();
	#else
	void Begin(FILE* input_stream, FILE* output_stream);
	#endif
	void SetMode(uint8_t mode);

	void Poll();
	static void WriteLine(char* string);
	static void WriteLineDirect(char* string);
	static void WriteDirect(char* string);
};


#endif /* ITCH_H_ */

