/************************************************
 ff_HAL.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Hardware Abstraction Layer (HAL)
************************************************/


/************************************************
 Includes
************************************************/
#include "ff_HAL.h"
#include "ff_sys_config.h"
#include "ff_datetime.h"
#include "ff_registry.h"
#include "ff_utils.h"
#include "ff_string_consts.h"
#include "ff_events.h"
#include "ff_debug.h"

#ifdef FF_ARDUINO
#include <Arduino.h>
#include <OneWire.h>                  // comms to Dallas temprature sensors
#include <DallasTemperature.h>
#include <U8g2lib.h>                  // LCD display library
#include <U8x8lib.h>
#include "RTClib.h"
#include "SD.h"
#include <time.h>
#include "Wire.h"
#endif

#ifdef U8X8_HAVE_HW_SPI
#undef U8X8_HAVE_HW_SPI
//#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#undef U8X8_HAVE_HW_I2C
//#include <Wire.h>
#endif

#ifdef FF_SIMULATOR
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#endif

/************************************************
 Data Structures
************************************************/

/************************************************
 Globals
************************************************/
#ifdef FF_ARDUINO
//moved to local
//OneWire one_wire(ONE_WIRE_BUS);            		// oneWire instance to communicate with any OneWire devices
//DallasTemperature temp_sensors(&one_wire);     // Pass our one_wire reference to Dallas Temperature

//moving this to local too (try - result: flashing. moved back)
U8G2_ST7920_128X64_1_SW_SPI lcd_128_64(U8G2_R0, /* clock=*/ 40 /* A4 */ , /* data=*/ 42 /* A2 */, /* CS=*/ 44 /* A3 */, /* reset=*/ U8X8_PIN_NONE); //LCD Device Object and Definition
//U8X8_ST7920_128X64_SW_SPI(uint8_t clock, uint8_t data, uint8_t cs, uint8_t reset = U8X8_PIN_NONE)
//trying 8x8 version - no frame buffer!!! less RAM
//U8X8_ST7920_128X64_SW_SPI lcd_128_64(40, 42, 44, U8X8_PIN_NONE);

//U8X8_ST7920_128X64_SW_SPI u8x8(40, 42, 44, U8X8_PIN_NONE);

RTC_DS1307 rtc;
#endif

uint8_t rtc_status = 0;		// assume dead until we can find it and talk to it

#ifdef FF_ARDUINO
#endif
#ifdef FF_SIMULATOR
#endif

/************************************************
 Functions
************************************************/

