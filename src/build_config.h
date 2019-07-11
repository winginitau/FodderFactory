/************************************************
 ff_sys_config.h
 FF System Core (Stand-alone Embedded)

 (c) Brendan McLearie - bmclearie@mclearie.com

 Preprocessor and compile time directives
 that configure the core system.

 See also: "ff_string_consts.h" for extended
 configuration enums, labels, etc

 ************************************************/
#ifndef BUILD_CONFIG_H_
#define BUILD_CONFIG_H_

/************************************************
 PLATFORM Directives
 ************************************************/
// These should be defined by the build system
// However the Sloeber build system doesn't allow
// -D defines for the C/C++ Pre-processor in the same
// way as eclipse CDT. So if not defined assume
// PLATFORM_ARDUINO

#if not defined PLATFORM_LINUX && not defined PLATFORM_ARDUINO
#define PLATFORM_ARDUINO
#endif // Not Defined Either

#ifdef PLATFORM_ARDUINO
#define BOARD_ARDUINO_MEGA2560
#endif

/****************************************************************************
 PLATFORM OPTIONALS - To be scripted for "build to order" firmware
 ***************************************************************************/
//#define UI_ATTACHED			// is there a directly attached UI? LCD? Console in sim?

#ifdef PLATFORM_ARDUINO
	#define ASSERT(x,y) ASSERT_ARDUINO(x,y)
	#define ARDUINO_VEDIRECT
	#define VE_DIRECT
	#define IF_ARDUINO_VEDIRECT
	#define ARDUINO_PROGMEM
	#define ARDUINO_HIGH_PROGMEM
	//#define ARDUINO_LCD
#endif //PLATFORM_ARDUINO


#ifdef PLATFORM_LINUX
	#define ASSERT(x,y) ASSERT_ARDUINO(x,y)
	#define FF_TEMPERATURE_SIM
	#define PSTR(x) x
	#define F(x) x
#endif

//#define RESURRECT_DEPRECIATED

//#define PLATFORM_LINUX
// Optionally with
//#define FF_SIM_PARSECONFIG	//optional parse the TXT config (default -> read BIN)

// OR
//#define FF_CONFIG

// Optionally:
//#define FF_TEMPERATURE_SIM
// OR with PLATFORM_ARDUINO
//#define FF_TEMP_SIM_WITH_DALLAS

//Use ITCH for CLI parsing ?
#define USE_ITCH
// OR only headers for enum arrary configs?
//#define ITCH_HEADERS_ONLY

#ifdef FF_CONFIG
#define PLATFORM_LINUX
#define DEBUG
#undef PLATFORM_ARDUINO
#endif


#ifdef FF_TEMPERATURE_SIM
//#define FF_DEFINED_TEMP_SIM
// OR
#define FF_RANDOM_TEMP_SIM

// Optionally:
//#define RANDOM_TEMP_SIM_DELAY
//#define TEMP_SIM_EXTRA_DELAY

#ifdef FF_RANDOM_TEMP_SIM
#define FF_RANDOM_TEMP_MIN 5.01
#define FF_RANDOM_TEMP_MAX 28.00
#endif

#ifdef RANDOM_TEMP_SIM_DELAY
#define TEMP_SIM_MIN_DELAY	600		//msec
#define TEMP_SIM_MAX_DELAY	1500	//msec
#endif
#endif

#ifdef FF_DEFINED_TEMP_SIM
#define SIM_TEMP_0 22.0
#define SIM_TEMP_1 22.1
#define SIM_TEMP_2 22.2
#define SIM_TEMP_3 22.3
#define SIM_TEMP_4 22.4
#define SIM_TEMP_5 22.5
#endif

#ifdef PLATFORM_ARDUINO
#define USE_PROGMEM
#define PROGMEM_BLOCK_DEFS
//#define OLD_SD	//OR
#define NEW_SD
#endif //PLATFORM_ARDUINO

#define EXCLUDE_DESCRIPTION			// to save .data segment and heap space on arduino
#define EXCLUDE_DISPLAYNAME			// to save .data segment and heap space on arduino

#define RTC_POLL_INTERVAL 30000		// milliseconds between calls to the rtc

#define VE_LOG_RATE 120				// how many seconds between VE.Direct energy system logging
#define VE_POLL_RATE 10				// how many seconds between VE.Direct energy system polling


/************************************************
 Localisation - TODO Move to Config
 ************************************************/
#define TIME_ZONE 0
#define DAY_LIGHT_SAVING 0

/************************************************
 DEBUG Directives
 ************************************************/
//#define SET_RTC YES				// WARNING - This hard codes the time on restart to the compile time
									// as the set time on device restart - use once to set then exclude
									// and immediately reflash with it disabled.

//#define DISABLE_OUTPUTS			// Allows full operation but the output relays don't activate

//#define DEBUG			// Call debug output at all or not?

