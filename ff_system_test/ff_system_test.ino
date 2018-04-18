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

void TestRelays(void) {
	uint8_t pins[] = {22, 24, 26, 28, 30, 32, 34, 36};
	uint8_t pin_count = 8;

	char buf[200];
	int c = 0;

	for (uint8_t i = 0; i < pin_count; i++) {
		pinMode(pins[i], OUTPUT);
		sprintf(buf, "Ready to Test   Relay:%d   Pin:%d    <CR/NL> to Activate\n", i, pins[i]);
		Serial.write(buf);

		c = Serial.read();
		while (c != '\n') {
			c = Serial.read();
		}
		Serial.flush();

		sprintf(buf, "HIGH            Relay:%d   Pin:%d    <CR/NL> to Deactivate\n", i, pins[i]);
		Serial.write(buf);
		digitalWrite(pins[i], HIGH);

		c = Serial.read();
		while (c != '\n') {
			c = Serial.read();
		}
		Serial.flush();

		sprintf(buf, "LOW             Relay:%d   Pin:%d    \n\n", i, pins[i]);
		Serial.write(buf);

		digitalWrite(pins[i], LOW);
	}
}

void setup() {
	Serial.begin(9600);
	Serial.write("Fodder Factory System Test\n");
}

void loop() {
	char c;

	Serial.write("(r)elays or (t)emps?\n");

	c = Serial.read();
	while ((c != 'r') && (c != 't')) {
		c = Serial.read();
	}

	if(c == 'r') TestRelays();
	if(c == 't') {
		TestTempSensors(ONE_WIRE_BUS_1);
		TestTempSensors(ONE_WIRE_BUS_2);
		TestTempSensors(ONE_WIRE_BUS_3);
	}

	Serial.flush();
	Serial.write("Iteration Complete\n");
}
