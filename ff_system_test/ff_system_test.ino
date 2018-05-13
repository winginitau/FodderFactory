// Simple routine to cycle through all inputs and outputs for testing
//
// - Temperature inputs - read
// - Relay outputs - Activate for 10 seconds
// - Pause for 10 secs between devices

#include "Arduino.h"
#include <OneWire.h>
#include <DallasTemperature.h>


#define ONE_WIRE_BUS_1 5                			//for Dallas temp signals
#define ONE_WIRE_BUS_2 6							//for Dallas temp signals
#define ONE_WIRE_BUS_3 7							//for Dallas temp signals

//#define OWB1_SENSOR_COUNT 3
//#define OWB2_SENSOR_COUNT 2


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

	d_count = temp_sensors.getDeviceCount();

	sprintf(str, "Bus %d Device Count: %d\n", bus, d_count);
	Serial.write(str);

	for (uint8_t d_index = 0; d_index < d_count; d_index++) {
		temp_sensors.getAddress(d_addr, d_index);
		temp_sensors.requestTemperaturesByIndex(d_index);
		d_temp = temp_sensors.getTempCByIndex(d_index);
		d_res = temp_sensors.getResolution(d_addr);

		FFFloatToCString(f_str, d_temp);

		sprintf(str, "Bus:%d Count:%d, Device:%d, Addr %.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X:%.2X, d_res: %d, d_temp: %s\n", bus, d_count, d_index, d_addr[0], d_addr[1], d_addr[2], d_addr[3], d_addr[4], d_addr[5], d_addr[6], d_addr[7], d_res, f_str);
		Serial.write(str);
	}

}

void Operate(void) {
	uint8_t pins[] = { 22, 24, 26, 28, 30, 32, 34, 36 };
	uint8_t status[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
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
		sprintf(buf, "LT  WH  NA  FC  FE  NA  WT  WB\n");
		Serial.write(buf);
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
	uint8_t pins[] = {22, 24, 26, 28, 30, 32, 34, 36};
	uint8_t pin_count = 8;

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

	Serial3.begin(19200);

	c = CheckForResponse();

	while (c == '\0') {
		if(Serial3.available()) {
			b = Serial3.read();
			Serial.write(b);
		}
		c = CheckForResponse();
	}
	Serial3.end();
}

void setup() {
	//delay(10000);	// to switch between avrdude and screen session

	delay(500); //let terminal settle
	Serial.begin(9600);
	delay(100);
	Serial.write("Fodder Factory System Test\n");
	Serial.flush();
}


void loop() {
	char c;

	Serial.write("Main Menu\n");
	Serial.write(" (use x to exit each sub-routine)\n");
	Serial.write(" (r)elays\n (t)emps\n (o)perate\n (s)erial3 dump\n");
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
	Serial.write("Finished sub-routine\n");
	Serial.flush();}
