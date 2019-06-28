/*****************************************************************
itch_test.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

******************************************************************/

// Define the target platform
#define PLATFORM_LINUX
//OR
//#define PLATFORM_ARDUINO

// include the itch header file
#include <itch.h>

// Include the outputs from the grammar compiled by glitch
// Make sure that the these files are in the current directory
// 	or their locations are included in your environment / IDE
//	as appropriate for:
//		C/C++ compiler includes
//		Source location
//		Library includes
//		Archiver includes
//		Assembler includes
//		Linker includes
//  Files generated by glitch:
//		out.h
//		out.cpp
// 		out_user_code.cpp
#include <out.h>


//#include <stdio.h>
//#include <stdlib.h>

//#ifdef LINUX
//#include <errno.h>
//#include <fcntl.h>
//#include <string.h>
//#include <termios.h>
//#include <unistd.h>
//#endif

#define PORT "/dev/ttyV1"

// Globally declare the itch singleton
// Note: It's possible to use it as a local, though since it is designed
// 	to be inserted between your program code and all "console" / Serial
//  communications it is likely to be instantiated regularly. On resource
//  constrained devices this means a hit to the stack when ever it comes
//  into scope (particularly the input buffer). Better to declare it global
//  so that it is allocated in .bss (if on Arduino) and its buffer usage
//  is visible at build time.

ITCH itch;


/****************************************************************************
   Main Calling Routine
****************************************************************************/
int main(void) {

	itch.Begin(stdin, stdout);


	//icli.Begin(isp, isp, ICLI_INTERACTIVE);

	while (1) {
		itch.Poll();
	}

	return 0;
}


