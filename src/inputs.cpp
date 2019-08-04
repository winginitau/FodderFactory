/************************************************
 ff_inputs.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Input Processing
 ************************************************/


/************************************************
  Includes
************************************************/
#include <build_config.h>
#include <debug_ff.h>
#include <events.h>
#include <HAL.h>
#include <inputs.h>
#include <registry.h>
#include <string_consts.h>
#include <block_common.h>
#include <utils.h>
#include <interfaces.h>

#ifdef ARDUINO_VEDIRECT
#include <VEDirect.h>
#endif //ARDUINO_VEDIRECT

/************************************************
  Data Structures
************************************************/


/************************************************
  Globals
************************************************/


/************************************************
  Input Processing Functions
************************************************/

void InputSetup(BlockNode *b) {

	switch (b->type) {
		case IN_ONEWIRE: {
			float tempC;
			uint8_t read_result = 0;

			#ifdef IN_LOGRATE_OVERRIDE
			b->settings.in.log_rate = IN_LOGRATE_OVERRIDE;
			#endif //IN_LOGRATE_OVERRIDE

			b->settings.in.last_polled = LastTime(0, b->settings.in.poll_rate);
			b->settings.in.last_logged = LastTime(0, b->settings.in.log_rate);

			// Read interface and wait for result
			read_result = InterfaceDS1820B_Read(&tempC, b->settings.in.interface, 1);
			if (read_result == 1) {
				// Good read
				b->f_val = tempC;
				EventMsg(b->id, E_DATA, M_F_READ, 0, b->f_val);
				b->status = STATUS_ENABLED_RUN;
			} else {
				EventMsg(b->id, E_WARNING, M_BAD_TEMPERATURE_READ, 0, b->f_val);
				b->status = STATUS_ENABLED_INVALID_DATA;
			}
			break;
		}

		case IN_DIGITAL:
			if (b->settings.in.interface == IF_DIG_PIN_IN) {
				HALInitDigitalInput(b->settings.in.if_num);
				b->settings.in.last_logged = TimeNow();
				b->bool_val = HALDigitalRead(b->settings.in.if_num);
			}
			b->status = STATUS_ENABLED_INIT;
			break;

		case IN_VEDIRECT: {
			#ifdef ARDUINO_VEDIRECT
			VEDirect ve(Serial3);

			if (ve.begin()) {
				//VE_last_logged = TimeNow();
				b->settings.in.last_polled = LastTime(0, b->settings.in.poll_rate);
				b->settings.in.last_logged = LastTime(0, b->settings.in.log_rate);
				//VE_last_polled = TV_TYPE_INIT;
				//VE_log_rate = VE_LOG_RATE;
				//VE_poll_rate = VE_POLL_RATE;
				//VE_status = STATUS_ENABLED_INIT;
				b->status = STATUS_ENABLED_RUN;
				EventMsg(SSS, E_INFO, M_VE_INIT);
			} else {
				//VE_status = STATUS_DISABLED_ERROR;
				b->status = STATUS_DISABLED_ERROR;
				EventMsg(SSS, E_ERROR, M_VE_INIT_ERROR);
			}
			#endif //ARDUINO_VEDIRECT
			break;
		}

		default:
			EventMsg(SSS, E_ERROR, M_UNKNOWN_BLOCK_TYPE);
			break;
	}
}



