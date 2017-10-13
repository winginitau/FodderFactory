/************************************************
 ff_debug.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Debug Functions
 ************************************************/
#ifndef FF_DEBUG_H_
#define FF_DEBUG_H_


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

//#include "ff_events.h"

/************************************************
 Public and Forward Data Structure Declarations
************************************************/


/************************************************
 Function Prototypes
************************************************/

#ifdef DEBUG

void DebugLog(const char* log_message);
void DebugLog(uint8_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val);

#endif //DEBUG



#endif /* FF_DEBUG_H_ */
