/************************************************
 ff_debug.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Debug Functions
************************************************/


/************************************************
 Includes
************************************************/
#include <ff_sys_config.h>
#include "ff_debug.h"
#include "ff_string_consts.h"

#ifdef FF_ARDUINO
#include <Arduino.h> //TODO get rid of this
#endif

#ifdef FF_SIMULATOR
#include <stdio.h>
#endif

#ifdef USE_ITCH
#include "itch.h"
#endif

#include "ff_datetime.h"
//#include "ff_display.h"
//#include "ff_datetime.h"
#include "ff_utils.h"
#include "ff_HAL.h"
#include "ff_registry.h"

//#include <time.h>

/************************************************
 Data Structures
************************************************/



/************************************************
 Functions
************************************************/

#ifdef DEBUG

#ifdef FF_ARDUINO
#ifdef DEBUG_MEMORY
char* GetMemPointers(char* str) {
/* This function places the current value of the heap and stack pointers in the
 * variables. You can call it from any place in your code and save the data for
 * outputting or displaying later. This allows you to check at different parts of
 * your program flow.
 * The stack pointer starts at the top of RAM and grows downwards. The heap pointer
 * starts just above the static variables etc. and grows upwards. SP should always
 * be larger than HP or you'll be in big trouble! The smaller the gap, the more
 * careful you need to be. Julian Gall 6-Feb-2009.
 */

	uint8_t * heapptr, * stackptr, *bll_tail;

	bll_tail = (uint8_t *)GetLastBlockAddr();
	stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
	heapptr = stackptr;                     // save value of heap pointer
	free(stackptr);      // free up the memory again (sets stackptr to 0)
	stackptr =  (uint8_t *)(SP);           // save value of stack pointer
	sprintf(str, "BLL: %d  SP: %d >< %d :HP", (int)bll_tail, (int)stackptr, (int)heapptr);
	return str;
}
#endif //DEBUG_MEMORY
#endif

#ifdef DEBUG_SERIAL
void DebugSerial(char *log_entry) {
    //Serial.begin(DEBUG_SERIAL_BAUDRATE);

#ifdef USE_ITCH
	HALItchWriteLnImmediate(log_entry);
#else
    int loop = 2000;
    while (loop > 0) {
      loop--;
      if (Serial) {
        break;
      };
    };
    if (loop > 0) {
      //ie. connected before timeout
      Serial.println(log_entry);
      Serial.flush();
    };
    //Serial.end();
#endif

}
#endif // DEBUG_SERIAL

#ifdef DEBUG_CONSOLE
void DebugConsole (const char* str) {
	printf("%s\n", str);
}
#endif //DEBUG_CONSOLE


void DebugLog(const char* log_message) {
	//Base function - add date and time to a string and send to defined debug destinations

	char log_entry[MAX_DEBUG_LENGTH];
	char dt[24];
	time_t now;
	now = TimeNow();
	strftime(dt, 24, "%Y-%m-%d,%H:%M:%S", localtime(&now));		//yyyy-mm-dd, 00:00:00
	#ifdef DEBUG_MEMORY
		char mem_str[MAX_DEBUG_LENGTH];
		GetMemPointers(mem_str);
		sprintf(log_entry, "%s, DEBUG, (%s) %s", dt, mem_str, log_message);  				//assemble log entry with time stamp
	#else
		sprintf(log_entry, "%s, DEBUG, %s", dt, log_message);  				//assemble log entry with time stamp
	#endif

	#ifdef DEBUG_SERIAL
		DebugSerial(log_entry);
	#endif

	#ifdef DEBUG_LCD
		HALDebugLCD(log_entry);
	#endif

	#ifdef DEBUG_FILE
		//TODO
		//DebugFile(log_entry);
	#endif

	#ifdef DEBUG_CONSOLE
		DebugConsole(log_entry);
	#endif
}


