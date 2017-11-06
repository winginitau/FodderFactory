/************************************************
 ff_string_consts.h

 FF System Core (Stand-alone Embedded)

 (c) Brendan McLearie - bmclearie@mclearie.com

 Explanatory notes:
 - General approach is to concentrate all strings
   here as static consts so they will be loaded into
   ROM on most embedded systems.
 - Then refer to all strings and lists via associated
   enums thereby leaving the run-time with memory and
   CPU efficient types.
 - enums are defined with:
    - Zero (0) element being a reserved error to
      catch via debug / error reporting any iterations
      or evaluations that return 0 unintentionally.
    - The last element is always defined as LAST_<type>
      as a fall through catch in loops and iterations
      and allowing nice informative loops eg:
       while (iterator < LAST_TYPE) {iterator++;};


 ************************************************/
#ifndef SRC_FF_STRINGS_H_
#define SRC_FF_STRINGS_H_


//#include "ff_sys_config.h"
//#include "ff_utils.h"
#ifdef FF_SIMULATOR
#include <stdint.h>
#endif

/**************************************************************
 * Platform Directives
 **************************************************************/
//PROGMEM for arduino

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
typedef struct STRING_ARRAY_TYPE {
	// Longest length of this string in all languages
	const int max_len;
	// Array of pointers to each language-specific string of type
	const char* const text[LAST_LANGUAGE];
} StringArray;

typedef struct SIMPLE_STRING_ARRAY_TYPE {
	// Array of strings (usually labels and messages) relating to enums
	// Inside this struct, extended intitaliser lists are ok
	const char* text;
} SimpleStringArray;

typedef struct BLOCK_CATS {
	const uint8_t cat_id;
	const char conf_section_label[MAX_LABEL_LENGTH];
	const char conf_section_key_base[MAX_LABEL_LENGTH];
	const char* const conf_keys[MAX_CONF_KEYS_PER_BLOCK];
} BlockCatArray;

/**************************************************************
 * Language Strings - Must Follow the typedef that defines it
 **************************************************************/
static const SimpleStringArray language_strings[LAST_LANGUAGE] = {
	"English",
	"Deutsch",
};

/**************************************************************
 * Block Category Definitions
 **************************************************************/
