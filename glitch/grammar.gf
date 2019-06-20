%# 
%# glitch - Grammar Lexer and Interactive Terminal Command sHell
%#
%# Copyright 2018, Brendan McLearie, all rights reserved.
%#
%#  This file: 
%#      - Cut down working version of the Fodder Factory Grammar

%header-start

#define MAX_INPUT_LINE_SIZE 150
#define MAX_OUTPUT_LINE_SIZE 150

#ifdef ARDUINO
#include <Arduino.h>
#define USE_PROGMEM
#endif

//#define DEBUG

#include "common_config.h"
#include <stdint.h>
#include <string.h>
#include <global_defs.h>
#include <registry.h>
//#include <string_consts.h>

//
// Structs used by the parser 
// 

typedef struct AST_ARRAY {
    uint8_t id;
    uint8_t type;
    const char label[MAX_AST_LABEL_SIZE];
    uint8_t action;
    uint8_t parent;
    uint8_t first_child;
    uint8_t next_sibling;
    const char action_identifier[MAX_AST_ACTION_SIZE];
} ASTA;

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
    char label[MAX_LABEL_LENGTH];
    uint16_t xlat_id;
} XLATMap;

typedef union {
    int16_t param_int16_t;
    uint16_t param_uint16_t;
    float param_float;
    char* param_char_star;
} ParamUnion;

%header-end

%code-start
%code-end

%grammar-start

%comment #
%comment //
%sub-section-close !
%sub-section-close EXIT
%sub-section-close 0x26
%redundant-close-as-comment
%ignore-case
%escape-sequence ^^^

%enum-terminating-member
%enum-plus-list-array
%enum-start-value 0
%enum-array-member-label text
%enum-array-type SimpleStringArray

%#------------------------------------ BLOCK CATEGORIES--------------------
%enum-identifier BLOCK_CATEGORY
%enum-array-instance block_cat_names
%enum-start
FF_ERROR_CAT        "ERROR_CAT"
FF_GENERIC_BLOCK    "GENERIC"
FF_SYSTEM           "SYSTEM"
FF_INTERFACE        "INTERFACE"
FF_INPUT            "INPUT"
FF_MONITOR          "MONITOR" 
FF_SCHEDULE         "SCHEDULE"
FF_RULE             "RULE" 
FF_CONTROLLER       "CONTROLLER"
FF_OUTPUT           "OUTPUT"
LAST_BLOCK_CAT
%enum-end

%#------------------------------------ BLOCK TYPES--------------------
%enum-identifier BLOCK_TYPES
%enum-array-instance block_type_strings
%enum-start
BT_ERROR                    "BT_ERROR"
SYS_SYSTEM                  "SYS_SYSTEM"
IN_ONEWIRE                  "IN_ONEWIRE"
IN_DIGITAL                  "IN_DIGITAL"
IN_VEDIRECT                 "IN_VEDIRECT"
MON_CONDITION_LOW           "MON_CONDITION_LOW"
MON_CONDITION_HIGH          "MON_CONDITION_HIGH"
MON_AVERAGE_CONDITION_LOW   "MON_AVERAGE_CONDITION_LOW"
MON_AVERAGE_CONDITION_HIGH  "MON_AVERAGE_CONDITION_HIGH"
MON_TRIGGER                 "MON_TRIGGER"
SCH_START_STOP              "SCH_START_STOP"
SCH_ONE_SHOT                "SCH_ONE_SHOT"
SCH_START_DURATION_REPEAT   "SCH_START_DURATION_REPEAT"
RL_LOGIC_ANDNOT             "RL_LOGIC_ANDNOT"
RL_LOGIC_SINGLE             "RL_LOGIC_SINGLE"
RL_LOGIC_AND                "RL_LOGIC_AND"
RL_LOGIC_SINGLENOT          "RL_LOGIC_SINGLENOT"
CON_ONOFF                   "CON_ONOFF"
CON_SYSTEM                  "CON_SYSTEM"
OUT_DIGITAL                 "OUT_DIGITAL"
OUT_SYSTEM_CALL             "OUT_SYSTEM_CALL"
LAST_BLOCK_TYPE
%enum-end

