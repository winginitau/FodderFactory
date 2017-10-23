/*
 * ff_strings.h
 *
 *  Created on: 4Oct.,2017
 *      Author: brendan
 */

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
 * Languages
 **************************************************************/
typedef enum LANG_TYPE {
	ENGLISH = 0, GERMAN, LAST_LANGUAGE
} LanguageType;

//TODO Language Strings

/**************************************************************
 * String array typedefs
 **************************************************************/
typedef struct STRING_ARRAY_TYPE {
	// Longest length of this string in all languages
	const int max_len;
	// Array of pointers to each language-specific string of type
	const char* const text[LAST_LANGUAGE];
} StringArray;

//typedef struct SIMPLE_STRING_ARRAY_TYPE {
	// Array of strings (usually labels and messages) relating to enums
//	const char* text;
//} SimpleStringArray;

typedef struct BLOCK_CATS {
	const uint8_t cat_id;
	const char conf_section_label[MAX_LABEL_LENGTH];
	const char conf_section_key_base[MAX_LABEL_LENGTH];
	const char* const conf_keys[MAX_CONF_KEYS_PER_BLOCK];
} BlockCatArray;

/**************************************************************
 * Block Category Definitions
 **************************************************************/
//typedef
enum {
	FF_SYSTEM = 0,
	FF_INPUT,
	FF_MONITOR,
	FF_SCHEDULE,
	FF_RULE,
	FF_CONTROLLER,
	FF_OUTPUT,
	LAST_BLOCK_CAT,
};//BlockCatEnum;

enum {
	IN_TYPE = 0,		//common to all block categories in this order
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
	OUT_TYPE,			//common to all block categories in this order
	OUT_DISPLAY_NAME,	//common to all block categories in this order
	OUT_DESCRIPTION,	//common to all block categories in this order
	OUT_DIGITAL_PIN,
	LAST_OUT_KEY_TYPE
};


static const BlockCatArray block_cat_defs[LAST_BLOCK_CAT] = {
	FF_SYSTEM, "system", "", {
			"temp_scale",
			"language",
			"week_start",
	},
	FF_INPUT, "inputs", "input", {
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
			"type",
			"display_name",
			"description",
			"param1",
			"param2",
			"param3",
			"param_not",
	},
	FF_CONTROLLER, "controllers", "controller", {
			"type",
			"display_name",
			"description",
			"rule",
			"output",
			"act_cmd",
			"deact_cmd",

	},

	FF_OUTPUT, "outputs", "output", {
			"type",
			"display_name",
			"description",
			"out_digital_pin",
	},
};



/**************************************************************
 * Block Types
 **************************************************************/

enum {
	IN_ONEWIRE = 0,
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
	CON_ONOFF,
	CON_SYSTEM,
	OUT_DIGITAL,
	LAST_BLOCK_TYPE
}; // BlockTypes;


static const char* block_type_strings[LAST_BLOCK_TYPE] {
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
	"CON_ONOFF",
	"CON_SYSTEM",
	"OUT_DIGITAL"
};

/**************************************************************
 * MessageTypes
 **************************************************************/
typedef enum {
	DEBUG_MSG = 0,
	INFO,
	DATA,
	WARNING,
	ERROR,
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
	12, { "DEBUG",				"DEBUGGEN" },
	12, { "INFO",				"INFO" },
	12, { "DATA",				"DATEN" },
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
	M_FF_AWAKE = 0,
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
	M36,
	M37,
	M38,
	M39,
	LAST_MESSAGE
} MessageEnum;

