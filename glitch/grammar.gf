%# 
%# glitch - Grammar Lexer and Interactive Terminal Command sHell
%#
%# Copyright 2018, Brendan McLearie, all rights reserved.
%#
%#  This file: 
%#      - A fully worked example of a complex grammar using   
%#        all of the features of the package.
%#      - In production use in https://github.com/winginitau/FodderFactory 
%#
%#  Input:   
%#      - A grammar definition file (this file) - default "grammar.gf"
%#         
%#  Output:
%#      - itch - Interactive Terminal Command sHell (parser) comprsing:   
%#          - itch header (out.h) 
%#              - Anonyous and/or typed enum lists (const reference/master data for the intended application)
%#              - The executable sytax logic - abstract syntax tree (AST) 
%#                rendered as a static const array (ASTA) 
%#              - ANSI C function prototypes for all "actions" that are called from the processing
%#                of parsed commands
%#              - Preprocessor logic for different platforms. Currently:
%#                  - "ARDUINO" - Atmnel AVR using PROGMEM for all static const data
%#                  - linux - implying a generic gcc (or equivalient) environment
%#              - Utility and lookup function defintions to operate on the ASTA, make calls, callbacks etc
%#              - c++ itch class wrapper (for what is otherise ANSI C)
%#              - Any included inline code (via %header-start and %header-end directives) 
%#          - itch code body (out.cpp)
%#              - The code to read inputs and parse it according to the ASTA logic 
%#              - Utility functions - static const lookups (and from PROGMEM), callers and callbacks
%#              - Any included inline code (via %code-start and %code-end directives)
%#          - Action calling skeleton functions (out_user_code.cpp)
%#              - Optionally overwriting previous output
%#              - Provides place-holders that can be used to fully develop the code that
%#                gets called (and calls callbacks) resulting from the parsing of a command  
%#              - Including correct passing of all paramaters that were entered with the command
%#   
%#  Directive: %# 
%#  - Used as comments in the grammar file (which allows #pre-processor directives in inline code)
%#
%#  Directive: %include ff_sys_config.h
%#  (***** Revisit and differentiate between include in header or code  
%#
%#  Directive: %header-start / %header-end / %code-start / %code-end 
%#  - Start and end of code to insert "as is" in the header or code files  
%#  - Use to define classes and/or typedef structs that are referenced in the grammar
%#

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

//
// Structs used by the parser 
// 

typedef struct SIMPLE_STRING_ARRAY_TYPE {
    // Array of strings relating to enums
    // Inside this struct, extended intitaliser lists are ok
    const char* text;
} SimpleStringArray;

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

%#
%#  Directive: %grammar-start
%#  - Grammar definition starts with %grammar-start and ends with %grammar-end  
%#

%grammar-start


%comment #
%comment //

%sub-section-close !
%sub-section-close EXIT
%sub-section-close 0x26

%redundant-close-as-comment

%ignore-case

%escape-sequence ^^^

%#  Usage: %enum-list
%#  
%#      %enum-list [ <EnumIdentififer> | anon ] <start-value> \
%#          <ArraryType> <static-const-var-identifier> <field-name>
%#          (<keyword-id>)  
%#
%#  Creates a code block:
%#
%#      enum { ENUM1 = start-value, ENUM2, ... } optional-EnumIdentifier;
%#


%enum-terminating-member
%enum-plus-list-array
%enum-start-value 0
%enum-array-member-label text
%enum-array-reserve-words

%enum-identifier block_category
%enum-array-type SimpleStringArray
%enum-array-instance block_cat_names
%enum-start
FF_ERROR_CAT        ERROR_CAT
FF_GENERIC_BLOCK    GENERIC
FF_SYSTEM           SYSTEM
FF_INPUT            INPUT
FF_MONITOR          MONITOR 
FF_SCHEDULE         SCHEDULE
FF_RULE             RULE
FF_CONTROLLER       CONROLLER
FF_OUTPUT           OUTPUT
LAST_BLOCK_CAT
%enum-end

%enum-identifier command
%enum-array-type SimpleStringArray
%enum-array-instance command_strings
%enum-start
CMD_ERROR           CMD_ERROR   
CMD_OUTPUT_OFF      CMD_OUTPUT_OFF
CMD_OUTPUT_ON       CMD_OUTPUT_ON
CMD_RESET_MIN_MAX   CMD_RESET_MIN_MAX
LAST_COMMAND
%enum-end

