/*****************************************************************
 FirstInclude.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 10 Apr. 2018

******************************************************************/
#include "FirstInclude.h"
#include "SecondInclude.h"
#include "config.h"
#include <stdio.h>


StrStruct global_string_1;

//char global_string_1[MAX_STRING_SIZE];

int FirstStaticFunction(void) {
	sprintf(global_string_1.str, "Global String 1");

	return SecondStaticFunction();

}
