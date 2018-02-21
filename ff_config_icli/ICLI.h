/*****************************************************************
 ICLI.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#ifndef ICLI_H_
#define ICLI_H_


#include "LineBuffer.h"
#include "OutputBuffer.h"
#include "Parser.h"

#include <stdio.h>

enum ICLI_MODES {
	ICLI_INIT,
	ICLI_INTERACTIVE,
	ICLI_FILE,
};


class ICLI {

private:
	FILE* isp;	// input stream pointer
	FILE* osp;	// output stream pointer
	int mode;
	char prompt[MAX_BUFFER_LENGTH];
	Parser parser;

public:
	ICLI();
	virtual ~ICLI();


	void Begin(FILE* input_stream, FILE* output_stream, int icli_mode);
	void Poll();
};

#endif /* ICLI_H_ */
