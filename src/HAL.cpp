/************************************************
 ff_HAL.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Hardware Abstraction Layer (HAL)
************************************************/


/************************************************
 Includes
************************************************/
#include <build_config.h>
#include <datetime_ff.h>
#include <debug_ff.h>
#include <events.h>
#include <HAL.h>
#include <registry.h>
#include <string_consts.h>
#include <utils.h>

#ifdef PLATFORM_ARDUINO
#include <Arduino.h>
#include <OneWire.h>                // comms to Dallas temprature sensors
#include <DallasTemperature.h>
#include <RTClib.h>
#include <SdFat.h>
#include <time.h>
#include "Wire.h"					// RTC talks ober i2c "Wire"
#include <avr/wdt.h>				// Watchdog timer for reboot routine
#endif //PLATFORM_ARDUINO

#ifdef ARDUINO_LCD
#include <U8g2lib.h>				// LCD display library
#include <U8x8lib.h>
#endif // ARDUINO_LCD

#ifdef USE_ITCH
#include <itch.h>
#endif

#ifdef U8X8_HAVE_HW_SPI
#undef U8X8_HAVE_HW_SPI
//#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#undef U8X8_HAVE_HW_I2C
//#include <Wire.h>
#endif

#ifdef PLATFORM_LINUX
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>	// for sleep() and usleep() in simulator modes
#endif

/************************************************
 Data Structures
************************************************/

/************************************************
 Globals
************************************************/
#ifdef PLATFORM_ARDUINO
#ifdef ARDUINO_LCD
//moving this to local too (try - result: flashing. moved back)
U8G2_ST7920_128X64_1_SW_SPI lcd_128_64(U8G2_R0, /* clock=*/ 40 /* A4 */ , /* data=*/ 42 /* A2 */, /* CS=*/ 44 /* A3 */, /* reset=*/ U8X8_PIN_NONE); //LCD Device Object and Definition
//U8X8_ST7920_128X64_SW_SPI(uint8_t clock, uint8_t data, uint8_t cs, uint8_t reset = U8X8_PIN_NONE)
//trying 8x8 version - no frame buffer!!! less RAM
//U8X8_ST7920_128X64_SW_SPI lcd_128_64(40, 42, 44, U8X8_PIN_NONE);
//U8X8_ST7920_128X64_SW_SPI u8x8(40, 42, 44, U8X8_PIN_NONE);
#endif //ARDUINO_LCD

//RTC_DS1307 rtc;				// Global singleton
unsigned long g_last_milli_val;	// used to pad calls to TimeNow so that the RTC isn't smashed
time_t g_epoch_time;
#endif //PLATFORM_ARDUINO

uint8_t rtc_status = 0;		// assume dead until we can find it and talk to it

#ifdef USE_ITCH
ITCH itch;					// Global singleton
#endif

#ifdef PLATFORM_LINUX
	int screen_refresh_counter = 0;
#endif



/************************************************
 Functions
************************************************/

