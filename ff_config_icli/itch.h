/*****************************************************************
 ITCH.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#ifndef ITCH_H_
#define ITCH_H_

#include "common_config.h"
#include "LineBuffer.h"
#include "OutputBuffer.h"
#include "Parser.h"

#include <stdio.h>

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
	char prompt[MAX_BUFFER_LENGTH];
	char prompt_replay[MAX_BUFFER_LENGTH];
	char prompt_base[MAX_BUFFER_LENGTH];
	Parser parser;

public:
	ITCH();
	virtual ~ITCH();


	void Begin(FILE* input_stream, FILE* output_stream, int icli_mode);
	void Poll();
	void WriteLine(char* string);
};


#endif /* ITCH_H_ */

