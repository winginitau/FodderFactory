/************************************************
 ff_datetime.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Fodder Factory Specific Date and Time
 ************************************************/

#ifndef FF_DATETIME_H_
#define FF_DATETIME_H_

#include "ff_sys_config.h"

#ifdef FF_ARDUINO
#include <Arduino.h>
#endif

#ifdef FF_SIMULATOR
#include <stdint.h>
#endif




/************************************************
 Data Structures
 ************************************************/

typedef struct FF_DATE_TIME {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} FFDateTime;

typedef struct FF_TIME {
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} FFTime;


/************************************************
 Function prototypes
 ************************************************/
/*
FFDateTime DateTimeToFFDateTime(DateTime rtcDT);
*/
//FFDateTime FFDT(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);

uint8_t FFDTGrEq(FFDateTime a, FFDateTime b);

FFDateTime FFDTAdd(FFDateTime a, FFDateTime b);

FFDateTime FFDTNow(void);

char* FFShortTimeCString(char* hm_str, FFDateTime dt);

char* FFTimeCString(char* hms_str, FFDateTime dt);

char* FFDateCString(char* ymd_str, FFDateTime dt);

char* FFDateTimeCStringNow (char* dt_str);

#ifdef FF_ARDUINO
//String FFDateTimeStringNow(void);

String FFShortTimeString(FFDateTime dt);

String FFDateString(FFDateTime dt);
#endif


void InitRTC(void);

#endif /* FF_DATETIME_H_ */