%# 
%#  Usage: %lookup-list <lookup-identifier> <FunctionName>
%#
%#  Results in a prototype being defined in the output header file which is the
%#  combination of the FunctionName and the identifier to be looked up, including
%#  any previous params, identifiers and lookups in the gramatical pathway.
%#  
%#  Example:
%#
%#      %lookup-list block_label LookupBlockLabel
%#
%#  to be used by the line:
%#
%#      %2 lookup block_label 
%#
%#  which has been defined in:
%#
%#      %1 keyword SHOW
%#      %2 keyword BLOCKS 
%#      %action SHOW_BLOCKS
%#      %2 keyword SYSTEM 
%#      %action SHOW_SYSTEM
%#      %2 identifier block_category 
%#      %action SHOW_BLOCK_CATEGORY
%#      %2 lookup block_label
%#      %action SHOW_BLOCK_LABEL
%#
%#  the resulting prototype would be:
%#
%#      char** LookupBlockLabel(char* block_label); 
%#  

%lookup-list block_label LookupBlockLabel


%# 
%#  Usage: %action-define <ACTION_IDENTIFIER> <FunctionName>
%#
%#  Results in a prototype being defined in the output header file which is a
%#  combination of the FunctionName and the grammar entered to call the action.
%#  The parts of the grammar included are identifiers, params and lookups used
%#  to arrive at the action. Keywords are excluded as redundant.
%#  
%#  Example:
%#
%#      %action-define MESSAGE_DATA_BCAT_N_INT SendMessage
%#
%#  to be triggered by the command:
%#
%#      MESSAGE DATA <block_category> <INT> FLOAT <FLOAT>
%#
%#  which has been defined as:
%#
%#      %1 keyword MESSAGE
%#      %2 keyword DATA
%#      %3 identifier block_category
%#      %4 param-integer
%#      %5 keyword INT
%#      %6 param-integer
%#
%#  the resulting prototype would be:
%#
%#      char** SendMessage( int block_category, 
%#                          int param_int_1, int param_int_2); 
%#  

%action-define SHOW_BLOCKS ShowBlocks
%action-define SHOW_SYSTEM ShowSystem
%action-define SHOW_BLOCK_LABEL ShowBlockByLabel
%action-define SHOW_BLOCK_CATEGORY ShowBlockByCategory
%action-define SHOW_BLOCK_CAT_N ShowBlockCatN
%action-define SHOW_BLOCK_ID ShowBlockByID

%# SHOW BLOCKS                   // List all blocks on device with their label and integer ID 
%# SHOW <block_category>         // List all blocks within a category with their label and integer ID
%# SHOW <block_label>            // Show settings and current values for block
%# SHOW <block-id>               // Show the details of the block with ID <block-id> 
%# SHOW SYSTEM                   // dislay system block settings

%1 keyword SHOW
%2 keyword BLOCKS 
%action SHOW_BLOCKS
%2 keyword SYSTEM 
%action SHOW_SYSTEM
%2 identifier block_category 
%action SHOW_BLOCK_CATEGORY
%3 param-integer
%action SHOW_BLOCK_CAT_N
%2 lookup block_label
%action SHOW_BLOCK_LABEL
%2 param-integer 
%action SHOW_BLOCK_ID



%action-define MUTE_MESSAGES_ON MessagesMute
%action-define MUTE_MESSAGES_OFF MessagesUnmute

%# MUTE MESSAGES ON				        // stop displaying event and debug messages to this terminal
%# MUTE MESSAGES OFF				    // start displaying.....

%1 keyword MUTE 
%2 keyword MESSAGES 
%3 keyword ON 
%action MUTE_MESSAGES_ON
%3 keyword OFF 
%action MUTE_MESSAGES_OFF



%action-define MONIOTR_START MonitorStart
%action-define MONITOR_STOP_ALL MonitorStop
%action-define MONITOR_STOP_BLOCK_LABEL MonitorRemoveByLabel
%action-define MONITOR_STOP_BLOCK_CAT_N MonitorRemoveByBlockCatN
%action-define MONITOR_BLOCK_LABEL MonitorByLabel
%action-define MONITOR_BLOCK_CAT_N MonitorByBlockCatN

%# MONITOR <block_label>                 // Add block to the monitor pool
%# MONITOR <block_category> <INT>        // Add block to the monitor pool
%# MONITOR STOP <block_label>            // Remove block from the monitor pool
%# MONITOR STOP <block_category> <INT>   // Remove block from the monitor pool
%# MONITOR STOP ALL                      // Remove all blocks from the monitor pool
%# MONITOR START                 // Show all activity from and to monitored blocks. <any key> to exit.

