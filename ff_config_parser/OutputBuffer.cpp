/*****************************************************************
 OutputBuffer.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 16 Feb. 2018

 ******************************************************************/

#include "OutputBuffer.h"

OutputBuffer::OutputBuffer() {
    output_available = false;;

	// TODO Auto-generated constructor stub

}

OutputBuffer::~OutputBuffer() {
	// TODO Auto-generated destructor stub
}

bool OutputBuffer::OutputAvailable() {
    return output_available;
}

char* OutputBuffer::GetOutputAsString(char* output_str) {

    DeQueue(output_string);
    if (GetSize() == 0) {
        output_available = false;
    }
    return strcpy(output_str, output_string);
}

void OutputBuffer::SetOutputAvailable(void) {
    output_available = true;

}
