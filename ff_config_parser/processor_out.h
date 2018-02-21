
#ifndef PROCESSOR_OUT_H_
#define PROCESSOR_OUT_H_

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

#define VERSION v0.4b

/************************************************
 PLATFORM Directives
 ************************************************/
//#define FF_ARDUINO
//#define FF_TEMPERATURE_SIM  //optionally
//OR
#define FF_SIMULATOR
#define FF_SIM_PARSECONFIG	//optional parse the TXT config (default -> read BIN)
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

//#define IN_LOGRATE_OVERRIDE 30				// seconds. Override defined logging rate (for debug)

#ifdef FF_ARDUINO
#define DEBUG_SERIAL                  		// For use when USB connected
#define DEBUG_SERIAL_BAUDRATE 9600
//#define DEBUG_LCD                     	// Debug to a connected LCD screen
#define DEBUG_LCD_DELAY 0
#endif

//#define DEBUG_FILE						//write a debug file on SD or locally
#define DEBUG_FILENAME "FFDEBUG.TXT"

#ifdef FF_SIMULATOR
#define DEBUG_CONSOLE						//write to stdout
#define FF_SIMULATOR_DATA_SCREEN			//simulate the LCD data screen to stdout
#define SCREEN_REFRESH 50000				//how many times through the loop should the data screen not be updated
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
#define OWB1_SENSOR_COUNT 3
#define OWB2_SENSOR_COUNT 2

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

//#define LCD_DISPLAY
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

#include <stdint.h>

/**************************************************************
 * Languages Enum - Must Precede typedefs that use it
 **************************************************************/
// We don't have an "error language" to start the enum here
// as it would bloat every multi language StringArrayType
// with an extra useless dimension.

typedef enum LANG_TYPE {
    ENGLISH = 0, GERMAN, LAST_LANGUAGE
} LanguageType;

/**************************************************************
 * String array typedefs
 **************************************************************/
#ifdef USE_PROGMEM
typedef struct STRING_ARRAY_TYPE {
    // Longest length of this string in all languages
    int max_len;
    // Array of pointers to each language-specific string of type
    char text[LAST_LANGUAGE][MAX_MESSAGE_STRING_LENGTH];
} StringArray;

#else
typedef struct STRING_ARRAY_TYPE {
    // Longest length of this string in all languages
    const int max_len;
    // Array of pointers to each language-specific string of type
    const char* const text[LAST_LANGUAGE];
} StringArray;
#endif

#ifdef USE_PROGMEM
typedef struct SIMPLE_STRING_ARRAY_TYPE {
    // Array of strings (usually labels and messages) relating to enums
    // Inside this struct, extended intitaliser lists are ok
    char text[MAX_MESSAGE_STRING_LENGTH];
} SimpleStringArray;
#else
typedef struct SIMPLE_STRING_ARRAY_TYPE {
    // Array of strings (usually labels and messages) relating to enums
    // Inside this struct, extended intitaliser lists are ok
    const char* text;
} SimpleStringArray;
#endif

typedef struct BLOCK_CATS {
    const uint8_t cat_id;
    const char conf_section_label[MAX_LABEL_LENGTH];
    const char conf_section_key_base[MAX_LABEL_LENGTH];
    const char* const conf_keys[MAX_CONF_KEYS_PER_BLOCK];
} BlockCatArray;

typedef struct AST_ARRAY {
    uint16_t id;
    int type;
    char label[MAX_LABEL_LENGTH];
//    char unique[MAX_LABEL_LENGTH];
//    char help[MAX_DESCR_LENGTH];
//    int term_level;
    bool action;
//    bool finish;
    uint16_t parent;
    uint16_t first_child;
    uint16_t next_sibling;
    char action_identifier[MAX_LABEL_LENGTH];
} ASTA;

/**************************************************************
 * Language Strings - Must Follow the typedef that defines it
 **************************************************************/
#ifdef USE_PROGMEM
static const SimpleStringArray language_strings[LAST_LANGUAGE] PROGMEM = {
#else
static const SimpleStringArray language_strings[LAST_LANGUAGE] = {
#endif
    "English",
    "Deutsch",
};

enum {
	FF_ERROR_CAT = 0,
	FF_GENERIC_BLOCK,
	FF_SYSTEM,
	FF_INPUT,
	FF_MONITOR,
	FF_SCHEDULE,
	FF_RULE,
	FF_CONTROLLER,
	FF_OUTPUT,
	LAST_BLOCK_CAT,
};

#ifdef USE_PROGMEM
static const SimpleStringArray block_cat_defs [LAST_BLOCK_CAT] PROGMEM = {
#else
static const SimpleStringArray block_cat_defs [LAST_BLOCK_CAT] = {
#endif
	"ERROR_CAT",
	"GENERIC",
	"SYSTEM",
	"INPUT",
	"MONITOR",
	"SCHEDULE",
	"RULE",
	"CONROLLER",
	"OUTPUT",
};

enum {
	CMD_ERROR = 0,
	CMD_OUTPUT_OFF,
	CMD_OUTPUT_ON,
	CMD_RESET_MIN_MAX,
	LAST_COMMAND,
};

#ifdef USE_PROGMEM
static const SimpleStringArray command_strings [LAST_COMMAND] PROGMEM = {
#else
static const SimpleStringArray command_strings [LAST_COMMAND] = {
#endif
	"CMD_ERROR",
	"CMD_OUTPUT_OFF",
	"CMD_OUTPUT_ON",
	"CMD_RESET_MIN_MAX",
};

>> Error on Line: 395
>> Duplication of keyword at this term level detected
>> %3 keyword ON
