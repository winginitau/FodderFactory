/************************************************
 ff_string_consts.h

 FF System Core (Stand-alone Embedded)

 (c) Brendan McLearie - bmclearie@mclearie.com

 Explanatory notes:
 - General approach is to concentrate all strings
   here as static consts so they will be pre initialised
   on embedded systems with Von Neuemann architctures.
 - For Harvard architectures (AtMel AVR / Arduino), the
   PROGMEM modifier is used (if #USE_PROGMEM is defined)
   to load into the .text / program section of flash memory .
 - All strings and lists are accessed via associated
   enums thereby leaving the run-time with memory efficient and
   CPU efficient types for performance, and referring to
   the string data only as needed for use.
 - String arrays in PROGMEM space have overloaded getter
   and index functions to address and return them from PROGMEM.
   If #USE_PROGMEM is not defined these functions simply
   return the string / index.
 - enums are defined with:
    - Zero (0) element being a reserved error to
      catch via debug and error reporting any iterations
      or evaluations that return 0 unintentionally.
    - The last element is always defined as LAST_<type>
      as a fall through catch in loops and iterations
      and allowing nicely written loops eg:
      while (iterator < LAST_TYPE) {iterator++;};
- 2019-06-15 all 1D SimpleStringArray's have been moved
  to the glitch grammar definition file to provide a single
  source for configuration, maintenance and feature additions

 ************************************************/
#ifndef SRC_FF_STRINGS_H_
#define SRC_FF_STRINGS_H_

#ifndef ARDUINO
#include <stddef.h>
#endif

#include <build_config.h>
//#include "ff_utils.h"
#include <global_defs.h>
#include <stdint.h>

#ifdef USE_ITCH
#include <itch.h>
#include <out.h>
#endif

#ifdef ITCH_HEADERS_ONLY
#include <itch.h>
#include <out.h>
#endif

/**************************************************************
 * Platform Directives
 **************************************************************/
#ifdef USE_PROGMEM
#include <Arduino.h>
#endif

/**************************************************************
 * Languages Enum - Must Precede typedefs that use it
 **************************************************************/
// An "error language", enum 0 not used here
// It would bloat every multi language StringArrayType
// with an extra useless dimension.

typedef enum LANG_TYPE {
	ENGLISH = 0,
	GERMAN,
	LAST_LANGUAGE
} LanguageType;

/**************************************************************
 * Multi Language String array typedef
 **************************************************************/
typedef struct STRING_ARRAY_TYPE {
	// Longest length of this string in all languages
	// Not used - hang over from earlier iteration
	// TODO - fix
	uint8_t max_len;
	// Array of strings to each language-specific string of type
	char text[LAST_LANGUAGE][MAX_MESSAGE_STRING_LENGTH];
} StringArray;

/**************************************************************
 * Language Strings - Must Follow the typedef that defines it
 **************************************************************/
