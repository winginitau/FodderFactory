/*****************************************************************
 OutputBuffer.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 16 Feb. 2018

 ******************************************************************/

#include "OutputBuffer.h"


OutputBuffer::OutputBuffer() {
    output_available = false;;

}

OutputBuffer::~OutputBuffer() {
	// Auto-generated destructor stub
}

bool OutputBuffer::OutputAvailable() {
    return output_available;
}

char* OutputBuffer::GetOutputAsString(char* output_str) {

    DeQueue(output_str);
    if (GetSize() == 0) {
        output_available = false;
    }
    return output_str;
}

void OutputBuffer::SetOutputAvailable(void) {
    if (GetSize() > 0) {
        output_available = true;
    } else {
    	output_available = false;
    }
}
