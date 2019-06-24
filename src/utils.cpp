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
#include <debug_ff.h>

#ifdef PLATFORM_LINUX
#include <stdint.h>
#include <stdio.h>
#include <time.h>
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

uint8_t VarianceExceedsPercent(int32_t old_val, int32_t new_val, uint8_t thres_pc) {
	int32_t var_abs;
	int32_t thres_abs;
	var_abs = old_val - new_val;
	var_abs = var_abs * ((var_abs > 0) - (var_abs < 0));

	thres_abs = old_val * thres_pc / 100;
	thres_abs = thres_abs * ((thres_abs > 0) - (thres_abs < 0));

	return (var_abs > thres_abs);
}

uint8_t VarianceExceedsAbsolute(int32_t old_val, int32_t new_val, uint16_t thres_abs) {
	int32_t var_abs;
	var_abs = old_val - new_val;
	var_abs = var_abs * ((var_abs > 0) - (var_abs < 0));
	return (var_abs > thres_abs);
}


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

#ifdef PLATFORM_ARDUINO
char* FFFloatToCString(char* buf, float f) {
	return dtostrf(f,-7,2,buf);
}
#endif //PLATFORM_ARDUINO
#ifdef PLATFORM_LINUX
char* FFFloatToCString(char* buf, float f) {
	sprintf(buf, "%.2f", f);
	return buf;
}
#endif

char* CTimeToISODateTimeString(char *iso_str, time_t t) {
	char ymd_str[14];
	char hms_str[12];
	char fmt_str[9];

	strcpy_hal(fmt_str, F("%Y-%m-%d"));
	strftime(ymd_str, 14, fmt_str, localtime(&(t)));
	strcpy_hal(fmt_str, F("%H:%M:%S"));
	strftime(hms_str, 12, fmt_str, localtime(&(t)));

	strcpy_hal(fmt_str, F("%s, %s"));
	sprintf(iso_str, fmt_str, ymd_str, hms_str);

	return iso_str;
}

char* CTimeToLocalTimeString(char *t_str, time_t t) {
	char hms_str[12];
	char fmt_str[9];

	strcpy_hal(fmt_str, F("%H:%M:%S"));
	strftime(hms_str, 12, fmt_str, localtime(&(t)));

	strcpy_hal(fmt_str, F("%s"));
	sprintf(t_str, fmt_str, hms_str);

	return t_str;
}

TV_TYPE StringToTimeValue(const char* time_str) {
	int hh, mm, ss;
	TV_TYPE tv = 0;
	char fmt_str[10];

	strcpy_hal(fmt_str, F("%d:%d:%d"));
	if (sscanf(time_str, fmt_str, &hh, &mm, &ss) == 3) {
		tv = ss + (mm * 60) + (hh * 60 * 60);
		return tv;
	} else {
		DebugLog(SSS, E_WARNING, M_BAD_TIME_STR);
		tv = UINT32_INIT;
		return tv;
	}
}

char* TimeValueToTimeString(char *HMSStr, const time_t tv) {
	//char hms_str[12];
	char fmt_str[9];
	strcpy_hal(fmt_str, F("%H:%M:%S"));
	//strftime(HMSStr, 12, fmt_str, localtime(&tv));
	strftime(HMSStr, 12, fmt_str, gmtime(&tv));
	return HMSStr;
}



/*
#ifdef PLATFORM_ARDUINO
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