%#------------------------------------ COMMAND STRINGS --------------------
%enum-identifier COMMAND_STRING
%enum-array-instance command_strings
%enum-start
CMD_ERROR           "CMD_ERROR"   
CMD_OUTPUT_OFF      "CMD_OUTPUT_OFF"
CMD_OUTPUT_ON       "CMD_OUTPUT_ON"
CMD_RESET_MIN_MAX   "CMD_RESET_MIN_MAX"
LAST_COMMAND
%enum-end

%#------------------------------------ UNITS --------------------
%enum-identifier UNIT_STRINGS
%enum-array-instance unit_strings
%enum-start
UNIT_ERROR      "UnitTypeError"
CELSIUS         "Celsius",
FAHRENHEIT      "Fahrenheit",
KELVIN          "Kelvin",
REL_HUM         "RelativeHumidity%",
CUBIC_M         "MetresCubed",
LITRES          "Litres",
PPM             "PartsPerMillion",
ONOFF           "BinaryOnOff",
RPM             "RevolutionsPerMinute"
MILLI_VOLTS     "MilliVolts"
MILLI_AMPS      "MilliAmps"
VOLTS           "Volts"
WATTS           "Watts"
DECI_PERCENT    "DeciPercent
PERCENT         "Percent"
LAST_UNIT
%enum-end

%#------------------------------------ UNITS ABBREVIATIONS ----------
%enum-identifier UNIT_ABBR_STRINGS
%enum-array-instance unit_abbr_strings
%enum-start
ABBR_UNIT_ERROR      "UnitAbbrTypeError"
ABBR_CELSIUS         "°C",
ABBR_FAHRENHEIT      "°F",
ABBR_KELVIN          "°K",
ABBR_REL_HUM         "%",
ABBR_CUBIC_M         "m3",
ABBR_LITRES          "l",
ABBR_PPM             "PPM",
ABBR_ONOFF           "ONOFF",
ABBR_RPM             "RPM"
ABBR_MILLI_VOLTS     "mV"
ABBR_MILLI_AMPS      "mA"
ABBR_VOLTS           "V"
ABBR_WATTS           "W"
ABBR_DECI_PERCENT    "deci%
ABBR_PERCENT         "%"
LAST_UNIT_ABBR
%enum-end

%#------------------------------------ DAYS of WEEK --------------------
%enum-identifier DAY_STRINGS
%enum-array-instance day_strings
%enum-start
SUN    "SUN"
MON    "MON"
TUE    "TUE"
WED    "WED"
THU    "THU"
FRI    "FRI"
SAT    "SAT"
LAST_DAY
%enum-end

%#------------------------------------ INTERFACES --------------------
%enum-identifier INTERFACE_STRINGS
%enum-array-instance interface_strings
%enum-start
IF_ERROR            "IF_ERROR"
IF_PWM_IN           "PWM_IN"
IF_PWM_OUT          "PWM_OUT"
IF_PPM_IN           "PPM_IN"
IF_PPM_OUT          "PPM_OUT"
IF_ONEWIRE          "ONEWIRE"
IF_DIG_PIN_IN       "DIG_PIN_IN"
IF_DIG_PIN_OUT      "DIG_PIN_OUT"
IF_SYSTEM_FUNCTION  "SYSTEM_FUNCTION"
IF_VED_VOLTAGE      "IF_VED_VOLTAGE"
IF_VED_CURRENT      "IF_VED_CURRENT"
IF_VED_POWER        "IF_VED_POWER"
IF_VED_SOC          "IF_VED_SOC"
LAST_INTERFACE
%enum-end

%#------------------------------------ BLOCK STATUS --------------------
%enum-identifier STATUS_STRINGS
%enum-array-instance status_strings
%enum-start
STATUS_ERROR                    "STATUS_ERROR"
STATUS_ENABLED                  "STATUS_ENABLED"
STATUS_ENABLED_INIT             "STATUS_ENABLED_INIT"
STATUS_ENABLED_VALID_DATA       "STATUS_ENABLED_VALID_DATA"
STATUS_ENABLED_INVALID_DATA     "STATUS_ENABLED_INVALID_DATA"
STATUS_DISABLED                 "STATUS_DISABLED"
STATUS_DISABLED_INIT            "STATUS_DISABLED_INIT"
STATUS_DISABLED_ERROR           "STATUS_DISABLED_ERROR"
STATUS_DISABLED_ADMIN           "STATUS_DISABLED_ADMIN"
LAST_STATUS
%enum-end

