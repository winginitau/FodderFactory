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
//#define FF_ARDUINO
//#define FF_TEMPERATURE_SIM  //optionally
//OR
#define FF_SIMULATOR
//OR
//#define FF_CONFIG

#ifdef FF_CONFIG
#define FF_SIMULATOR
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
 DEBUG Directives
 ************************************************/
#define DEBUG                         	// Call debug output at all or not?

#ifdef DEBUG

#ifdef FF_ARDUINO
#define DEBUG_SERIAL                  	// For use when USB connected
#define DEBUG_SERIAL_BAUDRATE 9600
//#define DEBUG_LCD                     	// Debug to a connected LCD screen
#define DEBUG_LCD_DELAY 3000
#endif

//#define DEBUG_FILE						//write a debug file on SD or locally
#define DEBUG_FILENAME "FFDEBUG.TXT"

#ifdef FF_SIMULATOR
#define DEBUG_CONSOLE					//write to stdout
#endif


#endif //DEBUG

//#define SET_RTC YES						//WARNING - This hard codes the time on restart to the compile time
										//as the set time on device restart - use once to set then exclude

/************************************************
 System, Hardware, PINS and Initialisation
 ************************************************/
#define DIG_HIGH 0x1
#define DIG_LOW 0x0

#define ONE_WIRE_BUS 6                	//for Dallas temp signals

#define EVENT_BUFFER_SIZE 5

#define UINT8_INIT 0xFF
#define UINT16_INIT 0xFFFF
#define UINT32_INIT 0xFFFFFFFF
#define FLOAT_INIT 255.000000
#define BLOCK_ID_BASE 1000
#define SSS 500   //System Reserved Source Block ID for Messages

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
#define MAX_LABEL_LENGTH 64
#define MAX_DESCR_LENGTH 80
#define MAX_BLOCKS 255
#define MAX_BLOCKS_PER_CATEGORY 32
#define MAX_CONF_KEYS_PER_BLOCK 12
/************************************************
 Inputs Definitions
 ************************************************/

/************************************************
 Controllers
 ************************************************/

/************************************************
 Outputs
 ************************************************/



#endif /* FF_SYS_CONFIG_H_ */
