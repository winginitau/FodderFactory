/************************************************
 ff_debug.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Debug Functions
************************************************/


/************************************************
 Includes
************************************************/
#include "ff_debug.h"
#include "ff_sys_config.h"
#include "ff_string_consts.h"

#ifdef FF_ARDUINO
#include <Arduino.h> //TODO get rid of this
#endif

#ifdef FF_SIMULATOR
#include <stdio.h>
#endif

#include "ff_datetime.h"
//#include "ff_display.h"
//#include "ff_datetime.h"
#include "ff_utils.h"
#include "ff_HAL.h"
#include "ff_registry.h"


/************************************************
 Data Structures
************************************************/



/************************************************
 Functions
************************************************/

#ifdef DEBUG


#ifdef DEBUG_SERIAL
void DebugSerial(char *log_entry) {
    Serial.begin(DEBUG_SERIAL_BAUDRATE);
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
    Serial.end();

}
#endif // DEBUG_SERIAL

#ifdef DEBUG_CONSOLE
void DebugConsole (const char* str) {
	printf("%s\n", str);
}
#endif //DEBUG_CONSOLE


void DebugLog(const char* log_message) {
  char log_entry[MAX_DEBUG_LENGTH];
  char dt[24];
  time_t now;
  now = time(NULL);
  strftime(dt, 24, "%Y-%m-%d, %H:%M:%S", localtime(&now));					//yyyy-mm-dd, 00:00:00
  sprintf(log_entry, "%s, %s", dt, log_message);  	//assemble log entry with time stamp

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

//overload function
void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val) {
	char debug_log_message[MAX_DEBUG_LENGTH];
	char f_str[8];
	if (msg_str == M_NULL) {
			sprintf(debug_log_message, "[%s] %s", GetBlockLabelString(source), GetMessageTypeString(msg_type));
	} else {
		if (i_val == UINT16_INIT) {
			sprintf(debug_log_message, "[%s], %s, %s", GetBlockLabelString(source), GetMessageTypeString(msg_type), GetMessageString(msg_str));
		} else {
			FFFloatToCString(f_str, f_val);
			sprintf(debug_log_message, "[%s], %s, %s, %d, %s", GetBlockLabelString(source), GetMessageTypeString(msg_type), GetMessageString(msg_str), i_val, f_str);
		}
	}
	DebugLog(debug_log_message);
}

#endif //DEBUG

#ifndef DEBUG
// XXX Debug Stubs for when DEBUG is not declared

void DebugLog(const char* log_message) {
}

void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val) {
}

#endif