%1 keyword MONITOR
%2 keyword START
%action MONIOTR_START
%2 keyword STOP
%3 keyword ALL
%action MONITOR_STOP_ALL
%3 lookup block_label
%action MONITOR_STOP_BLOCK_LABEL
%3 identifier block_category
%4 param-integer
%action MONITOR_STOP_BLOCK_CAT_N
%2 lookup block_label
%action MONITOR_BLOCK_LABEL
%2 identifier block_category
%3 param-integer
%action MONITOR_BLOCK_CAT_N



%action-define CLEAR_BLOCK_LABEL ClearBlockByLabel
%action-define CLEAR_BLOCK_CAT_N ClearBlockByBlockCatN

%# CLEAR <block_label>                   // Clear block's local datalog
%# CLEAR <block_category> <INT>          // Clear block's local datalog

%1 keyword CLEAR
%2 lookup block_label
%action CLEAR_BLOCK_LABEL
%2 identifier block_category
%3 param-integer
%action CLEAR_BLOCK_CAT_N



%action-define COMMAND_BLOCK_CAT_N SendCommandToBlockCatN
%action-define COMMAND_BLOCK_LABEL SendCommandToBlockLabel
%action-define MESSAGE_DATA_BCAT_N_INT SimIntDataMessageFromBCatN
%action-define MESSAGE_DATA_BCAT_N_FLOAT SimFloatDataMessageFromBCatN
%action-define MESSAGE_DATA_LABEL_INT SimIntDataMessageFromBlockLabel
%action-define MESSAGE_DATA_LABEL_FLOAT SimFloatDataMessageFromBlockLabel

%# MESSAGE COMMAND <block_category> <INT> <command>          // Send command message to output block
%# MESSAGE COMMAND <block_label> <command>
%# MESSAGE DATA <block_label> INT <INT>              // Simulate input block sending integer data
%# MESSAGE DATA <block_category> <INT> INT <INT>     // Simulate input block sending integer data
%# MESSAGE DATA <block_label> FLOAT <FLOAT>          // Simulate input block sending float data
%# MESSAGE DATA <block_category> <INT> FLOAT <FLOAT> // Simulate input block sending float data

%1 keyword MESSAGE
%2 keyword COMMAND
%3 identifier block_category
%4 param-integer
%5 identifier command
%action COMMAND_BLOCK_CAT_N
%3 lookup block_label
%4 identifier command
%action COMMAND_BLOCK_LABEL
%2 keyword DATA
%3 identifier block_category
%4 param-integer
%5 keyword INT
%6 param-integer
%action MESSAGE_DATA_BCAT_N_INT
%5 keyword FLOAT
%6 param-float
%action MESSAGE_DATA_BCAT_N_FLOAT
%3 lookup block_label
%4 keyword INT
%5 param-integer
%action MESSAGE_DATA_LABEL_INT
%4 keyword FLOAT
%5 param-float
%action MESSAGE_DATA_LABEL_FLOAT



%action-define DEBUG_ON DebugOn
%action-define DEBUG_OFF DebugOff
%action-define DEBUG_TERMINAL_ON DebugTermOn
%action-define DEBUG_TERMINAL_OFF DebugTermOff
%action-define DEBUG_LEVEL DebugSetLevel

%# DEBUG LEVEL <INT>                 // 1 - 10 (lower is more verbose)
%# DEBUG ON                          // Turn debug messages on or off system wide
%# DEBUG OFF                         //     to all configured message interfaces
%# DEBUG TERMINAL ON                 // Send debug messages to this terminal 
%# DEBUG TERMINAL OFF                // Stop sending debug messages to this terminal 

%1 keyword DEBUG
%2 keyword ON
%action DEBUG_ON
%2 keyword OFF
%action DEBUG_OFF
%2 keyword TERMINAL
%3 keyword ON
%action DEBUG_TERMINAL_ON
%3 keyword OFF
%action DEBUG_TERMINAL_OFF
%2 keyword LEVEL
%3 param-integer
%action DEBUG_LEVEL



%action-define SHOW_TIME ShowTime
%action-define SET_TIME SetTime
%action-define SHOW_DATE ShowDate
%action-define SET_DATE SetDate

%# CLOCK                 // Show the time
%# CLOCK SET <hh:mm:ss>  // Set the time
%# DATE                  // Show the date
%# DATE SET <yyyy-mm-dd> // Set the date

%1 keyword CLOCK
%action SHOW_TIME
%2 keyword SET
%3 param-time
%action SET_TIME
%1 keyword DATE
%action SHOW_DATE
%2 keyword SET
%3 param-date
%action SET_DATE



%action-define CONF_CLEAR_BLOCKS ConfClearBlocks
%action-define CONF_CLEAR_ALL ConfClearAll
%action-define CONF_FILE_DEFAULT ConfReadDefaultFile
%action-define CONF_FILE_FILENAME ConfReadFilename