uint8_t HALSaveEventBuffer(void) {

	uint8_t save_success = 0;

#ifdef PLATFORM_ARDUINO
	#ifdef OLD_SD
	File e_file;

	// make sure that the default chip select pin is set to
	// output, even if you don't use it:
	//const int chipSelect = 10;
	pinMode(SS, OUTPUT);
	pinMode(10, OUTPUT);
	//pinMode(53, OUTPUT);
	// see if the card is present and can be initialized:

	//try using updated library
	if (SD.begin(10, 11, 12, 13)) {
//	if (SD.begin(10)) {

	#ifdef DEBUG
		//DebugLog(F("(HALSaveEventBuffer) DEBUG INFO sd.begin successful")); //cant use EventMsg
	#endif
		EventNode* e;
		char e_str[MAX_LOG_LINE_LENGTH];

		if (!EventBufferEmpty()) {
			e_file = SD.open(EVENT_FILENAME, FILE_WRITE);
			if (e_file) {
				#ifdef DEBUG
				//DebugLog(F("(HALSaveEventBuffer) DEBUG INFO SD.open successful"));
				#endif
				while (!EventBufferEmpty()) {
					e = EventBufferPop();
					FormatEventMessage(e, e_str);
					e_file.println(e_str);
				}
				e_file.flush();
				e_file.close();
//				DebugLog("DEBUG INFO Event Buffer Saved");
				save_success = 1;
			} else {
				DebugLog(SSS, E_ERROR, M_SD_NO_FILE_HANDLE);
				//DebugLog("ERROR: (HALSaveEventBuffer) No SD File Handle");
			}

		} else {
			DebugLog(SSS, E_ERROR, M_ERROR_EVENTS_EMPTY);
		}

		SD.end();
//		DebugLog("DEBUG INFO SD.end");
	} else {
		DebugLog(SSS, E_ERROR, M_SD_BEGIN_FAIL);
	}
	#endif //OLD_SD
#endif //PLATFORM_ARDUINO


#ifdef PLATFORM_LINUX
	FILE *e_file;
	EventNode* e;
	char e_str[MAX_LOG_LINE_LENGTH];

	if (!EventBufferEmpty()) {
		e_file = fopen(EVENT_FILENAME, "a");
		if (e_file) {
			//DebugLog("File open GOOD, have file handle");
			while (!EventBufferEmpty()) {

				e = EventBufferPop();
				FormatEventMessage(e, e_str);
				fprintf(e_file, "%s\n", e_str);
				/*
				char time_str[12];
				char date_str[14];
				strftime(date_str, 14, "%Y-%m-%d", localtime(&(e->time_stamp)));
				strftime(time_str, 12, "%H:%M:%S", localtime(&(e->time_stamp)));
				fprintf(e_file, "%s,", date_str);
				fprintf(e_file, "%s,", time_str);
				fprintf(e_file, "%s,", GetBlockLabelString(e->source));
				fprintf(e_file, "%s,", GetBlockLabelString(e->destination));
				fprintf(e_file, "%s,", GetMessageTypeString(e->message_type));
				fprintf(e_file, "%s,", GetMessageString(e->message));
				fprintf(e_file, "%ld,", e->int_val);
				fprintf(e_file, "%f\n", e->float_val);
				*/
			}
			fclose(e_file);
			DebugLog(SSS, E_VERBOSE, M_BUF_SAVED, UINT16_INIT, 0);
			save_success = 1;
		} else {
			DebugLog(SSS, E_ERROR, M_ERR_EVENT_FILE, 0, 0);
		}

	} else {
		DebugLog(SSS, E_ERROR, M_SAVE_EMPTY_BUF);
	}
#endif

	return save_success;
}


uint8_t HALInitSerial(uint8_t port, uint16_t baudrate) {
#ifdef PLATFORM_ARDUINO
	switch (port) {
		case 0:
			#ifdef SERIAL_0_USED
			Serial.begin(baudrate);
			#endif
			break;
		case 1:
			#ifdef SERIAL_1_USED
			Serial1.begin(baudrate);
			#endif
			break;
		case 2:
			#ifdef SERIAL_2_USED
			Serial2.begin(baudrate);
			#endif
			break;
		case 3:
			#ifdef SERIAL_3_USED
			Serial3.begin(baudrate);
			#endif
			break;
		default:
			break;
	}
	#endif //PLATFORM_ARDUINO
	return 1;
}

