/*****************************************************************
 OutputBuffer.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 16 Feb. 2018

 ******************************************************************/

#ifndef OUTPUTBUFFER_H_
#define OUTPUTBUFFER_H_

#include "StringList.h"

class OutputBuffer: public StringList {
private:
    bool output_available;
    char output_string[MAX_BUFFER_LENGTH];

public:
	OutputBuffer();
	virtual ~OutputBuffer();

	bool OutputAvailable();
	char* GetOutputAsString(char* output_str);
	void SetOutputAvailable();
};

#endif /* OUTPUTBUFFER_H_ */