#ifdef DEBUG
//Optionallly:
#define SUPRESS_EVENT_MESSAGES

//#define IN_LOGRATE_OVERRIDE 30				// seconds. Override defined logging rate (for debug)

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

#ifdef PLATFORM_ARDUINO
#define DEBUG_SERIAL                  		// For use when USB connected
#define DEBUG_SERIAL_BAUDRATE 9600
//#define DEBUG_LCD                     	// Debug to a connected LCD screen
//#define DEBUG_LCD_DELAY 0
#endif //PLATFORM_ARDUINO

#define DEBUG_FILE						//write a debug file on SD or locally
#define DEBUG_FILENAME "FFDEBUG.TXT"

#ifdef PLATFORM_LINUX
#define DEBUG_CONSOLE						//write to stdout
#define PLATFORM_LINUX_DATA_SCREEN			//simulate the LCD data screen to stdout
#define SCREEN_REFRESH 50000				//how many times through the loop should the data screen not be updated
#endif

#ifdef PLATFORM_ARDUINO
#define DEBUG_MEMORY					//print heap and stack pointers in the debug process
//#define DEBUG_DALLAS				//Check and dump details on Dallas temperature sensors during start up
#endif //PLATFORM_ARDUINO

#endif //DEBUG


/************************************************
 System, Hardware, PINS and Initialisation
 ************************************************/
#define DIG_HIGH 0x1
#define DIG_LOW 0x0

#define ONE_WIRE_BUS_1 5                			//for Dallas temp signals
#define ONE_WIRE_BUS_2 6                			//for Dallas temp signals
#define ONE_WIRE_BUS_3 7                			//for Dallas temp signals
#define OWB1_SENSOR_COUNT 2
#define OWB2_SENSOR_COUNT 2
#define OWB3_SENSOR_COUNT 1

#define EVENT_SERIAL							//send event messages over a serial link
#define EVENT_SERIAL_PORT 0
#define EVENT_SERIAL_BAUDRATE 9600

#define SERIAL_0_USED
//#define SERIAL_1_USED
//#define SERIAL_2_USED
//#define SERIAL_3_USED


#ifdef PLATFORM_LINUX
#define EVENT_CONSOLE
#ifdef FF_CONFIG
#undef EVENT_CONSOLE
#endif //FF_CONFIG
#endif //PLATFORM_LINUX

#define ONE_SHOT_DURATION 2						//SCH_ONE_SHOT stay active seconds - inc case loop delay >= 1 second
#define RESET_MINMAX_SCH_BLOCK "SCH_RESET_MIN_MAX_COUNTERS"  //workaround - the block that triggers resetting of min and max display counters

#ifdef ARDUINO_LCD
// Data sources for display on 128x64 LCD screen
#define DISPLAY_INSIDE_SOURCE_BLOCK "IN_INSIDE_TOP_TEMP"
#define DISPLAY_OUTSIDE_SOURCE_BLOCK "IN_OUTSIDE_TEMP"
#define DISPLAY_WATER_SOURCE_BLOCK "IN_WATER_TEMP"
#endif //ARDUINO_LCD

#define EVENT_BUFFER_SIZE 1

// Initialisers
#define UINT8_INIT 0xFF
#define UINT16_INIT 0xFFFF
#define UINT32_INIT 0xFFFFFFFF
#define INT16_INIT (int16_t)0xFFFF
#define INT32_INIT (int32_t)0xFFFFFFFF
#define FLOAT_INIT (float)255.000000
#define BLOCK_ID_INIT UINT16_INIT
#define CMD_INIT UINT8_INIT

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
#define TV_TYPE_INIT 0xFFFFFFFF

// Maximum length for filename, excluding NULL char 26 chars allows an
// 8.3 filename instead and 8.3 directory with a leading slash
#define INI_FILE_MAX_FILENAME_LEN 26

#define EVENT_FILENAME "FFEVENTS.TXT"
#define CONFIG_INI_FILENAME "FFCONFIG.TXT"
#define CONFIG_BIN_FILENAME "FFCONFIG.BIN"
#define CONFIG_TXT_FILENAME "CONFIG.TXT"

#define INI_FILE_MAX_LINE_LENGTH 150
#define INI_ERROR_TYPES 9

#define MAX_MESSAGE_STRING_LENGTH 80  //80
#define MAX_LOG_LINE_LENGTH 150 //150
#define MAX_DEBUG_LENGTH 150	//150		//line buffer length for debug messages "stack smashing if too short"
#define MAX_LABEL_LENGTH 48
#define MAX_DISPLAY_NAME_LENGTH 80
#define MAX_DESCR_LENGTH 80
//#define MAX_BLOCKS 255
#define MAX_BLOCKS_PER_CATEGORY 32
#define MAX_CONF_KEYS_PER_BLOCK 12
#define TOKEN_DELIM " "


#endif /* BUILD_CONFIG_H_ */