uint8_t HALEventSerialSend(EventNode* e, uint8_t port) {

	char e_str[MAX_LOG_LINE_LENGTH];
	FormatEventMessage(e, e_str);

	#ifdef PLATFORM_ARDUINO

	// Serial begin now in InitSerial, so that when ITCH is in use
	// it can be open listening all the time

	int loop = 5000;
	int check = 0;
    while (loop > 0) {
    	loop--;
    	switch (port) {
    		case 0:
				#ifdef SERIAL_0_USED
    			check = Serial;
				#endif
    			break;
    		case 1:
				#ifdef SERIAL_1_USED
    			check = Serial1;
				#endif
    			break;
    		case 2:
				#ifdef SERIAL_2_USED
    			check = Serial2;
				#endif
    			break;
    		case 3:
				#ifdef SERIAL_3_USED
    			check = Serial3;
				#endif
    			break;
    		default:
    			break;
    	}

    	if (check) {
    		break;
    	}
    }
    if (loop > 0) {
    	//ie. connected before timeout
    	switch (port) {
    		case 0:
				#ifdef USE_ITCH
    			ITCHWriteLine(e_str);
				#else
    			Serial.println(e_str);
    	    	Serial.flush();
				#endif
    			break;
    		case 1:
				#ifdef USE_ITCH
    			ITCHWriteLine(e_str);
				#else
    			Serial1.println(e_str);
    	    	Serial1.flush();
				#endif
    			break;
    		case 2:
				#ifdef USE_ITCH
    			ITCHWriteLine(e_str);
				#else
    			Serial2.println(e_str);
    	    	Serial2.flush();
				#endif
    			break;
       		case 3:
    			#ifdef USE_ITCH
        		ITCHWriteLine(e_str);
    			#else
       			Serial3.println(e_str);
       	    	Serial3.flush();
				#endif
       			break;
    		default:
    			break;
    	}
    }

	#endif //PLATFORM_ARDUINO
	return 1;
}


#ifdef USE_ITCH
void HALInitItch(void) {
	#ifdef PLATFORM_ARDUINO
		itch.Begin();
	#endif //PLATFORM_ARDUINO
	#ifdef PLATFORM_LINUX
		itch.Begin(stdin, stdout);
	#endif //PLATFORM_LINUX
}

void HALPollItch(void) {
	itch.Poll();
}

void HALItchWriteLnImmediate(const char *str) {
	ITCHWriteLine(str);
}
// XXX work around to handle consts and non consts
void HALItchWriteLnImmediate(char *str) {
	ITCHWriteLine(str);
}

void HALItchSetBufferStuffMode(void) {
	itch.SetMode(ITCH_BUFFER_STUFF);
}

void HALItchSetTextDataMode(void) {
	itch.SetMode(ITCH_TEXT_DATA);
}

void HALItchStuffAndProcess(char *str) {
	itch.StuffAndProcess(str);
}

#endif //USE_ITCH


#ifdef PLATFORM_ARDUINO
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
#endif //PLATFORM_ARDUINO

//XXX Temporary hard coding of device addresses until itch can support
// device addresses as part of config processing


// 2018-09-18 - Swapped Water heat into Top Temp after top temp gurney damage.
//Bus 5 Device Count: 2
//	TOP 	Bus:5 Count:2, Device:0, Addr 28:74:AB:03:00:00:80:1D, d_res: 12, d_temp: 19.13
//	BOT		Bus:5 Count:2, Device:1, Addr 28:FF:EA:32:85:16:03:93, d_res: 12, d_temp: 15.19
//Bus 6 Device Count: 2
//	CAB		Bus:6 Count:2, Device:0, Addr 28:FF:68:23:85:16:03:6C, d_res: 9, d_temp: 22.00
//	OUT		Bus:6 Count:2, Device:1, Addr 28:FF:B7:2C:85:16:03:C7, d_res: 12, d_temp: 85.00
//Bus 7 Device Count: 1
//	TANK	Bus:7 Count:1, Device:0, Addr 28:FF:20:34:85:16:03:05, d_res: 12, d_temp: 16.50

//2019-05-28 New Top Sensor Installed

// Bus 5 Device Count: 2
// 	TOP		Bus:5 Count:2, Device:0, Addr 28:C7:F9:45:92:11:02:C3, d_res: 12, d_temp: 17.75
// 	BOT		Bus:5 Count:2, Device:1, Addr 28:FF:EA:32:85:16:03:93, d_res: 12, d_temp: 16.25
// Bus 6 Device Count: 2
// 	CAB		Bus:6 Count:2, Device:0, Addr 28:FF:68:23:85:16:03:6C, d_res: 9, d_temp: 17.50
// 	OUT		Bus:6 Count:2, Device:1, Addr 28:FF:B7:2C:85:16:03:C7, d_res: 12, d_temp: 15.56
// Bus 7 Device Count: 1
// 	TANK	Bus:7 Count:1, Device:0, Addr 28:FF:B7:2C:85:16:03:C7, d_res: 0, d_temp: 11.25

