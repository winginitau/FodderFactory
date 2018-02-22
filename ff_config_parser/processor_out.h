
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

//TODO: Command Line Options Processing
//TODO: AST Validation Walk - 
//TODO: AST Flag end points
//TODO: AST Order Ambiguity Report
//TODO: AST Determine Partial Keyword Uniqueness
//TODO: AST Warn unused IDs, lookups, params
//TODO: Parser/AST output Action prototypes
//TODO: Configuration Grammar with %section directive
//TODO: Context change on <identifier> value

char** ShowBlocksInCategory(int block_category);

char** ShowBlockByLabel(char* block_label);

char** ShowBlockByID(int param0_int);

char** MonitorRemoveByLabel(char* block_label);

char** MonitorRemoveByBlockCatN(int block_category, int param0_int);

char** MonitorByLabel(char* block_label);

char** MonitorByBlockCatN(int block_category, int param0_int);

char** ClearBlockByLabel(char* block_label);

char** ClearBlockByBlockCatN(int block_category, int param0_int);

char** SendCommandToBlockCatN(int block_category, int param0_int, int command);

char** SendCommandToBlockLabel(char* block_label, int command);

char** SimIntDataMessageFromBCatN(int block_category, int param1_int, int param0_int);

char** SimFloatDataMessageFromBCatN(int block_category, int param1_int, float param0_float);

char** SimIntDataMessageFromBlockLabel(char* block_label, int param0_int);

char** SimFloatDataMessageFromBlockLabel(char* block_label, float param0_float);

char** DebugSetLevel(int param0_int);

char** SetTime(char* param0_time);

char** SetDate(char* param0_date);

char** ConfReadFilename(char* param0_string);

char** WriteToFilenameConfigFile(char* param0_string);

char** CopyConfigFileToFile(char* param1_string, char* param0_string);

char** BlockDisableByBlockCatN(int block_category, int param0_int);

char** BlockDisableByLabel(char* param0_string);

char** BlockEnableByBlockCatN(int block_category, int param0_int);

char** BlockEnableByLabel(char* param0_string);

char** BlockDeleteByBlockCatN(int block_category, int param0_int);

char** BlockDeleteByLabel(char* param0_string);

char** BlockRename(char* param1_string, char* param0_string);

