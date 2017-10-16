/************************************************
 ff_datetime.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Fodder Factory Specific Date and Time
 ************************************************/

/************************************************
 Includes
 ************************************************/

#include "ff_sys_config.h"

#ifdef FF_ARDUINO
#include <Arduino.h>
//#include "RTClib.h"
//#include "Time.h"
//#include "Wire.h"
#endif

#ifdef FF_SIMULATOR
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#endif

#include "ff_datetime.h"
#include "ff_string_consts.h"
#include "ff_HAL.h"


//#include "ff_utils.h"

/************************************************
 Data Structures
 ************************************************/

/*
enum enumMonthDays {
	NOMONTH = 0,
	JANUARY = 31,
	FEBRUARY = 28,
	MARCH = 31,
	APRIL = 30,
	MAY = 31,
	JUNE = 30,
	JULY = 31,
	AUGUST = 31,
	SEPTEMBER = 30,
	OCTOBER = 31,
	DECEMBER = 31
};
*/


/************************************************
  Globals
************************************************/

//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

uint8_t MonthDays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/************************************************
  Date and Time Functions
************************************************/

/*

FFDateTime FFDT(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
	FFDateTime dt;
	dt.year = year;
	dt.month = month;
	dt.day = day;
	dt.hour = hour;
	dt.minute = min;
	dt.second = sec;
	return dt;
}
*/