//input1 = IN_TOP_TEMP    0
//input2 = IN_BOT_TEMP    1
//input3 = IN_OUT_TEMP    4
//input4 = IN_WATER_TEMP  2
//input5 = IN_CAB_TEMP    3
//input6 = IN_TANK_TEMP   5

//Bus 5 Device Count: 2
//	TOP		Bus:5 Count:2, Device:0, Addr 28:C7:F9:45:92:11:02:C3, d_res: 12, d_temp: 7.88
//	BOT		Bus:5 Count:2, Device:1, Addr 28:FF:EA:32:85:16:03:93, d_res: 12, d_temp: 6.44
//Bus 6 Device Count: 2
//	CAB		Bus:6 Count:2, Device:0, Addr 28:FF:68:23:85:16:03:6C, d_res: 9, d_temp: 12.50
//	OUT		Bus:6 Count:2, Device:1, Addr 28:FF:B7:2C:85:16:03:C7, d_res: 12, d_temp: 9.00
//Bus 7 Device Count: 1
//	TANK	Bus:7 Count:1, Device:0, Addr 28:FF:20:34:85:16:03:05, d_res: 12, d_temp: 11.13

#ifdef ARDUINO
const DeviceAddress devices[5] PROGMEM = {
		0x28,0xC7,0xF9,0x45,0x92,0x11,0x02,0xC3,	// 0 TOP
		0x28,0xFF,0xEA,0x32,0x85,0x16,0x03,0x93,	// 1 BOT
		0x28,0xFF,0x68,0x23,0x85,0x16,0x03,0x6C,	// 2 CAB
		0x28,0xFF,0xB7,0x2C,0x85,0x16,0x03,0xC7,	// 3 OUT
		0x28,0xFF,0x20,0x34,0x85,0x16,0x03,0x05		// 4 TANK
};

#endif

