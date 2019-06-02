%# 
%# glitch - Grammar Lexer and Interactive Terminal Command sHell
%#
%# Copyright 2018, Brendan McLearie, all rights reserved.
%#
%#  This file: 
%#      - Cut down working version of the Fodder Factory Grammar

%header-start

#define MAX_INPUT_LINE_SIZE 80
#define MAX_OUTPUT_LINE_SIZE 150

#define MAX_BUFFER_SIZE MAX_OUTPUT_LINE_SIZE
#define MAX_BUFFER_LENGTH MAX_OUTPUT_LINE_SIZE

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
    uint16_t id;
    int type;
    char label[MAX_AST_LABEL_SIZE];
    bool action;
    uint16_t parent;
    uint16_t first_child;
    uint16_t next_sibling;
    char action_identifier[MAX_AST_ACTION_SIZE];
} ASTA;

typedef struct XLAT_MAP {
	char label[MAX_IDENTIFIER_LABEL_SIZE];
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

%enum-identifier BLOCK_CATEGORY
%enum-array-instance block_cat_names
%enum-start
FF_ERROR_CAT        "ERROR_CAT"
FF_GENERIC_BLOCK    "GENERIC"
FF_SYSTEM           "SYSTEM"
FF_INPUT            "INPUT"
FF_MONITOR          "MONITOR" 
FF_SCHEDULE         "SCHEDULE"
FF_RULE             "RULE" 
FF_CONTROLLER       "CONROLLER"
FF_OUTPUT           "OUTPUT"
LAST_BLOCK_CAT
%enum-end

%enum-identifier COMMAND_STRING
%enum-array-instance command_strings
%enum-start
CMD_ERROR           "CMD_ERROR"   
CMD_OUTPUT_OFF      "CMD_OUTPUT_OFF"
CMD_OUTPUT_ON       "CMD_OUTPUT_ON"
CMD_RESET_MIN_MAX   "CMD_RESET_MIN_MAX"
LAST_COMMAND
%enum-end

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

%action-define CONFIG_RESET ConfigReset
%action-define CONFIG_LOAD ConfigLoad
%action-define CONFIG_LOAD_BINARY ConfigLoadBinary
%action-define CONFIG_LOAD_INI ConfigLoadINI
%action-define CONFIG_SAVE ConfigSave
%action-define CONFIG_SAVE_BINARY ConfigSaveBinary

%action-define CONFIG_BLOCK_SYSTEM ConfigBlockSystem
%action-define CONFIG_BLOCK_INPUT ConfigBlockInput 
%action-define CONFIG_BLOCK_MONITOR ConfigBlockMonitor
%action-define CONFIG_BLOCK_SCHEDULE ConfigBlockSchedule
%action-define CONFIG_BLOCK_RULE ConfigBlockRule
%action-define CONFIG_BLOCK_CONTROLLER ConfigBlockController
%action-define CONFIG_BLOCK_OUTPUT ConfigBlockOutput
 
%1 keyword CONFIG
%2 keyword RESET
%action CONFIG_RESET
%2 keyword LOAD
%action CONFIG_LOAD
%3 keyword BINARY
%action CONFIG_LOAD_BINARY
%3 keyword INI
%action CONFIG_LOAD_INI
%2 keyword SAVE
%action CONFIG_SAVE
%3 keyword BINARY
%action CONFIG_SAVE_BINARY

%# ---------------------------------------------------------

%2 keyword system
%3 param-string

%4 keyword type
%5 param-string
%action CONFIG_BLOCK_SYSTEM

%4 keyword display_name
%5 param-string
%action CONFIG_BLOCK_SYSTEM

%4 keyword description
%5 param-string
%action CONFIG_BLOCK_SYSTEM

%4 keyword temp_scale
%5 param-string
%action CONFIG_BLOCK_SYSTEM

%4 keyword language
%5 param-string
%action CONFIG_BLOCK_SYSTEM

%4 keyword week_start
%5 param-string
%action CONFIG_BLOCK_SYSTEM

%# ---------------------------------------------------------

%2 keyword input
%3 param-string

%4 keyword type
%5 param-string
%action CONFIG_BLOCK_INPUT

%4 keyword display_name
%5 param-string
%action CONFIG_BLOCK_INPUT

%4 keyword description
%5 param-string
%action CONFIG_BLOCK_INPUT

%4 keyword interface
%5 param-string
%action CONFIG_BLOCK_INPUT

%4 keyword if_num
%5 param-string
%action CONFIG_BLOCK_INPUT

%4 keyword log_rate
%5 param-string
%action CONFIG_BLOCK_INPUT

%4 keyword data_units
%5 param-string
%action CONFIG_BLOCK_INPUT

%# ---------------------------------------------------------

%2 keyword monitor
%3 param-string

%4 keyword type
%5 param-string
%action CONFIG_BLOCK_MONITOR

%4 keyword display_name
%5 param-string
%action CONFIG_BLOCK_MONITOR

%4 keyword description
%5 param-string
%action CONFIG_BLOCK_MONITOR

%4 keyword input1
%5 param-string
%action CONFIG_BLOCK_MONITOR

%4 keyword input2
%5 param-string
%action CONFIG_BLOCK_MONITOR

%4 keyword input3
%5 param-string
%action CONFIG_BLOCK_MONITOR

%4 keyword input4
%5 param-string
%action CONFIG_BLOCK_MONITOR

%4 keyword act_val
%5 param-string
%action CONFIG_BLOCK_MONITOR

%4 keyword deact_val
%5 param-string
%action CONFIG_BLOCK_MONITOR

%# ---------------------------------------------------------

%2 keyword schedule
%3 param-string

%4 keyword type
%5 param-string
%action CONFIG_BLOCK_SCHEDULE

%4 keyword display_name
%5 param-string
%action CONFIG_BLOCK_SCHEDULE

%4 keyword description
%5 param-string
%action CONFIG_BLOCK_SCHEDULE

%4 keyword days
%5 param-string
%action CONFIG_BLOCK_SCHEDULE

%4 keyword time_start
%5 param-string
%action CONFIG_BLOCK_SCHEDULE

%4 keyword time_end
%5 param-string
%action CONFIG_BLOCK_SCHEDULE

%4 keyword time_duration
%5 param-string
%action CONFIG_BLOCK_SCHEDULE

%4 keyword time_repeat
%5 param-string
%action CONFIG_BLOCK_SCHEDULE

%# ---------------------------------------------------------

%2 keyword rule
%3 param-string

%4 keyword type
%5 param-string
%action CONFIG_BLOCK_RULE

%4 keyword display_name
%5 param-string
%action CONFIG_BLOCK_RULE

%4 keyword description
%5 param-string
%action CONFIG_BLOCK_RULE

%4 keyword param1
%5 param-string
%action CONFIG_BLOCK_RULE

%4 keyword param2
%5 param-string
%action CONFIG_BLOCK_RULE

%4 keyword param3
%5 param-string
%action CONFIG_BLOCK_RULE

%4 keyword param_not
%5 param-string
%action CONFIG_BLOCK_RULE

%# ---------------------------------------------------------

%2 keyword controller
%3 param-string

%4 keyword type
%5 param-string
%action CONFIG_BLOCK_CONTROLLER

%4 keyword display_name
%5 param-string
%action CONFIG_BLOCK_CONTROLLER

%4 keyword description
%5 param-string
%action CONFIG_BLOCK_CONTROLLER

%4 keyword rule
%5 param-string
%action CONFIG_BLOCK_CONTROLLER

%4 keyword output
%5 param-string
%action CONFIG_BLOCK_CONTROLLER

%4 keyword act_cmd
%5 param-string
%action CONFIG_BLOCK_CONTROLLER

%4 keyword deact_cmd
%5 param-string
%action CONFIG_BLOCK_CONTROLLER

%# ---------------------------------------------------------

%2 keyword output
%3 param-string

%4 keyword type
%5 param-string
%action CONFIG_BLOCK_OUTPUT

%4 keyword display_name
%5 param-string
%action CONFIG_BLOCK_OUTPUT

%4 keyword description
%5 param-string
%action CONFIG_BLOCK_OUTPUT

%4 keyword interface
%5 param-string
%action CONFIG_BLOCK_OUTPUT

%4 keyword if_num
%5 param-string
%action CONFIG_BLOCK_OUTPUT

%# ---------------------------------------------------------

%#1 keyword CONFIGURE
%#2 keyword TERMINAL
%#change-mode CONFIG_INTERACTIVE


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

%#action-define DISABLE_BLOCK_CAT_N BlockDisableByBlockCatN
%#action-define DISABLE_BLOCK_LABEL BlockDisableByLabel
%#action-define ENABLE_BLOCK_CAT_N BlockEnableByBlockCatN
%#action-define ENABLE_BLOCK_LABEL BlockEnableByLabel
%#action-define DELETE_BLOCK_CAT_N BlockDeleteByBlockCatN
%#action-define DELETE_BLOCK_LABEL BlockDeleteByLabel
%#action-define RENAME_BLOCK BlockRename

%# DISABLE <block_label>         // enable and disable blocks from processing
%# DISABLE <block_category> <INT>    
%# ENABLE <block_label>
%# ENABLE <block_category> <INT>
%# DELETE <block_label>          // delete block from running config
%# DELETE <block_category> <INT>
%# RENAME <block_label> new_block_label  // only possible within block types and avoiding duplicates names

%#1 keyword DISABLE
%#2 enum-array block_category
%#3 param-integer
%#action DISABLE_BLOCK_CAT_N
%#2 param-string
%#action DISABLE_BLOCK_LABEL
%#1 keyword ENABLE
%#2 enum-array block_category
%#3 param-integer
%#action ENABLE_BLOCK_CAT_N
%#2 lookup block_label
%#action ENABLE_BLOCK_LABEL
%#1 keyword DELETE
%#2 enum-array block_category
%#3 param-integer
%#action DELETE_BLOCK_CAT_N
%#2 param-string
%#action DELETE_BLOCK_LABEL
%#1 keyword RENAME
%#2 param-string
%#3 param-string
%#action RENAME_BLOCK

%#action-define EXIT TerminalExit
%#action-define LOGOUT TerminalLogout

%action-define REBOOT SystemReboot

%# EXIT                          // close configuration section, move to enclosing scope
%#                                // Also ! and CTRL-Z for same purpose
%#                                // EXIT in termal mode (non config) implies LOGOUT 
%# LOGOUT                                // disconnect this terminal
%# REBOOT

%#1 keyword EXIT
%#action EXIT
%#1 keyword LOGOUT
%#action LOGOUT

%1 keyword REBOOT
%action REBOOT

%action-define BLOCK_ID_CMD_ON BlockIDCmdOn
%action-define BLOCK_ID_CMD_OFF BlockIDCmdOff

%1 param-integer
%2 keyword ON
%action BLOCK_ID_CMD_ON
%2 keyword OFF
%action BLOCK_ID_CMD_OFF

%grammar-end


