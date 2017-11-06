/*
 * ff_string_consts.cpp
 *
 *  Created on: 22 Oct. 2017
 *      Author: brendan
 */

#include "ff_sys_config.h"
#include "ff_string_consts.h"

#ifdef FF_SIMULATOR
#include <string.h>
#include <stdint.h>
#endif


uint8_t GetLanguage(void) {
	//TODO language switching functions
	return ENGLISH;
}

char const* GetMessageTypeString(int message_type_enum) {
	return message_type_strings[message_type_enum].text[GetLanguage()];
}

char const* GetMessageString(int message_enum) {
	return message_strings[message_enum].text[GetLanguage()];
}

uint8_t DayStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_DAY; i++) {
		if (strcmp(key, day_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}

uint8_t UnitStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_UNIT; i++) {
		if (strcmp(key, unit_strings[i].text[ENGLISH]) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}

uint8_t LanguageStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_LANGUAGE; i++) {
		if (strcmp(key, language_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}


uint8_t InterfaceStringArrayIndex(const char* key) {
	//
	for (int i = 0; i < LAST_INTERFACE; i++) {
		if (strcmp(key, interface_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}

uint8_t BlockTypeStringArrayIndex(const char* key) {
	//
	for (int i = 0; i < LAST_BLOCK_TYPE; i++) {
		if (strcmp(key, block_type_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}

/*
uint8_t SimpleStringArrayIndex(const SimpleStringArray array[], const char* key) {
	//
	for (int i = 0; i < sizeof(array); i++) {
		if (strcmp(key, array[i].text) == 0) {
			return i;
		}
	}
	return 255;
}
*/