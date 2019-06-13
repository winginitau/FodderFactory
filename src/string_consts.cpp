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

/*
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
*/

uint8_t DayStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_DAY; i++) {
		if (strcmp_hal(key, day_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}


uint8_t UnitStringArrayIndex(const char* key) {
	for (int i = LAST_UNIT -1; i > 0; i--) {
		if (strcmp_hal(key, unit_strings[i].text) == 0) {
			return i;
		}
	}
	return UNIT_ERROR;
}

uint8_t CommandStringArrayIndex(const char* key) {
	for (int i = LAST_COMMAND -1; i > 0; i--) {
		if (strcmp_hal(key, command_strings[i].text) == 0) {
			return i;
		}
	}
	return CMD_ERROR;
}


/*
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
*/

uint8_t LanguageStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_LANGUAGE; i++) {
		if (strcmp_hal(key, language_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}


/*
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
*/

uint8_t InterfaceStringArrayIndex(const char* key) {
	//
	for (int i = 0; i < LAST_INTERFACE; i++) {
		if (strcmp_hal(key, interface_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}


/*
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
*/

uint8_t BlockTypeStringArrayIndex(const char* key) {
	/*
	char temp[MAX_MESSAGE_STRING_LENGTH];
	char temp2[MAX_MESSAGE_STRING_LENGTH];
	char chhex[MAX_LABEL_LENGTH];
	int len;
	DebugLog("Getting BlockTypeStringArrayIndex(char* key). Key is:");
	DebugLog(key);
	len = strlen(key);
	strcpy(temp2, "");
	for (uint8_t i = 0; i < len; i++) {
		sprintf(chhex, " %02x", key[i]);
		strcat(temp2, chhex);
	}
	sprintf(temp, "Len: %d    Hex is: %s", len, temp2);
	DebugLog(temp);
	DebugLog("Iterating block_type_strings in reverse for match");
	*/
	for (int ii = LAST_BLOCK_TYPE - 1; ii > 0; ii--) {
		/*
		strcpy_hal(temp, block_type_strings[ii].text);
		DebugLog(temp);
		len = strlen(temp);
		strcpy(temp2, "");
		for (uint8_t i = 0; i < len; i++) {
			sprintf(chhex, " %02x", temp[i]);
			strcat(temp2, chhex);
		}
		sprintf(temp, "Len: %d    Hex is: %s", len, temp2);
		DebugLog(temp);
		*/

		if (strcmp_hal(key, block_type_strings[ii].text) == 0) {
			//DebugLog("Matched Block Type!");
			// match
			return ii;
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

#endif

/**************************************************************************/

/*
char *strcat_hal(char *dest, const char *src) {
#ifdef ARDUINO
	return strcat_P(dest, src);
#else
	return strcat(dest, src);
#endif
}
*/


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