//typedef
enum {
	FF_ERROR_CAT = 0,
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


static const BlockCatArray block_cat_defs[LAST_BLOCK_CAT] = {
	FF_ERROR_CAT, "\0", "\0", {
		"\0",
	},

	FF_SYSTEM, "system", "", {
		"",		//not used - dummy to skip error enum = 0
		"type",
		"display_name",
		"description",
		"language",
		"temp_scale",
		"week_start",
	},
	FF_INPUT, "inputs", "input", {
		"",		//not used - dummy to skip error enum = 0
		"type",
		"display_name",
		"description",
		"interface",
		"if_num",
		"log_rate",
		"data_units",
		"data_type"
	},
	FF_MONITOR, "monitors", "monitor", {
		"",		//not used - dummy to skip error enum = 0
		"type",
		"display_name",
		"description",
		"input1",
		"input2",
		"input3",
		"input4",
		"act_val",
		"deact_val"
	},
	FF_SCHEDULE, "schedules", "schedule", {
		"",		//not used - dummy to skip error enum = 0
		"type",
		"display_name",
		"description",
		"days",
		"time_start",
		"time_end",
		"time_duration",
		"time_repeat",
	},
	FF_RULE, "rules", "rule", {
		"",		//not used - dummy to skip error enum = 0
		"type",
		"display_name",
		"description",
		"param1",
		"param2",
		"param3",
		"param_not",
	},
	FF_CONTROLLER, "controllers", "controller", {
		"",		//not used - dummy to skip error enum = 0
		"type",
		"display_name",
		"description",
		"rule",
		"output",
		"act_cmd",
		"deact_cmd",

	},

	FF_OUTPUT, "outputs", "output", {
		"",		//not used - dummy to skip error enum = 0
		"type",
		"display_name",
		"description",
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


static const SimpleStringArray block_type_strings[LAST_BLOCK_TYPE] = {
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

/**************************************************************
 * EventTypes
 **************************************************************/

//TODO move these fully to events prune old strings
typedef enum {
	E_ZERO_ERROR = 0,
	E_DEBUG_MSG,
	E_INIT,
	E_INFO,
	E_DATA,
	E_ACT,
	E_DEACT,
	E_COMMAND,
	E_WARNING,
	E_ERROR,
	SCHED_DAILY_ON,
	SCHED_DAILY_OFF,
	CONDITION_HIGH,
	CONDITION_LOW,
	TRIGGER_ON,
	TRIGGER_OFF,
	SYSTEM_MSG,
	CLEAR_DONE,
	RESET_MIN_MAX,
	SET,
	LAST_MESSAGE_TYPE
} MessageTypeEnum;

static const StringArray message_type_strings[LAST_MESSAGE_TYPE] = {
	12, { "ZERO_ERROR",			"" },
	12, { "DEBUG",				"DEBUGGEN" },
	12, { "INITIALISED"			"" },
	12, { "INFO",				"INFO" },
	12, { "DATA",				"DATEN" },
	12, { "ACTIVATED",			"" },
	12, { "DEACTIVATED", 		"" },
	12, { "COMMAND", 			"" },
	10, { "WARNING",			"WARNUNG" },
	12, { "ERROR",				"FEHLER" },
	24, { "SCHED_DAILY_ON",		"ZEITPLAN_TÄGLICH_AUF" },
	24,	{ "SCHED_DAILY_OFF",	"ZEITPLAN_TÄGLICH_AUS" },
	18, { "CONDITION_HIGH",		"BEDINGUNG_HOCH" },
	20, { "CONDITION_LOW",		"BEDINGUNG_NIEDRIG" },
	14, { "TRIGGER_ON",			"AUSLÖSER_AUF" },
	14,	{ "TRIGGER_OFF",		"AUSLÖSER_AUS" },
	12, { "SYSTEM",				"SYSTEM" },
	14,	{ "CLEAR_DONE",			"KLAR_ERLEDIGT" },
	16, { "RESET_MIN_MAX", 		"RESET_MIN_MAX" },
	6,	{ "SET", 				"SET" }
};

/**************************************************************
 * Message Strings
 **************************************************************/
typedef enum {
	M_FF_ERROR = 0,
	M_FF_AWAKE,
	M_D_SERIAL,
	M_RTC_DETECT,
	M_RTC_REPORT_RUNNING,
	M_WARN_SET_RTC,
	M_WARN_RTC_HARD_CODED,
	M_RTC_NOT_RUNNING,
	M_RTC_NOT_FOUND,
	M_LCD_INIT,
	M_SD_INIT_ATTEMPT,
	M_SD_CARD_FAIL,
	M_SD_CARD_GOOD,
	M_TEMPS_INIT,
	M_F_READ,
	M_INPUT_LABEL_NOT_FOUND,
	M_SETTING_CONTROLLERS,
	M_CONTROL_ACTIVATED,
	M_CONTROL_DEACT,
	M_MIN_MAX_CLEAR,
	M_MIDNIGHT_DONE,
	M_CONTROLLERS_INIT,
	M_ON,
	M_OFF,
	M_SETTING_OUTPUTS,
	M_OUTPUTS_INIT,
	M_ERR_EVENT_FILE,
	M_OUTPUT_LABEL_NOT_FOUND,
	M_BUF_SAVED,
	M_SIM,
	M_SIM_SYS_TIME,
	M_SIM_CONSOLE,
	M_NO_CONFIG,
	M_CONFIG_NOT_VALID,
	M_SD_NO_FILE_HANDLE,
	M_ERROR_EVENTS_EMPTY,
	M_SD_BEGIN_FAIL,
	M_NULL,
	M_FLIPFLOP,
	M39,
	M40,
	LAST_MESSAGE
} MessageEnum;

static const StringArray message_strings[LAST_MESSAGE] = {
	22, { "Error - Use of Zero Index"			"M0" },
	22, { "Fodder Factory Awake",				"M1" },
	12, { "DEBUG_SERIAL Declared",				"M2" },
	12, { "RTC Detected",						"M3" },
	10, { "RTC Reports as Running",				"M4" },
	12, { "SET_RTC DEFINED",					"M5" },
	24, { "HARD CODED TIME SET",				"M6" },
	24,	{ "RTC Not Running - Hard Coding",		"M7" },
	18, { "RTC Not Found",						"M8" },
	20, { "u8g2 LCD Module Initialised",		"M9" },
	14, { "Attempting SD Card Init",			"M10" },
	14, { "SD Card Failed or Not Present",		"M11" },
	14, { "SD Card Initialised",				"M12" },
	14, { "Temp Sensors Initialised",			"M13" },
	14, { "F_READ",								"M14" },
	14, { "Input Device Label Not Found",		"M15" },
	14, { "Setting up Controllers",				"M16" },
	14, { "Control Activated",					"M17" },
	14, { "Control De-activated",				"M18" },
	14, { "Min and Max Cleared",				"M19" },
	14, { "Midnight Rollover",					"M20" },
	14, { "Controllers Initialised",			"M21" },
	14, { "ON",									"M22" },
	14, { "OFF",								"M23" },
	14, { "Setting up Outputs",					"M24" },
	14, { "Outputs Initialised",				"M25" },
	14, { "Error Opening Event Log File",		"M26" },
	14, { "Output Device Label Not Found",		"M27" },
	14, { "Buffer Flushed to File",				"M28" },
	14, { "Running in Simulator Mode",			"M29" },
	14, { "Simulator will Use System Time",		"M30" },
	14, { "Simulator using console for UI",		"M31" },
	14, { "Config not found. Can't continue",	"M32" },
	14, { "Config file failed validation",		"M33" },
	14, { "DEBUG ERROR SD File Handle NULL",	"M34" },
	14, { "DEBUG ERROR Event Buffer Empty",		"M35" },
	14, { "DEBUG ERROR SD.begin FAIL",			"M36" },
	14, { "",									""},
	14, { "Flip or Flop",						"M38" },
	14, { "M39",								"M39" },
	14, { "M40",								"M40" },
};

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

static const SimpleStringArray ini_error_strings[INI_ERROR_TYPES] = {
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
//	KELVIN,
//	REL_HUM,
//	CUBIC_M,
//	LITRES,
//	PPM,
	ONOFF,
	LAST_UNIT
} UnitsEnum;

static const StringArray unit_strings[LAST_UNIT] = {
	12, {"UnitTypeError", 		"UnitTypeError"},
	12, {"Celsius", 			"Celsius"},
	12, {"Fahrenheit", 			"Fahrenheit"},
//	12, {"Kelvin", 				"Kelvin"},
//	12, {"Rel Hum",				"Rel Hum"},
//	12, {"Cubic Metres",		"m3"},
//	12, {"litres", 				"litres"},
//	12, {"Parts per Million",	"Parts per Million"},
	12, {"ONOFF",				"ONOFF"},
};

enum {
	SUN = 0,
	MON,
	TUE,
	WED,
	THU,
	FRI,
	SAT,
	LAST_DAY
};

static const SimpleStringArray day_strings[LAST_DAY] = {
	"SUN",
	"MON",
	"TUE",
	"WED",
	"THU",
	"FRI",
	"SAT",
};

enum OUTPUT_COMMANDS {
	CMD_ERROR = 0,
	CMD_OUTPUT_OFF,
	CMD_OUTPUT_ON,
	CMD_RESET_MIN_MAX,
	LAST_COMMAND
};

static const SimpleStringArray command_strings[LAST_COMMAND] = {
	"CMD_ERROR",
	"CMD_OUTPUT_OFF",
	"CMD_OUTPUT_ON",
	"CMD_RESET_MIN_MAX",
};

enum {
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

static const SimpleStringArray interface_strings[LAST_INTERFACE] = {
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

enum {
	STATUS_ERROR = 0,
	STATUS_INIT,
	STATUS_VALID_DATA,
	STATUS_INVALID_DATA,
	LAST_STATUS,
};

static const SimpleStringArray status_strings[LAST_STATUS] = {
	"STATUS_ERROR",
	"STATUS_INIT",
	"STATUS_VALID_DATA",
	"STATUS_INVALID_DATA",
};

#ifndef FF_ARDUINO
enum {
	LOW = 0,
	HIGH = 1,
};
#endif

/**************************************************************
 * String Functions Prototypes
 **************************************************************/
char const* GetMessageTypeString(int message_type_enum);

char const* GetMessageString(int message_enum);

uint8_t GetLanguage(void);

uint8_t DayStringArrayIndex(const char* key);

uint8_t UnitStringArrayIndex(const char* key);

uint8_t LanguageStringArrayIndex(const char* key);

uint8_t InterfaceStringArrayIndex(const char* key);

uint8_t BlockTypeStringArrayIndex(const char* key);

//uint8_t SimpleStringArrayIndex(const SimpleStringArray array, const char* key);


#endif /* SRC_FF_STRINGS_H_ */
