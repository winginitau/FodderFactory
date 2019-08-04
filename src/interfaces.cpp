/************************************************
 interfaces.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Interface Processing
 ************************************************/


/************************************************
  Includes
************************************************/
#include <build_config.h>
//#include <debug_ff.h>
#include <events.h>
//#include <HAL.h>
#include <interfaces.h>
#include <registry.h>
#include <string_consts.h>
//#include <block_common.h>
#include <utils.h>

//#ifdef ARDUINO_VEDIRECT
//#include <VEDirect.h>
//#endif //ARDUINO_VEDIRECT

#include <OneWire.h>                // comms to Dallas temprature sensors
#include <DallasTemperature.h>


/************************************************
  Data Structures
************************************************/


/************************************************
  Globals
************************************************/


/************************************************
  Interface Processing Functions
************************************************/


void ScanAndMatchDallasSensors(InterfaceNode* i, uint8_t report) {
	// Scan the OneWire port specified by bus and iterate the
	// 	DallasTemperature devices dicovered.
	// For each device, lookup the interface list for DS1820B
	//  interface types and compare the addresses of the devices
	// If report == 0, raise an EventMsg warning for any unmatched
	//  devices.
	// IF report == 1, write verbose information via ITCH.

	InterfaceONEWIRE_BUS *ifs;
	ifs = (InterfaceONEWIRE_BUS *)i->IFSettings;
	OneWire one_wire(ifs->bus_pin);            		// oneWire instance to communicate with any OneWire devices
	DallasTemperature sensors(&one_wire);     // Pass our one_wire reference to Dallas Temperature

	DeviceAddress addr;
	uint8_t count;
	uint8_t res;
	uint16_t match;

	float tempC;
	char fmt[12];
	char out[MAX_LOG_LINE_LENGTH];

	sensors.begin();
	count = sensors.getDeviceCount();

	if (report) {
		ITCHWrite(F("Bus"));
		strcpy_hal(fmt, F(" %d"));
		sprintf(out, fmt, ifs->bus_pin);
		ITCHWrite(out);

		ITCHWrite(F(" Device Count:"));
		sprintf(out, fmt, count);
		ITCHWriteLine(out);
	}

	for (uint8_t index = 0; index < count; index++) {
		sensors.getAddress(addr, index);
		sensors.setResolution(addr, 12);
		sensors.setCheckForConversion(true);
		sensors.setWaitForConversion(true);
		//delay(300);
		sensors.requestTemperaturesByIndex(index);
		//delay(500);
		tempC = sensors.getTempCByIndex(index);
		//delay(300);
		res = sensors.getResolution(addr);
		sensors.setWaitForConversion(true);

		if (report) {
			ITCHWrite(F("Bus:"));
			strcpy_hal(fmt, F(" %d"));
			sprintf(out, fmt, ifs->bus_pin);
			ITCHWrite(out);

			ITCHWrite(F(" Index:"));
			strcpy_hal(fmt, F(" %d Addr: "));
			sprintf(out, fmt, index);
			ITCHWrite(out);

			DallasAddressArrayToString(out, addr);
			ITCHWrite(out);

			ITCHWrite(F(" Resolution:"));
			strcpy_hal(fmt, F(" %d TempC: "));
			sprintf(out, fmt, res);
			ITCHWrite(out);

			FFFloatToCString(out, tempC);
			ITCHWrite(out);
		}

		match = GetInterfaceWithMatchingDallas(addr);
		if (match) {
			if (report) {
				ITCHWrite(F(" - Matched To: "));
				strcpy_hal(fmt, F("%d "));
				sprintf(out, fmt, match);
				ITCHWrite(out);
				ITCHWriteLine(GetBlockLabelString(match));
			} else {
				EventMsg(i->id, E_INFO, M_DALLAS_DETECTED);
			}
		} else {
			if (report) {
				ITCHWriteLine(F(" - NOT MATCHED."));
				//ITCHWriteLine(F(" - Assign first unmatched with: \"INTERFACE ONEWIRE ASSIGN <bus_id> <device_id>\""));
			} else {
				EventMsg(i->id, E_WARNING, M_UNASSIGNED_DALLAS);
			}
		}

	}
}

