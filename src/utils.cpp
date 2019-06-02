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

#include <build_config.h>
#include <stdlib.h>
#include <string.h>
#include <string_consts.h>
#include <utils.h>

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

char* FlagToDayStr(char* day_str, uint8_t day_flag[7]) {
	uint8_t count = 0;
	uint8_t i = 0;

	strcpy(day_str, "\0");

	for (; i < 7; i++) {
		if (day_flag[i] == 1) {
			count++;
		}
	}

	if (count == 7) {
		strcpy_hal (day_str, F("All"));
		return day_str;
	} else {
		for (i = 0; i < 7; i++) {
			switch(i) {
				case 0:
					if (day_flag[i] == 1) {
						strcat_hal(day_str, F("SUN"));
						count--;
					}
					break;
				case 1:
					if (day_flag[i] == 1) {
						strcat_hal(day_str, F("MON"));
						count--;
					}
					break;
				case 2:
					if (day_flag[i] == 1) {
						strcat_hal(day_str, F("TUE"));
						count--;
					}
					break;
				case 3:
					if (day_flag[i] == 1) {
						strcat_hal(day_str, F("WED"));
						count--;
					}
					break;
				case 4:
					if (day_flag[i] == 1) {
						strcat_hal(day_str, F("THU"));
						count--;
					}
					break;
				case 5:
					if (day_flag[i] == 1) {
						strcat_hal(day_str, F("FRI"));
						count--;
					}
					break;
				case 6:
					if (day_flag[i] == 1) {
						strcat_hal(day_str, F("SAT"));
						count--;
					}
					break;
				default:
					break;
				if (count != 0) {
					strcat_hal(day_str, F(", "));
				}
			}
		}
	}
	return day_str;
}

uint8_t DayStrToFlag(uint8_t day_flag[7], const char* day_str) {
	// convert any combination of ALL, MON, TUE, WED, THU, FRI, SAT, SUN
	// into an array[7] of booleans
	// with reference to ff system day 0 = enum value (default SUN)

	uint8_t found = 0;
	if (strcasecmp(day_str, "ALL") == 0) {
		for (int i=0; i<7; i++) {
			day_flag[i] = 1;
		}
		found = 1;
	} else {
#ifdef USE_PROGMEM
		SimpleStringArray temp;
		for (int i = 0; i < LAST_DAY; i++ ) {
			memcpy_P(&temp, &day_strings[i], sizeof(temp));
			if (strcasestr(day_str, temp.text) != NULL) {
				day_flag[i] = 1;
				found = 1;
			} else {
				day_flag[i] = 0;
			}
		}
#else
		for (int i = 0; i < LAST_DAY; i++ ) {
			if (strcasestr(day_str, day_strings[i].text) != NULL) {
				day_flag[i] = 1;
				found = 1;
			} else {
				day_flag[i] = 0;
			}
		}
#endif

	}
	return found;
}

#ifdef FF_ARDUINO
char* FFFloatToCString(char* buf, float f) {
	return dtostrf(f,-7,2,buf);
}
#endif
#ifdef FF_SIMULATOR
char* FFFloatToCString(char* buf, float f) {
	sprintf(buf, "%.2f", f);
	return buf;
}
#endif


/*
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
*/












