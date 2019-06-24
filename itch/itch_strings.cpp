/*****************************************************************
 itch_strings.cpp

 Copyright (C) 2019 Brendan McLearie 

 Created on: 24 Jun 2019

******************************************************************/


#include <itch_strings.h>
#include <itch.h>

void ITCHWriteLineError(uint8_t error_enum) {
	#ifdef PLATFORM_ARDUINO
		ITCHWriteLine_P(itch_error_strings[error_enum].text);
	#endif
	#ifdef PLATFORM_LINUX
		ITCHWriteLine(itch_error_strings[error_enum].text);
	#endif
}

void ITCHWriteLineMisc(uint8_t misc_string_enum) {
	#ifdef PLATFORM_ARDUINO
		ITCHWriteLine_P(misc_itch_strings[misc_string_enum].text);
	#endif
	#ifdef PLATFORM_LINUX
		ITCHWriteLine(misc_itch_strings[misc_string_enum].text);
	#endif
}

void ITCHWriteMisc(uint8_t misc_string_enum) {
	#ifdef PLATFORM_ARDUINO
		ITCHWrite_P(misc_itch_strings[misc_string_enum].text);
	#endif
	#ifdef PLATFORM_LINUX
		ITCHWrite(misc_itch_strings[misc_string_enum].text);
	#endif
}