void AssignDallasSensor(InterfaceNode *s, InterfaceNode *t, uint8_t report) {
	InterfaceONEWIRE_BUS *ifs_s;
	InterfaceDS1820B *ifs_t;
	ifs_s = (InterfaceONEWIRE_BUS *)s->IFSettings;
	ifs_t = (InterfaceDS1820B *)t->IFSettings;

	OneWire one_wire(ifs_s->bus_pin);            		// oneWire instance to communicate with any OneWire devices
	DallasTemperature sensors(&one_wire);     // Pass our one_wire reference to Dallas Temperature

	DeviceAddress addr;
	uint8_t count;

	uint16_t match;

	char fmt[11];
	char out[MAX_LOG_LINE_LENGTH];

	sensors.begin();
	count = sensors.getDeviceCount();

	for (uint8_t index = 0; index < count; index++) {
		sensors.getAddress(addr, index);
		match = GetInterfaceWithMatchingDallas(addr);
		if (match) {
			if (report) {
				ITCHWrite(F("Bus:"));
				strcpy_hal(fmt, F(" %d"));
				sprintf(out, fmt, ifs_s->bus_pin);
				ITCHWrite(out);

				ITCHWrite(F(" Index:"));
				strcpy_hal(fmt, F(" %d Addr: "));
				sprintf(out, fmt, index);
				ITCHWrite(out);

				DallasAddressArrayToString(out, addr);
				ITCHWrite(out);
				ITCHWrite(F(" - Matched to: "));
				strcpy_hal(fmt, F("%d "));
				sprintf(out, fmt, match);
				ITCHWrite(out);
				ITCHWriteLine(GetBlockLabelString(match));
			}
		} else {
			// assign it to the Dallas interface
			ifs_t->bus_pin = ifs_s->bus_pin;
			DallasCopy(addr, ifs_t->dallas_address);
			if (report) {
				ITCHWrite(F("Bus:"));
				strcpy_hal(fmt, F(" %d"));
				sprintf(out, fmt, ifs_s->bus_pin);
				ITCHWrite(out);

				ITCHWrite(F(" Index:"));
				strcpy_hal(fmt, F(" %d Addr: "));
				sprintf(out, fmt, index);
				ITCHWrite(out);
				DallasAddressArrayToString(out, addr);
				ITCHWrite(out);
				ITCHWrite(F(" - Assigned to "));
				ITCHWriteLine(t->label);
			}
			return; // only assign the first unmatched device
		}
		//delay(500);
	}

}

void InterfaceONEWIRE_BUS_Setup(InterfaceNode* b) {
	ScanAndMatchDallasSensors(b, 0);
}

void InterfaceDS1820B_Setup(InterfaceNode *b) {
	InterfaceDS1820B* ifs;
	ifs = (InterfaceDS1820B *)b->IFSettings;
	float tempC;

	OneWire one_wire(ifs->bus_pin); // oneWire instance to communicate with any OneWire devices
	DallasTemperature sensors(&one_wire);     // Pass our one_wire reference to Dallas Temperature

	sensors.begin();
	if (sensors.isConnected(ifs->dallas_address)) {
		sensors.setResolution(ifs->dallas_address, 12);
		sensors.setCheckForConversion(true);
		sensors.setWaitForConversion(true);
		sensors.requestTemperaturesByAddress(ifs->dallas_address);
		tempC = sensors.getTempC(ifs->dallas_address);
		EventMsg(b->id, E_INFO, M_DS1820B_INIT, tempC, FLOAT_INIT);
		sensors.setWaitForConversion(false);
		ifs->in_progress = 0;
	} else {
		EventMsg(b->id, E_WARNING, M_DS1820B_INIT_FAIL);
		ifs->in_progress = 0;
	}
}