uint8_t HALSaveEventBuffer(void) {

	uint8_t save_success = 0;

#ifdef FF_ARDUINO
	File e_file;

	// make sure that the default chip select pin is set to
	// output, even if you don't use it:
	//const int chipSelect = 10;
	pinMode(SS, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(53, OUTPUT);
	// see if the card is present and can be initialized:
	if (SD.begin(10, 11, 12, 13)) {
		//DebugLog("DEBUG INFO sd.begin"); //cant use EventMsg

		EventNode* e;
		char e_str[MAX_DEBUG_LENGTH];
		char ymd_str[12];
		char hms_str[12];
		const char* source_str;
		const char* msg_type_str;
		const char* msg_str;
		char float_str[20];

		if (!EventBufferEmpty()) {
			e_file = SD.open(EVENT_FILENAME, FILE_WRITE);
			if (e_file) {
				//DebugLog("DEBUG INFO SD.open");
				while (!EventBufferEmpty()) {

					e = EventBufferPop();

					FFDateCString((char *)ymd_str, e->time_stamp);
					FFTimeCString((char *)hms_str, e->time_stamp);
					source_str = GetBlockLabelString(e->source);
					msg_type_str = GetMessageTypeString(e->message_type);
					msg_str = GetMessageString(e->message);
					FFFloatToCString((char *)float_str, e->float_val);

					sprintf(e_str,"%s, %s, %s, %s, %s, %d, %s",  ymd_str, hms_str, source_str, msg_type_str, msg_str, e->int_val, float_str);

					e_file.println(e_str);

				}
				e_file.flush();
				e_file.close();
				//DebugLog("DEBUG INFO Event Buffer Saved");
				save_success = 1;
			} else {
				DebugLog(GetMessageString(M_SD_NO_FILE_HANDLE));
			}

		} else {
			DebugLog(GetMessageString(M_ERROR_EVENTS_EMPTY));
		}

		SD.end();
		//DebugLog("DEBUG INFO SD.end");
	} else {
		DebugLog(GetMessageString(M_SD_BEGIN_FAIL));
	}
#endif
#ifdef FF_SIMULATOR
	FILE *e_file;
	EventNode* e;
	if (!EventBufferEmpty()) {
		e_file = fopen(EVENT_FILENAME, "w");
		if (e_file) {
			DebugLog("File open GOOD, have file handle");
			while (!EventBufferEmpty()) {

				e = EventBufferPop();
				char time_str[12];
				char date_str[14];
				strftime(date_str, 14, "%Y-%m-%d", localtime(&(e->time_stamp)));
				strftime(time_str, 12, "%H:%M:%S", localtime(&(e->time_stamp)));
				fprintf(e_file, "%s,", date_str);
				fprintf(e_file, "%s,", time_str);
				fprintf(e_file, "%s,", GetBlockLabelString(e->source));
				fprintf(e_file, "%s,", GetMessageTypeString(e->message_type));
				fprintf(e_file, "%s,", GetMessageString(e->message));
				fprintf(e_file, "%d,", e->int_val);
				fprintf(e_file, "%f\n", e->float_val);
			}
			fclose(e_file);
			DebugLog(SSS, INFO, M_BUF_SAVED, 0, 0);
			save_success = 1;
		} else {
			DebugLog(SSS, ERROR, M_ERR_EVENT_FILE, 0, 0);
		}

	} else {
		DebugLog("ERROR Buffer empty");
	}
#endif

	return save_success;

}


#ifdef FF_ARDUINO
#ifdef DEBUG_LCD
void HALDebugLCD(String log_entry) {
	//U8G2_ST7920_128X64_F_SW_SPI lcd_128_64(U8G2_R0, /* clock=*/ 40 /* A4 */ , /* data=*/ 42 /* A2 */, /* CS=*/ 44 /* A3 */, /* reset=*/ U8X8_PIN_NONE); //LCD Device Object and Definition
	//lcd_128_64.begin();

	//lcd_128_64.clearBuffer();                     // clear the internal memory
	//lcd_128_64.clearDisplay();
	//  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
	lcd_128_64.setFont(u8g2_font_6x12_tr);
	lcd_128_64.firstPage();
	//lcd_128_64.setFont(u8x8_font_chroma48medium8_r);
	do {
	lcd_128_64.drawStr(0, 10, "                                "); // clear the screen
	lcd_128_64.drawStr(0, 20, "                                "); // clear the screen
	lcd_128_64.drawStr(0, 30, "                                "); // clear the screen
	lcd_128_64.drawStr(0, 40, "                                "); // clear the screen
	lcd_128_64.drawStr(0, 50, "                                "); // clear the screen
	lcd_128_64.drawStr(0, 60, "                                "); // clear the screen
	} while (lcd_128_64.nextPage());
	//lcd_128_64.sendBuffer();          // transfer internal memory to the display
//delay(1000);

	//lcd_128_64.clearBuffer();                     // clear the internal memory
//  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
	//lcd_128_64.setFont(u8g2_font_6x12_tr);
	//lcd_128_64.setFont(u8x8_font_chroma48medium8_r);
	int a, b, c, d, e;
	a = log_entry.indexOf(",");       //time stamp
	b = log_entry.indexOf(",", a + 1);  //source
	c = log_entry.indexOf(",", b + 1);  //msg type
	d = c + 23;  //msg
	e = d + 23;
// 170812 00:00:00, TEMP_1, DEBUG, Temparature is 23.56
// 0123456789012345678901234567890123456789012345678901234567890
// 0              a       b      c
	lcd_128_64.firstPage();
	do {
	lcd_128_64.drawStr(0, 10, log_entry.substring(0, a).c_str()); // write something to the internal memory (forcing String Obj to be a char string)
	lcd_128_64.drawStr(0, 20, log_entry.substring(a + 2, b).c_str()); // write something to the internal memory (forcing String Obj to be a char string)
	lcd_128_64.drawStr(0, 30, log_entry.substring(b + 2, c).c_str()); // write something to the internal memory (forcing String Obj to be a char string)
	lcd_128_64.drawStr(0, 40, log_entry.substring(c + 2, d).c_str()); // write something to the internal memory (forcing String Obj to be a char string)
	lcd_128_64.drawStr(0, 50, log_entry.substring(d, e).c_str()); // write something to the internal memory (forcing String Obj to be a char string)
	lcd_128_64.drawStr(0, 60, log_entry.substring(e).c_str()); // write something to the internal memory (forcing String Obj to be a char string)
	} while (lcd_128_64.nextPage());

	//lcd_128_64.sendBuffer();          // transfer internal memory to the display
	delay(DEBUG_LCD_DELAY);
}
#endif
#endif


float GetTemperature(int if_num) {
	float temp_c;
#ifdef FF_ARDUINO
	//try this as a local each time called to save global memory
	OneWire one_wire(ONE_WIRE_BUS);            		// oneWire instance to communicate with any OneWire devices
	DallasTemperature temp_sensors(&one_wire);     // Pass our one_wire reference to Dallas Temperature
	temp_sensors.begin();
	temp_sensors.requestTemperatures();  //tell them to take a reading (stored on device)

	//original code
	temp_c = temp_sensors.getTempCByIndex(if_num);
#endif
#ifdef FF_TEMP_SIM

	switch (if_num) {
	case 0:
		temp_c = SIM_TEMP_0;
#ifdef FF_ARDUINO
		temp_c = random(5.01, 39.99);
#endif
		break;
	case 1:
		temp_c = SIM_TEMP_1;
#ifdef FF_ARDUINO
		temp_c = random(5.01, 39.99);
#endif
		break;
	case 2:
		temp_c = SIM_TEMP_2;
#ifdef FF_ARDUINO
		temp_c = random(5.01, 39.99);
#endif
		break;
	case 3:
		temp_c = SIM_TEMP_2;
#ifdef FF_ARDUINO
		temp_c = random(5.01, 39.99);
#endif
		break;
	default:
		temp_c = -50;
	}
#endif
	return temp_c;
}

void HALDigitalWrite (uint8_t if_num, uint8_t digital_val) {
#ifdef FF_ARDUINO
	digitalWrite(if_num, digital_val);
#endif
#ifdef FF_SIMULATOR
#endif
}


void HALInitDigitalOutput (uint8_t if_num) {
#ifdef FF_ARDUINO
	pinMode(if_num, OUTPUT);
#endif

#ifdef FF_SIMULATOR
#endif

}

void TempSensorsTakeReading(void) {
	#ifdef FF_ARDUINO
	//temp_sensors.requestTemperatures();  //tell them to take a reading (stored on device)
	#endif
}

void InitTempSensors(void) {
	#ifdef FF_ARDUINO
	//Dalas temp sensor
	//temp_sensors.begin();
	#endif
}

void HALInitUI(void) {
#ifdef FF_ARDUINO
	lcd_128_64.begin();
#endif
#ifdef FF_SIMULATOR
#endif
}


void HALDrawDataScreenCV(const UIDataSet* uids, time_t dt) {
#ifdef FF_ARDUINO
	//U8G2_ST7920_128X64_F_SW_SPI lcd_128_64(U8G2_R0, /* clock=*/ 40 /* A4 */ , /* data=*/ 42 /* A2 */, /* CS=*/ 44 /* A3 */, /* reset=*/ U8X8_PIN_NONE); //LCD Device Object and Definition
	//lcd_128_64.begin();

	//lcd_128_64.clearBuffer();                     // clear the internal memory
	//lcd_128_64.clearDisplay();
	lcd_128_64.setFont(u8g2_font_6x12_tr);
	//lcd_128_64.setFont(u8x8_font_chroma48medium8_r);
	//template with u8g2_font_6x12_tr font
	//lcd_128_64.drawStr(0, 10, "16:23 IN   OUT  WATER");
	//lcd_128_64.drawStr(0, 20, "NOW 20.23 20.45 25.34");
	//lcd_128_64.drawStr(0, 30, "MIN 20.23 20.45 25.34");
	//lcd_128_64.drawStr(0, 40, "    03:24 04:15 05:00");
	//lcd_128_64.drawStr(0, 50, "MAX 20.23 20.45 25.34");
	//lcd_128_64.drawStr(0, 60, "    03:24 04:15 05:00");

	char str_buf[8]; //assuming that lcd extracts display chars from the string and builds a separate buffer.

	lcd_128_64.firstPage();
	do {
	lcd_128_64.drawStr(0, 7, FFShortTimeCString(str_buf, dt));
	lcd_128_64.drawStr(0, 18, "");
	lcd_128_64.drawStr(0, 31, "Min");
	lcd_128_64.drawStr(0, 41, "  @");
	lcd_128_64.drawStr(0, 54, "Max");
	lcd_128_64.drawStr(0, 64, "  @");

	lcd_128_64.drawStr(43, 7, "IN");
	lcd_128_64.drawStr(25, 18, FFFloatToCString(str_buf, uids->inside_current));
	lcd_128_64.drawStr(25, 31, FFFloatToCString(str_buf, uids->inside_min));
	lcd_128_64.drawStr(25, 41, FFShortTimeCString(str_buf, uids->inside_min_dt));
	lcd_128_64.drawStr(25, 54, FFFloatToCString(str_buf, uids->inside_max));
	lcd_128_64.drawStr(25, 64, FFShortTimeCString(str_buf, uids->inside_max_dt));

	lcd_128_64.drawStr(72, 7, "OUT");
	lcd_128_64.drawStr(60, 18, FFFloatToCString(str_buf, uids->outside_current));
	lcd_128_64.drawStr(60, 31, FFFloatToCString(str_buf, uids->outside_min));
	lcd_128_64.drawStr(60, 41, FFShortTimeCString(str_buf, uids->outside_min_dt));
	lcd_128_64.drawStr(60, 54, FFFloatToCString(str_buf, uids->outside_max));
	lcd_128_64.drawStr(60, 64, FFShortTimeCString(str_buf, uids->outside_max_dt));

	lcd_128_64.drawStr(110, 7, "WAT");
	lcd_128_64.drawStr(95, 18, FFFloatToCString(str_buf, uids->water_current));
	lcd_128_64.drawStr(95, 31, FFFloatToCString(str_buf, uids->water_min));
	lcd_128_64.drawStr(95, 41, FFShortTimeCString(str_buf, uids->water_min_dt));
	lcd_128_64.drawStr(95, 54, FFFloatToCString(str_buf, uids->water_max));
	lcd_128_64.drawStr(95, 64, FFShortTimeCString(str_buf, uids->water_max_dt));
	} while (lcd_128_64.nextPage());

	//lcd_128_64.sendBuffer();                      // transfer internal memory to the display
#endif
#ifdef FF_SIMULATOR
	char time_now_str[10];
	char time_in_min[10];
	char time_out_min[10];
	char time_wat_min[10];
	char time_in_max[10];
	char time_out_max[10];
	char time_wat_max[10];

	strftime(time_now_str, 10, "%H:%M", localtime(&dt));
	strftime(time_in_min, 10, "%H:%M", localtime(&(uids->inside_min_dt)));
	strftime(time_out_min, 10, "%H:%M", localtime(&(uids->outside_min_dt)));
	strftime(time_wat_min, 10, "%H:%M", localtime(&(uids->water_min_dt)));
	strftime(time_in_max, 10, "%H:%M", localtime(&(uids->inside_max_dt)));
	strftime(time_out_max, 10, "%H:%M", localtime(&(uids->outside_max_dt)));
	strftime(time_wat_max, 10, "%H:%M", localtime(&(uids->water_max_dt)));


	//printf("\e[1;1H\e[2J"); //clear screen
	for (int n = 0; n<20; n++) {
		printf("\n");
	}
	printf("%s IN   OUT  WATER\n", time_now_str);
	printf("NOW %f %f %f\n", uids->inside_current, uids->outside_current, uids->water_current);
	printf("MIN %f %f %f\n", uids->inside_min, uids->outside_min, uids->water_min);
	printf("    %s %s %s\n", time_in_min, time_out_min, time_wat_min);
	printf("MAX %f %f %f\n", uids->inside_max, uids->outside_max, uids->water_max);
	printf("    %s %s %s\n", time_in_max, time_out_max, time_wat_max);
#endif
}


FFDateTime HALFFDTNow(void) {
	FFDateTime dt;

#ifdef FF_ARDUINO
	if(rtc.isrunning()) {
		DateTime rtcDT;
		rtcDT = rtc.now();
		dt.year = rtcDT.year();
		dt.month = rtcDT.month();
		dt.day = rtcDT.day();
		dt.hour = rtcDT.hour();
		dt.minute = rtcDT.minute();
		dt.second = rtcDT.second();
	} else {
		dt.year = 0;
		dt.month = 0;
		dt.day = 0;
		dt.hour = 0;
		dt.minute = 0;
		dt.second = 0;
	}
	return dt;
#endif
#ifdef FF_SIMULATOR

	time_t sys_time  = time(0);           // get time now
	struct tm* local_time_ptr  = localtime( & sys_time ); // convert to local time
	struct tm  local_time = *local_time_ptr;              // make a local copy.

	dt.year = local_time.tm_year +1900; //tm_year The number of years since 1900.
	dt.month = local_time.tm_mon +1;    //tm_mon The number of months since January, in the range 0 to 11.
	dt.day = local_time.tm_mday;
	dt.hour = local_time.tm_hour;
	dt.minute = local_time.tm_min;
	dt.second = local_time.tm_sec;
	return dt;
#endif
}

void HALInitRTC(void) {
#ifdef FF_ARDUINO
	//setup the Real Time Clock
	//TODO check for sensible RTC result - in all cases the RTC value should be later
	// than the compile time date time hard coded directives
	//TODO substitute this section and all time references to use system time rather than RTC
	// and sync system time to RTC periodically - that way time still progresses even if RTC broken
	Wire.begin();
	if (rtc.begin()) {
		EventMsg(SSS, DEBUG_MSG, M_RTC_DETECT, 0, 0);
		if (rtc.isrunning()) {
			EventMsg(SSS, DEBUG_MSG, M_RTC_REPORT_RUNNING, 0, 0);
#ifdef SET_RTC
			EventMsg(SSS, WARNING, M_WARN_SET_RTC, 0, 0);
			// following line sets the RTC to the date & time this sketch was compiled
			rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
			EventMsg(SSS, WARNING, M_WARN_RTC_HARD_CODED, 0, 0);
#endif
		} else {
			EventMsg(SSS, WARNING, M_RTC_NOT_RUNNING, 0, 0);
			// following line sets the RTC to the date & time this sketch was compiled
			rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
			EventMsg(SSS, WARNING, M_WARN_RTC_HARD_CODED, 0, 0);
		};
		rtc_status = 1;
	} else
		EventMsg(SSS, ERROR, M_RTC_NOT_FOUND, 0, 0);
	#endif
#ifdef FF_SIMULATOR
	EventMsg(SSS, INFO, M_SIM_SYS_TIME, 0, 0);
#endif
}


