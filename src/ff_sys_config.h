/************************************************
 ff_sys_config.h
 FF System Core (Stand-alone Embedded)

 (c) Brendan McLearie - bmclearie@mclearie.com

 Preprocessor and compile time directives
 that configure the core system.

 See also: "ff_string_consts.h" for extended
 configuration enums, labels, etc

 ************************************************/
#ifndef FF_SYS_CONFIG_H_
#define FF_SYS_CONFIG_H_

#define VERSION v0.31dev

/************************************************
 PLATFORM Directives
 ************************************************/
#define FF_ARDUINO
//#define FF_TEMPERATURE_SIM  //optionally
//OR
//#define FF_SIMULATOR
//#define FF_SIM_PARSECONFIG	//optional parse the TXT config (default -> read BIN)
//OR
//#define FF_CONFIG


#ifdef FF_CONFIG
#define FF_SIMULATOR
#define DEBUG
#undef FF_ARDUINO
#endif

#ifdef FF_SIMULATOR
#define FF_TEMPERATURE_SIM
#endif

#ifdef FF_TEMPERATURE_SIM
//#define FF_DEFINED_TEMP_SIM
// OR
#define FF_RANDOM_TEMP_SIM
#endif


#ifdef FF_DEFINED_TEMP_SIM
#define SIM_TEMP_0 18
#define SIM_TEMP_1 21
#define SIM_TEMP_2 26
#define SIM_TEMP_3 29
#endif

#ifdef FF_ARDUINO
#define USE_PROGMEM
#endif

#define EXCLUDE_DESCRIPTION			// to save .data segment and heap space on arduino
#define EXCLUDE_DISPLAYNAME			// to save .data segment and heap space on arduino

/************************************************
 Localisation - TODO Move to Config
 ************************************************/
#define TIME_ZONE 0
#define DAY_LIGHT_SAVING 0

/************************************************
 DEBUG Directives
 ************************************************/
#define DEBUG			// Call debug output at all or not?

#ifdef DEBUG

#define DEBUG_LEVEL 1	// level and higher will be sent to debug log
/*
1 = E_DEBUG_MSG
2 = E_VERBOSE
3 = E_INFO
4 = E_DATA
5 = E_ACT
6 = E_DEACT
7 = E_COMMAND
8 = E_WARNING
9 = E_ERROR
10 = E_STOP
*/

#ifdef FF_ARDUINO
#define DEBUG_SERIAL                  	// For use when USB connected
#define DEBUG_SERIAL_BAUDRATE 9600
//#define DEBUG_LCD                     	// Debug to a connected LCD screen
#define DEBUG_LCD_DELAY 0
#endif

//#define DEBUG_FILE						//write a debug file on SD or locally
#define DEBUG_FILENAME "FFDEBUG.TXT"

#ifdef FF_SIMULATOR
#define DEBUG_CONSOLE					//write to stdout
#define FF_SIMULATOR_DATA_SCREEN		//simulate the LCD data screen to stdout
#define SCREEN_REFRESH 50000		//how many times through the loop should the data screen not be updated
#endif

#ifdef FF_ARDUINO
//#define DEBUG_MEMORY					//print heap and stack pointers in the debug process
//#define DEBUG_DALLAS				//Check and dump details on Dallas temperature sensors during start up
#endif

#endif //DEBUG

//#define SET_RTC YES						//WARNING - This hard codes the time on restart to the compile time
										//as the set time on device restart - use once to set then exclude

/************************************************
 System, Hardware, PINS and Initialisation
 ************************************************/
#define DIG_HIGH 0x1
#define DIG_LOW 0x0

#define ONE_WIRE_BUS_1 6                			//for Dallas temp signals
#define ONE_WIRE_BUS_2 7                			//for Dallas temp signals

#define EVENT_SERIAL							//send event messages over a serial link

#define EVENT_SERIAL_PORT 0
#define EVENT_SERIAL_BAUDRATE 9600

#ifdef FF_SIMULATOR
#define EVENT_CONSOLE
#ifdef FF_CONFIG
#undef EVENT_CONSOLE
#endif //FF_CONFIG
#endif //FF_SIMULATOR

#define ONE_SHOT_DURATION 2						//SCH_ONE_SHOT stay active seconds - inc case loop delay >= 1 second
#define RESET_MINMAX_SCH_BLOCK "SCH_RESET_MIN_MAX_COUNTERS"  //workaround - the block that triggers resetting of min and max display counters

// Data sources for display on 128x64 LCD screen
#define DISPLAY_INSIDE_SOURCE_BLOCK "IN_INSIDE_TOP_TEMP"
#define DISPLAY_OUTSIDE_SOURCE_BLOCK "IN_OUTSIDE_TEMP"
#define DISPLAY_WATER_SOURCE_BLOCK "IN_WATER_TEMP"

#define EVENT_BUFFER_SIZE 5

#define UINT8_INIT 0xFF
#define UINT16_INIT 0xFFFF
#define UINT32_INIT 0xFFFFFFFF
#define INT16_INIT 0xFFFF
#define FLOAT_INIT 255.000000

//Reserved block IDs
#define BLOCK_ID_BASE 1000
#define SSS 500   				//System Reserved Source Block ID for Messages
#define BLOCK_ID_BCAST 555		//reserved destination id representing all blocks ie broadcast
#define BLOCK_ID_NA 600			//for calls that need to specify a destination ID when it is not applicable

//Time Value type for rates and periods (in seconds)
// uint8_t = 255 seconds = 4.25 minutes
// unit16_t = 65535 seconds = 1092.25 minutes = 18.21 hours
// uint32_t = About 137 years....
#define TV_TYPE	uint32_t

// Maximum length for filename, excluding NULL char 26 chars allows an
// 8.3 filename instead and 8.3 directory with a leading slash
#define INI_FILE_MAX_FILENAME_LEN 26

#define EVENT_FILENAME "FFEVENTS.TXT"
#define CONFIG_FILENAME "FFCONFIG.TXT"
#define BIN_CONFIG_FILENAME "FFCONFIG.BIN"

#define INI_FILE_MAX_LINE_LENGTH 150
#define INI_ERROR_TYPES 9

#define MAX_MESSAGE_STRING_LENGTH 80
#define MAX_LOG_LINE_LENGTH 150
#define MAX_DEBUG_LENGTH 150			//line buffer length for debug messages "stack smashing if too short"
#define MAX_LABEL_LENGTH 48
#define MAX_DESCR_LENGTH 80
#define MAX_BLOCKS 255
#define MAX_BLOCKS_PER_CATEGORY 32
#define MAX_CONF_KEYS_PER_BLOCK 12



#endif /* FF_SYS_CONFIG_H_ */
