/*****************************************************************
 SecondInclude.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 10 Apr. 2018

******************************************************************/

#include "SecondInclude.h"
#include "FirstInclude.h"
#include "config.h"
#include <stdio.h>


char global_string_2[MAX_STRING_SIZE];
extern StrStruct global_string_1;

int SecondStaticFunction(void) {
	sprintf(global_string_2, "Contents of global_string_1: %s\n", global_string_1.str);
	return 1;
}