float GetTemperature(int if_num) {
	float temp_c;
#ifdef PLATFORM_ARDUINO
#ifndef FF_TEMPERATURE_SIM
	//XXX Temporary hard coding of device addresses - see above

	//Select the bus based on the devices per bus defs
	uint8_t bus_pin;
	if (if_num < OWB1_SENSOR_COUNT) {
		bus_pin = ONE_WIRE_BUS_1;
	} else {
		if(if_num < (OWB1_SENSOR_COUNT + OWB2_SENSOR_COUNT)) {
			bus_pin = ONE_WIRE_BUS_2;
		} else {
			bus_pin = ONE_WIRE_BUS_3;

		}
	}
	OneWire one_wire(bus_pin); // oneWire instance to communicate with any OneWire devices
	DallasTemperature temp_sensors(&one_wire);     // Pass our one_wire reference to Dallas Temperature
	temp_sensors.begin();

	DeviceAddress d;
	memcpy_P(d, devices[if_num], sizeof(DeviceAddress));

	temp_sensors.requestTemperaturesByAddress(d);
	temp_c = temp_sensors.getTempC(d);


	#ifdef FF_TEMP_SIM_WITH_DALLAS
	temp_c = random(17.01, 26.99);
	#endif

#endif //#ifndef FF_TEMPERATURE_SIM
#endif //PLATFORM_ARDUINO

#ifdef FF_TEMPERATURE_SIM
#ifdef FF_RANDOM_TEMP_SIM
#ifdef PLATFORM_LINUX
	temp_c = (float)FF_RANDOM_TEMP_MIN + ((float)((rand() % (((int)FF_RANDOM_TEMP_MAX - (int)FF_RANDOM_TEMP_MIN) *100))) / 100);
#endif
#endif
#endif

#ifdef FF_TEMPERATURE_SIM
#ifdef FF_DEFINED_TEMP_SIM
	switch (if_num) {
	case 0:
		temp_c = SIM_TEMP_0;
		break;
	case 1:
		temp_c = SIM_TEMP_1;
		break;
	case 2:
		temp_c = SIM_TEMP_2;
		break;
	case 3:
		temp_c = SIM_TEMP_3;
		break;
	case 4:
		temp_c = SIM_TEMP_4;
		#ifdef TEMP_SIM_EXTRA_DELAY
		#ifdef PLATFORM_ARDUINO
			delay(random(TEMP_SIM_MIN_DELAY, TEMP_SIM_MAX_DELAY));
		#else
			usleep( (TEMP_SIM_MIN_DELAY * 1000) + \
					( (rand() % (TEMP_SIM_MAX_DELAY - TEMP_SIM_MIN_DELAY)) *1000));
		#endif //PLATFORM_ARDUINO
		#endif //TEMP_SIM_EXTRA_DELAY
		break;
	case 5:
		temp_c = SIM_TEMP_5;
		#ifdef TEMP_SIM_EXTRA_DELAY
		#ifdef PLATFORM_ARDUINO
			delay(random(TEMP_SIM_MIN_DELAY, TEMP_SIM_MAX_DELAY));
		#else
			usleep( (TEMP_SIM_MIN_DELAY * 1000) + \
					( (rand() % (TEMP_SIM_MAX_DELAY - TEMP_SIM_MIN_DELAY)) *1000));
		#endif //PLATFORM_ARDUINO
		#endif //TEMP_SIM_EXTRA_DELAY
		break;
	default:
		temp_c = FLOAT_INIT;
	}
#endif
#endif

#ifdef FF_TEMPERATURE_SIM
#ifdef FF_RANDOM_TEMP_SIM
#ifdef PLATFORM_ARDUINO
	if_num=1;
	temp_c = random(FF_RANDOM_TEMP_MIN, FF_RANDOM_TEMP_MAX) * (float)if_num;
#endif //PLATFORM_ARDUINO
#endif //FF_RANDOM_TEMP_SIM
#endif //FF_TEMPERATURE_SIM

#ifdef RANDOM_TEMP_SIM_DELAY
#ifdef PLATFORM_ARDUINO
		delay(random(TEMP_SIM_MIN_DELAY, TEMP_SIM_MAX_DELAY));
#else
	usleep( (TEMP_SIM_MIN_DELAY * 1000) + \
			( (rand() % (TEMP_SIM_MAX_DELAY - TEMP_SIM_MIN_DELAY)) *1000));
#endif //PLATFORM_ARDUINO
#endif //RANDOM_TEMP_SIM_DELAY

	return temp_c;
}

void HALDigitalWrite(uint8_t if_num, uint8_t digital_val) {
#ifdef PLATFORM_ARDUINO
	digitalWrite(if_num, digital_val);
#endif //PLATFORM_ARDUINO
#ifdef PLATFORM_LINUX
#endif
}

void HALInitDigitalOutput(uint8_t if_num) {
#ifdef PLATFORM_ARDUINO
	pinMode(if_num, OUTPUT);
#endif //PLATFORM_ARDUINO

#ifdef PLATFORM_LINUX
#endif

}

uint8_t HALDigitalRead(uint8_t if_num) {
#ifdef PLATFORM_ARDUINO
	return digitalRead(if_num);
#endif //PLATFORM_ARDUINO

#ifdef PLATFORM_LINUX
	return rand() % 2;
#endif
}

void HALInitDigitalInput(uint8_t if_num) {
#ifdef PLATFORM_ARDUINO
	pinMode(if_num, INPUT);
#endif //PLATFORM_ARDUINO
#ifdef PLATFORM_LINUX
	// nothing to do
#endif
}

void TempSensorsTakeReading(void) {
#ifdef PLATFORM_ARDUINO
	//temp_sensors.requestTemperatures();  //tell them to take a reading (stored on device)
#endif //PLATFORM_ARDUINO
}

