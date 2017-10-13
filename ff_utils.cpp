/************************************************
 ff_utils.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Utility and System Functions
 ************************************************/


/************************************************
  Includes
************************************************/
//#include <Arduino.h>

#include "ff_utils.h"
#include "ff_sys_config.h"
#include "ff_string_consts.h"
//#include <string.h>

#ifdef FF_SIMULATOR
#include <stdint.h>
#include <stdio.h>
#endif

/************************************************
 Data Structures
 ************************************************/


/************************************************
  Globals
************************************************/


/************************************************
  Utility and System Functions
************************************************/

uint8_t GetLanguage(void) {
	//TODO language switching functions
	return ENGLISH;
}

char const* GetMessageTypeString(int message_type_enum) {
	return message_type_strings[message_type_enum].text[GetLanguage()];
}

char const* GetMessageString(int message_enum) {
	return message_strings[message_enum].text[GetLanguage()];
}


#ifdef FF_ARDUINO
char* FFFloatToCString(char* buf, float f) {
	return dtostrf(f,-7,2,buf);
}
#endif
#ifdef FF_SIMULATOR
char* FFFloatToCString(char* buf, float f) {
	sprintf(buf, "%f", f);
	return buf;
}
#endif


#ifdef FF_ARDUINO
String FFFloatString(float f) {
	String s;
	if (f<10) {
		s = " " + (String)f;
		return s;
	} else {
		s = (String)f;
		return s;
	}
}
#endif













