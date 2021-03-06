/************************************************
 ff_debug.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Debug Functions
 ************************************************/
#ifndef DEBUG_FF_H_
#define DEBUG_FF_H_


/************************************************
 Includes
************************************************/
#include <build_config.h>

#ifdef PLATFORM_ARDUINO
#include <Arduino.h>
#endif //PLATFORM_ARDUINO
#ifdef PLATFORM_LINUX
#include <stdint.h>
#endif

//#include "ff_events.h"
#include <registry.h>

/************************************************
 Public and Forward Data Structure Declarations
************************************************/


/************************************************
 Function Prototypes
************************************************/

void DebugShowMemory(const char* msg);

#ifdef PLATFORM_ARDUINO
void DebugShowMemory(const __FlashStringHelper *msg);
void DebugLog(const __FlashStringHelper *log_message);
#endif //PLATFORM_ARDUINO

void DebugLog(const char* log_message);

void DebugLog(uint16_t source, uint16_t destination, uint8_t msg_type, uint8_t msg_str, int32_t i_val, float f_val);
void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str, int32_t i_val, float f_val);
void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str);

void D(char* tag, time_t t);
void D(char* tag, tm* t);
void D(uint16_t source, char* str);
void Dump(BlockNode *b, char *tag);


#endif /* DEBUG_FF_H_ */
