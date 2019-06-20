/*****************************************************************
 interfaces.cpp

 Copyright (C) 2019 Brendan McLearie 

 Created on: 17 Jun 2019

******************************************************************/
#include <system.h>
#include <block_common.h>
#include <string_consts.h>
#include <events.h>
#include <HAL.h>
#include <utils.h>

#ifdef TARGET_PLATFORM_LINUX
#include <unistd.h>
#endif //TARGET_PLATFORM_LINUX
#ifdef TARGET_PLATFORM_ARDUINO
#include <Arduino.h>
#ifdef IF_ARDUINO_VEDIRECT_OLD
#include <VEDirect.h>
#endif //IF_ARDUINO_VEDIRECT
#endif //TARGET_PLATFORM_ARDUINO

#ifdef IF_ARDUINO_VEDIRECT_OLD
// XXX HACK VE Direct
time_t VE_last_polled;
time_t VE_last_logged;
TV_TYPE VE_log_rate;
TV_TYPE VE_poll_rate;
int32_t VE_soc, VE_power;
int32_t VE_voltage, VE_current;
uint8_t VE_status;

VEDirect ve(Serial3);
#endif //IF_ARDUINO_VEDIRECT_OLD

void SystemSetup(BlockNode* b) {

	if(b->settings.sys.start_delay > 0) {
		EventMsg(SSS, E_INFO, M_START_DELAY_ACTIVE);
		#ifdef TARGET_PLATFORM_ARDUINO
			// Milliseconds
			delay(b->settings.sys.start_delay);
		#endif
		#ifdef TARGET_PLATFORM_LINUX
			sleep(b->settings.sys.start_delay / 1000);
		#endif
	}

#ifdef IF_ARDUINO_VEDIRECT_OLD
#if defined VE_DIRECT && defined FF_ARDUINO
// XXX VE Hack
if (ve.begin()) {
	VE_last_logged = TimeNow();
	VE_last_polled = TV_TYPE_INIT;
	VE_log_rate = VE_LOG_RATE;
	VE_poll_rate = VE_POLL_RATE;
	VE_status = STATUS_ENABLED_INIT;
	EventMsg(SSS, E_INFO, M_VE_INIT);
} else {
	VE_status = STATUS_DISABLED_ERROR;
	EventMsg(SSS, E_ERROR, M_VE_INIT_ERROR);
}
#endif //VE_DIRECT

#endif //IF_ARDUINO_VEDIRECT
}

void SystemOperate(BlockNode* b) {
	(void)b;

	#ifdef IF_ARDUINO_VEDIRECT_OLD
// XXX VE Hack

//	State of Charge (SOC): 999    9.99   %
//	Power:                 -27    27     W
//	Voltage                25954  25.954 V
//	Current                -955   0.955  A

	if ((VE_status > STATUS_ERROR) && (VE_status < STATUS_DISABLED)) {
		time_t VE_now = TimeNow();
		time_t VE_next_poll;
		time_t VE_next_log;
		uint32_t new_current;
		VE_next_log = VE_last_logged + VE_log_rate;
		VE_next_poll = VE_last_polled + VE_poll_rate;
		if (VE_now >= VE_next_poll) {
			VE_last_polled = TimeNow();

			VE_soc = ve.read(VE_SOC);
			VE_power = ve.read(VE_POWER);
			VE_voltage = ve.read(VE_VOLTAGE);

			new_current = ve.read(VE_CURRENT);
			if (VarianceExceedsAbsolute(VE_current, new_current, 1000)) {
				VE_next_log = TimeNow() - 5; // set logging to earlier to trigger test
			}
			VE_current = new_current;

		}
		if (VE_now >= VE_next_log) {
			VE_last_logged = TimeNow();
			VE_status = STATUS_ENABLED_VALID_DATA;
			EventMsg(SSS, E_DATA, M_VE_SOC, VE_soc, 0);
			EventMsg(SSS, E_DATA, M_VE_VOLTAGE, VE_voltage, 0);
			EventMsg(SSS, E_DATA, M_VE_POWER, VE_power, 0);
			EventMsg(SSS, E_DATA, M_VE_CURRENT, VE_current, 0);
		}
	}

#endif //IF_ARDUINO_VEDIRECT
}

void SystemShow(BlockNode *b, void(Callback(char *))) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	char fmt_str[MAX_LABEL_LENGTH];
	char label_str[MAX_LABEL_LENGTH];

	CommonShow(b, Callback);

	strcpy_hal(out_str, F("System Block Specific Data"));
	Callback(out_str);

	strcpy_hal(fmt_str, F(" temp_scale:   %s (%d)"));
	strcpy_hal(label_str, unit_strings[b->settings.sys.temp_scale].text);
	sprintf(out_str, fmt_str, label_str, b->settings.sys.temp_scale);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" language:     %s (%d)"));
	strcpy_hal(label_str, language_strings[b->settings.sys.language].text);
	sprintf(out_str, fmt_str, label_str, b->settings.sys.language);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" week_start    %s (%d)"));
	strcpy_hal(label_str, day_strings[b->settings.sys.week_start].text);
	sprintf(out_str, fmt_str, label_str, b->settings.sys.week_start);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" start_delay   %d"));
	sprintf(out_str, fmt_str, b->settings.sys.start_delay);
	Callback(out_str);
}


