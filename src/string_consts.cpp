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
#include <debug_ff.h>

/************************************************
  Globals
************************************************/


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

uint8_t DayStringArrayIndex(const char* key) {
	// TODO - revisit for error catching (0 == SUN rather than DAY_ERROR)
	for (int i = 0; i < LAST_DAY; i++) {
		if (strcmp_hal(key, day_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}


uint8_t UnitStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_UNIT; i++) {
		if (strcmp_hal(key, unit_strings[i].text) == 0) {
			return i;
		}
	}
	return UNIT_ERROR;
}

uint8_t CommandStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_COMMAND; i++) {
		if (strcmp_hal(key, command_strings[i].text) == 0) {
			return i;
		}
	}
	return CMD_ERROR;
}

uint8_t LanguageStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_LANGUAGE; i++) {
		if (strcmp_hal(key, language_strings[i].text) == 0) {
			return i;
		}
	}
	return 0; // DEFAULT ENGLISH == 0
}

uint8_t InterfaceStringArrayIndex(const char* key) {
	//
	for (uint8_t i = 0; i < LAST_INTERFACE; i++) {
		if (strcmp_hal(key, interface_strings[i].text) == 0) {
			return i;
		}
	}
	return IF_ERROR; //ERROR
}

uint8_t BlockTypeStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_BLOCK_TYPE; i++) {
		if (strcmp_hal(key, block_type_strings[i].text) == 0) {
			return i;
		}
	}
	return BT_ERROR; //ERROR
}


/**************************************************************************
 * Properly formed Generic string function overloads accommodating PROGMEM
 *************************************************************************/

// The hal form should only be used for source or 2nd parameter strings
// that are known to be in PROGMEM on AVR platforms.

char *strcpy_hal(char *dest, const char *src) {
	#ifdef ARDUINO
		return strcpy_P(dest, src);
	#else
		return strcpy(dest, src);
	#endif
}

char *strcat_hal(char *dest, const char *src) {
	#ifdef ARDUINO
		return strcat_P(dest, src);
	#else
		return strcat(dest, src);
#endif
}

int strcmp_hal(const char *s1, const char *s2) {
	#ifdef ARDUINO
		return strcmp_P(s1, s2);
	#else
		return strcmp(s1, s2);
	#endif
}

int strcasecmp_hal(const char *s1, const char *s2) {
	#ifdef ARDUINO
		return strcasecmp_P(s1, s2);
	#else
		return strcasecmp(s1, s2);
	#endif
}

void *memcpy_hal(void *dest, const void *src, size_t sz) {
	#ifdef ARDUINO
		return memcpy_P(dest, src, sz);
	#else
		return memcpy(dest, src, sz);
	#endif
}

#ifdef ARDUINO
// These are for the form strxxx_hal(char* dest, F("String in PROGMEM"))
//  and overload when that form is used.
// When compiling for non-PROGMEM platforms (linux) an F macro is defined
//	in build_config.h F(x) = (x) so that code can be the same.

char *strcpy_hal(char *dest, const __FlashStringHelper *src) {
	return strcpy_P(dest, (const char *)src);
}

char *strcat_hal(char *dest, const __FlashStringHelper *src) {
	return strcat_P(dest, (const char *)src);
}

int strcmp_hal(const char *s1, const __FlashStringHelper *s2) {
	return strcmp_P(s1, (const char *)s2);
}

int strcasecmp_hal(const char *s1, const __FlashStringHelper *s2) {
	return strcasecmp_P(s1, (const char *)s2);
}

#endif

