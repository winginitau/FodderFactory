/************************************************
 ff_utils.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Utility and System Functions
 ************************************************/
#ifndef FF_UTILS_H_
#define FF_UTILS_H_

/************************************************
 Includes
 ************************************************/

#include "ff_sys_config.h"

#ifdef FF_ARDUINO
#include <Arduino.h>
#endif


#ifdef FF_SIMULATOR
#include <stdint.h>
#endif

/************************************************
 Data Structures
 ************************************************/



/************************************************
 Function prototypes
 ************************************************/

//cool macro for easier string handling
//#define WriteMessageType(X) WriteMessageType(GetMessageTypeString((X)))

uint8_t DayStrToFlag(uint8_t day_flag[7], const char* day_str);

char* FFFloatToCString(char* buf, float f);

#ifdef FF_ARDUINO
String FFFloatString(float f);
#endif




#endif /* FF_UTILS_H_ */
