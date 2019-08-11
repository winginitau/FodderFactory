/************************************************
 ff_debug.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Debug Functions
************************************************/


/************************************************
 Includes
************************************************/
#include <build_config.h>
#include <debug_ff.h>
#include <string_consts.h>

#ifdef PLATFORM_ARDUINO
#include <Arduino.h> //TODO get rid of this
#endif //PLATFORM_ARDUINO

#ifdef PLATFORM_LINUX
#include <stdio.h>
#endif

#ifdef USE_ITCH
#include "itch.h"
#endif

#include <datetime_ff.h>
//#include "ff_display.h"
//#include "ff_datetime.h"
#include <utils.h>
#include <HAL.h>
#include <registry.h>

//#include <time.h>

/************************************************
 Data Structures
************************************************/



/************************************************
 Functions
************************************************/



#ifdef DEBUG

void DebugShowMemory(const char* msg) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	GetMemPointers(out_str);
	ITCHWrite(out_str);
	ITCHWrite(F("  "));
	ITCHWriteLine(msg);
}

#ifdef PLATFORM_ARDUINO
void DebugShowMemory(const __FlashStringHelper *msg) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	GetMemPointers(out_str);
	ITCHWrite(out_str);
	ITCHWrite(F("  "));
	ITCHWriteLine(msg);
}
#endif //PLATFORM_ARDUINO