uint8_t InterfaceDS1820B_Read(float* tempC, uint16_t id, uint8_t wait) {
	// Read the temperature in degrees C of the Dallas DS1820B device
	// configured as an interface identified by id.
	// If successful update the result in *tempC
	// If wait is specified as true (1), wait for the result after
	// 	requesting a conversion (ie telling device to read and convert temp)
	// If wait is false (0), request a conversion and return immediately
	// Returns:
	//	0 - Error - device disconnected or result out of range
	//  1 - Conversion completed and *tempC has been updated
	//  2 - if wait is false - conversion requested or in progress, not yet available

	InterfaceNode* b;
	InterfaceDS1820B* ifs;
	float temptempC = FLOAT_INIT;

	b = GetInterfaceByID(id);
	if (b != NULL) {
		ifs = (InterfaceDS1820B *)b->IFSettings;

		OneWire one_wire(ifs->bus_pin);            		// oneWire instance to communicate with any OneWire devices
		DallasTemperature sensors(&one_wire);     // Pass our one_wire reference to Dallas Temperature

		//sensors.begin();

		if (wait) {
			sensors.setWaitForConversion(true);
			sensors.setResolution(12);
			if (sensors.requestTemperaturesByAddress(ifs->dallas_address)) {
				temptempC = sensors.getTempC(ifs->dallas_address);
				if ( (temptempC > (float)TEMP_C_LOW) && temptempC < (float)TEMP_C_HIGH) {
					*tempC = temptempC;
					b->status = STATUS_ENABLED_RUN;
					ifs->in_progress = 0;
					return 1;
				} else {
					EventMsg(b->id, E_WARNING, M_BAD_TEMPERATURE_READ, 0, temptempC);
					b->status = STATUS_ENABLED_INVALID_DATA;
					ifs->in_progress = 0;
					return 0;
				}
			} else {
				EventMsg(b->id, E_ERROR, M_DS1820B_INIT_FAIL);
				b->status = STATUS_DISABLED_ERROR;
				ifs->in_progress = 0;
				return 0;
			}
		} else { // not wait
			if (ifs->in_progress == 1 && sensors.isConversionComplete()) {
				// requested previously and now available
				temptempC = sensors.getTempC(ifs->dallas_address);
				ifs->in_progress = 0;
				if ( (temptempC > (float)TEMP_C_LOW) && temptempC < (float)TEMP_C_HIGH) {
					*tempC = temptempC;
					b->status = STATUS_ENABLED_RUN;
					return 1;
				} else {
					EventMsg(b->id, E_WARNING, M_BAD_TEMPERATURE_READ, 0, temptempC);
					b->status = STATUS_ENABLED_INVALID_DATA;
					return 0;
				}
			} else {
				if (ifs->in_progress == 1) {
					// therefore isConversionAvailable was false - still converting
					b->status = STATUS_ENABLED_BUSY;
					return 2;
				} else {
					// not in_progress
					sensors.setWaitForConversion(false);
					sensors.setResolution(12);
					if (sensors.requestTemperaturesByAddress(ifs->dallas_address)) {
						b->status = STATUS_ENABLED_BUSY;
						ifs->in_progress = 1;
						return 2;
					} else {
						EventMsg(b->id, E_WARNING, M_BAD_TEMPERATURE_READ, 0, temptempC);
						ifs->in_progress = 0;
						b->status = STATUS_DISABLED_ERROR;
						return 0;
					}
				}
			}
		}
	} else {
		// b== NULL
		EventMsg(SSS, E_ERROR, M_DS1820B_BID_NULL);
		return 0;
	}
	return 0;
}

