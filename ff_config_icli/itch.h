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

#include "out.h"
#include "LineBuffer.h"
#include "OutputBuffer.h"
#include "Parser.h"
#include "common_config.h"

#include <stdio.h>

extern void M(char * strn);

enum ITCH_MODES {
	ITCH_INIT,
	ITCH_INTERACTIVE,
	ITCH_FILE,
};

class ITCH {

private:
	FILE* isp;	// input stream pointer
	FILE* osp;	// output stream pointer
	int mode;
	char prompt[MAX_OUTPUT_LINE_SIZE];
	char prompt_replay[MAX_OUTPUT_LINE_SIZE];
	char prompt_base[MAX_OUTPUT_LINE_SIZE];
	Parser parser;


public:
	ITCH();
	virtual ~ITCH();


	#ifdef ARDUINO
	void Begin(int icli_mode);
	#else
	void Begin(FILE* input_stream, FILE* output_stream, int itch_mode);
	#endif

	void Poll();
	void WriteLine(char* string);
};


#endif /* ITCH_H_ */

