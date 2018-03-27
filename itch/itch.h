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

enum ITCH_MODES {
	ITCH_INIT,
	ITCH_INTERACTIVE,
	ITCH_FILE,
};

typedef struct ITCH_FLAGS {
	uint8_t mode;
	uint8_t replay;
} I_FLAGS;

#ifndef ARDUINO
#define strcpy_P(d, s) strcpy(d, s)
#define strcat_P(d, c) strcat(d, c)
#endif


class ITCH {

private:
	#ifndef ARDUINO
	FILE* isp;	// input stream pointer
	FILE* osp;	// output stream pointer
	#endif

	I_FLAGS iflags;

	char prompt[MAX_OUTPUT_LINE_SIZE];
	//char prompt_replay[MAX_OUTPUT_LINE_SIZE];
	char prompt_base[5];

public:
	ITCH();
	virtual ~ITCH();

	#ifdef ARDUINO
	void Begin(int mode);
	#else
	void Begin(FILE* input_stream, FILE* output_stream, int mode);
	#endif

	void Poll();
	void WriteLine(char* string);
	void WriteImmediate(char* string);
};


#endif /* ITCH_H_ */