void InputOperate(BlockNode *b) {
	time_t now = TimeNow();
	time_t next;

	switch (b->type) {
		case IN_ONEWIRE: {
			float tempC;
			uint8_t read_result = 0;

			next = b->settings.in.last_logged + b->settings.in.log_rate;
			//next = NextTime(0, b->settings.in.log_rate);

			if (now >= next) {
				// Due to read and log
				// Read interface. Don't wait for result - wait == 0!
				read_result = InterfaceDS1820B_Read(&tempC, b->settings.in.interface, 0);
				switch (read_result) {
					case 2:
						// Temperature conversion in progress not yet finished
						b->status = STATUS_ENABLED_BUSY;
						break;
					case 1:
						// Good read
						b->settings.in.last_polled = LastTime(0, b->settings.in.poll_rate);
						b->settings.in.last_logged = LastTime(0, b->settings.in.log_rate);
						b->f_val = tempC;
						EventMsg(b->id, E_DATA, M_F_READ, 0, b->f_val);
						b->status = STATUS_ENABLED_RUN;
						break;
					default:
						b->settings.in.last_polled = LastTime(0, b->settings.in.poll_rate);
						b->settings.in.last_logged = LastTime(0, b->settings.in.log_rate);
						EventMsg(b->id, E_WARNING, M_BAD_TEMPERATURE_READ, 0, b->f_val);
						b->status = STATUS_ENABLED_INVALID_DATA;
						break;
				}
			}
			break;
		}
		case IN_DIGITAL: {
			//TODO remove log_rate test and allow asynchronous triggering
			// currently implemented with log_rate to allow random flip-flop
			// state to be generated each loop in HALDigitalRead
			next = b->settings.in.last_logged + b->settings.in.log_rate;
			if (now >= next) {
				uint8_t reading = HALDigitalRead(b->settings.in.if_num);
				if (reading != b->bool_val) {
					b->settings.in.last_logged = now;
					b->bool_val = reading;
					EventMsg(b->id, E_DATA, M_FLIPFLOP);
				}
			}
			break;
		}

		case IN_VEDIRECT: {
			#ifdef ARDUINO_VEDIRECT
			//	State of Charge (SOC): 999    9.99   %
			//	Power:                 -27    27     W
			//	Voltage                25954  25.954 mV -> V
			//	Current                -955   0.955  mA -> A
			if ((b->status > STATUS_ERROR) && (b->status < STATUS_DISABLED)) {
				// Test for poll_rate - is it time?
				next = b->settings.in.last_polled + b->settings.in.poll_rate;
				if (now > next) {
					VEDirect ve(Serial3);
					if (ve.begin()) {
						// Update the poll time to now
						b->settings.in.last_polled = LastTime(0, b->settings.in.poll_rate);
						switch(b->settings.in.interface) {
							case IF_VED_VOLTAGE:
								b->int_val = ve.read(VE_VOLTAGE);
								break;
//XXX
							case IF_VED_CURRENT: {
								int32_t new_current = ve.read(VE_CURRENT);
								// Test if it differs substantially from previous (load on/of)
								if (VarianceExceedsAbsolute(b->int_val, new_current, 1000)) {
									// Trigger it to log next loop by setting last_logged back
									b->settings.in.last_logged = TimeNow() - b->settings.in.log_rate;
								}
								b->int_val = new_current;
							}
								break;

							case IF_VED_POWER:
								b->int_val = ve.read(VE_POWER);
								break;

							case IF_VED_SOC:
								b->int_val = ve.read(VE_SOC);
								break;
						} //switch

						if (TimeNow() >= (b->settings.in.last_logged + b->settings.in.log_rate)) {
							b->settings.in.last_logged = TimeNow();
							b->status = STATUS_ENABLED_VALID_DATA;
							// XXX Update source "SSS" to BID after RPI update
							if(b->settings.in.interface == IF_VED_VOLTAGE) {
								EventMsg(SSS, E_DATA, M_VE_VOLTAGE, b->int_val, 0);
							}
							if(b->settings.in.interface == IF_VED_CURRENT) {
								EventMsg(SSS, E_DATA, M_VE_CURRENT, b->int_val, 0);
							}
							if(b->settings.in.interface == IF_VED_POWER) {
								EventMsg(SSS, E_DATA, M_VE_POWER, b->int_val, 0);
							}
							if(b->settings.in.interface == IF_VED_SOC) {
								EventMsg(SSS, E_DATA, M_VE_SOC, b->int_val, 0);
							}
						}
					} else {
						// begin failed
						EventMsg(b->id, E_ERROR, M_VE_FAILED);
						b->status = STATUS_DISABLED_ERROR;
					}
				}
			}
			#endif //ARDUINO_VEDIRECT
			break;
		}

		default:
			EventMsg(SSS, E_ERROR, M_UNKNOWN_BLOCK_TYPE);
			break;
	}
}

void InputShow(BlockNode *b, void(Callback(const char *))) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	char fmt_str[MAX_LABEL_LENGTH];
	char label_str[MAX_LABEL_LENGTH];
	const char* label_ptr;

	CommonShow(b, Callback);

	strcpy_hal(out_str, F("Input:"));
	Callback(out_str);

	// XXX transition while moving all IN types to talk to interfaces
	if (b->type == IN_ONEWIRE) {
		strcpy_hal(fmt_str, F(" interface:    %s (%d)"));
		label_ptr = GetBlockLabelString(b->settings.in.interface);
		sprintf(out_str, fmt_str, label_ptr, b->settings.in.interface);
		Callback(out_str);
	} else {
		strcpy_hal(fmt_str, F(" interface:    %s (%d)"));
		strcpy_hal(label_str, interface_type_strings[b->settings.in.interface].text);
		sprintf(out_str, fmt_str, label_str, b->settings.in.interface);
		Callback(out_str);
	}

	strcpy_hal(fmt_str, F(" if_num:       %d"));
	sprintf(out_str, fmt_str, b->settings.in.if_num);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" log_rate:     %s (%lu)"));
	TimeValueToTimeString(label_str, b->settings.in.log_rate);
	sprintf(out_str, fmt_str, label_str, (unsigned long) b->settings.in.log_rate); //TV_TYPE
	Callback(out_str);

	strcpy_hal(fmt_str, F(" poll_rate:    %s (%lu)"));
	TimeValueToTimeString(label_str, b->settings.in.poll_rate);
	sprintf(out_str, fmt_str, label_str, (unsigned long) b->settings.in.poll_rate); //TV_TYPE
	Callback(out_str);

	strcpy_hal(fmt_str, F(" data_units:   %s (%d)"));
	strcpy_hal(label_str, unit_strings[b->settings.in.data_units].text);
	sprintf(out_str, fmt_str, label_str, b->settings.in.data_units);
	Callback(out_str);

	//strcpy_hal(fmt_str, F(" data_type:    %s (%d)"));
	//sprintf(out_str, fmt_str, b->settings.in.data_type);		// float, int
	//Callback(out_str);

	if (b->settings.in.last_polled == TV_TYPE_INIT) {
		strcpy_hal(out_str, F(" last_update:  TV_TYPE_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" last_update:  %s (%lu)"));
		CTimeToISODateTimeString(label_str, b->settings.in.last_polled);
		sprintf(out_str, fmt_str, label_str, b->settings.in.last_polled);
	}
	Callback(out_str);

	if (b->settings.in.last_logged == TV_TYPE_INIT) {
		strcpy_hal(out_str, F(" last_logged:  TV_TYPE_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" last_logged:  %s (%lu)"));
		CTimeToISODateTimeString(label_str, b->settings.in.last_logged);
		sprintf(out_str, fmt_str, label_str, b->settings.in.last_logged);
	}
	Callback(out_str);


}










