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
#ifdef USE_PROGMEM
	//StringArray temp;

	//memcpy_P(&temp, &message_type_strings[message_type_enum], sizeof(temp));
	//strcpy(str_buf, temp.text[GetLanguage()]);
	strcpy_hal(str_buf, message_type_strings[message_type_enum].text[GetLanguage()]);
#else
	//memcpy(&temp, &message_type_strings[message_type_enum], sizeof(temp));
	strcpy_hal(str_buf, message_type_strings[message_type_enum].text[GetLanguage()]);
#endif

	//return str_buf;
}

void GetMessageString(char *str_buf, int message_enum) {

	#ifdef PLATFORM_ARDUINO
	StringArray temp;

	#ifdef ARDUINO_HIGH_PROGMEM
	uint32_t far_base_ptr;
	uint32_t far_offset;
/*
	byte b;
	far_base_ptr = pgm_get_far_address(PAD16K1);
	Serial.print("Far address of PAD16K1 (DEC/HEX): ");
	Serial.print(far_base_ptr, DEC);
	Serial.print(" : ");
	Serial.println(far_base_ptr, HEX);
	b = pgm_read_byte_far(far_base_ptr);
	Serial.print("Byte at that address: ");
	Serial.println(b, HEX);

	far_base_ptr = pgm_get_far_address(PAD16K2);
	Serial.print("Far address of PAD16K2 (DEC/HEX): ");
	Serial.print(far_base_ptr, DEC);
	Serial.print(" : ");
	Serial.println(far_base_ptr, HEX);
	b = pgm_read_byte_far(far_base_ptr);
	Serial.print("Byte at that address: ");
	Serial.println(b, HEX);
*/
	far_base_ptr = pgm_get_far_address(message_strings);

	//Serial.print("Far address of message_strings: ");
	//Serial.print(far_base_ptr, DEC);
	//Serial.print(" : ");
	//Serial.println(far_base_ptr, HEX);

	far_offset = (uint32_t)(sizeof(StringArray) * message_enum);
	far_base_ptr = far_base_ptr + far_offset;
	//Serial.print("Far address of message_strings[offset]: ");
	//Serial.print(far_base_ptr, DEC);
	//Serial.print(" : ");
	//Serial.println(far_base_ptr, HEX);

	memcpy_PF(&temp, far_base_ptr, sizeof(temp));
	//Serial.print("String at that address: ");
	//Serial.println(temp.text[GetLanguage()]);
	#else //!ARDUINO_HIGH_PROGMEM
		memcpy_P(&temp, &message_strings[message_enum], sizeof(temp));
	#endif //ARDUINO_HIGH_PROGMEM

	strcpy(str_buf, temp.text[GetLanguage()]);

	#endif //PLATFORM_ARDUINO

	#ifdef PLATFORM_LINUX
		//memcpy(&temp, &message_strings[message_enum], sizeof(temp));
		strcpy_hal(str_buf, message_strings[message_enum].text[GetLanguage()]);
	#endif //PLATFORM_LINUX
}

uint8_t DayStringArrayIndex(const char* key) {
	// TODO - revisit for error catching (0 == SUN rather than DAY_ERROR)
	for (int i = 0; i < LAST_DAY; i++) {
		if (strcasecmp_hal(key, day_strings[i].text) == 0) {
			return i;
		}
	}
	return UINT8_INIT;
}


uint8_t UnitStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_UNIT; i++) {
		if (strcasecmp_hal(key, unit_strings[i].text) == 0) {
			return i;
		}
	}
	return UNIT_ERROR;
}

uint8_t CommandStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_COMMAND; i++) {
		if (strcasecmp_hal(key, command_strings[i].text) == 0) {
			return i;
		}
	}
	return CMD_ERROR;
}

uint8_t LanguageStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_LANGUAGE; i++) {
		if (strcasecmp_hal(key, language_strings[i].text) == 0) {
			return i;
		}
	}
	return 0; // DEFAULT ENGLISH == 0
}

uint8_t InterfaceTypeStringArrayIndex(const char* key) {
	for (uint8_t i = 0; i < LAST_INTERFACE; i++) {
		if (strcasecmp_hal(key, interface_type_strings[i].text) == 0) {
			return i;
		}
	}
	return IF_ERROR; //ERROR
}

uint8_t BlockTypeStringArrayIndex(const char* key) {
	for (int i = 0; i < LAST_BLOCK_TYPE; i++) {
		if (strcasecmp_hal(key, block_type_strings[i].text) == 0) {
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

size_t strlen_hal(const char *s) {
	#ifdef PLATFORM_ARDUINO
		return strlen_P(s);
	#else
		return strlen(s);
	#endif
}

char *strcpy_hal(char *dest, const char *src) {
	#ifdef PLATFORM_ARDUINO
		return strcpy_P(dest, src);
	#else
		return strcpy(dest, src);
	#endif
}

char *strcat_hal(char *dest, const char *src) {
	#ifdef PLATFORM_ARDUINO
		return strcat_P(dest, src);
	#else
		return strcat(dest, src);
#endif
}

int strcmp_hal(const char *s1, const char *s2) {
	#ifdef PLATFORM_ARDUINO
		return strcmp_P(s1, s2);
	#else
		return strcmp(s1, s2);
	#endif
}

int strcasecmp_hal(const char *s1, const char *s2) {
	#ifdef PLATFORM_ARDUINO
		return strcasecmp_P(s1, s2);
	#else
		return strcasecmp(s1, s2);
	#endif
}

void *memcpy_hal(void *dest, const void *src, size_t sz) {
	#ifdef PLATFORM_ARDUINO
		return memcpy_P(dest, src, sz);
	#else
		return memcpy(dest, src, sz);
	#endif
}

#ifdef PLATFORM_ARDUINO
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

#endif //PLATFORM_ARDUINO