%# -------------------------------------CONFIG KEYS---------------------
%enum-identifier SYS_CONFIG
%enum-array-instance sys_config_keys
%enum-start
SYS_CONFIG_ERROR        "SYS_CONFIG_ERROR"
SYS_CONFIG_TYPE         "type"
SYS_CONFIG_DISABLE      "disable"   
SYS_CONFIG_DISPLAY_NAME "display_name"
SYS_CONFIG_DESCRIPTION  "description"
SYS_CONFIG_LANGUAGE     "language"
SYS_CONFIG_TEMP_SCALE   "temp_scale"
SYS_CONFIG_WEEK_START   "week_start"
SYS_CONFIG_START_DELAY  "start_delay"
LAST_SYS_CONFIG
%enum-end

%enum-identifier IN_CONFIG
%enum-array-instance in_config_keys
%enum-start
IN_CONFIG_ERROR         "IN_CONFIG_ERROR"
IN_CONFIG_TYPE          "type"
IN_CONFIG_DISABLE       "disable"
IN_CONFIG_DISPLAY_NAME  "display_name"
IN_CONFIG_DESCRIPTION   "description"
IN_CONFIG_INTERFACE     "interface"
IN_CONFIG_IF_NUM        "if_num"
IN_CONFIG_LOG_RATE      "log_rate"
IN_CONFIG_POLL_RATE     "poll_rate"
IN_CONFIG_DATA_UNITS    "data_units"
IN_CONFIG_DATA_TYPE     "data_type"
LAST_IN_CONFIG
%enum-end

%enum-identifier MON_CONFIG
%enum-array-instance mon_config_keys
%enum-start
MON_CONFIG_ERROR            "MON_CONFIG_ERROR"
MON_CONFIG_TYPE             "type"
MON_CONFIG_DISABLE          "disable"
MON_CONFIG_DISPLAY_NAME     "display_name
MON_CONFIG_DESCRIPTION      "description"
MON_CONFIG_INPUT1           "input1
MON_CONFIG_INPUT2           "input2
MON_CONFIG_INPUT3           "input3
MON_CONFIG_INPUT4           "input4
MON_CONFIG_ACT_VAL          "act_val"
MON_CONFIG_DEACT_VAL        "deact_val"
LAST_MON_CONFIG
%enum-end

%enum-identifier SCH_CONFIG
%enum-array-instance sch_config_keys
%enum-start
SCH_CONFIG_ERROR            "SCH_CONFIG_ERROR"
SCH_CONFIG_TYPE             "type"
SCH_CONFIG_DISABLE          "disable"
SCH_CONFIG_DISPLAY_NAME     "display_name
SCH_CONFIG_DESCRIPTION      "description"
SCH_CONFIG_DAYS             "days"
SCH_CONFIG_TIME_START       "time_start"
SCH_CONFIG_TIME_END         "time_end"
SCH_CONFIG_TIME_DURATION    "time_duration"
SCH_CONFIG_TIME_REPEAT      "time_repeat"
LAST_SCH_CONFIG
%enum-end

%enum-identifier RL_CONFIG
%enum-array-instance rl_config_keys
%enum-start
RL_CONFIG_ERROR            "RL_CONFIG_ERROR"
RL_CONFIG_TYPE             "type"
RL_CONFIG_DISABLE          "disable"
RL_CONFIG_DISPLAY_NAME     "display_name
RL_CONFIG_DESCRIPTION      "description"
RL_CONFIG_PARAM1            "param1"
RL_CONFIG_PARAM2            "param2"
RL_CONFIG_PARAM3            "param3"
RL_CONFIG_PARAM_NOT         "param_not"
LAST_RL_CONFIG
%enum-end

%enum-identifier CON_CONFIG
%enum-array-instance con_config_keys
%enum-start
CON_CONFIG_ERROR            "CON_CONFIG_ERROR"
CON_CONFIG_TYPE             "type"
CON_CONFIG_DISABLE          "disable"
CON_CONFIG_DISPLAY_NAME     "display_name
CON_CONFIG_DESCRIPTION      "description"
CON_CONFIG_RULE             "rule"
CON_CONFIG_OUTPUT           "output"
CON_CONFIG_ACT_CMD          "act_cmd"
CON_CONFIG_DEACT_CMD        "deact_cmd"
LAST_CON_CONFIG
%enum-end