void DebugLog(uint16_t source, uint16_t destination, uint8_t msg_type, uint8_t msg_str, int16_t i_val, float f_val) {
	// Full version - mirrors event message format
	if (msg_type >= DEBUG_LEVEL) {
		char debug_log_message[MAX_DEBUG_LENGTH];
		char f_str[8];

		if (destination != UINT16_INIT) {
			FFFloatToCString(f_str, f_val);
			sprintf(debug_log_message, "[%s]->[%s], %s, %s, %d, %s", GetBlockLabelString(source), GetBlockLabelString(destination), GetMessageTypeString(msg_type), GetMessageString(msg_str), i_val, f_str);
		} else {
			if (msg_str == M_NULL) {
				sprintf(debug_log_message, "[%s] %s", GetBlockLabelString(source), GetMessageTypeString(msg_type));
			} else {
				if (i_val == (int16_t)INT16_INIT) {
					sprintf(debug_log_message, "[%s], %s, %s", GetBlockLabelString(source), GetMessageTypeString(msg_type), GetMessageString(msg_str));
				} else {
					FFFloatToCString(f_str, f_val);
					sprintf(debug_log_message, "[%s], %s, %s, %d, %s", GetBlockLabelString(source), GetMessageTypeString(msg_type), GetMessageString(msg_str), i_val, f_str);
				}
			}
		}
		DebugLog(debug_log_message);
	}
}

void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str, int16_t i_val, float f_val) {
	//full version without destination
	DebugLog(source, UINT16_INIT, msg_type, msg_str, i_val, f_val);
}


void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str) {
	// Simplified version
	if (msg_type >= DEBUG_LEVEL) {
		char debug_log_message[MAX_DEBUG_LENGTH];
		sprintf(debug_log_message, "[%s], %s, %s", GetBlockLabelString(source), GetMessageTypeString(msg_type), GetMessageString(msg_str));
		DebugLog(debug_log_message);
	}
}



void D(char* tag, time_t t) {
	// short-hand dev version of DebugLog
	char debug_log_message[MAX_DEBUG_LENGTH];
	sprintf(debug_log_message, "    %s: time_t:\t %lu", tag, t);
	DebugLog(debug_log_message);
}

void D(char* tag, tm* t) {
	// short-hand dev version of DebugLog
	char debug_log_message[MAX_DEBUG_LENGTH];
	sprintf(debug_log_message, "    %s: tm:\t %d-%d-%d %d:%d:%d", tag, t->tm_year, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	DebugLog(debug_log_message);
}


void D(uint16_t source, char* str) {
	// short-hand dev version of DebugLog
	char debug_log_message[MAX_DEBUG_LENGTH];
	sprintf(debug_log_message, "[%s] %s", GetBlockLabelString(source), str);
	DebugLog(debug_log_message);
}

void Dump(BlockNode *b, char* tag) {
	// short-hand dev version of DebugLog
	char debug_log_message[MAX_DEBUG_LENGTH];

	sprintf(debug_log_message, "    ");
	DebugLog(debug_log_message);

	sprintf(debug_log_message, "    %s label:\t %s", tag, b->block_label);
	DebugLog(debug_log_message);

	sprintf(debug_log_message, "    %s id:\t %u", tag, b->block_id);
	DebugLog(debug_log_message);

	sprintf(debug_log_message, "    %s cat:\t %u", tag, b->block_cat);
	DebugLog(debug_log_message);

	sprintf(debug_log_message, "    %s type:\t %u", tag, b->block_type);
	DebugLog(debug_log_message);

	sprintf(debug_log_message, "    %s act:\t %u", tag, b->active);
	DebugLog(debug_log_message);
}

#endif //DEBUG

#ifndef DEBUG
// XXX Debug Stubs for when DEBUG is not declared at all but calls remain in the code base

void DebugLog(const char* log_message) {};

void DebugLog(uint16_t source, uint16_t destination, uint8_t msg_type, uint8_t msg_str, int16_t i_val, float f_val) {};
void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str, int16_t i_val, float f_val) {};
void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str) {};


#endif