#ifdef USE_PROGMEM
static const ASTA asta [87] PROGMEM = {
#else
static const ASTA asta [87] = {
#endif
	1, 1, "SHOW", 0, 0, 2, 7, "",
	2, 1, "BLOCKS", 1, 1, 0, 3, "SHOW_BLOCKS",
	3, 1, "SYSTEM", 1, 1, 0, 4, "SHOW_SYSTEM",
	4, 2, "block_cat_defs", 1, 1, 0, 5, "SHOW_BLOCK_CATEGORY",
	5, 3, "block_label", 1, 1, 0, 6, "SHOW_BLOCK_LABEL",
	6, 5, "", 1, 1, 0, 0, "SHOW_BLOCK_ID",
	7, 1, "MUTE", 0, 0, 8, 11, "",
	8, 1, "MESSAGES", 0, 7, 9, 0, "",
	9, 1, "ON", 1, 8, 0, 10, "MUTE_MESSAGES_ON",
	10, 1, "OFF", 1, 8, 0, 0, "MUTE_MESSAGES_OFF",
	11, 1, "MONITOR", 0, 0, 12, 21, "",
	12, 1, "START", 1, 11, 0, 13, "MONIOTR_START",
	13, 1, "STOP", 0, 11, 14, 18, "",
	14, 1, "ALL", 1, 13, 0, 15, "MONITOR_STOP_ALL",
	15, 3, "block_label", 1, 13, 0, 16, "MONITOR_STOP_BLOCK_LABEL",
	16, 2, "block_cat_defs", 0, 13, 17, 0, "",
	17, 5, "", 1, 16, 0, 0, "MONITOR_STOP_BLOCK_CAT_N",
	18, 3, "block_label", 1, 11, 0, 19, "MONITOR_BLOCK_LABEL",
	19, 2, "block_cat_defs", 0, 11, 20, 0, "",
	20, 5, "", 1, 19, 0, 0, "MONITOR_BLOCK_CAT_N",
	21, 1, "CLEAR", 0, 0, 22, 25, "",
	22, 3, "block_label", 1, 21, 0, 23, "CLEAR_BLOCK_LABEL",
	23, 2, "block_cat_defs", 0, 21, 24, 0, "",
	24, 5, "", 1, 23, 0, 0, "CLEAR_BLOCK_CAT_N",
	25, 1, "MESSAGE", 0, 0, 26, 44, "",
	26, 1, "COMMAND", 0, 25, 27, 32, "",
	27, 2, "block_cat_defs", 0, 26, 28, 30, "",
	28, 5, "", 0, 27, 29, 0, "",
	29, 2, "command_strings", 1, 28, 0, 0, "COMMAND_BLOCK_CAT_N",
	30, 3, "block_label", 0, 26, 31, 0, "",
	31, 2, "command_strings", 1, 30, 0, 0, "COMMAND_BLOCK_LABEL",
	32, 1, "DATA", 0, 25, 33, 0, "",
	33, 2, "block_cat_defs", 0, 32, 34, 39, "",
	34, 5, "", 0, 33, 35, 0, "",
	35, 1, "INT", 0, 34, 36, 37, "",
	36, 5, "", 1, 35, 0, 0, "MESSAGE_DATA_BCAT_N_INT",
	37, 1, "FLOAT", 0, 34, 38, 0, "",
	38, 6, "", 1, 37, 0, 0, "MESSAGE_DATA_BCAT_N_FLOAT",
	39, 3, "block_label", 0, 32, 40, 0, "",
	40, 1, "INT", 0, 39, 41, 42, "",
	41, 5, "", 1, 40, 0, 0, "MESSAGE_DATA_LABEL_INT",
	42, 1, "FLOAT", 0, 39, 43, 0, "",
	43, 6, "", 1, 42, 0, 0, "MESSAGE_DATA_LABEL_FLOAT",
	44, 1, "DEBUG", 0, 0, 45, 52, "",
	45, 1, "ON", 1, 44, 0, 46, "DEBUG_ON",
	46, 1, "OFF", 1, 44, 0, 47, "DEBUG_OFF",
	47, 1, "TERMINAL", 0, 44, 48, 50, "",
	48, 1, "ON", 1, 47, 0, 49, "DEBUG_TERMINAL_ON",
	49, 1, "OFF", 1, 47, 0, 0, "DEBUG_TERMINAL_OFF",
	50, 1, "LEVEL", 0, 44, 51, 0, "",
	51, 5, "", 1, 50, 0, 0, "DEBUG_LEVEL",
	52, 1, "CLOCK", 1, 0, 53, 55, "SHOW_TIME",
	53, 1, "SET", 0, 52, 54, 0, "",
	54, 7, "", 1, 53, 0, 0, "SET_TIME",
	55, 1, "DATE", 1, 0, 56, 58, "SHOW_DATE",
	56, 1, "SET", 0, 55, 57, 0, "",
	57, 8, "", 1, 56, 0, 0, "SET_DATE",
	58, 1, "CONFIGURE", 0, 0, 59, 64, "",
	59, 1, "CLEAR", 0, 58, 60, 62, "",
	60, 1, "BLOCKS", 1, 59, 0, 61, "CONF_CLEAR_BLOCKS",
	61, 1, "ALL", 1, 59, 0, 0, "CONF_CLEAR_ALL",
	62, 1, "FILE", 1, 58, 63, 0, "CONF_FILE_DEFAULT",
	63, 4, "", 1, 62, 0, 0, "CONF_FILE_FILENAME",
	64, 1, "WRITE", 1, 0, 65, 67, "WRITE_DEFAULT",
	65, 1, "INIT", 1, 64, 0, 66, "WRITE_INIT",
	66, 4, "", 1, 64, 0, 0, "WRITE_FILENAME",
	67, 1, "COPY", 0, 0, 68, 70, "",
	68, 4, "", 0, 67, 69, 0, "",
	69, 4, "", 1, 68, 0, 0, "COPY_FILE_FILE",
	70, 1, "DISABLE", 0, 0, 71, 74, "",
	71, 2, "block_cat_defs", 0, 70, 72, 73, "",
	72, 5, "", 1, 71, 0, 0, "DISABLE_BLOCK_CAT_N",
	73, 4, "", 1, 70, 0, 0, "DISABLE_BLOCK_LABEL",
	74, 1, "ENABLE", 0, 0, 75, 78, "",
	75, 2, "block_cat_defs", 0, 74, 76, 77, "",
	76, 5, "", 1, 75, 0, 0, "ENABLE_BLOCK_CAT_N",
	77, 4, "", 1, 74, 0, 0, "ENABLE_BLOCK_LABEL",
	78, 1, "DELETE", 0, 0, 79, 82, "",
	79, 2, "block_cat_defs", 0, 78, 80, 81, "",
	80, 5, "", 1, 79, 0, 0, "DELETE_BLOCK_CAT_N",
	81, 4, "", 1, 78, 0, 0, "DELETE_BLOCK_LABEL",
	82, 1, "RENAME", 0, 0, 83, 85, "",
	83, 4, "", 0, 82, 84, 0, "",
	84, 4, "", 1, 83, 0, 0, "RENAME_BLOCK",
	85, 1, "EXIT", 1, 0, 0, 86, "EXIT",
	86, 1, "LOGOUT", 1, 0, 0, 87, "LOGOUT",
	87, 1, "REBOOT", 1, 0, 0, 0, "REBOOT",
};

#endif /* PROCESSOR_OUT_H_ */