%enum-identifier OUT_CONFIG
%enum-array-instance out_config_keys
%enum-start
OUT_CONFIG_ERROR            "OUT_CONFIG_ERROR"
OUT_CONFIG_TYPE             "type"
OUT_CONFIG_DISABLE          "disable"
OUT_CONFIG_DISPLAY_NAME     "display_name
OUT_CONFIG_DESCRIPTION      "description"
OUT_CONFIG_INTERFACE        "interface"
OUT_CONFIG_IF_NUM           "if_num"
LAST_OUT_CONFIG
%enum-end

%# ------------------------------------------------------------------

%lookup-list BLOCK_LABEL LookupBlockLabel
%#lookup-list some_other_lookup_list LookupOtherList

%action-define SHOW_BLOCKS ShowBlocks
%action-define SHOW_SYSTEM ShowSystem
%action-define SHOW_BLOCK_LABEL ShowBlockByLabel
%action-define SHOW_BLOCK_ID ShowBlockByID

%1 keyword SHOW
%2 keyword BLOCKS 
%action SHOW_BLOCKS
%2 keyword SYSTEM 
%action SHOW_SYSTEM
%2 lookup BLOCK_LABEL
%action SHOW_BLOCK_LABEL
%2 param-integer 
%action SHOW_BLOCK_ID

%action-define COMMAND_BLOCK_LABEL SendCommandToBlockLabel
%action-define COMMAND_BLOCK_ID SendCommandToBlockID

%#action-define MESSAGE_DATA_LABEL_INT SimIntDataMessageFromBlockLabel
%#action-define MESSAGE_DATA_LABEL_FLOAT SimFloatDataMessageFromBlockLabel

%#action-define MESSAGE_DATA_BLOCK_ID_INT SimIntDataMessageFromBlockID
%#action-define MESSAGE_DATA_BLOCK_ID_FLOAT SimFloatDataMessageFromBlockID

%1 keyword MESSAGE
%2 keyword COMMAND
%3 lookup BLOCK_LABEL
%4 enum-array COMMAND_STRING
%action COMMAND_BLOCK_LABEL
%3 param-integer
%4 enum-array COMMAND_STRING
%action COMMAND_BLOCK_ID

%#2 keyword DATA

%#3 lookup block_label
%#4 keyword INT
%#5 param-integer
%#action MESSAGE_DATA_LABEL_INT
%#4 keyword FLOAT
%#5 param-float
%#action MESSAGE_DATA_LABEL_FLOAT

%#3 param-integer
%#4 keyword INT
%#5 param-integer
%#action MESSAGE_DATA_BLOCK_ID_INT
%#4 keyword FLOAT
%#5 param-float
%#action MESSAGE_DATA_BLOCK_ID_FLOAT

%action-define SHOW_TIME ShowTime
%action-define SET_TIME SetTime
%action-define SHOW_DATE ShowDate
%action-define SET_DATE SetDate

%1 keyword TIME
%action SHOW_TIME
%2 keyword SET
%3 param-time
%action SET_TIME
%1 keyword DATE
%action SHOW_DATE
%2 keyword SET
%3 param-date
%action SET_DATE

%action-define CONFIG_CLEAR ConfigClear
%action-define CONFIG_LOAD ConfigLoad
%action-define CONFIG_SAVE ConfigSave

%#action-define CONFIG_LOAD_BINARY ConfigLoadBinary
%#action-define CONFIG_LOAD_INI ConfigLoadINI
%#action-define CONFIG_SAVE_BINARY ConfigSaveBinary

%action-define CONFIG_BLOCK_SYSTEM ConfigBlockSystem
%action-define CONFIG_BLOCK_INPUT ConfigBlockInput 
%action-define CONFIG_BLOCK_MONITOR ConfigBlockMonitor
%action-define CONFIG_BLOCK_SCHEDULE ConfigBlockSchedule
%action-define CONFIG_BLOCK_RULE ConfigBlockRule
%action-define CONFIG_BLOCK_CONTROLLER ConfigBlockController
%action-define CONFIG_BLOCK_OUTPUT ConfigBlockOutput

