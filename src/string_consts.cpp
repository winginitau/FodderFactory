/************************************************
 ff_string_consts.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

  String Constants Handling
 ************************************************/

/************************************************
  Includes
************************************************/

#include <build_config.h>
#include <string.h>
#include <stdint.h>
#include <string_consts.h>

/************************************************
  Globals
************************************************/

#ifdef USE_PROGMEM
// holder for strings copied from PROGMEM
//static char temp_pgm_string_msg_type[MAX_MESSAGE_STRING_LENGTH];
//static char temp_pgm_string_msg[MAX_MESSAGE_STRING_LENGTH];
#endif

/************************************************
  Functions
************************************************/

uint8_t GetLanguage(void) {
	//TODO language switching functions
	return ENGLISH;
}


void GetMessageTypeString(char *str_buf, int message_type_enum) {
	StringArray temp;
#ifdef USE_PROGMEM
	//XXX
	memcpy_P(&temp, &message_type_strings[message_type_enum], sizeof(temp));
#else
	memcpy(&temp, &message_type_strings[message_type_enum], sizeof(temp));
#endif
	strcpy(str_buf, temp.text[GetLanguage()]);
	//return str_buf;
}

void GetMessageString(char *str_buf, int message_enum) {
	StringArray temp;
#ifdef USE_PROGMEM
	memcpy_P(&temp, &message_strings[message_enum], sizeof(temp));
#else
	memcpy(&temp, &message_strings[message_enum], sizeof(temp));
#endif
	strcpy(str_buf, temp.text[GetLanguage()]);
	//return temp_pgm_string_msg;
}

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

char *strcpy_misc(char *dest, uint8_t src) {
	#ifdef ARDUINO
		return strcpy_P(dest, misc_strings[src].text);
	#else
		return strcpy(dest, misc_strings[src].text);
	#endif
}

char *strcat_misc(char *dest, uint8_t src) {
	#ifdef ARDUINO
		return strcat_P(dest, misc_strings[src].text);
	#else
		return strcat(dest, misc_strings[src].text);
	#endif
}


/**************************************************************************
 * Properly formed Generic string function overloads accommodating PROGMEM
 *************************************************************************/
char *strcpy_hal(char *dest, const char *src) {
	return strcpy(dest, src);
}

int strcmp_hal(char *dest, const char *src) {
	return strcmp(dest, src);
}

#ifdef ARDUINO

char *strcpy_hal(char *dest, const __FlashStringHelper *src) {
	return strcpy_P(dest, (const char *)src);
}

int strcmp_hal(char *dest, const __FlashStringHelper *src) {
	return strcmp_P(dest, (const char *)src);
}

#endif

/**************************************************************************/

char *strcat_hal(char *dest, const char *src) {
#ifdef ARDUINO
	return strcat_P(dest, src);
#else
	return strcat(dest, src);
#endif
}

void *memcpy_hal(void *dest, const void *src, size_t sz) {
#ifdef ARDUINO
	return memcpy_P(dest, src, sz);
#else
	return memcpy(dest, src, sz);
#endif
}

/*
#ifdef DEBUG
char *strcpy_debug(char *dest, uint8_t src) {
	#ifdef ARDUINO
		return strcpy_P(dest, debug_strings[src].text);
	#else
		return strcpy(dest, itch_debug_strings[src].text);
	#endif
}

char *strcat_debug(char *dest, uint8_t src) {
	#ifdef ARDUINO
		return strcat_P(dest, debug_strings[src].text);
	#else
		return strcat(dest, itch_debug_strings[src].text);
	#endif
}
*/