static const StringArray message_strings[LAST_MESSAGE] = {
	22, { "Fodder Factory Awake",				"" },
	12, { "DEBUG_SERIAL Declared",				"M1" },
	12, { "RTC Detected",						"M2" },
	10, { "RTC Reports as Running",				"M3" },
	12, { "SET_RTC DEFINED",					"M4" },
	24, { "HARD CODED TIME SET",				"M5" },
	24,	{ "RTC Not Running - Hard Coding",		"M6" },
	18, { "RTC Not Found",						"M7" },
	20, { "u8g2 LCD Module Initialised",		"M8" },
	14, { "Attempting SD Card Init",			"M9" },
	14, { "SD Card Failed or Not Present",		"M10" },
	14, { "SD Card Initialised",				"M11" },
	14, { "Temp Sensors Initialised",			"M12" },
	14, { "F_READ",								"M13" },
	14, { "Input Device Label Not Found",		"M14" },
	14, { "Setting up Controllers",				"M15" },
	14, { "Control Activated",					"M16" },
	14, { "Control De-activated",				"M17" },
	14, { "Min and Max Cleared",				"M18" },
	14, { "Midnight Rollover",					"M19" },
	14, { "Controllers Initialised",			"M20" },
	14, { "ON",									"M21" },
	14, { "OFF",								"M22" },
	14, { "Setting up Outputs",					"M23" },
	14, { "Outputs Initialised",				"M24" },
	14, { "Error Opening Event Log File",		"M25" },
	14, { "Output Device Label Not Found",		"M26" },
	14, { "Buffer Flushed to File",				"M27" },
	14, { "Running in Simulator Mode",			"M28" },
	14, { "Simulator will Use System Time",		"M29" },
	14, { "Simulator using console for UI",		"M30" },
	14, { "Config not found. Can't continue",	"M31" },
	14, { "Config file failed validation",		"M32" },
	14, { "DEBUG ERROR SD File Handle NULL",	"M33" },
	14, { "DEBUG ERROR Event Buffer Empty",		"M34" },
	14, { "DEBUG ERROR SD.begin FAIL",			"M35" },
	14, { "M36",								"M36" },
	14, { "M37",								"M37" },
	14, { "M38",								"M38" },
	14, { "M39",								"M39" },
};

// INI Error enum defined in IniFile Class
//
// 	errorNoError = 0,
//  errorFileNotFound,
//  errorFileNotOpen,
//  errorBufferTooSmall,
//  errorSeekError,
//  errorSectionNotFound,
//  errorKeyNotFound,
//  errorEndOfFile,
//	errorUnknownError,


static const char* ini_error_strings[INI_ERROR_TYPES] = {
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
 * Device Strings
 **************************************************************/
/*
typedef enum {
	D_SYS = 0,
	D1,
	D2,
	D3,
	D4,
	D_INSIDE_TEMP,
	D_OUTSIDE_TEMP,
	D_WATER_TEMP,
	D_RESET_MIN_MAX,
	D9,
	LAST_DEVICE
} DeviceEnum;

static const StringArray device_strings[LAST_DEVICE] = {
	22, { "D_SYS",			"" },
	12, { "D1",				"D1" },
	12, { "D2",				"D2" },
	10, { "D3"				"D3" },
	12, { "D4",				"D4" },
	24, { "D_INSIDE_TEMP",				"D5" },
	24,	{ "D_OUTSIDE_TEMP",				"D6" },
	18, { "D_WATER_TEMP",				"D7" },
	20, { "D_RESET_MIN_MAX",				"D8" },
	14, { "D9",				"D9" }
};

*/

/**************************************************************
 * Units and Scale Strings
 **************************************************************/

typedef enum {
	DEG_C = 0,
//	DEG_F,
//	DEG_K,
//	REL_HUM,
//	CUBIC_M,
//	LITRES,
//	PPM,
	LAST_UNIT_SCALE
} UnitsEnum;

static const StringArray unit_strings[LAST_UNIT_SCALE] = {
	12, {"Celsius", 			"Celsius"},
//	12, {"Fahrenheit", 			"Fahrenheit"},
//	12, {"Kelvin", 				"Kelvin"},
//	12, {"Rel Hum",				"Rel Hum"},
//	12, {"Cubic Metres",		"m3"},
//	12, {"litres", 				"litres"},
//	12, {"Parts per Million",	"Parts per Million"}
};

/**************************************************************
 * String Functions Prototypes
 **************************************************************/

uint8_t SimpleStringArrayIndex(const char* array[], const char* key);
//uint8_t SimpleStringArrayIndex(const SimpleStringArray array, const char* key);


#endif /* SRC_FF_STRINGS_H_ */
