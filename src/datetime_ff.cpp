/************************************************
 ff_datetime.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Fodder Factory Specific Date and Time
 ************************************************/

/************************************************
 Includes
 ************************************************/

#include <build_config.h>

#ifdef PLATFORM_ARDUINO
#include <Arduino.h>
#endif //PLATFORM_ARDUINO

#ifdef PLATFORM_LINUX
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#endif

#include <datetime_ff.h>
#include <string_consts.h>
#include <HAL.h>
#include <debug_ff.h>



void InitRTC(void) {		//setup the Real Time Clock
	HALInitRTC();
}



