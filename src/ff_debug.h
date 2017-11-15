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
#include "ff_registry.h"

/************************************************
 Public and Forward Data Structure Declarations
************************************************/


/************************************************
 Function Prototypes
************************************************/

void D(time_t t);
void D(uint16_t source, char* str);
void Dump(BlockNode *b, char *tag);
void DebugLog(const char* log_message);
void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str);
void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val);


#endif /* FF_DEBUG_H_ */
