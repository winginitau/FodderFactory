/************************************************
 ff_string_consts.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

  String Constants Handling
 ************************************************/

/************************************************
  Includes
************************************************/

#include "ff_sys_config.h"
#include "ff_string_consts.h"

#include <string.h>
#include <stdint.h>

/************************************************
  Globals
************************************************/

#ifdef USE_PROGMEM
// holder for strings copied from PROGMEM
static char temp_pgm_string[MAX_MESSAGE_STRING_LENGTH];
#endif

/************************************************
  Functions
************************************************/

uint8_t GetLanguage(void) {
	//TODO language switching functions
	return ENGLISH;
}

#ifdef USE_PROGMEM
char const* GetMessageTypeString(int message_type_enum) {
	StringArray temp;
	memcpy_P(&temp, &message_type_strings[message_type_enum], sizeof(temp));
	strcpy(temp_pgm_string, temp.text[GetLanguage()]);
	return temp_pgm_string;
}

char const* GetMessageString(int message_enum) {
	StringArray temp;
	memcpy_P(&temp, &message_strings[message_enum], sizeof(temp));
	strcpy(temp_pgm_string, temp.text[GetLanguage()]);
	return temp_pgm_string;
}
#else
char const* GetMessageTypeString(int message_type_enum) {
	return message_type_strings[message_type_enum].text[GetLanguage()];
}

char const* GetMessageString(int message_enum) {
	return message_strings[message_enum].text[GetLanguage()];
}
#endif

#ifdef USE_PROGMEM
uint8_t DayStringArrayIndex(const char* key) {
	SimpleStringArray temp;
	for (int i = 0; i < LAST_DAY; i++) {
		memcpy_P (&temp, &day_strings[i], sizeof temp);
		if (strcmp(key, temp.text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}
#else
uint8_t DayStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_DAY; i++) {
		if (strcmp(key, day_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}
#endif

#ifdef USE_PROGMEM
uint8_t UnitStringArrayIndex(const char* key) {
	SimpleStringArray temp;
	for (int i = 0; i < LAST_UNIT; i++) {
		memcpy_P(&temp, &unit_strings[i], sizeof(temp));
		if (strcmp(key, temp.text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}

#else
uint8_t UnitStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_UNIT; i++) {
		if (strcmp(key, unit_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}
#endif

#ifdef USE_PROGMEM
uint8_t LanguageStringArrayIndex(const char* key) {
	SimpleStringArray temp;
	for (int i = 0; i < LAST_LANGUAGE; i++) {
		memcpy_P (&temp, &language_strings[i], sizeof temp);
		if (strcmp(key, temp.text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}
#else
uint8_t LanguageStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_LANGUAGE; i++) {
		if (strcmp(key, language_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}
#endif


#ifdef USE_PROGMEM
uint8_t InterfaceStringArrayIndex(const char* key) {
	//
	for (int i = 0; i < LAST_INTERFACE; i++) {
		if (strcmp_P(key, interface_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}
#else
uint8_t InterfaceStringArrayIndex(const char* key) {
	//
	for (int i = 0; i < LAST_INTERFACE; i++) {
		if (strcmp(key, interface_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}
#endif

#ifdef USE_PROGMEM
uint8_t BlockTypeStringArrayIndex(const char* key) {
	SimpleStringArray temp;
	for (int i = 0; i < LAST_BLOCK_TYPE; i++) {
		memcpy_P (&temp, &block_type_strings[i], sizeof temp);
		if (strcmp(key, temp.text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}

#else
uint8_t BlockTypeStringArrayIndex(const char* key) {
	//
	for (int i = 0; i < LAST_BLOCK_TYPE; i++) {
		if (strcmp(key, block_type_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}
#endif

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