uint8_t FFDTGrEq(FFDateTime a, FFDateTime b) {
	if (a.year > b.year) {
		return 1;
	} else {
		if (a.year < b.year) {
			return 0;
		} else { // must be same year
			if (a.month > b.month) {
				return 1;
			} else {
				if (a.month < b.month) {
					return 0;
				} else { //must be same month
					if (a.day > b.day) {
						return 1;
					} else {
						if (a.day < b.day) {
							return 0;
						} else { //must be same day
							if (a.hour > b.hour) {
								return 1;
							} else {
								if (a.hour < b.hour) {
									return 0;
								} else { // must be same hour
									if (a.minute > b.minute) {
										return 1;
									} else {
										if (a.minute < b.minute) {
											return 0;
										} else { //must be same minute
											if (a.second > b.second) {
												return 1;
											} else {
												if (a.second < b.second) {
													return 0;
												} else { // must be the same time!
													return 1;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

FFDateTime FFDTAdd(FFDateTime a, FFDateTime b) {
	//TODO Handle Leap years
	FFDateTime dt;
	dt.year = a.year;
	dt.month = a.month;
	dt.day = a.day;
	dt.hour = a.hour;
	dt.minute = a.minute;
	dt.second = a.second;

	dt.second = dt.second + b.second;
	while (dt.second > 59) {
		dt.minute++;
		dt.second = dt.second-60;
	}
	dt.minute = dt.minute + b.minute;
	while (dt.minute > 59) {
		dt.hour++;
		dt.minute = dt.minute-60;
	}
	dt.hour = dt.hour + b.hour;
	while (dt.hour > 23) {
		dt.day++;
		dt.hour = dt.hour-24;
	}
	dt.day = dt.day + b.day;
	while (dt.day > MonthDays[dt.month]) {
		dt.day = dt.day - MonthDays[dt.month];
		if(dt.month != 12) {
			dt.month++;
		} else {
			dt.month=1;
			dt.year++;
		}
	}
	dt.month = dt.month + b.month;
	while (dt.month > 12) {
		dt.year++;
		dt.month = dt.month-12;
	}
	dt.year = dt.year + b.year;
	return dt;
}

char* FFShortTimeCString(char* hm_str, FFDateTime dt) {
	uint8_t h, m;
	char sh[6];
	char sm[6];
	h = dt.hour;
	m = dt.minute;
	sprintf(sh, "%d", h);
	sprintf(sm, "%d", m);
	if (h >= 10) {
		strcpy(hm_str, sh);  //sets it up as a string
	} else {
		strcpy(hm_str, "0"); //sets it up as a string
		strcat(hm_str, sh);
	}
	strcat(hm_str, ":");
	if (m >= 10) {
		strcat(hm_str, sm);
	} else {
		strcat(hm_str, "0");
		strcat(hm_str, sm);
	}
	return hm_str;
}

char* FFTimeCString(char* hms_str, FFDateTime dt) {
	uint8_t h, m, s;
	char sh[6];
	char sm[6];
	char ss[6];
	h = dt.hour;
	m = dt.minute;
	s = dt.second;
	sprintf(sh, "%d", h);
	sprintf(sm, "%d", m);
	sprintf(ss, "%d", s);

	if (h >= 10) {
		strcpy(hms_str, sh);  //sets it up as a string
	} else {
		strcpy(hms_str, "0"); //sets it up as a string
		strcat(hms_str, sh);
	}

	strcat(hms_str, ":");

	if (m >= 10) {
		strcat(hms_str, sm);
	} else {
		strcat(hms_str, "0");
		strcat(hms_str, sm);
	}

	strcat(hms_str, ":");

	if (s >= 10) {
		strcat(hms_str, ss);
	} else {
		strcat(hms_str, "0");
		strcat(hms_str, ss);
	}

	return hms_str;
}

char* FFDateCString(char* ymd_str, FFDateTime dt) {
	uint16_t y;
	uint8_t m, d;
	char ys[6];
	char ms[4];
	char ds[4];
	y = dt.year;
	m = dt.month;
	d = dt.day;

	sprintf(ys, "%d", y);

	m >= 10 ? sprintf(ms, "%d", m) : sprintf(ms, "0%d", m);

	d >= 10 ? sprintf(ds, "%d", d) : sprintf(ds, "0%d", d);

	sprintf(ymd_str, "%s-%s-%s", ys, ms, ds);
	return ymd_str;
}


FFDateTime FFDTNow(void) {
	return HALFFDTNow();
}

char* FFDateTimeCStringNow (char* dt_str) {
	FFDateTime dt = FFDTNow();
	char date[12];				//yyyy-mm-dd\0
	char time[10];				//hh:mm:dd\9
	FFDateCString(date, dt);
	FFTimeCString(time, dt);
	sprintf(dt_str, "%s, %s", date, time);
	return dt_str;
}

#ifdef FF_ARDUINO
String FFShortTimeString(FFDateTime dt) {
	uint8_t h, m;
	String sh, sm, shm;
	h = dt.hour;
	m = dt.minute;
	if (h >= 10) {
		sh = (String)h;
	} else {
		sh = "0" + (String)h;
	}

	if (m >= 10) {
		sm = (String)m;
	} else {
		sm = "0" + (String)m;
	}
	shm = sh + ":" + sm;
	return shm;
}

String FFDateString(FFDateTime dt) {
	uint16_t y;
	uint8_t m, d;
	String sy, sm, sd, sdate;

	y = dt.year;
	m = dt.month;
	d = dt.day;

	sy = (String)y;

	if (m >= 10) {
		sm = (String)m;
	} else {
		sm = "0" + (String)m;
	}

	if (d >= 10) {
		sd = (String)d;
	} else {
		sd = "0" + (String)d;
	}

	sdate = sy + "-" + sm + "-" + sd;
	return sdate;
}

/*
FFDateTime DateTimeToFFDateTime(DateTime rtcDT) {
	FFDateTime dt;
	dt.year = rtcDT.year();
	dt.month = rtcDT.month();
	dt.day = rtcDT.day();
	dt.hour = rtcDT.hour();
	dt.minute = rtcDT.minute();
	dt.second = rtcDT.second();
	return dt;
}
*/

/*
String FFDateTimeStringNow(void) {
	// Generate a date and time stamp as a string
	FFDateTime dt = FFDTNow();
	String date = FFDateString(dt);
	String time = FFShortTimeString(dt);
	return date + " " + time;
}
*/

#endif

void InitRTC(void) {		//setup the Real Time Clock
	HALInitRTC();
}



