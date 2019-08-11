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
#include <HAL.h>

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
	return dtostrf(f,-4,2,buf);
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

/* interesting snippet:
currentMillis = millis();
if ( (currentMillis - previousMillis) >= 1000){
previousMillis = previousMillis + 1000;
seconds = seconds +1;
if (seconds == 60){
seconds = 0;
minutes = minutes +1;
if(minutes == 60){
minutes = 0;
hours = hours +1;
if (hours == 24){
hours = 0;
} // end hrs check
} // end minutes check
} // end seconds check
} // end time check
*/

uint32_t StringToTimeValue(const char* time_str) {
	int hh, mm, ss;
	uint32_t tv = 0;
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

uint32_t NextTime(uint32_t sch_start, uint32_t sch_repeat) {
	// Takes a start, repeat time schedule and returns the next
	// 	scheduled start time relative to time now.
	// Inputs:
	//  sch_start: seconds since zero (00:00:00) today
	//  sch_repeat: seconds
	// Returns: time_t of next start (unix/epoch time - varies by platform)

	struct tm *zero_today_tm;
	time_t zero_today_t;
	time_t now_t;

	//char debug[50];

	#ifdef PLATFORM_ARDUINO
		now_t = TimeNow();
	#else
		now_t = time(NULL);
	#endif //PLATFORM_ARDUINO

	zero_today_tm = localtime(&now_t);
	zero_today_tm->tm_hour = 0;
	zero_today_tm->tm_min = 0;
	zero_today_tm->tm_sec = 0;

	zero_today_t = mktime(zero_today_tm);

	uint32_t secs_since_zero_today;
	uint32_t last_period_num;
	uint32_t next_period_start_t;

	secs_since_zero_today = now_t - zero_today_t;
	last_period_num = (secs_since_zero_today - sch_start) / sch_repeat;
	next_period_start_t = zero_today_t + sch_start + ((last_period_num +1) * sch_repeat);

	// For Tests
	//printf("Time now_t: %s\n", CTimeToISODateTimeString(debug, now_t));
	//printf("Zero Today_t: %s\n", CTimeToISODateTimeString(debug, zero_today_t));
	//printf("Last Period Num: %d\n", last_period_num);
	//printf("Next Period Start_t: %s\n", CTimeToISODateTimeString(debug, next_period_start_t));

	return next_period_start_t;
}

uint32_t LastTime(uint32_t sch_start, uint32_t sch_repeat) {
	// Takes a start, repeat time schedule and returns the most
	// 	recent scheduled start time relative to time now.
	// Inputs:
	//  sch_start: seconds since zero (00:00:00) today
	//  sch_repeat: seconds
	// Returns: time_t of last scheduled start (unix/epoch time - varies by platform)

	struct tm *zero_today_tm;
	time_t zero_today_t;
	time_t now_t;

	#ifdef PLATFORM_ARDUINO
	now_t = TimeNow();
	#else
	now_t = time(NULL);
	#endif //PLATFORM_ARDUINO

	zero_today_tm = localtime(&now_t);
	zero_today_tm->tm_hour = 0;
	zero_today_tm->tm_min = 0;
	zero_today_tm->tm_sec = 0;

	zero_today_t = mktime(zero_today_tm);

	uint32_t secs_since_zero_today;
	uint32_t last_period_num;
	uint32_t last_period_start_t;

	secs_since_zero_today = now_t - zero_today_t;
	last_period_num = (secs_since_zero_today - sch_start) / sch_repeat;
	last_period_start_t = zero_today_t + sch_start + (last_period_num * sch_repeat);

	return last_period_start_t;
}

uint8_t DallasAddressStringToArray(const char *addr_str, uint8_t addr[8]) {
	// Form is: 28:FF:68:23:85:16:03:6C
	char cpy[24];
	strncpy(cpy, addr_str, 24);
	// terminate it in case it overran
	cpy[23] = '\0';

	char* tok;
	char* res;

	tok = strtok(cpy, ":");
	for (uint8_t i = 0; i < 8; i++) {
		if (tok != NULL) {
			addr[i] = strtol(tok, &res, 16);
			if (*res != '\0') return 0; //error
			tok = strtok(NULL, ":");
		} else {
			// tok == NULL
			return 0;
		}
	}
	return 1;
}

char* DallasAddressArrayToString(char *addr_str, uint8_t addr[8]) {
	addr_str[0] = '\0';
	char temp[3] = "\0";

	for (uint8_t i = 0; i < 8; i++) {
	    // zero pad the address if necessary
	    if (addr[i] < 16) {
	    	strcat(addr_str, "0");
	    }
	    sprintf(temp, "%x", addr[i]);
	    strcat(addr_str, temp);
	    if (i < 7) {
	    	strcat(addr_str, ":");
	    }
	  }
	return addr_str;
}

uint8_t DallasCompare(uint8_t a1[8], uint8_t a2[8]) {
	for (uint8_t i = 0; i < 8; i++) {
		if (a1[i] != a2[i]) return 1;
	}
	return 0;
}

void DallasCopy(uint8_t s[8], uint8_t d[8]) {
	for (uint8_t i = 0; i < 8; i++) {
		d[i] = s[i];
	}
}

char *strpad(char *s, uint8_t w) {
	// ***** Untested *****
	// Add spaces to a null terminated string until it is
	// w length. Re-terminate.
	char *i = s;
	while (i < (s+w)) {
		if (*i == '\0') {
			*i++ = ' ';
			*i = '\0';
		}
	}
	return s;
}

char *strcatpad_hal(char *d, const char *s, uint8_t w) {
	// Concatenate s to d with trailing spaces for a total
	// field width of s being w long.
	#ifdef PLATFORM_ARDUINO
		int16_t pad = w - strlen_P(s);
		if (pad < 0) {
			// String is longer than field width
			// Only concatenate w bytes of s
			strncat_P(d, s, w);
			return d;
		} else {
			strcat_P(d, s);
		}
	#else
		int16_t pad = w - strlen(s);
		if (pad < 0) {
			// String is longer than field width
			// Only concatenate w bytes of s
			strncat(d, s, w);
			return d;
		} else {
			strcat(d, s);
		}
	#endif //PLATFORM_ARDUINO
	char *i = d;
	while (*i != '\0') {
		i++;
	}
	for (; pad-- > 0 ;) {
		*i++ = ' ';
		*i = '\0';
	}
	return d;
}

char *strcatpad(char *d, const char *s, uint8_t w) {
	// Concatenate s to d with trailing spaces for a total
	// field width of s being w long.

	int16_t pad = w - strlen(s);
	if (pad < 0) {
		// String is longer than field width
		// Only concatenate w bytes of s
		strncat(d, s, w);
		return d;
	} else {
		strcat(d, s);
	}

	char *i = d;
	while (*i != '\0') {
		i++;
	}
	for (; pad-- > 0 ;) {
		*i++ = ' ';
		*i = '\0';
	}
	return d;
}

#ifdef PLATFORM_ARDUINO
char *strcatpad_hal(char *d, const __FlashStringHelper *s, uint8_t w) {
	// Arduino overload where F macro is used
	// Concatenate s to d with trailing spaces for a total
	// field width of s being w long.
	return strcatpad_hal(d, (const char *)s, w);
}
#endif //PLATFORM_ARDUINO

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