#ifdef USE_PROGMEM
const SimpleStringArray language_strings[LAST_LANGUAGE] PROGMEM = {
#else
const SimpleStringArray language_strings[LAST_LANGUAGE] = {
#endif
	"English",
	"Deutsch",
};

/**************************************************************
 * Message Types
 **************************************************************/
typedef enum {
	E_ZERO_ERROR = 0,
	E_DEBUG_MSG,
	E_VERBOSE,
	E_INFO,
	E_DATA,
	E_ACT,
	E_DEACT,
	E_COMMAND,
	E_ADMIN,
	E_WARNING,
	E_ERROR,
	E_STOP,
	LAST_MESSAGE_TYPE
} MessageTypeEnum;

#ifdef USE_PROGMEM
const StringArray message_type_strings[LAST_MESSAGE_TYPE] PROGMEM = {
#else
const StringArray message_type_strings[LAST_MESSAGE_TYPE] = {
#endif

	12, { "ZERO_ERROR",			"ZERO_ERROR" },
	12, { "DEBUG",				"DEBUGGEN" },
	12, { "VERBOSE",			"VERBOSE" },
	12, { "INFO",				"INFO" },
	12, { "DATA",				"DATEN" },
	12, { "ACTIVATED",			"ACTIVATED" },
	12, { "DEACTIVATED", 		"DEACTIVATED" },
	12, { "COMMAND", 			"COMMAND" },
	12, { "ADMIN", 				"ADMIN" },
	10, { "WARNING",			"WARNUNG" },
	12, { "ERROR",				"FEHLER" },
	12, { "STOP",				"HALT" },
};

/**************************************************************
 * Message Strings and Command Strings
 **************************************************************/
typedef enum {
	M_FF_ERROR = 0,
	M_FF_AWAKE,					//001
	M_D_SERIAL,					//002
	M_RTC_DETECT,				//003
	M_TIME_SYNCH_RTC,	 		//004
	M_WARN_SET_RTC,				//005
	M_WARN_RTC_HARD_CODED,		//006
	M_RTC_NOT_RUNNING,			//007
	M_RTC_NOT_FOUND,			//008
	M_LCD_INIT,					//009
	M_SR_BAD_ID,				//010
	M_BAD_TIME_STR,				//011
	M_SET_BCAT_NOT_MATCHED,		//012
	M_CMD_TO_BLOCK_NOT_OUTPUT,	//013
	M_F_READ,					//014
	M_CMD_BID_NULL,				//015
	M_RP_CONFIG,
	M_PROC_SYS_BLK,
	M_CONF_REG_BLKS,
	M_CONF_BLKS,
	M_OPR_BCAT_NOT_MATCHED,
	M_DISP_VALIDATE,
	M_CMD_ACT,
	M_CMD_DEACT,
	M_DISP_SETUP,
	M_DISP_OPER_1ST,
	M_ERR_EVENT_FILE,
	M_DISP_OPER_LOOP,
	M_BUF_SAVED,
	M_SIM,
	M_SIM_SYS_TIME,
	M_SIM_CONSOLE,
	M_NO_CONFIG,
	M_CONFIG_NOT_VALID,
	M_SD_NO_FILE_HANDLE,
	M_ERROR_EVENTS_EMPTY,
	M_SD_BEGIN_FAIL,		//M36
	M_NULL,					//M37
	M_FLIPFLOP,
	M_IA_NULL,
	M_GFV_NULL,
	M_GBV_NULL,
	M_INI_SR,
	M_SAVE_EMPTY_BUF,
	M_CONFKEY_ERROR_CAT,	//M44
	M_CONFKEY_LAST_BCAT,	//M45
	M_CONFKEY_NOTIN_DEFS,	//M46
	M_EVENTMSG_BEFORE_INIT,	//M47
	M_BUF_NOT_EMPTY,		//M48
	M_INIT_EVENT_BUF,		//M49
	M_SYS_BAD_DEF,			//M50
	M_DAY_FLAG_EMPTY,		//M51
	M_SCH_BAD_DEF,			//M52
	M_MON_BAD_DEF,			//M53
	M_IN_BAD_DEF,			//M54
	M_RL_BAD_DEF,			//M55
	M_CON_BAD_DEF,			//M56
	M_OUT_BAD_DEF,			//M57
	M_BAD_DATA_UNITS,		//M58
	M_ACT_CMD_UNKNOWN,		//M59
	M_DEACT_CMD_UNKNOWN,	//M60
	M_KEY_IDX_ZERO,			//M61
	M_BAD_BLOCK_CAT,		//M62
	M_ADDING_BLOCK,			//M63
	M_BLOCK_READ_BINARY,	//M64
	M_ADDBLOCK_ERROR,		//M65
	CMD_RESET_MINMAX,		//M66
	M_SR_MINMAX_RESET,
	M_BAD_TEMPERATURE_READ,	//M68
	M_VE_INIT,				//M69
	M_VE_SOC,
	M_VE_POWER,
	M_VE_VOLTAGE,
	M_VE_CURRENT,
	M_SYS_HEART_BEAT,
	M_VE_INIT_ERROR,
	M_OUTPUT_DISABLED,		//M76
	M_START_DELAY_ACTIVE,
	M_ASSERT_FAILED,		//M78
	NO_CONFIG_FILE,
	M_INI_LOAD_NOT_AVAIL,
	M_BIN_LOAD_NOT_AVAIL,
	M_BIN_SAVE_NOT_AVAIL,
	M_CONFIG_LOAD_BY_DIRECT_PARSE,		//M83
	M_CONFIGURE_BLOCK_ERROR,
	M_SETUP_ON_BLOCK_NOT_DISABLED_INIT,	//M85
	M_UNKNOWN_BLOCK_TYPE,
	M_VE_FAILED, 						//M87
	M_ADMIN_CMD_NOT_SUPPORTED,
	M_CONF_IF_UNKOWN_IF_TYPE,			//M89
	M_CONF_IF_DISABLED_NOT_BOOLEAN,		//M90
	M_CONF_IF_TYPE_NOT_SET,				//M91
	M_ADDIFS_MALLOC_NULL,				//M92
	M_CONFIFS_KEY_INVALID_FOR_TYPE,		//M93
	M_CONFIFS_WIFI_STATIC_NOT_BOOL,		//M94
	M_INVALID_DALLAS_ADDR,				//M95
	M_IF_TYPE_FIRST_CONFIG,				//M96
	M_CONFIFS_NOT_ALLOCATED,			//M97
	M_UNASSIGNED_DALLAS,				//M98
	M_DS1820B_INIT,						//M99
	M_DS1820B_INIT_FAIL,				//M100
	M_DALLAS_DETECTED,					//M101
	M_DS1820B_BID_NULL,					//M102
	M_ERROR_RTC_GARBAGE,				//M103
	M_BLOCK_ID_NOT_FOUND,				//M104
	M105,								//M105
	M106,								//M106
	M107,								//M107
	M108,								//M108
	M109,								//M109
	LAST_MESSAGE
} MessageEnum;


#ifdef ARDUINO_HIGH_PROGMEM
#define HIGHPROGMEM8 __attribute__((section(".fini8")))
#define HIGHPROGMEM7 __attribute__((section(".fini7")))
#define PAD32 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF
#define PAD256 PAD32, PAD32, PAD32, PAD32, PAD32, PAD32, PAD32, PAD32
#define PAD1K PAD256, PAD256, PAD256, PAD256
#define PAD8K PAD1K, PAD1K, PAD1K, PAD1K, PAD1K, PAD1K, PAD1K, PAD1K

const uint64_t PAD16K1[] HIGHPROGMEM8 = {
		PAD8K, PAD8K
};
const uint64_t PAD16K2[] HIGHPROGMEM8 = {
		PAD8K, PAD8K
};
#endif

#ifdef PLATFORM_ARDUINO
	#ifdef ARDUINO_HIGH_PROGMEM
		const StringArray message_strings[LAST_MESSAGE] HIGHPROGMEM7 = {
	#else
		const StringArray message_strings[LAST_MESSAGE] PROGMEM = {
	#endif //ARDUINO_HIGH_PROGMEM
#endif
#ifdef PLATFORM_LINUX
	const StringArray message_strings[LAST_MESSAGE] = {
#endif
	22, { "Error - Use of Zero Index"			"M0" },
	22, { "Fodder Factory Awake",				"M1" },
	12, { "Debug Serial Declared",				"M2" },
	12, { "RTC Detected",						"M3" },
	10, { "Time Synchronised to RTC",			"M4" },
	12, { "SET_RTC DEFINED",					"M5" },
	24, { "HARD CODED TIME SET",				"M6" },
	24,	{ "RTC Not Running - Hard Coding",		"M7" },
	18, { "RTC Not Found",						"M8" },
	20, { "u8g2 LCD Module Initialised",					"M9" },
	14, { "(UpdateStateRegister) Source ID not matched",	"M10" },
	14, { "(StrToTV) Malformed Time Interval String",		"M11" },
	14, { "(Setup) Block Cat Not Matched",					"M12" },
	14, { "(SetCommand) Command to non-FF_OUTPUT block",	"M13" },
	14, { "F_READ",											"M14" },
	14, { "(SetCommand) GetBlockByID returned NULL",		"M15" },
	14, { "Reading and Processing Configuration File",		"M16" },
	14, { "Processing [SYSTEM] Block",						"M17" },
	14, { "Registering Config Blocks and Labels",			"M18" },
	14, { "Processing Each Block Conf",						"M19" },
	14, { "(Operate) Block Cat Not Matched",				"M20" },
	14, { "(DISPATCHER) Called (Validate) for each block",	"M21" },
	14, { "CMD_ACT",								"M22" },
	14, { "CMD_DEACT",								"M23" },
	14, { "(DISPATCHER) Calling (Setup) for each block",	"M24" },
	14, { "(DISPATCHER) First call to (Operate)",			"M25" },
	14, { "Error Opening Event Log File",					"M26" },
	14, { "(DISPATCHER) Moving to (Operate) loop",			"M27" },
	14, { "Buffer Flushed to File",							"M28" },
	14, { "Running on Linux Platform",						"M29" },
	14, { "Using Linux System Time",						"M30" },
	14, { "Using stdout for console UI",					"M31" },
	14, { "Config not found. Can't continue",				"M32" },
	14, { "Config file failed validation",					"M33" },
	14, { "ERROR SD File Handle NULL",						"M34" },
	14, { "ERROR Event Buffer Empty",						"M35" },
	14, { "ERROR SD.begin FAIL",							"M36" },
	14, { "",												"M37"},
	14, { "Flip or Flop",									"M38" },
	14, { "(IsActive) GetBlockByID returned NULL",			"M39" },
	14, { "(GetFVal) GetBlockByID returned NULL",			"M40" },
	14, { "(GetBVal) GetBlockByID returned NULL",			"M41" },
	14, { "Initialising Persistence Store - State Register","M42" },
	14, { "(HALSaveEventBuffer) Save empty buffer",			"M43" },
	14, { "(GetConfKeyIndex) block_cat = FF_ERROR_CAT",		"M44" },
	14, { "(GetConfKeyIndex) block_cat >= LAST_BLOCK_CAT",	"M45" },
	14, { "(GetConfKeyIndex) Key not found in config defs",	"M46" },
	14, { "(EventMsg) EventMsg Before Init",				"M47" },
	14, { "(EventMsg) Buffer not empty after save",			"M48" },
	14, { "Init Message Queue (Event Ring Buffer)",			"M49" },
	14, { "(ConfigureSYSSetting) unknown B-Cat definition",	"M50" },
	14, { "(ConfigureSCHSetting) Days Flag returned empty",	"M51" },
	14, { "(ConfigureSCHSetting) unknown B-Cat definition",	"M52" },
	14, { "(ConfigureMONSetting) unknown B-Cat definition",	"M53" },
	14, { "(ConfigureINSetting) unknown B-Cat definition",	"M54" },
	14, { "(ConfigureRLSetting) unknown B-Cat definition",	"M55" },
	14, { "(ConfigureCONSetting) unknown B-Cat definition",	"M56" },
	14, { "(ConfigureOUTSetting) unknown B-Cat definition",	"M57" },
	14, { "(ConfigureINSetting) Malformed data_units",		"M58" },
	14, { "(ConfigureCONSetting) ACT_CMD str not known",	"M59" },
	14, { "(ConfigureCONSetting) DEACT_CMD str not known",	"M60" },
	14, { "(ConfigureCommonSetting) key_idx = zero error",	"M61" },
	14, { "(ConfigureBlock) Invalid Block Category",		"M62" },
	14, { "(ConfigureBlock/IFBlock) Add returned NULL",		"M63" },
	14, { "Block Configuration Read from Binary File",		"M64" },
	14, { "(AddBlock) malloc returned NULL",				"M65" },
	14, { "CMD_RESET_MINMAX",								"M66" },
	14, { "(UpdateS..R..) MinMax Display Registers Reset",	"M67" },
	14, { "DallasTemerature Invalid Repeating Previous",	"M68" },
	14, { "VE.Direct Connection Initialised",				"M69" },
	14, { "VE_DATA_SOC",									"M70" },
	14, { "VE_DATA_POWER",									"M71" },
	14, { "VE_DATA_VOLTAGE",								"M72" },
	14, { "VE_DATA_CURRENT",								"M73" },
	14, { "System Heart Beat",								"M74" },
	14, { "Failed to Initialise VE.Direct Connection",		"M75" },
	14, { "Output Disabled by DISABLE_OUTPUTS directive",	"M76" },
	14, { "Start-up Delay Active",							"M77" },
	14, { "ASSERT FAILED**************************",		"M78" },
	14, { "Config file could not be opened",				"M79" },
	14, { "INI Load not available on this platform",		"M80" },
	14, { "Binary Load not available on this platform",		"M81" },
	14, { "Binary Save not available on this platform",		"M82" },
	14, { "Configuration loaded and parsed from file",		"M83" },
	14, { "ConfigureBlock Reported an Error.",	"M84" },
	14, { "Setup not run on block that is not STATUS_DISABLED_INIT",	"M85" },
	14, { "Unknown Block Type in Setup or Operate Loop",				"M86" },
	14, { "VEDirect failed to begin in operate loop. Disabled",			"M87" },
	14, { "Set Output CMD by Admin. Command not supported",				"M88" },
	14, { "(ConfigureIFSetting) Unkown Interface Type",					"M89" },
	14, { "(ConfigureIFSetting) Disabled must be True or False",		"M90" },
	14, { "(ConfigureIFSetting) Type must be set first",				"M91" },
	14, { "Error allocating memory for Interface Settings",				"M92" },
	14, { "Error, Key not valid for type of interface",					"M93" },
	14, { "Config WIFI, Static must be True, False or absent",			"M94" },
	14, { "Invalid Maxim/Dallas Address",								"M95" },
	14, { "First config call must specify Interface Type",				"M96" },
	14, { "Interface specific settings not allocated",					"M97" },
	14, { "Unassigned Dallas Device on OneWire Bus",					"M98" },
	14, { "Matching DS1820B Detected and Initialised",					"M99" },
	14, { "Specified DS1820B was not detected on Onewire bus",			"M100" },
	14, { "DS1820B Device Detected",									"M101" },
	14, { "DS1820B Read GetBlockByID returned NULL",					"M102" },
	14, { "RTC Returned Invalid Time",									"M103" },
	14, { "Block ID not found",											"M104" },
	14, { "M105",														"M105" },
	14, { "M106",														"M106" },
	14, { "M107",														"M107" },
	14, { "M108",														"M108" },
	14, { "M109",														"M109" },
};

/**************************************************************
 * String Access Functions Prototypes
 **************************************************************/
void GetMessageTypeString(char *str_buf, int message_type_enum);

void GetMessageString(char *str_buf, int message_enum);

uint8_t GetLanguage(void);

uint8_t DayStringArrayIndex(const char* key);
uint8_t UnitStringArrayIndex(const char* key);
uint8_t CommandStringArrayIndex(const char* key);
uint8_t LanguageStringArrayIndex(const char* key);
uint8_t InterfaceTypeStringArrayIndex(const char* key);

uint8_t BlockTypeStringArrayIndex(const char* key);

//uint8_t SimpleStringArrayIndex(const SimpleStringArray array, const char* key);

char *strcpy_misc(char *dest, uint8_t src);
char *strcat_misc(char *dest, uint8_t src);

/********* Properly Formed Overloaded Generics Accommodating PROGMEM **************/
size_t strlen_hal(const char *s);
char *strcpy_hal(char *dest, const char *src);
char *strcat_hal(char *dest, const char *src);
int strcmp_hal(const char *s1, const char *s2);
int strcasecmp_hal(const char *s1, const char *s2);

#ifdef ARDUINO
char *strcpy_hal(char *dest, const __FlashStringHelper *src);
char *strcat_hal(char *dest, const __FlashStringHelper *src);
int strcmp_hal(const char *s1, const __FlashStringHelper *s2);
int strcasecmp_hal(const char *s1, const __FlashStringHelper *s2);
#endif

/**********************************************************************************/
//char *strcat_hal(char *dest, const char *src);
void *memcpy_hal(void *dest, const void *src, size_t sz);



/**********************************************************************************
 * Depreciated code - either no longer used or moved to glitch generation
**********************************************************************************/

#if not defined USE_ITCH && not defined ITCH_HEADERS_ONLY

typedef struct BLOCK_CATS {
	const uint8_t cat_id;
	const char conf_section_label[MAX_LABEL_LENGTH];
	const char conf_section_key_base[MAX_LABEL_LENGTH];
	const char* const conf_keys[MAX_CONF_KEYS_PER_BLOCK];
} BlockCatArray;


#ifdef PROGMEM_BLOCK_DEFS
static const BlockCatArray block_cat_defs[LAST_BLOCK_CAT] PROGMEM = {
#else
static const BlockCatArray block_cat_defs[LAST_BLOCK_CAT] = {
#endif

	FF_ERROR_CAT,
	"\0",
	"\0",
	{
		"\0",
	},

	FF_GENERIC_BLOCK,
	"\0",					// config headings in INI config - "inputs" "outputs" etc
	"\0",					// config label singular - "input3" "CONFIG INPUT Input_name"
	{
		"",					// not used - dummy to skip error enum = 0
		"type",				// Block Type - See block_type_strings
		"display_name",		// Short user friendly display label
		"description",		// Optional descriptive for design stage
	},

	FF_SYSTEM,
	"system",
	"system",
	{
		"",					// not used - dummy to skip error enum = 0
		"type",				// See Generic Above
		"display_name",		// See Generic Above
		"description",		// See Generic Above
		"language",			// TODO: Partially Implemented on message and debug strings
		"temp_scale",		// TODO: Setable but ignored presently. Celisus assumed
		"week_start",		// TODO: implement, currently hard coded
	},

	FF_INPUT,
	"inputs",
	"input",
	{
		"",					//not used - dummy to skip error enum = 0
		"type",				// See Generic Above
		"display_name",		// See Generic Above
		"description",		// See Generic Above
		"interface",
		"if_num",
		"log_rate",
		"data_units",
		"data_type"
	},

	FF_MONITOR,
	"monitors",
	"monitor",
	{
		"",					// not used - dummy to skip error enum = 0
		"type",				// See Generic Above
		"display_name",		// See Generic Above
		"description",		// See Generic Above
		"input1",
		"input2",
		"input3",
		"input4",
		"act_val",
		"deact_val"
	},

	FF_SCHEDULE,
	"schedules",
	"schedule",
	{
		"",					// not used - dummy to skip error enum = 0
		"type",				// See Generic Above
		"display_name",		// See Generic Above
		"description",		// See Generic Above
		"days",
		"time_start",
		"time_end",
		"time_duration",
		"time_repeat",
	},

	FF_RULE,
	"rules",
	"rule",
	{
		"",					// not used - dummy to skip error enum = 0
		"type",				// See Generic Above
		"display_name",		// See Generic Above
		"description",		// See Generic Above
		"param1",
		"param2",
		"param3",
		"param_not",
	},

	FF_CONTROLLER,
	"controllers",
	"controller",
	{
		"",					// not used - dummy to skip error enum = 0
		"type",				// See Generic Above
		"display_name",		// See Generic Above
		"description",		// See Generic Above
		"rule",
		"output",
		"act_cmd",
		"deact_cmd",

	},

	FF_OUTPUT,
	"outputs",
	"output",
	{
		"",					// not used - dummy to skip error enum = 0
		"type",				// See Generic Above
		"display_name",		// See Generic Above
		"description",		// See Generic Above
		"interface",
		"if_num",
	},
};


/**************************************************************
 * Block Types
 **************************************************************/


enum {
	BT_ERROR = 0,
	SYS_SYSTEM,
	IN_ONEWIRE,
	IN_DIGITAL,
	MON_CONDITION_LOW,
	MON_CONDITION_HIGH,
	MON_AVERAGE_CONDITION_LOW,
	MON_AVERAGE_CONDITION_HIGH,
	MON_TRIGGER,
	SCH_START_STOP,
	SCH_ONE_SHOT,
	SCH_START_DURATION_REPEAT,
	RL_LOGIC_ANDNOT,
	RL_LOGIC_SINGLE,
	RL_LOGIC_AND,
	RL_LOGIC_SINGLENOT,
	CON_ONOFF,
	CON_SYSTEM,
	OUT_DIGITAL,
	OUT_SYSTEM_CALL,
	LAST_BLOCK_TYPE
}; // BlockTypes;



#ifdef USE_PROGMEM
static const SimpleStringArray block_type_strings[LAST_BLOCK_TYPE] PROGMEM = {
#else
static const SimpleStringArray block_type_strings[LAST_BLOCK_TYPE] = {
#endif
	"BT_ERROR",
	"SYS_SYSTEM",
	"IN_ONEWIRE",
	"IN_DIGITAL",
	"MON_CONDITION_LOW",
	"MON_CONDITION_HIGH",
	"MON_AVERAGE_CONDITION_LOW",
	"MON_AVERAGE_CONDITION_HIGH",
	"MON_TRIGGER",
	"SCH_START_STOP",
	"SCH_ONE_SHOT",
	"SCH_START_DURATION_REPEAT",
	"RL_LOGIC_ANDNOT",
	"RL_LOGIC_SINGLE",
	"RL_LOGIC_AND",
	"RL_LOGIC_SINGLENOT",
	"CON_ONOFF",
	"CON_SYSTEM",
	"OUT_DIGITAL",
	"OUT_SYSTEM_CALL",
};


enum OUTPUT_COMMANDS {
	CMD_ERROR = 0,
	CMD_OUTPUT_OFF,
	CMD_OUTPUT_ON,
	CMD_RESET_MIN_MAX,
	LAST_COMMAND
};

#ifdef USE_PROGMEM
static const SimpleStringArray command_strings[LAST_COMMAND] PROGMEM = {
#else
static const SimpleStringArray command_strings[LAST_COMMAND] = {
#endif
	"CMD_ERROR",
	"CMD_OUTPUT_OFF",
	"CMD_OUTPUT_ON",
	"CMD_RESET_MIN_MAX",
};


/**************************************************************
 * Block Category Definitions
 **************************************************************/
//typedef
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
};//BlockCatEnum;


enum {
	SYS_ERROR_KEY = 0,
	SYS_TYPE,			//common to all block categories in this order
	SYS_DISPLAY_NAME,	//common to all block categories in this order
	SYS_DESCRIPTION,		//common to all block categories in this order
	SYS_LANGUAGE,
	SYS_TEMPERATURE,
	SYS_WEEK_START,
	LAST_SYS_KEY_TYPE,
};

enum {
	IN_ERROR_KEY = 0,
	IN_TYPE,			//common to all block categories in this order
	IN_DISPLAY_NAME,	//common to all block categories in this order
	IN_DESCRIPTION,		//common to all block categories in this order
	IN_INTERFACE,
	IN_IF_NUM,
	IN_LOG_RATE,
	IN_DATA_UNITS,
	IN_DATA_TYPE,
	LAST_IN_KEY_TYPE
};// InputConfKeyList;

enum {
	MON_ERROR_KEY = 0,
	MON_TYPE,			//common to all block categories in this order
	MON_DISPLAY_NAME,	//common to all block categories in this order
	MON_DESCRIPTION,	//common to all block categories in this order
	MON_INPUT1,
	MON_INPUT2,
	MON_INPUT3,
	MON_INPUT4,
	MON_ACT_VAL,
	MON_DEACT_VAL,
	LAST_MON_KEY_TYPE
};

enum {
	SCH_ERROR_KEY = 0,
	SCH_TYPE,			//common to all block categories in this order
	SCH_DISPLAY_NAME,	//common to all block categories in this order
	SCH_DESCRIPTION,	//common to all block categories in this order
	SCH_DAYS,
	SCH_TIME_START,
	SCH_TIME_END,
	SCH_TIME_DURATION,
	SCH_TIME_REPEAT,
	LAST_SCH_KEY_TYPE
};

enum {
	RL_ERROR_KEY = 0,
	RL_TYPE,			//common to all block categories in this order
	RL_DISPLAY_NAME,	//common to all block categories in this order
	RL_DESCRIPTION,		//common to all block categories in this order
	RL_PARAM_1,
	RL_PARAM_2,
	RL_PARAM_3,
	RL_PARAM_NOT,
	LAST_RL_KEY_TYPE
};

enum {
	CON_ERROR_KEY = 0,
	CON_TYPE,			//common to all block categories in this order
	CON_DISPLAY_NAME,	//common to all block categories in this order
	CON_DESCRIPTION,	//common to all block categories in this order
	CON_RULE,
	CON_OUTPUT,
	CON_ACT_CMD,
	CON_DEACT_CMD,
	LAST_CON_KEY_TYPE
};

enum {
	OUT_ERROR_TYPE = 0,
	OUT_TYPE,			//common to all block categories in this order
	OUT_DISPLAY_NAME,	//common to all block categories in this order
	OUT_DESCRIPTION,	//common to all block categories in this order
	OUT_INTERFACE,
	OUT_IF_NUM,
	LAST_OUT_KEY_TYPE
};


/*
// INI Error enum defined in IniFile Class per following
enum {
	errorNoError = 0,
	errorFileNotFound,
	errorFileNotOpen,
	errorBufferTooSmall,
	errorSeekError,
	errorSectionNotFound,
	errorKeyNotFound,
	errorEndOfFile,
	errorUnknownError,
};
*/

#ifdef USE_PROGMEM
static const SimpleStringArray ini_error_strings[INI_ERROR_TYPES] PROGMEM = {
#else
static const SimpleStringArray ini_error_strings[INI_ERROR_TYPES] = {
#endif
	"No Error",
	"Config file not found",
	"Config File Not Open",
	"INI Line Buffer Too Small",
	"Config File Seek Error",
	"Config Section Not Found",
	"Config Key Not Found",
	"Reached end of Config File Unexpectedly",
	"Config Unknown Error",
};

/**************************************************************
 * Units, Scales & Interfaces
 **************************************************************/
typedef enum {
	UNIT_ERROR,
	CELSIUS,
	FAHRENHEIT,
	KELVIN,
	REL_HUM,
	CUBIC_M,
	LITRES,
	PPM,
	ONOFF,
	RPM,
	LAST_UNIT
} UnitsEnum;

#ifdef USE_PROGMEM
static const SimpleStringArray unit_strings[LAST_UNIT] PROGMEM = {
#else
static const SimpleStringArray unit_strings[LAST_UNIT] = {
#endif
	"UnitTypeError",
	"Celsius",
	"Fahrenheit",
	"Kelvin",
	"Relative Humidity",
	"Cubic Metres",
	"litres",
	"Parts per Million",
	"ONOFF",
	"Revolutions per Minute"
};

enum {				// TODO: Code presently assumes SUN = 0
	SUN = 0,
	MON,
	TUE,
	WED,
	THU,
	FRI,
	SAT,
	LAST_DAY
};

#ifdef USE_PROGMEM
static const SimpleStringArray day_strings[LAST_DAY] PROGMEM = {
#else
static const SimpleStringArray day_strings[LAST_DAY] = {
#endif
	"SUN",
	"MON",
	"TUE",
	"WED",
	"THU",
	"FRI",
	"SAT",
};

enum {						// Interface Types
	IF_ERROR = 0,
	IF_PWM_IN,
	IF_PWM_OUT,
	IF_PPM_IN,
	IF_PPM_OUT,
	IF_ONEWIRE,
	IF_DIG_PIN_IN,
	IF_DIG_PIN_OUT,
	IF_SYSTEM_FUNCTION,
	LAST_INTERFACE
};

#ifdef USE_PROGMEM
static const SimpleStringArray interface_strings[LAST_INTERFACE] PROGMEM = {
#else
static const SimpleStringArray interface_strings[LAST_INTERFACE] = {
#endif
	"IF_ERROR",
	"PWM_IN",
	"PWM_OUT",
	"PPM_IN",
	"PPM_OUT",
	"ONEWIRE",
	"DIG_PIN_IN",
	"DIG_PIN_OUT",
	"SYSTEM_FUNCTION",
};

enum {						// Block states
	STATUS_ERROR = 0,
	STATUS_ENABLED,
	STATUS_ENABLED_INIT,
	STATUS_ENABLED_VALID_DATA,
	STATUS_ENABLED_INVALID_DATA,
	STATUS_DISABLED,
	STATUS_DISABLED_INIT,
	STATUS_DISABLED_ERROR,
	STATUS_DISABLED_ADMIN,
	LAST_STATUS,
};

#ifdef USE_PROGMEM
static const SimpleStringArray status_strings[LAST_STATUS] PROGMEM = {
#else
static const SimpleStringArray status_strings[LAST_STATUS] = {
#endif
	"STATUS_ERROR",
	"STATUS_ENABLED",
	"STATUS_ENABLED_INIT",
	"STATUS_ENABLED_VALID_DATA",
	"STATUS_ENABLED_INVALID_DATA",
	"STATUS_DISABLED",
	"STATUS_DISABLED_INIT",
	"STATUS_DISABLED_ERROR",
	"STATUS_DISABLED_ADMIN",

};



#endif




#endif /* SRC_FF_STRINGS_H_ */