%# CONFIGURE CLEAR BLOCKS        // drop all block config except SYSTEM
%# CONFIGURE CLEAR ALL           // drop all config
%# CONFIGURE TERMINAL            // read configurtion statements from the terminal
%# CONFIGURE FILE                // read the default file into the running config
%# CONFIGURE FILE filename       // read a specified file into the running config

%# context-define CONFIG

%1 keyword CONFIGURE
%2 keyword CLEAR
%3 keyword BLOCKS
%action CONF_CLEAR_BLOCKS
%3 keyword ALL
%action CONF_CLEAR_ALL
%# 2 keyword TERMINAL
%# context CONFIG
%2 keyword FILE
%action CONF_FILE_DEFAULT
%3 param-string
%action CONF_FILE_FILENAME



%action-define WRITE_DEFAULT WriteToDefaultConfigFile
%action-define WRITE_INIT WriteEmptyDefaultConfigFile
%action-define WRITE_FILENAME WriteToFilenameConfigFile
%action-define COPY_FILE_FILE CopyConfigFileToFile

%# WRITE                 // writes running config to default file "config.txt"
%# WRITE INIT            // write empty init config to the default config "config.txt"
%# WRITE filename        // write running config to a specific filename
%# COPY filename filename  // copy contents of one config file to another

%#                        // Init start-up behavior: If config.txt cannot be found or
%#                        //  if no SYSTEM section is present (in order to define a terminal)
%#                        //  the system will attempt to start terminal interfaces at 9600 N81
%#                        //  on well known serial ports. For Adruino - "Serial". On mega
%#                        //  Serrial, Serial1, Serial2 Serial3. On UNIX systems
%#                        //  /dev/ttyACM0 /dev/ttyS0, /dev/ttyUSB0 and on 
%#                        //  tcp ports 23, 6001, 8001
%#                        // If a SYSTEM section is found, but it does not specify a
%#                        //  TERMINAL section then no terminals will be started as this
%#                        //  may be intended behavior - eg if access to the config file is
%#                        //  is provided by the operating system. 

%1 keyword WRITE
%action WRITE_DEFAULT
%2 keyword INIT
%action WRITE_INIT
%2 param-string
%action WRITE_FILENAME
%1 keyword COPY
%2 param-string
%3 param-string
%action COPY_FILE_FILE



%action-define DISABLE_BLOCK_CAT_N BlockDisableByBlockCatN
%action-define DISABLE_BLOCK_LABEL BlockDisableByLabel
%action-define ENABLE_BLOCK_CAT_N BlockEnableByBlockCatN
%action-define ENABLE_BLOCK_LABEL BlockEnableByLabel
%action-define DELETE_BLOCK_CAT_N BlockDeleteByBlockCatN
%action-define DELETE_BLOCK_LABEL BlockDeleteByLabel
%action-define RENAME_BLOCK BlockRename

%# DISABLE <block_label>         // enable and disable blocks from processing
%# DISABLE <block_category> <INT>    
%# ENABLE <block_label>
%# ENABLE <block_category> <INT>
%# DELETE <block_label>          // delete block from running config
%# DELETE <block_category> <INT>
%# RENAME <block_label> new_block_label  // only possible within block types and avoiding duplicates names

%1 keyword DISABLE
%2 identifier block_category
%3 param-integer
%action DISABLE_BLOCK_CAT_N
%2 param-string
%action DISABLE_BLOCK_LABEL
%1 keyword ENABLE
%2 identifier block_category
%3 param-integer
%action ENABLE_BLOCK_CAT_N
%2 lookup block_label
%action ENABLE_BLOCK_LABEL
%1 keyword DELETE
%2 identifier block_category
%3 param-integer
%action DELETE_BLOCK_CAT_N
%2 param-string
%action DELETE_BLOCK_LABEL
%1 keyword RENAME
%2 param-string
%3 param-string
%action RENAME_BLOCK

%action-define EXIT TerminalExit
%action-define LOGOUT TerminalLogout
%action-define REBOOT SystemReboot

%# EXIT                          // close configuration section, move to enclosing scope
%#                                // Also ! and CTRL-Z for same purpose
%#                                // EXIT in termal mode (non config) implies LOGOUT 
%# LOGOUT                                // disconnect this terminal
%# REBOOT

%1 keyword EXIT
%action EXIT
%1 keyword LOGOUT
%action LOGOUT
%1 keyword REBOOT
%action REBOOT

%# NO        // *** undecided - per cisco IOS model, negate a statement

%grammar-end