void InitTempSensors(void) {
#ifdef PLATFORM_ARDUINO
#ifdef DEBUG_DALLAS
	//XXX Now out of date with HW config - which has 3x busses

	OneWire one_wire_1(ONE_WIRE_BUS_1);            		// oneWire instance to communicate with any OneWire devices
	//OneWire one_wire_2(ONE_WIRE_BUS_2);            		// oneWire instance to communicate with any OneWire devices
	DallasTemperature temp_sensors_1(&one_wire_1);     // Pass our one_wire reference to Dallas Temperature
	//DallasTemperature temp_sensors_2(&one_wire_2);     // Pass our one_wire reference to Dallas Temperature


	uint8_t d_count;
	uint8_t d_addr_arr[9];
	uint8_t *d_addr;
	d_addr = &d_addr_arr[0];
	//uint8_t d_index;
	uint8_t d_res;

	float d_temp;
	char f_str[10];

	char str[MAX_DEBUG_LENGTH];

	DebugLog("\t Dallas Temperature Sensors Debug");

	temp_sensors_1.begin();

	d_count = temp_sensors_1.getDeviceCount();

	sprintf(str, "\t Bus 1 Device Count: %d", d_count);
	DebugLog(str);

	for (uint8_t d_index = 0; d_index < d_count; d_index++) {
		temp_sensors_1.getAddress(d_addr, d_index);
		temp_sensors_1.requestTemperaturesByIndex(d_index);
		d_temp = temp_sensors_1.getTempCByIndex(d_index);
		d_res = temp_sensors_1.getResolution(d_addr);

		FFFloatToCString(f_str, d_temp);

		sprintf(str, "\t bus: 2/%d, d_index: %d, d_addr: %u:%u:%u:%u:%u:%u:%u:%u, d_res: %d, d_temp: %s", d_count, d_index, d_addr_arr[0], d_addr_arr[1], d_addr_arr[2], d_addr_arr[3], d_addr_arr[4], d_addr_arr[5], d_addr_arr[6], d_addr_arr[7], d_res, f_str);
		DebugLog(str);

	}
//XXX more than once instance of Onewire / Dallas fails - suspect due lack of memory

	OneWire one_wire_2(ONE_WIRE_BUS_2);            		// oneWire instance to communicate with any OneWire devices
	DallasTemperature temp_sensors_2(&one_wire_2);     // Pass our one_wire reference to Dallas Temperature

	temp_sensors_2.begin();
	d_count = temp_sensors_2.getDeviceCount();
	sprintf(str, "\t Bus 2 Device Count: %d", d_count);
	DebugLog(str);
	for (uint8_t d_index = 0; d_index < d_count; d_index++) {
		temp_sensors_2.getAddress(d_addr, d_index);
		temp_sensors_2.requestTemperaturesByIndex(d_index);
		d_temp = temp_sensors_2.getTempCByIndex(d_index);
		d_res = temp_sensors_2.getResolution(d_addr);

		FFFloatToCString(f_str, d_temp);

		sprintf(str, "\t bus: 2/%d, d_index: %d, d_addr: %u:%u:%u:%u:%u:%u:%u:%u, d_res: %d, d_temp: %s", d_count, d_index, d_addr_arr[0], d_addr_arr[1], d_addr_arr[2], d_addr_arr[3], d_addr_arr[4], d_addr_arr[5], d_addr_arr[6], d_addr_arr[7], d_res, f_str);
		DebugLog(str);

	}

#endif
#endif //PLATFORM_ARDUINO
}

#ifdef UI_ATTACHED
void HALInitUI(void) {

#ifdef ARDUINO_LCD
	lcd_128_64.begin();
#endif //ARDUINO_LCD

#ifdef PLATFORM_LINUX
#endif
}
#endif

