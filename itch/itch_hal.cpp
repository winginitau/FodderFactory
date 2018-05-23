/*****************************************************************
 itch_hal.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 9 Apr. 2018

******************************************************************/

#include "config.h"
#include "itch_strings.h"
#include <stdio.h>


#ifdef ARDUINO
#include <Arduino.h>
#endif

char *strcpy_itch_misc(char *dest, uint8_t src) {
	#ifdef ARDUINO
		return strcpy_P(dest, misc_itch_strings[src].text);
	#else
		return strcpy(dest, misc_itch_strings[src].text);
	#endif
}

char *strcat_itch_misc(char *dest, uint8_t src) {
	#ifdef ARDUINO
		return strcat_P(dest, misc_itch_strings[src].text);
	#else
		return strcat(dest, misc_itch_strings[src].text);
	#endif
}

char *strcpy_itch_hal(char *dest, const char *src) {
	#ifdef ARDUINO
		return strcpy_P(dest, src);
	#else
		return strcpy(dest, src);
	#endif
}

char *strcat_itch_hal(char *dest, const char *src) {
#ifdef ARDUINO
	return strcat_P(dest, src);
#else
	return strcat(dest, src);
#endif
}

void *memcpy_itch_hal(void *dest, const void *src, size_t sz) {
#ifdef ARDUINO
	return memcpy_P(dest, src, sz);
#else
	return memcpy(dest, src, sz);
#endif
}

#ifdef ITCH_DEBUG
char *strcpy_itch_debug(char *dest, uint8_t src) {
	#ifdef ARDUINO
		return strcpy_P(dest, itch_debug_strings[src].text);
	#else
		return strcpy(dest, itch_debug_strings[src].text);
	#endif
}

char *strcat_itch_debug(char *dest, uint8_t src) {
	#ifdef ARDUINO
		return strcat_P(dest, itch_debug_strings[src].text);
	#else
		return strcat(dest, itch_debug_strings[src].text);
	#endif
}

#endif //ITCH_DEBUG