%# ----Config Commands---------------------------------------------------
 
%1 keyword CONFIG
%2 keyword CLEAR
%action CONFIG_CLEAR
%2 keyword LOAD
%action CONFIG_LOAD
%2 keyword SAVE
%action CONFIG_SAVE

%2 keyword system
%3 param-string
%4 enum-array SYS_CONFIG
%5 param-string
%action CONFIG_BLOCK_SYSTEM

%2 keyword input
%3 param-string
%4 enum-array IN_CONFIG
%5 param-string
%action CONFIG_BLOCK_INPUT

%2 keyword monitor
%3 param-string
%4 enum-array MON_CONFIG
%5 param-string
%action CONFIG_BLOCK_MONITOR

%2 keyword schedule
%3 param-string
%4 enum-array SCH_CONFIG
%5 param-string
%action CONFIG_BLOCK_SCHEDULE

%2 keyword rule
%3 param-string
%4 enum-array RL_CONFIG
%5 param-string
%action CONFIG_BLOCK_RULE

%2 keyword controller
%3 param-string
%4 enum-array CON_CONFIG
%5 param-string
%action CONFIG_BLOCK_CONTROLLER

%2 keyword output
%3 param-string
%4 enum-array OUT_CONFIG
%5 param-string
%action CONFIG_BLOCK_OUTPUT

%# ---------------------------FUTURE WRITE / FILE------------------------------

%#action-define WRITE_DEFAULT WriteToDefaultConfigFile
%#action-define WRITE_INIT WriteEmptyDefaultConfigFile
%#action-define WRITE_FILENAME WriteToFilenameConfigFile
%#action-define COPY_FILE_FILE CopyConfigFileToFile

%# WRITE                 // writes running config to default file "config.txt"
%# WRITE INIT            // write empty init config to the default config "config.txt"
%# WRITE filename        // write running config to a specific filename
%# COPY filename filename  // copy contents of one config file to another

%#1 keyword WRITE
%#action WRITE_DEFAULT
%#2 keyword INIT
%#action WRITE_INIT
%#2 param-string
%#action WRITE_FILENAME
%#1 keyword COPY
%#2 param-string
%#3 param-string
%#action COPY_FILE_FILE

%# --------------------------- INIT------------------------------

%action-define INIT_SETUP_ALL InitSetupAll
%action-define INIT_VALIDATE_ALL InitValidateAll
%action-define INIT_DISABLE_ALL InitDisableAll

%action-define INIT_SETUP_BID InitSetupBID
%action-define INIT_VALIDATE_BID InitValidateBID
%action-define INIT_DISABLE_BID InitDisableBID

%1 keyword INIT

%2 keyword SETUP
%3 keyword ALL
%action INIT_SETUP_ALL
%3 param-integer
%action INIT_SETUP_BID

%2 keyword VALIDATE
%3 keyword ALL
%action INIT_VALIDATE_ALL
%3 param-integer
%action INIT_VALIDATE_BID

%2 keyword DISABLE
%3 keyword ALL
%action INIT_DISABLE_ALL
%3 param-integer
%action INIT_DISABLE_BID

%# --------------------------- ADMIN------------------------------

%action-define ADMIN_DISABLE_BID AdminDisableBID
%action-define ADMIN_ENABLE_BID AdminEnableBID
%action-define ADMIN_DELETE_BID AdminDeleteBID
%action-define ADMIN_CMD_ON_BID AdminCmdOnBID
%action-define ADMIN_CMD_OFF_BID AdminCmdOffBID

%1 keyword ADMIN

%2 keyword DISABLE
%3 param-integer
%action ADMIN_DISABLE_BID

%2 keyword ENABLE
%3 param-integer
%action ADMIN_ENABLE_BID

%2 keyword DELETE
%3 param-integer
%action ADMIN_DELETE_BID

%2 keyword ON
%3 param-integer
%action ADMIN_CMD_ON_BID
%2 keyword OFF
%3 param-integer
%action ADMIN_CMD_OFF_BID


%action-define REBOOT SystemReboot

%1 keyword REBOOT
%action REBOOT



%grammar-end


