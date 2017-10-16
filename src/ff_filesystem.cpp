/************************************************
 ff_filesystem.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 File System IO and Processing
************************************************/


/************************************************
 Includes
************************************************/
#include <stdint.h>
#include "ff_HAL.h"

/************************************************
 Data Structures
************************************************/


/************************************************
 Globals
************************************************/





/************************************************
 Functions
************************************************/

void InitFileSystem(void) {
	// placeholder if fs needs to persist between accesses
}


uint8_t SaveEventBuffer(void) {
	return HALSaveEventBuffer();
}