#ifdef UI_ATTACHED
void HALDrawDataScreenCV(const UIDataSet* uids, time_t dt) {

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


#ifdef ARDUINO_LCD
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
	lcd_128_64.drawStr(0, 7, time_now_str);
	lcd_128_64.drawStr(0, 18, "");
	lcd_128_64.drawStr(0, 31, "Min");
	lcd_128_64.drawStr(0, 41, "  @");
	lcd_128_64.drawStr(0, 54, "Max");
	lcd_128_64.drawStr(0, 64, "  @");

	lcd_128_64.drawStr(43, 7, "IN");
	lcd_128_64.drawStr(25, 18, FFFloatToCString(str_buf, uids->inside_current));
	lcd_128_64.drawStr(25, 31, FFFloatToCString(str_buf, uids->inside_min));
	lcd_128_64.drawStr(25, 41, time_in_min);
	lcd_128_64.drawStr(25, 54, FFFloatToCString(str_buf, uids->inside_max));
	lcd_128_64.drawStr(25, 64, time_in_max);

	lcd_128_64.drawStr(72, 7, "OUT");
	lcd_128_64.drawStr(60, 18, FFFloatToCString(str_buf, uids->outside_current));
	lcd_128_64.drawStr(60, 31, FFFloatToCString(str_buf, uids->outside_min));
	lcd_128_64.drawStr(60, 41, time_out_min);
	lcd_128_64.drawStr(60, 54, FFFloatToCString(str_buf, uids->outside_max));
	lcd_128_64.drawStr(60, 64, time_out_max);

	lcd_128_64.drawStr(110, 7, "WAT");
	lcd_128_64.drawStr(95, 18, FFFloatToCString(str_buf, uids->water_current));
	lcd_128_64.drawStr(95, 31, FFFloatToCString(str_buf, uids->water_min));
	lcd_128_64.drawStr(95, 41, time_wat_min);
	lcd_128_64.drawStr(95, 54, FFFloatToCString(str_buf, uids->water_max));
	lcd_128_64.drawStr(95, 64, time_wat_max);
	} while (lcd_128_64.nextPage());

	//lcd_128_64.sendBuffer();                      // transfer internal memory to the display

#endif /ARDUINO_LCD

#ifdef PLATFORM_LINUX_DATA_SCREEN
	//printf("\e[1;1H\e[2J"); //clear screen
	if (screen_refresh_counter >= SCREEN_REFRESH) {
		printf("\n");
		printf("%s IN   OUT  WATER\n", time_now_str);
		printf("NOW %f %f %f\n", uids->inside_current, uids->outside_current, uids->water_current);
		printf("MIN %f %f %f\n", uids->inside_min, uids->outside_min, uids->water_min);
		printf("    %s %s %s\n", time_in_min, time_out_min, time_wat_min);
		printf("MAX %f %f %f\n", uids->inside_max, uids->outside_max, uids->water_max);
		printf("    %s %s %s\n", time_in_max, time_out_max, time_wat_max);
		printf("\n");
		screen_refresh_counter = 0;
	} else {
		screen_refresh_counter +=1;
	}
#endif
}
#endif //UI_ATTACHED


/********************************************************************
 * Time and Date Functions
 ********************************************************************/

time_t TimeNow(void) {
#ifdef PLATFORM_ARDUINO
	RTC_DS1307 rtc;
	time_t epoch_time;
	unsigned long milli_now = millis();
	DateTime rtcDT;

	if(milli_now < g_last_milli_val) {
		// Assume millis has rolled over to zero (every 59 days)
		rtcDT = rtc.now();
		g_epoch_time = rtcDT.secondstime();
		epoch_time = g_epoch_time;
		g_last_milli_val = milli_now;
	} else {
		if((milli_now - g_last_milli_val) > RTC_POLL_INTERVAL) {
			rtcDT = rtc.now();
			g_epoch_time = rtcDT.secondstime();
			epoch_time = g_epoch_time;
			g_last_milli_val = milli_now;
		} else {
			epoch_time = g_epoch_time + ((milli_now - g_last_milli_val) / 1000);
		}
	}
	return epoch_time;
#endif //PLATFORM_ARDUINO

#ifdef PLATFORM_LINUX
	return time(NULL);
#endif
}

#ifdef PLATFORM_ARDUINO
uint8_t HALSetRTCTime(const char *time_str) {
	RTC_DS1307 rtc;
	DateTime nowDT;
	uint8_t hh,mm,ss;
	char fmt_str[9];
	// Set uint8_t values for hh, mm, ss from time_str
	strcpy_hal(fmt_str, F("%d:%d:%d"));
	sscanf(time_str, fmt_str, &hh, &mm, &ss);
	// Get the current RTC time
	nowDT = rtc.now();
	// Adjust using current and new set point
	rtc.adjust(DateTime(nowDT.year(), nowDT.month(), nowDT.day(), hh, mm, ss));
	return 0;
}

