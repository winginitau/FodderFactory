/*****************************************************************
 icli.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

******************************************************************/
//#include "LineBuffer.h"


#include "ICLI.h"
#include <stdio.h>
#include <stdlib.h>
//#include "processor_out.h"


/****************************************************************************
   Main Calling Routine
****************************************************************************/
int main(void) {

	ICLI icli;

	FILE* isp;
	//FILE* osp;


	isp = fopen("config_file.txt", "r");

	if (isp) {
		icli.Begin(isp, stdout, ICLI_FILE);
	} else {
		icli.Begin(stdin, stdout, ICLI_INTERACTIVE);
	}

	while (true) {
		icli.Poll();
	}

	return 0;
}




