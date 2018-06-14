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
#include <ff_global_defs.h>
#include <ff_registry.h>
//#include <ff_string_consts.h>

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

%#action-define CONF_CLEAR_BLOCKS ConfClearBlocks
%#action-define CONF_CLEAR_ALL ConfClearAll
%#action-define CONF_FILE_DEFAULT ConfReadDefaultFile
%#action-define CONF_FILE_FILENAME ConfReadFilename

%# CONFIGURE CLEAR BLOCKS        // drop all block config except SYSTEM
%# CONFIGURE CLEAR ALL           // drop all config
%# CONFIGURE TERMINAL            // read configurtion statements from the terminal
%# CONFIGURE FILE                // read the default file into the running config
%# CONFIGURE FILE filename       // read a specified file into the running config

%1 keyword CONFIGURE
%2 keyword TERMINAL
%change-mode CONFIG_INTERACTIVE

%#2 keyword CLEAR
%#3 keyword BLOCKS
%#action CONF_CLEAR_BLOCKS
%#3 keyword ALL
%#action CONF_CLEAR_ALL

%#2 keyword FILE
%#action CONF_FILE_DEFAULT
%#3 param-string
%#action CONF_FILE_FILENAME

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

%action-define BLOCK_ID_ON BlockIDOn
%action-define BLOCK_ID_OFF BlockIDOff

%1 param-integer
%2 keyword ON
%action BLOCK_ID_ON
%2 keyword OFF
%action BLOCK_ID_OFF

%grammar-end


