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

#ifdef PLATFORM_LINUX
#include <unistd.h>
#endif //PLATFORM_LINUX
#ifdef PLATFORM_ARDUINO
#include <Arduino.h>
#endif //PLATFORM_ARDUINO

void SystemSetup(BlockNode* b) {

	if(b->settings.sys.start_delay > 0) {
		EventMsg(SSS, E_INFO, M_START_DELAY_ACTIVE);
		#ifdef PLATFORM_ARDUINO
			// Milliseconds
			delay(b->settings.sys.start_delay);
		#endif //PLATFORM_ARDUINO
		#ifdef PLATFORM_LINUX
			sleep(b->settings.sys.start_delay / 1000);
		#endif //PLATFORM_LINUX
	}
}

void SystemOperate(BlockNode* b) {
	(void)b;
}

void SystemShow(BlockNode *b, void(Callback(const char *))) {
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


