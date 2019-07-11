//
// DO NOT MODIFY THIS FILE
//
// THIS FILE IS AUTOMATICALLY GENERATED IN ITS ENTIRETY
// ANY CHANGES MADE IN THIS FILE WILL BE OVERWRITTEN WITHOUT WARNING
// WHENEVER GLITCH IS INVOKED.
//

#ifndef OUT_H_
#define OUT_H_

#include <itch_config.h>
#include <stdint.h>
#include <string.h>
#include <common_config.h>

#define MAX_AST_IDENTIFIER_SIZE 28
#define MAX_AST_LABEL_SIZE 19
#define MAX_AST_ACTION_SIZE 25
#define AST_NODE_COUNT 72
#define MAX_PARAM_COUNT 3

#define MAX_GRAMMAR_DEPTH 5

#define MAX_XLAT_LABEL_SIZE 28
#define XLAT_IDENT_MAP_COUNT 15
#define XLAT_LOOKUP_MAP_COUNT 1
#define XLAT_FUNC_MAP_COUNT 33
#if defined(PLATFORM_LINUX)
#elif defined(PLATFORM_ARDUINO)
#include <Arduino.h>
#else
#error "A PLATFORM_ Directive must be provided"
#endif
// Structs and String Arrays used by the parser

typedef struct SIMPLE_STRING_ARRAY_TYPE {
    // Array of strings indexed by related enum
    char text[MAX_AST_IDENTIFIER_SIZE];
} SimpleStringArray;

typedef struct AST_NODE {
    uint8_t id;
    uint8_t type;
    char label[MAX_AST_LABEL_SIZE];
    uint8_t action;
    uint8_t parent;
    uint8_t first_child;
    uint8_t next_sibling;
    char action_identifier[MAX_AST_ACTION_SIZE];
} ASTA_Node;

typedef struct XLAT_MAP {
    char label[MAX_XLAT_LABEL_SIZE];
    uint16_t xlat_id;
} XLATMap;

typedef union {
    int16_t param_int16_t;
    uint16_t param_uint16_t;
    float param_float;
    char* param_char_star;
} ParamUnion;

// %header-start content begin

//#include <common_config.h>
//#include <stdint.h>
//#include <string.h>
#include <registry.h>

#define MAX_INPUT_LINE_SIZE 150
#define MAX_OUTPUT_LINE_SIZE 150

// %header-end content end

#define ITCH_TERM_ESC_SEQ "^^^"
#define ITCH_TERM_ESC_SEQ_SIZE 3

#define ITCH_CCC_ESC_SEQ "%%%"
#define ITCH_CCC_ESC_SEQ_SIZE 3

