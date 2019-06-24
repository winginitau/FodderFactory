/************************************************
 ff_datetime.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Fodder Factory Specific Date and Time
 ************************************************/

#ifndef DATETIME_FF_H_
#define DATETIME_FF_H_

#include <build_config.h>
#include <time.h>

#ifdef FF_ARDUINO
#include <Arduino.h>
#endif

#ifdef PLATFORM_LINUX
#include <stdint.h>
#endif




/************************************************
 Data Structures
 ************************************************/

/************************************************
 Function prototypes
 ************************************************/

void InitRTC(void);

#endif /* DATETIME_FF_H_ */