void InterfaceShow(InterfaceNode *b, void(Callback(const char *))) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	char fmt_str[MAX_LABEL_LENGTH];
	char label_str[MAX_LABEL_LENGTH];

	strcpy_hal(out_str, F("Base Data"));
	Callback(out_str);

	strcpy_hal(fmt_str, F(" id:           %d"));
	sprintf(out_str, fmt_str, b->id);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" cat:          %s (%d)"));
	strcpy_hal(label_str, block_cat_strings[FF_INTERFACE].text);
	sprintf(out_str, fmt_str, label_str, FF_INTERFACE);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" type:         %s (%d)"));
	strcpy_hal(label_str, interface_type_strings[b->type].text);
	sprintf(out_str, fmt_str, label_str, b->type);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" label:        %s"));
	sprintf(out_str, fmt_str, b->label);
	Callback(out_str);

#ifndef EXCLUDE_DISPLAYNAME
	strcpy_hal(fmt_str, F(" display_name: %s"));
	sprintf(out_str, fmt_str, b->display_name);
	Callback(out_str);
#endif
#ifndef EXCLUDE_DESCRIPTION
	strcpy_hal(fmt_str, F(" description:  %s"));
	sprintf(out_str, fmt_str, b->description);
	Callback(out_str);
#endif

	strcpy_hal(out_str, F("Operational Data"));
	Callback(out_str);

	strcpy_hal(fmt_str, F(" active:       %s"));
	strcpy_hal(label_str, F("NA"));
	sprintf(out_str, fmt_str, label_str);
	Callback(out_str);

	/*
	strcpy_hal(fmt_str, F(" bool_val:     %s (%d)"));
	if (b->bool_val == 1) {
		strcpy_hal(label_str, F("True"));
	} else {
		strcpy_hal(label_str, F("False"));
	}
	sprintf(out_str, fmt_str, label_str, b->bool_val);
	Callback(out_str);

	if (b->int_val == INT32_INIT) {
		strcpy_hal(out_str, F(" int_val:      INT32_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" int_val:      %ld"));
		sprintf(out_str, fmt_str, b->int_val);
	}
	Callback(out_str);

	if (b->f_val == FLOAT_INIT) {
		strcpy_hal(out_str, F(" f_val:        FLOAT_INIT"));
	} else {
		FFFloatToCString(label_str, b->f_val);
		strcpy_hal(fmt_str, F(" f_val:        %s"));
		sprintf(out_str, fmt_str, label_str);
	}
	Callback(out_str);
	*/

	strcpy_hal(fmt_str, F(" status:       %s (%d)"));
	strcpy_hal(label_str, status_strings[b->status].text);
	sprintf(out_str, fmt_str, label_str, b->status);
	Callback(out_str);

	strcpy_hal(out_str, F("Interface:"));
	Callback(out_str);

	switch (b->type) {
		case IF_ONEWIRE_BUS: {
			InterfaceONEWIRE_BUS* ifs;
			ifs = (InterfaceONEWIRE_BUS *)b->IFSettings;

			strcpy_hal(fmt_str, F(" bus_pin:      %d"));
			sprintf(out_str, fmt_str, ifs->bus_pin);
			Callback(out_str);

			strcpy_hal(fmt_str, F(" device_count  %d"));
			sprintf(out_str, fmt_str, ifs->device_count);
			Callback(out_str);
		}
			break;
		case IF_DS1820B: {
			InterfaceDS1820B* ifs;
			ifs = (InterfaceDS1820B *)b->IFSettings;

			strcpy_hal(fmt_str, F(" bus_pin:      %d"));
			sprintf(out_str, fmt_str, ifs->bus_pin);
			Callback(out_str);

			strcpy_hal(fmt_str, F(" dallas_addr   %s"));
			DallasAddressArrayToString(label_str, ifs->dallas_address);
			sprintf(out_str, fmt_str, label_str);
			Callback(out_str);

		}
			break;
		default:
			break;
	}
}