uint8_t HALSetRTCDate(const char *date_str) {
	RTC_DS1307 rtc;
	DateTime nowDT;
	int YYYY,MM,DD;
	char fmt_str[9];
	// Set uint8_t values for YYYY, MM, DD from the date_str
	strcpy_hal(fmt_str, F("%d-%d-%d"));
	sscanf(date_str, fmt_str, &YYYY, &MM, &DD);
	// Get the current date
	nowDT = rtc.now();
	// Adjust using current and new set point
	rtc.adjust(DateTime(YYYY, MM, DD, nowDT.hour(), nowDT.minute(), nowDT.second()));
	return 0;
}
#endif //PLATFORM_ARDUINO

void HALInitRTC(void) {
#ifdef PLATFORM_ARDUINO
	//setup the Real Time Clock
	//TODO check for sensible RTC result - in all cases the RTC value should be later
	// than the compile time date time hard coded directives
	//TODO substitute this section and all time references to use system time rather than RTC
	// and sync system time to RTC periodically - that way time still progresses even if RTC broken
	RTC_DS1307 rtc;
	DateTime rtcDT;
	//Wire.begin();
	if (rtc.begin()) {
		//EventMsg(SSS, E_INFO, M_RTC_DETECT, 0, 0);
		if (rtc.isrunning()) {
			// Store the current millis timer value - used to avoid calling on the RTC
			// 	for every time query which drains the rtc battery
			rtcDT = rtc.now();
			g_epoch_time = rtcDT.secondstime();
			g_last_milli_val = millis();
			EventMsg(SSS, E_INFO, M_RTC_REPORT_RUNNING, 0, 0);
			#ifdef SET_RTC
			EventMsg(SSS, E_WARNING, M_WARN_SET_RTC, 0, 0);
			// following line sets the RTC localtime() to the date & time this sketch was compiled
			rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
			EventMsg(SSS, E_WARNING, M_WARN_RTC_HARD_CODED, 0, 0);
			#endif
			//HALSetSysTimeToRTC();
		} else {
			EventMsg(SSS, E_WARNING, M_RTC_NOT_RUNNING, 0, 0);
			// following line sets the RTC to the date & time this sketch was compiled
			rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
			EventMsg(SSS, E_WARNING, M_WARN_RTC_HARD_CODED, 0, 0);
		};
		   /**
		        Initialize the system time. Examples are...

		        From a Clock / Calendar type RTC:
		        \code
		        struct tm rtc_time;

		        read_rtc(&rtc_time);
		        rtc_time.tm_isdst = 0;
		        set_system_time( mktime(&rtc_time) );
		        \endcode

		        From a Network Time Protocol time stamp:
		        \code
		        set_system_time(ntp_timestamp - NTP_OFFSET);
		        \endcode

		        From a UNIX time stamp:
		         \code
		        set_system_time(unix_timestamp - UNIX_OFFSET);
		        \endcode

		    */
		//set_dst();
		//set_zone(TIME_ZONE * 3600);
		//HALSetSysTimeToRTC();
		rtc_status = 1;
	} else
		EventMsg(SSS, E_ERROR, M_RTC_NOT_FOUND, 0, 0);
#endif //PLATFORM_ARDUINO
#ifdef PLATFORM_LINUX
	EventMsg(SSS, E_INFO, M_SIM_SYS_TIME);
#endif
}

void HALReboot(void) {
	#ifdef PLATFORM_ARDUINO
	MCUSR = 0;  // Clear out any flags of prior resets
	wdt_enable(WDTO_500MS); // Turn on wdt and dont stroke it
	while(1) {
	  // Do nothing, wait for wdt to reset
	}
	#endif //PLATFORM_ARDUINO

	#ifdef PLATFORM_LINUX
	#ifdef USE_ITCH
		itch.RestoreTermAndExit();
	#endif //USE_ITCH
		exit(0);
	#endif //PLATFORM_LINUX
}

