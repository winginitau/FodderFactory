// Simple routine to cycle through all inputs and outputs for testing
//
// - Temperature inputs - read
// - Relay outputs - Activate for 10 seconds
// - Pause for 10 secs between devices

#include "Arduino.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"
#include <time.h>


#define ONE_WIRE_BUS_1 5                			//for Dallas temp signals
#define ONE_WIRE_BUS_2 6							//for Dallas temp signals
#define ONE_WIRE_BUS_3 7							//for Dallas temp signals

#define RTC_POLL_INTERVAL 10000
#define VE_LOG_RATE 10

//#define OWB1_SENSOR_COUNT 3
//#define OWB2_SENSOR_COUNT 2

/************************************************************
	Global Pins and Status
************************************************************/
uint8_t pins[] = { 22, 24, 26, 28, 30, 32, 34, 36 };
uint8_t status[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t pin_count = 8;

enum {
	M_VE_SOC,
	M_VE_VOLTAGE,
	M_VE_POWER,
	M_VE_CURRENT,
};

RTC_DS1307 rtc;

uint8_t rtc_status = 0;		// assume dead until we can find it and talk to it

unsigned long g_last_milli_val;	// used to pad calls to TimeNow so that the RTC isn't smashed
DateTime g_last_rtc_DT;

time_t g_VE_last_update;
time_t g_VE_log_rate;
//int16_t	g_VE_soc;
//int16_t	g_VE_power;
//int16_t	g_VE_voltage;
//int16_t	g_VE_current;



char* FFFloatToCString(char* buf, float f) {
	return dtostrf(f,-7,2,buf);
}

char WaitForCRorLF(void) {
	char c = '\0';

	while ((c != '\n') && (c != '\r')) {
		if(Serial.available()) {
			c = Serial.read();
		}
	}

	delay(10); // wait for and discard extra chars on the line
	while(Serial.available()) {
		Serial.read();
		delay(10);
	}

	return c;
}

char WaitForResponse(void) {
	char c = '\0';

	while (c == '\0') {
		if(Serial.available()) {
			c = Serial.read();
		}
	}

	delay(10); // wait for and discard extra chars on the line
	while(Serial.available()) {
		Serial.read();
		delay(10);
	}

	return c;
}

char CheckForResponse(void) {
	char c = '\0';
		if(Serial.available()) {
			c = Serial.read();
		}

	if (c != '\0') {
		delay(10); // wait for and discard extra chars on the line
		while(Serial.available()) {
			Serial.read();
			delay(10);
		}
	}
	return c;
}

void TestTempSensors(uint8_t bus) {

	OneWire one_wire(bus);            		// oneWire instance to communicate with any OneWire devices
	//OneWire one_wire_2(ONE_WIRE_BUS_2);            		// oneWire instance to communicate with any OneWire devices
	DallasTemperature temp_sensors(&one_wire);     // Pass our one_wire reference to Dallas Temperature
	//DallasTemperature temp_sensors_2(&one_wire_2);     // Pass our one_wire reference to Dallas Temperature

	DeviceAddress d_addr;
	uint8_t d_count;
	//uint8_t d_addr_arr[9];
	//uint8_t *d_addr;
	//d_addr = &d_addr_arr[0];
	uint8_t d_res;

	float d_temp;
	char f_str[10];

	char str[200];

	//Serial.write("Dallas Temperature Sensors Debug\n");

	temp_sensors.begin();
	//delay(1000);
	d_count = temp_sensors.getDeviceCount();

	sprintf(str, "Bus %d Device Count: %d\n", bus, d_count);
	Serial.write(str);

	for (uint8_t d_index = 0; d_index < d_count; d_index++) {
		temp_sensors.getAddress(d_addr, d_index);
		delay(200);
		temp_sensors.requestTemperaturesByIndex(d_index);
		delay(500);
		d_temp = temp_sensors.getTempCByIndex(d_index);
		delay(200);
		d_res = temp_sensors.getResolution(d_addr);

		FFFloatToCString(f_str, d_temp);

		sprintf(str, "Bus:%d Count:%d, Device:%d, Addr %.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X, d_res: %d, d_temp: %s\n", bus, d_count, d_index, d_addr[0], d_addr[1], d_addr[2], d_addr[3], d_addr[4], d_addr[5], d_addr[6], d_addr[7], d_res, f_str);
		Serial.write(str);
		delay(500);
	}

}

void Operate(void) {
	char buf[200];

	char on[] = "ON";
	char off[] = "--";

	char c = '\0';
	int i;
	int n;

	sprintf(buf, "Use Numbers 0 - 7 to Toggle Relays, \"x\" to Exit\n\n");
	Serial.write(buf);

	while (c != 'x') {
		sprintf(buf, "0   1   2   3   4   5   6   7 \n");
		Serial.write(buf);
		Serial.flush();
		sprintf(buf, "LT  WH  NA  FC  FE  CW  WT  WB\n");
		Serial.write(buf);
		Serial.flush();
		buf[0] = '\0';
		for (i = 0; i < 8; i++) {
			if (status[i] == 0) {
				strcat(buf, off);
				strcat(buf, "  ");
			} else {
				strcat(buf, on);
				strcat(buf, "  ");
			}
		}
		strcat(buf, "\n\n");
		Serial.write(buf);
		Serial.flush();

		c = WaitForResponse();
		n = c - '0';

		if ((n >= 0) && (n < 8)) {
			pinMode(pins[n], OUTPUT);
			if (status[n] == 0) {
				status[n] = 1;
				digitalWrite(pins[n], HIGH);
			} else {
				status[n] = 0;
				digitalWrite(pins[n], LOW);
			}
		}
	}
}

void TestRelays(void) {
	//uint8_t pins[] = {22, 24, 26, 28, 30, 32, 34, 36};

	char buf[200];

	for (uint8_t i = 0; i < pin_count; i++) {
		sprintf(buf, "Ready to Test   Relay:%d   Pin:%d    <CR/NL> to Activate\n", i, pins[i]);
		Serial.write(buf);
		Serial.flush();

		WaitForCRorLF();

		pinMode(pins[i], OUTPUT);

		sprintf(buf, "HIGH            Relay:%d   Pin:%d    <CR/NL> to Deactivate\n", i, pins[i]);
		Serial.write(buf);
		Serial.flush();

		digitalWrite(pins[i], HIGH);

		WaitForCRorLF();

		sprintf(buf, "LOW             Relay:%d   Pin:%d    \n\n", i, pins[i]);
		Serial.write(buf);
		Serial.flush();

		digitalWrite(pins[i], LOW);
	}
}

void Serial3Dump() {
	char c;
	int b;

	int loop = 0;

	Serial3.begin(19200);

	c = CheckForResponse();

	while ((c == '\0') && (loop < 3)) {
		if(Serial3.available()) {
			b = Serial3.read();
			if (b == 'h') loop ++;	// the only place 'h' appears in the victron dump is "Checksum"
			Serial.write(b);
		}
		c = CheckForResponse();
	}
	Serial3.end();
}

void HALInitRTC(void) {
	Wire.begin();
	if (rtc.begin()) {
		if (rtc.isrunning()) {
			g_last_rtc_DT = rtc.now();
			g_last_milli_val = millis();
		} else {
			rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
		};

	}
}

time_t TimeNow(void) {
/*
	DateTime rtcDT;
	time_t epoch_time;

	rtcDT = rtc.now();
	epoch_time = rtcDT.secondstime();
	return epoch_time;
*/
	time_t epoch_time;
	unsigned long milli_now = millis();

	if(milli_now < g_last_milli_val) {
		// Assume millis has rolled over to zero (every 59 days)
		g_last_rtc_DT = rtc.now();
		epoch_time = g_last_rtc_DT.secondstime();
		g_last_milli_val = milli_now;
	} else {
		if((milli_now - g_last_milli_val) > RTC_POLL_INTERVAL) {
			g_last_rtc_DT = rtc.now();
			epoch_time = g_last_rtc_DT.secondstime();
			g_last_milli_val = milli_now;
		} else {
			epoch_time = g_last_rtc_DT.secondstime() + ((milli_now - g_last_milli_val) / 1000);
		}
	}
	return epoch_time;

}

int16_t HALReadVEData(uint16_t data_type) {
	//int16_t VE_soc = 2;
	//int16_t VE_power = 3;
	//int16_t VE_voltage = 4;
	//int16_t VE_current = 5;
	int16_t ret = 65535;
	char VE_line[80];
	char temp_str[80];
	char* label;
	char* value;
	uint8_t buf_idx = 0;

	const char delim[2] = "\t";
	// state machine
	uint8_t block_count = 0;
	//uint8_t nl = 0;
	uint8_t cr = 0;
	uint8_t checksum = 0;

	uint8_t b;

	Serial3.begin(19200);

	while (block_count < 3) {	// BMV transmits 2x blocks to deliver all data - read 3 to make sure we get all values
		if(Serial3.available()) {
			b = Serial3.read();
			//Serial.write(b);
			switch (b) {
			case '\n':  // start of newline - reset buffer
				cr = 0;
				//nl = 1;
				VE_line[0] = '\0';
				buf_idx = 0;
				break;
			case '\r': // eol - terminate the buffer
				cr = 1;
				//nl = 0;
				VE_line[buf_idx] = '\0';
				buf_idx++;
				break;
			default: // Check special case "Checksum" or throw it in the buffer
				if(checksum) {
					//TODO: ignore it and preceding tab for now, assume eol, reset and inc block_count
					if(b != '\t') {
						cr = 0;
						VE_line[0] = '\0';
						buf_idx = 0;
						block_count++;
						checksum = 0;
						//sprintf(temp_str, "Block Count is: %d\n", block_count);
						//Serial.write(temp_str);
					}
				} else {
					cr = 0;
					//nl = 0;
					VE_line[buf_idx] = b;
					buf_idx++;
					if(strncmp(VE_line, "Checksum", 8) == 0) {
						Serial.write("Checksum\n");
						checksum = 1;
					}
				}
				Serial.flush();
			}
		}
		if ( cr && buf_idx) { // whole line in buffer
			//Serial.write("CR detected. Buffer Contents: ");
			//Serial.write(VE_line);
			//Serial.write("\n");

			label = strtok(VE_line, delim);
			value = strtok(0, delim);

			//Serial.write("Label is: ");
			//Serial.write(label);
			//Serial.write("\n");
			//Serial.write("Value is: ");
			//Serial.write(value);
			//Serial.write("\n");

			switch (data_type) {
				case M_VE_SOC:
					if (strcmp(label, "SOC") == 0 ) {
						ret = atoi(value);
						sprintf(temp_str, "SOC atoi value is: %d\n", ret);
						Serial.write(temp_str);
						return ret;
					}
					break;
				case M_VE_VOLTAGE:
					if (strcmp(label, "V") == 0 ) {
						ret = atoi(value);
						//sprintf(temp_str, "V atoi value is: %d\n", VE_voltage);
						//Serial.write(temp_str);
						return ret;
					}
					break;
				case M_VE_POWER:
					if (strcmp(label, "P") == 0 ) {
						ret = atoi(value);
						sprintf(temp_str, "P atoi value is: %d\n", ret);
						Serial.write(temp_str);
						return ret;
					}
					break;
				case M_VE_CURRENT:
					if (strcmp(label, "I") == 0 ) {
						ret = atoi(value);
						//sprintf(temp_str, "I atoi value is: %d\n", VE_current);
						//Serial.write(temp_str);
						return ret;
					}
					break;
				default:
					break;
			}
			Serial.flush();
		}
	}
	Serial3.flush();
	Serial3.end();
	return ret;
}


void TestVEDirect(void) {

	char message_line[80];

	// XXX VE Hack
	time_t VE_now = TimeNow();
	time_t VE_next;
	int16_t VE_soc, VE_power, VE_voltage, VE_current;
	VE_next = g_VE_last_update + g_VE_log_rate;
	if (VE_now >= VE_next) {

		Serial.write("Now >= Next - Read VE\n");

		g_VE_last_update = VE_now;
		VE_soc = HALReadVEData(M_VE_SOC);
		VE_power = HALReadVEData(M_VE_POWER);
		VE_voltage = HALReadVEData(M_VE_VOLTAGE);
		VE_current = HALReadVEData(M_VE_CURRENT);

		sprintf(message_line, "VE SOC: %d\n", VE_soc);
		Serial.write(message_line);
		sprintf(message_line, "VE mV: %d\n", VE_voltage);
		Serial.write(message_line);
		sprintf(message_line, "VE W: %d\n", VE_power);
		Serial.write(message_line);
		sprintf(message_line, "VE mA: %d\n", VE_current);
		Serial.write(message_line);
		Serial.flush();


		//EventMsg(SYS_SYSTEM, E_DATA, M_VE_SOC, VE_soc, 0);
		//EventMsg(SYS_SYSTEM, E_DATA, M_VE_VOLTAGE, VE_voltage, 0);
		//EventMsg(SYS_SYSTEM, E_DATA, M_VE_POWER, VE_power, 0);
		//EventMsg(SYS_SYSTEM, E_DATA, M_VE_CURRENT, VE_current, 0);
	}
}

void InitVEDirect(void) {
// XXX VE Hack
	g_VE_last_update = TimeNow();
	g_VE_log_rate = VE_LOG_RATE;
	Serial.write("VE Initialised\n");
	Serial.flush();
}



void ResetRelays(void) {
	for (int n = 0; n < pin_count; n++) {
		pinMode(pins[n], OUTPUT);
		digitalWrite(pins[n], LOW);
		status[n] = 0;
	}
}

void setup() {
	//delay(10000);	// to switch between avrdude and screen session

	delay(500); //let terminal settle
	Serial.begin(9600);
	delay(100);
	Serial.write("Fodder Factory System Test\n");
	Serial.flush();

	HALInitRTC();
	InitVEDirect();

}


void loop() {
	char c;

	Serial.write("Main Menu\n");
	Serial.flush();
	Serial.write(" (use x to exit each sub-routine)\n");
	Serial.flush();
	Serial.write(" (r) Relays\n (t) Temps\n (o) Operate\n (s) Serial3 dump\n (x) Reset Relays\n (v) VE Test\n");
	Serial.flush();

	c = WaitForResponse();

	if(c == 'r') {
		TestRelays();
	}
	if(c == 't') {
		TestTempSensors(ONE_WIRE_BUS_1);
		TestTempSensors(ONE_WIRE_BUS_2);
		TestTempSensors(ONE_WIRE_BUS_3);
	}
	if(c == 'o') {
		Operate();
	}
	if(c == 's') {
		Serial3Dump();
	}
	if(c == 'x') {
		ResetRelays();
		Serial.write("\nRelays reset\n\n");
		Serial.flush();
	}
	if(c == 'v') {
		TestVEDirect();
		Serial.write("\nVE Poll Done\n\n");
		Serial.flush();
	}
	Serial.flush();}