#ifdef DEBUG_SERIAL
void DebugSerial(char *log_entry) {
	#ifdef USE_ITCH
		ITCHWriteLine(log_entry);
		//ITCHWriteLine(F("After Writing DEBUG Line"));
	#else
		int loop = 5000;
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

#ifdef PLATFORM_ARDUINO
void DebugLog(const __FlashStringHelper *log_message) {
	char log_entry[MAX_DEBUG_LENGTH];
	strcpy_hal(log_entry, log_message);
	DebugLog(log_entry);
}
#endif //PLATFORM_ARDUINO

void DebugLog(const char* log_message) {
	//Base function - add date and time to a string and send to defined debug destinations
	char log_entry[MAX_DEBUG_LENGTH];
	char dt[24];
	time_t now;
	now = TimeNow();
	char fmt_str[19];
	strcpy_hal(fmt_str, F("%Y-%m-%d,%H:%M:%S"));
	strftime(dt, 24, fmt_str, localtime(&now));		//yyyy-mm-dd, 00:00:00
	#ifdef DEBUG_MEMORY
		char mem_str[MAX_DEBUG_LENGTH];
		//ITCHWriteLine(log_message);
		//ITCHWriteLine(F("Before GetMemPointers()"));
		GetMemPointers(mem_str);
		//ITCHWriteLine(F("After GetMemPointers()"));
		strcpy_hal(fmt_str, F("%s, DEBUG, (%s) %s"));
		sprintf(log_entry, fmt_str, dt, mem_str, log_message);  				//assemble log entry with time stamp
	#else
		strcpy_hal(fmt_str, F("%s, DEBUG, %s"));
		sprintf(log_entry, fmt_str, dt, log_message);  				//assemble log entry with time stamp
	#endif
		//ITCHWriteLine(log_entry);
		//ITCHWriteLine(F("Before DebugSerial()"));
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

void DebugLog(uint16_t source, uint16_t destination, uint8_t msg_type, uint8_t msg_str, int32_t i_val, float f_val) {
	char fmt_str[28];
	char msg_type_string[MAX_MESSAGE_STRING_LENGTH];
	char msg_string[MAX_MESSAGE_STRING_LENGTH];
	char no_cfg_str[16];
	const char * source_str;
	// Full version - mirrors event message format
	if (msg_type >= DEBUG_LEVEL) {
		char debug_log_message[MAX_DEBUG_LENGTH];
		char f_str[8];
		GetMessageTypeString(msg_type_string, msg_type);
		GetMessageString(msg_string, msg_str);

		source_str = GetBlockLabelString(source);
		if (source_str == NULL) {
			strcpy_hal(no_cfg_str, F("SYSTEM-NOCONFIG"));
			source_str = no_cfg_str;
		}

		if (destination != UINT16_INIT) {
			FFFloatToCString(f_str, f_val);
			strcpy_hal(fmt_str, F("[%s]->[%s], %s, %s, %ld, %s"));
			sprintf(debug_log_message, fmt_str, source_str, GetBlockLabelString(destination), msg_type_string, msg_string, i_val, f_str);
		} else {
			if (msg_str == M_NULL) {
				strcpy_hal(fmt_str, F("[%s] %s"));
				sprintf(debug_log_message, fmt_str, source_str, msg_type_string);
			} else {
				if (i_val == (int32_t)INT32_INIT) {
					strcpy_hal(fmt_str, F("[%s], %s, %s"));
					sprintf(debug_log_message, fmt_str, source_str, msg_type_string, msg_string);
				} else {
					FFFloatToCString(f_str, f_val);
					strcpy_hal(fmt_str, F("[%s], %s, %s, %ld, %s"));
					sprintf(debug_log_message, fmt_str, source_str, msg_type_string, msg_string, i_val, f_str);
				}
			}
		}
		DebugLog(debug_log_message);
	}
}

void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str, int32_t i_val, float f_val) {
	//full version without destination
	DebugLog(source, UINT16_INIT, msg_type, msg_str, i_val, f_val);
}


void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_num) {
	// Simplified version
	char fmt_str[13];
	const char * source_str;
	char msg_type_string[MAX_MESSAGE_STRING_LENGTH];
	char msg_string[MAX_MESSAGE_STRING_LENGTH];
	char debug_log_message[MAX_DEBUG_LENGTH];
	char no_cfg_str[16];

	if (msg_type >= DEBUG_LEVEL) {

		GetMessageTypeString(msg_type_string, msg_type);
		//ITCHWrite(F("msg_type_string: "));
		//ITCHWriteLine(msg_type_string);

		GetMessageString(msg_string, msg_num);
		//ITCHWrite(F("msg_string: "));
		//ITCHWriteLine(msg_string);

		strcpy_hal(fmt_str, F("[%s], %s, %s"));

		source_str = GetBlockLabelString(source);
		if (source_str == NULL) {
			strcpy_hal(no_cfg_str, F("SYSTEM-NOCONFIG"));
			source_str = no_cfg_str;
		}

		sprintf(debug_log_message, fmt_str, source_str, msg_type_string, msg_string);

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

	sprintf(debug_log_message, "    %s label:\t %s", tag, b->label);
	DebugLog(debug_log_message);

	sprintf(debug_log_message, "    %s id:\t %u", tag, b->id);
	DebugLog(debug_log_message);

	sprintf(debug_log_message, "    %s cat:\t %u", tag, b->cat);
	DebugLog(debug_log_message);

	sprintf(debug_log_message, "    %s type:\t %u", tag, b->type);
	DebugLog(debug_log_message);

	sprintf(debug_log_message, "    %s act:\t %u", tag, b->active);
	DebugLog(debug_log_message);
}

#endif //DEBUG

#ifndef DEBUG
// XXX Debug Stubs for when DEBUG is not declared at all but calls remain in the code base

void DebugShowMemory(const char* msg) {
	(void)msg;
}

void DebugShowMemory(const __FlashStringHelper *msg) {
	(void)msg;
}

void DebugLog(const char* log_message) {
	(void)log_message;
};

void DebugLog(uint16_t source, uint16_t destination, uint8_t msg_type, uint8_t msg_str, int32_t i_val, float f_val) {
	(void)source;
	(void)destination;
	(void)msg_type;
	(void)msg_str;
	(void)i_val;
	(void)f_val;
};

void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str, int32_t i_val, float f_val) {
	(void)source;
	(void)msg_type;
	(void)msg_str;
	(void)i_val;
	(void)f_val;
};

void DebugLog(uint16_t source, uint8_t msg_type, uint8_t msg_str) {
	(void)source;
	(void)msg_type;
	(void)msg_str;
};

void DebugLog(const __FlashStringHelper *log_message) {
	(void)log_message;
}

#endif