enum {
	FF_ERROR_CAT = 0,
	FF_GENERIC_BLOCK,
	FF_SYSTEM,
	FF_INTERFACE,
	FF_INPUT,
	FF_MONITOR,
	FF_SCHEDULE,
	FF_RULE,
	FF_CONTROLLER,
	FF_OUTPUT,
	LAST_BLOCK_CAT,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray block_cat_names [LAST_BLOCK_CAT] PROGMEM = {
#else
const SimpleStringArray block_cat_names [LAST_BLOCK_CAT] = {
#endif //PLATFORM_ARDUINO
	"ERROR_CAT",
	"GENERIC",
	"SYSTEM",
	"INTERFACE",
	"INPUT",
	"MONITOR",
	"SCHEDULE",
	"RULE",
	"CONTROLLER",
	"OUTPUT",
};

enum {
	BT_ERROR = 0,
	SYS_SYSTEM,
	IN_ONEWIRE,
	IN_DIGITAL,
	IN_VEDIRECT,
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
	LAST_BLOCK_TYPE,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray block_type_strings [LAST_BLOCK_TYPE] PROGMEM = {
#else
const SimpleStringArray block_type_strings [LAST_BLOCK_TYPE] = {
#endif //PLATFORM_ARDUINO
	"BT_ERROR",
	"SYS_SYSTEM",
	"IN_ONEWIRE",
	"IN_DIGITAL",
	"IN_VEDIRECT",
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

enum {
	CMD_ERROR = 0,
	CMD_OUTPUT_OFF,
	CMD_OUTPUT_ON,
	CMD_RESET_MIN_MAX,
	LAST_COMMAND,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray command_strings [LAST_COMMAND] PROGMEM = {
#else
const SimpleStringArray command_strings [LAST_COMMAND] = {
#endif //PLATFORM_ARDUINO
	"CMD_ERROR",
	"CMD_OUTPUT_OFF",
	"CMD_OUTPUT_ON",
	"CMD_RESET_MIN_MAX",
};

enum {
	UNIT_ERROR = 0,
	CELSIUS,
	FAHRENHEIT,
	KELVIN,
	REL_HUM,
	CUBIC_M,
	LITRES,
	PPM,
	ONOFF,
	RPM,
	MILLI_VOLTS,
	MILLI_AMPS,
	VOLTS,
	WATTS,
	DECI_PERCENT,
	PERCENT,
	LAST_UNIT,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray unit_strings [LAST_UNIT] PROGMEM = {
#else
const SimpleStringArray unit_strings [LAST_UNIT] = {
#endif //PLATFORM_ARDUINO
	"UnitTypeError",
	"Celsius",
	"Fahrenheit",
	"Kelvin",
	"RelativeHumidity%",
	"MetresCubed",
	"Litres",
	"PartsPerMillion",
	"BinaryOnOff",
	"RevolutionsPerMinute",
	"MilliVolts",
	"MilliAmps",
	"Volts",
	"Watts",
	"DeciPercent",
	"Percent",
};

enum {
	ABBR_UNIT_ERROR = 0,
	ABBR_CELSIUS,
	ABBR_FAHRENHEIT,
	ABBR_KELVIN,
	ABBR_REL_HUM,
	ABBR_CUBIC_M,
	ABBR_LITRES,
	ABBR_PPM,
	ABBR_ONOFF,
	ABBR_RPM,
	ABBR_MILLI_VOLTS,
	ABBR_MILLI_AMPS,
	ABBR_VOLTS,
	ABBR_WATTS,
	ABBR_DECI_PERCENT,
	ABBR_PERCENT,
	LAST_UNIT_ABBR,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray unit_abbr_strings [LAST_UNIT_ABBR] PROGMEM = {
#else
const SimpleStringArray unit_abbr_strings [LAST_UNIT_ABBR] = {
#endif //PLATFORM_ARDUINO
	"UnitAbbrTypeError",
	"°C",
	"°F",
	"°K",
	"%",
	"m3",
	"l",
	"PPM",
	"ONOFF",
	"RPM",
	"mV",
	"mA",
	"V",
	"W",
	"deci%",
	"%",
};

enum {
	SUN = 0,
	MON,
	TUE,
	WED,
	THU,
	FRI,
	SAT,
	LAST_DAY,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray day_strings [LAST_DAY] PROGMEM = {
#else
const SimpleStringArray day_strings [LAST_DAY] = {
#endif //PLATFORM_ARDUINO
	"SUN",
	"MON",
	"TUE",
	"WED",
	"THU",
	"FRI",
	"SAT",
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
	IF_VED_VOLTAGE,
	IF_VED_CURRENT,
	IF_VED_POWER,
	IF_VED_SOC,
	LAST_INTERFACE,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray interface_strings [LAST_INTERFACE] PROGMEM = {
#else
const SimpleStringArray interface_strings [LAST_INTERFACE] = {
#endif //PLATFORM_ARDUINO
	"IF_ERROR",
	"PWM_IN",
	"PWM_OUT",
	"PPM_IN",
	"PPM_OUT",
	"ONEWIRE",
	"DIG_PIN_IN",
	"DIG_PIN_OUT",
	"SYSTEM_FUNCTION",
	"IF_VED_VOLTAGE",
	"IF_VED_CURRENT",
	"IF_VED_POWER",
	"IF_VED_SOC",
};

enum {
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

#ifdef PLATFORM_ARDUINO
const SimpleStringArray status_strings [LAST_STATUS] PROGMEM = {
#else
const SimpleStringArray status_strings [LAST_STATUS] = {
#endif //PLATFORM_ARDUINO
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

enum {
	SYS_CONFIG_ERROR = 0,
	SYS_CONFIG_TYPE,
	SYS_CONFIG_DISABLE,
	SYS_CONFIG_DISPLAY_NAME,
	SYS_CONFIG_DESCRIPTION,
	SYS_CONFIG_LANGUAGE,
	SYS_CONFIG_TEMP_SCALE,
	SYS_CONFIG_WEEK_START,
	SYS_CONFIG_START_DELAY,
	LAST_SYS_CONFIG,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray sys_config_keys [LAST_SYS_CONFIG] PROGMEM = {
#else
const SimpleStringArray sys_config_keys [LAST_SYS_CONFIG] = {
#endif //PLATFORM_ARDUINO
	"SYS_CONFIG_ERROR",
	"type",
	"disable",
	"display_name",
	"description",
	"language",
	"temp_scale",
	"week_start",
	"start_delay",
};

enum {
	IN_CONFIG_ERROR = 0,
	IN_CONFIG_TYPE,
	IN_CONFIG_DISABLE,
	IN_CONFIG_DISPLAY_NAME,
	IN_CONFIG_DESCRIPTION,
	IN_CONFIG_INTERFACE,
	IN_CONFIG_IF_NUM,
	IN_CONFIG_LOG_RATE,
	IN_CONFIG_POLL_RATE,
	IN_CONFIG_DATA_UNITS,
	IN_CONFIG_DATA_TYPE,
	LAST_IN_CONFIG,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray in_config_keys [LAST_IN_CONFIG] PROGMEM = {
#else
const SimpleStringArray in_config_keys [LAST_IN_CONFIG] = {
#endif //PLATFORM_ARDUINO
	"IN_CONFIG_ERROR",
	"type",
	"disable",
	"display_name",
	"description",
	"interface",
	"if_num",
	"log_rate",
	"poll_rate",
	"data_units",
	"data_type",
};

enum {
	MON_CONFIG_ERROR = 0,
	MON_CONFIG_TYPE,
	MON_CONFIG_DISABLE,
	MON_CONFIG_DISPLAY_NAME,
	MON_CONFIG_DESCRIPTION,
	MON_CONFIG_INPUT1,
	MON_CONFIG_INPUT2,
	MON_CONFIG_INPUT3,
	MON_CONFIG_INPUT4,
	MON_CONFIG_ACT_VAL,
	MON_CONFIG_DEACT_VAL,
	LAST_MON_CONFIG,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray mon_config_keys [LAST_MON_CONFIG] PROGMEM = {
#else
const SimpleStringArray mon_config_keys [LAST_MON_CONFIG] = {
#endif //PLATFORM_ARDUINO
	"MON_CONFIG_ERROR",
	"type",
	"disable",
	"display_name",
	"description",
	"input1",
	"input2",
	"input3",
	"input4",
	"act_val",
	"deact_val",
};

enum {
	SCH_CONFIG_ERROR = 0,
	SCH_CONFIG_TYPE,
	SCH_CONFIG_DISABLE,
	SCH_CONFIG_DISPLAY_NAME,
	SCH_CONFIG_DESCRIPTION,
	SCH_CONFIG_DAYS,
	SCH_CONFIG_TIME_START,
	SCH_CONFIG_TIME_END,
	SCH_CONFIG_TIME_DURATION,
	SCH_CONFIG_TIME_REPEAT,
	LAST_SCH_CONFIG,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray sch_config_keys [LAST_SCH_CONFIG] PROGMEM = {
#else
const SimpleStringArray sch_config_keys [LAST_SCH_CONFIG] = {
#endif //PLATFORM_ARDUINO
	"SCH_CONFIG_ERROR",
	"type",
	"disable",
	"display_name",
	"description",
	"days",
	"time_start",
	"time_end",
	"time_duration",
	"time_repeat",
};

enum {
	RL_CONFIG_ERROR = 0,
	RL_CONFIG_TYPE,
	RL_CONFIG_DISABLE,
	RL_CONFIG_DISPLAY_NAME,
	RL_CONFIG_DESCRIPTION,
	RL_CONFIG_PARAM1,
	RL_CONFIG_PARAM2,
	RL_CONFIG_PARAM3,
	RL_CONFIG_PARAM_NOT,
	LAST_RL_CONFIG,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray rl_config_keys [LAST_RL_CONFIG] PROGMEM = {
#else
const SimpleStringArray rl_config_keys [LAST_RL_CONFIG] = {
#endif //PLATFORM_ARDUINO
	"RL_CONFIG_ERROR",
	"type",
	"disable",
	"display_name",
	"description",
	"param1",
	"param2",
	"param3",
	"param_not",
};

enum {
	CON_CONFIG_ERROR = 0,
	CON_CONFIG_TYPE,
	CON_CONFIG_DISABLE,
	CON_CONFIG_DISPLAY_NAME,
	CON_CONFIG_DESCRIPTION,
	CON_CONFIG_RULE,
	CON_CONFIG_OUTPUT,
	CON_CONFIG_ACT_CMD,
	CON_CONFIG_DEACT_CMD,
	LAST_CON_CONFIG,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray con_config_keys [LAST_CON_CONFIG] PROGMEM = {
#else
const SimpleStringArray con_config_keys [LAST_CON_CONFIG] = {
#endif //PLATFORM_ARDUINO
	"CON_CONFIG_ERROR",
	"type",
	"disable",
	"display_name",
	"description",
	"rule",
	"output",
	"act_cmd",
	"deact_cmd",
};

enum {
	OUT_CONFIG_ERROR = 0,
	OUT_CONFIG_TYPE,
	OUT_CONFIG_DISABLE,
	OUT_CONFIG_DISPLAY_NAME,
	OUT_CONFIG_DESCRIPTION,
	OUT_CONFIG_INTERFACE,
	OUT_CONFIG_IF_NUM,
	LAST_OUT_CONFIG,
};

#ifdef PLATFORM_ARDUINO
const SimpleStringArray out_config_keys [LAST_OUT_CONFIG] PROGMEM = {
#else
const SimpleStringArray out_config_keys [LAST_OUT_CONFIG] = {
#endif //PLATFORM_ARDUINO
	"OUT_CONFIG_ERROR",
	"type",
	"disable",
	"display_name",
	"description",
	"interface",
	"if_num",
};

void ShowBlocks(void);
void ShowSystem(void);
void ShowBlockByLabel(char* BLOCK_LABEL);
void ShowBlockByID(int16_t param1_int);
void SetCommandOnBlockLabel(char* BLOCK_LABEL, uint16_t COMMAND_STRING);
void SetCommandOnBlockID(int16_t param1_int, uint16_t COMMAND_STRING);
void ShowTime(void);
void SetTime(char* param1_time);
void ShowDate(void);
void SetDate(char* param1_date);
void ConfigClear(void);
void ConfigLoad(void);
void ConfigSave(void);
void ConfigBlockSystem(char* param1_string, uint16_t SYS_CONFIG, char* param2_string);
void ConfigBlockInput(char* param1_string, uint16_t IN_CONFIG, char* param2_string);
void ConfigBlockMonitor(char* param1_string, uint16_t MON_CONFIG, char* param2_string);
void ConfigBlockSchedule(char* param1_string, uint16_t SCH_CONFIG, char* param2_string);
void ConfigBlockRule(char* param1_string, uint16_t RL_CONFIG, char* param2_string);
void ConfigBlockController(char* param1_string, uint16_t CON_CONFIG, char* param2_string);
void ConfigBlockOutput(char* param1_string, uint16_t OUT_CONFIG, char* param2_string);
void InitSetupAll(void);
void InitSetupBID(int16_t param1_int);
void InitValidateAll(void);
void InitValidateBID(int16_t param1_int);
void InitDisableAll(void);
void InitDisableBID(int16_t param1_int);
void AdminDisableBID(int16_t param1_int);
void AdminEnableBID(int16_t param1_int);
void AdminDeleteBID(int16_t param1_int);
void AdminDeleteBlockLabel(char* BLOCK_LABEL);
void AdminCmdOnBID(int16_t param1_int);
void AdminCmdOffBID(int16_t param1_int);
void SystemReboot(void);

// id, type, label, actionable, parent, first_child, next_sibling, action_id
#ifdef PLATFORM_ARDUINO
const ASTA_Node asta [72] PROGMEM = {
#else
const ASTA_Node asta [72] = {
#endif //PLATFORM_ARDUINO
	1, 1, "SHOW", 0, 0, 2, 6, "",
	2, 1, "BLOCKS", 1, 1, 0, 3, "SHOW_BLOCKS",
	3, 1, "SYSTEM", 1, 1, 0, 4, "SHOW_SYSTEM",
	4, 3, "BLOCK_LABEL", 1, 1, 0, 5, "SHOW_BLOCK_LABEL",
	5, 6, "param-integer", 1, 1, 0, 0, "SHOW_BLOCK_ID",
	6, 1, "MESSAGE", 0, 0, 7, 12, "",
	7, 1, "COMMAND", 0, 6, 8, 0, "",
	8, 3, "BLOCK_LABEL", 0, 7, 9, 10, "",
	9, 2, "command_strings", 1, 8, 0, 0, "SET_COMMAND_BLOCK_LABEL",
	10, 6, "param-integer", 0, 7, 11, 0, "",
	11, 2, "command_strings", 1, 10, 0, 0, "SET_COMMAND_BLOCK_ID",
	12, 1, "TIME", 1, 0, 13, 15, "SHOW_TIME",
	13, 1, "SET", 0, 12, 14, 0, "",
	14, 5, "param-time", 1, 13, 0, 0, "SET_TIME",
	15, 1, "DATE", 1, 0, 16, 18, "SHOW_DATE",
	16, 1, "SET", 0, 15, 17, 0, "",
	17, 4, "param-date", 1, 16, 0, 0, "SET_DATE",
	18, 1, "CONFIG", 0, 0, 19, 50, "",
	19, 1, "CLEAR", 1, 18, 0, 20, "CONFIG_CLEAR",
	20, 1, "LOAD", 1, 18, 0, 21, "CONFIG_LOAD",
	21, 1, "SAVE", 1, 18, 0, 22, "CONFIG_SAVE",
	22, 1, "system", 0, 18, 23, 26, "",
	23, 13, "param-string", 0, 22, 24, 0, "",
	24, 2, "sys_config_keys", 0, 23, 25, 0, "",
	25, 13, "param-string", 1, 24, 0, 0, "CONFIG_BLOCK_SYSTEM",
	26, 1, "input", 0, 18, 27, 30, "",
	27, 13, "param-string", 0, 26, 28, 0, "",
	28, 2, "in_config_keys", 0, 27, 29, 0, "",
	29, 13, "param-string", 1, 28, 0, 0, "CONFIG_BLOCK_INPUT",
	30, 1, "monitor", 0, 18, 31, 34, "",
	31, 13, "param-string", 0, 30, 32, 0, "",
	32, 2, "mon_config_keys", 0, 31, 33, 0, "",
	33, 13, "param-string", 1, 32, 0, 0, "CONFIG_BLOCK_MONITOR",
	34, 1, "schedule", 0, 18, 35, 38, "",
	35, 13, "param-string", 0, 34, 36, 0, "",
	36, 2, "sch_config_keys", 0, 35, 37, 0, "",
	37, 13, "param-string", 1, 36, 0, 0, "CONFIG_BLOCK_SCHEDULE",
	38, 1, "rule", 0, 18, 39, 42, "",
	39, 13, "param-string", 0, 38, 40, 0, "",
	40, 2, "rl_config_keys", 0, 39, 41, 0, "",
	41, 13, "param-string", 1, 40, 0, 0, "CONFIG_BLOCK_RULE",
	42, 1, "controller", 0, 18, 43, 46, "",
	43, 13, "param-string", 0, 42, 44, 0, "",
	44, 2, "con_config_keys", 0, 43, 45, 0, "",
	45, 13, "param-string", 1, 44, 0, 0, "CONFIG_BLOCK_CONTROLLER",
	46, 1, "output", 0, 18, 47, 0, "",
	47, 13, "param-string", 0, 46, 48, 0, "",
	48, 2, "out_config_keys", 0, 47, 49, 0, "",
	49, 13, "param-string", 1, 48, 0, 0, "CONFIG_BLOCK_OUTPUT",
	50, 1, "INIT", 0, 0, 51, 60, "",
	51, 1, "SETUP", 0, 50, 52, 54, "",
	52, 1, "ALL", 1, 51, 0, 53, "INIT_SETUP_ALL",
	53, 6, "param-integer", 1, 51, 0, 0, "INIT_SETUP_BID",
	54, 1, "VALIDATE", 0, 50, 55, 57, "",
	55, 1, "ALL", 1, 54, 0, 56, "INIT_VALIDATE_ALL",
	56, 6, "param-integer", 1, 54, 0, 0, "INIT_VALIDATE_BID",
	57, 1, "DISABLE", 0, 50, 58, 0, "",
	58, 1, "ALL", 1, 57, 0, 59, "INIT_DISABLE_ALL",
	59, 6, "param-integer", 1, 57, 0, 0, "INIT_DISABLE_BID",
	60, 1, "ADMIN", 0, 0, 61, 72, "",
	61, 1, "DISABLE", 0, 60, 62, 63, "",
	62, 6, "param-integer", 1, 61, 0, 0, "ADMIN_DISABLE_BID",
	63, 1, "ENABLE", 0, 60, 64, 65, "",
	64, 6, "param-integer", 1, 63, 0, 0, "ADMIN_ENABLE_BID",
	65, 1, "DELETE", 0, 60, 66, 68, "",
	66, 6, "param-integer", 1, 65, 0, 67, "ADMIN_DELETE_BID",
	67, 3, "BLOCK_LABEL", 1, 65, 0, 0, "ADMIN_DELETE_BLOCK_LABEL",
	68, 1, "ON", 0, 60, 69, 70, "",
	69, 6, "param-integer", 1, 68, 0, 0, "ADMIN_CMD_ON_BID",
	70, 1, "OFF", 0, 60, 71, 0, "",
	71, 6, "param-integer", 1, 70, 0, 0, "ADMIN_CMD_OFF_BID",
	72, 1, "REBOOT", 1, 0, 0, 0, "REBOOT",
};

#ifdef PLATFORM_ARDUINO
const XLATMap ident_map [15] PROGMEM = {
#else
const XLATMap ident_map [15] = {
#endif //PLATFORM_ARDUINO
	"block_cat_names", 0,
	"block_type_strings", 1,
	"command_strings", 2,
	"unit_strings", 3,
	"unit_abbr_strings", 4,
	"day_strings", 5,
	"interface_strings", 6,
	"status_strings", 7,
	"sys_config_keys", 8,
	"in_config_keys", 9,
	"mon_config_keys", 10,
	"sch_config_keys", 11,
	"rl_config_keys", 12,
	"con_config_keys", 13,
	"out_config_keys", 14,
};

#ifdef PLATFORM_ARDUINO
const XLATMap lookup_map [1] PROGMEM = {
#else
const XLATMap lookup_map [1] = {
#endif //PLATFORM_ARDUINO
	"BLOCK_LABEL", 0,
};

#ifdef PLATFORM_ARDUINO
const XLATMap func_map [33] PROGMEM = {
#else
const XLATMap func_map [33] = {
#endif //PLATFORM_ARDUINO
	"SHOW_BLOCKS", 0,
	"SHOW_SYSTEM", 1,
	"SHOW_BLOCK_LABEL", 2,
	"SHOW_BLOCK_ID", 3,
	"SET_COMMAND_BLOCK_LABEL", 4,
	"SET_COMMAND_BLOCK_ID", 5,
	"SHOW_TIME", 6,
	"SET_TIME", 7,
	"SHOW_DATE", 8,
	"SET_DATE", 9,
	"CONFIG_CLEAR", 10,
	"CONFIG_LOAD", 11,
	"CONFIG_SAVE", 12,
	"CONFIG_BLOCK_SYSTEM", 13,
	"CONFIG_BLOCK_INPUT", 14,
	"CONFIG_BLOCK_MONITOR", 15,
	"CONFIG_BLOCK_SCHEDULE", 16,
	"CONFIG_BLOCK_RULE", 17,
	"CONFIG_BLOCK_CONTROLLER", 18,
	"CONFIG_BLOCK_OUTPUT", 19,
	"INIT_SETUP_ALL", 20,
	"INIT_VALIDATE_ALL", 21,
	"INIT_DISABLE_ALL", 22,
	"INIT_SETUP_BID", 23,
	"INIT_VALIDATE_BID", 24,
	"INIT_DISABLE_BID", 25,
	"ADMIN_DISABLE_BID", 26,
	"ADMIN_ENABLE_BID", 27,
	"ADMIN_DELETE_BID", 28,
	"ADMIN_DELETE_BLOCK_LABEL", 29,
	"ADMIN_CMD_ON_BID", 30,
	"ADMIN_CMD_OFF_BID", 31,
	"REBOOT", 32,
};

uint16_t LookupIdentMap (char* key);
uint16_t LookupLookupMap (char* key);
uint16_t LookupFuncMap (char* key);
uint16_t LookupIdentifierMembers(uint16_t ident_xlat, char* lookup_string, uint8_t str_len);
uint8_t LookupLookupMembers(uint16_t ident_xlat, char* lookup_string, uint8_t str_len);
uint8_t LookupBlockLabel(char* lookup_string);
uint16_t CallFunction(uint8_t func_xlat, ParamUnion params[]);

#endif // OUT_H_

