#define MAX_LINE_LENGTH 150
#define MAX_WORD_LENGTH 150
#define MAX_WORDS_PER_LINE 30
#define MAX_IDENTIFIERS 50

#define MAX_MESSAGE_STRING_LENGTH 150
#define MAX_LABEL_LENGTH 150
#define MAX_CONF_KEYS_PER_BLOCK 150

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

typedef struct GRAMMAR_DEF {
    uint16_t id;
    char label[MAX_WORD_LENGTH];
    char unique[MAX_WORD_LENGTH];
    char help[MAX_LINE_LENGTH];
    int term_level;
    int type;
    bool action;
    bool finish;
    uint16_t parent;
    uint16_t next_sibling;
    uint16_t first_child;
    char action_identifier[MAX_WORD_LENGTH];
} GrammarDef;

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

//TODO: Lexer Lookup Hooks
//TODO: Command Line Options Processing
//TODO: AST Validation Walk - 
//TODO: AST Flag end points
//TODO: AST Order Ambiguity Report
//TODO: AST Determine Partial Keyword Uniqueness
//TODO: AST Warn unused IDs, lookups, params
//TODO: Parser/AST output Action prototypes
//TODO: Configuration Grammar with %section directive
//TODO: Context change on <identifier> value

#ifdef USE_PROGMEM
static const GrammarDef grammar_def [88] PROGMEM = {
#else
static const GrammarDef grammar_def [88] = {
#endif
	1, "SHOW", "", "", 1, 1, 0, 0, 0, 2, 7, "",
	2, "BLOCKS", "", "", 2, 1, 1, 0, 1, 0, 3, "SHOW_BLOCKS",
	3, "SYSTEM", "", "", 2, 1, 1, 0, 1, 0, 4, "SHOW_SYSTEM",
	4, "block-category", "", "", 2, 2, 1, 0, 1, 0, 5, "SHOW_BLOCK_CATEGORY",
	5, "block-label", "", "", 2, 3, 1, 0, 1, 0, 6, "SHOW_BLOCK_LABEL",
	6, "block-id", "", "", 2, 3, 1, 0, 1, 0, 0, "SHOW_BLOCK_ID",
	7, "MUTE", "", "", 1, 1, 0, 0, 0, 8, 11, "",
	8, "MESSAGES", "", "", 2, 1, 0, 0, 7, 9, 0, "",
	9, "ON", "", "", 3, 1, 1, 0, 8, 0, 10, "MUTE_MESSAGES_ON",
	10, "OFF", "", "", 3, 1, 1, 0, 8, 0, 0, "MUTE_MESSAGES_OFF",
	11, "MONITOR", "", "", 1, 1, 0, 0, 0, 12, 21, "",
	12, "START", "", "", 2, 1, 1, 0, 11, 0, 13, "MONIOTR_START",
	13, "STOP", "", "", 2, 1, 0, 0, 11, 14, 18, "",
	14, "ALL", "", "", 3, 1, 1, 0, 13, 0, 15, "MONITOR_STOP_ALL",
	15, "<block-label>", "", "", 3, 3, 1, 0, 13, 0, 16, "MONITOR_STOP_BLOCK_LABEL",
	16, "<block-category>", "", "", 3, 2, 0, 0, 13, 17, 0, "",
	17, "", "", "", 4, 5, 1, 0, 16, 0, 0, "MONITOR_STOP_BLOCK_CAT_N",
	18, "<block-label>", "", "", 2, 3, 1, 0, 11, 0, 19, "MONITOR_BLOCK_LABEL",
	19, "<block-category>", "", "", 2, 2, 0, 0, 11, 20, 0, "",
	20, "", "", "", 3, 5, 1, 0, 19, 0, 0, "MONITOR_BLOCK_CAT_N",
	21, "CLEAR", "", "", 1, 1, 0, 0, 0, 22, 25, "",
	22, "<block-label>", "", "", 2, 3, 1, 0, 21, 0, 23, "CLEAR_BLOCK_LABEL",
	23, "<block-category>", "", "", 2, 2, 0, 0, 21, 24, 0, "",
	24, "", "", "", 3, 5, 1, 0, 23, 0, 0, "CLEAR_BLOCK_CAT_N",
	25, "MESSAGE", "", "", 1, 1, 0, 0, 0, 26, 32, "",
	26, "COMMAND", "", "", 2, 1, 0, 0, 25, 27, 0, "",
	27, "<block-category>", "", "", 3, 2, 0, 0, 26, 28, 30, "",
	28, "", "", "", 4, 5, 0, 0, 27, 29, 0, "",
	29, "<command>", "", "", 5, 2, 1, 0, 28, 0, 0, "COMMAND_BLOCK_CAT_N",
	30, "<block-label>", "", "", 3, 3, 0, 0, 26, 31, 0, "",
	31, "<command>", "", "", 4, 2, 1, 0, 30, 0, 0, "COMMAND_BLOCK_LABEL",
	32, "MESSAGE", "", "", 1, 1, 0, 0, 0, 33, 45, "",
	33, "DATA", "", "", 2, 1, 0, 0, 32, 34, 0, "",
	34, "<block-category>", "", "", 3, 2, 0, 0, 33, 35, 40, "",
	35, "", "", "", 4, 5, 0, 0, 34, 36, 0, "",
	36, "INT", "", "", 5, 1, 0, 0, 35, 37, 38, "",
	37, "", "", "", 6, 5, 1, 0, 36, 0, 0, "MESSAGE_DATA_BCAT_N_INT",
	38, "FLOAT", "", "", 5, 1, 0, 0, 35, 39, 0, "",
	39, "", "", "", 6, 6, 1, 0, 38, 0, 0, "MESSAGE_DATA_BCAT_N_FLOAT",
	40, "<block-label>", "", "", 3, 3, 0, 0, 33, 41, 0, "",
	41, "INT", "", "", 4, 1, 0, 0, 40, 42, 43, "",
	42, "", "", "", 5, 5, 1, 0, 41, 0, 0, "MESSAGE_DATA_LABEL_INT",
	43, "FLOAT", "", "", 4, 1, 0, 0, 40, 44, 0, "",
	44, "", "", "", 5, 6, 1, 0, 43, 0, 0, "MESSAGE_DATA_LABEL_FLOAT",
	45, "DEBUG", "", "", 1, 1, 0, 0, 0, 46, 53, "",
	46, "ON", "", "", 2, 1, 1, 0, 45, 0, 47, "DEBUG_ON",
	47, "OFF", "", "", 2, 1, 1, 0, 45, 0, 48, "DEBUG_OFF",
	48, "TERMINAL", "", "", 2, 1, 0, 0, 45, 49, 51, "",
	49, "ON", "", "", 3, 1, 1, 0, 48, 0, 50, "DEBUG_TERMINAL_ON",
	50, "OFF", "", "", 3, 1, 1, 0, 48, 0, 0, "DEBUG_TERMINAL_OFF",
	51, "LEVEL", "", "", 2, 1, 0, 0, 45, 52, 0, "",
	52, "", "", "", 3, 5, 1, 0, 51, 0, 0, "DEBUG_LEVEL",
	53, "CLOCK", "", "", 1, 1, 1, 0, 0, 54, 56, "SHOW_TIME",
	54, "SET", "", "", 2, 1, 0, 0, 53, 55, 0, "",
	55, "", "", "", 3, 7, 1, 0, 54, 0, 0, "SET_TIME",
	56, "DATE", "", "", 1, 1, 1, 0, 0, 57, 59, "SHOW_DATE",
	57, "SET", "", "", 2, 1, 0, 0, 56, 58, 0, "",
	58, "", "", "", 3, 8, 1, 0, 57, 0, 0, "SET_DATE",
	59, "CONFIGURE", "", "", 1, 1, 0, 0, 0, 60, 65, "",
	60, "CLEAR", "", "", 2, 1, 0, 0, 59, 61, 63, "",
	61, "BLOCKS", "", "", 3, 1, 1, 0, 60, 0, 62, "CONF_CLEAR_BLOCKS",
	62, "ALL", "", "", 3, 1, 1, 0, 60, 0, 0, "CONF_CLEAR_ALL",
	63, "FILE", "", "", 2, 1, 1, 0, 59, 64, 0, "CONF_FILE_DEFAULT",
	64, "", "", "", 3, 4, 1, 0, 63, 0, 0, "CONF_FILE_FILENAME",
	65, "WRITE", "", "", 1, 1, 1, 0, 0, 66, 68, "WRITE_DEFAULT",
	66, "INIT", "", "", 2, 1, 1, 0, 65, 0, 67, "WRITE_INIT",
	67, "", "", "", 2, 4, 1, 0, 65, 0, 0, "WRITE_FILENAME",
	68, "COPY", "", "", 1, 1, 0, 0, 0, 69, 71, "",
	69, "", "", "", 2, 4, 0, 0, 68, 70, 0, "",
	70, "", "", "", 3, 4, 1, 0, 69, 0, 0, "COPY_FILE_FILE",
	71, "DISABLE", "", "", 1, 1, 0, 0, 0, 72, 75, "",
	72, "<block-category>", "", "", 2, 2, 0, 0, 71, 73, 74, "",
	73, "", "", "", 3, 5, 1, 0, 72, 0, 0, "DISABLE_BLOCK_CAT_N",
	74, "", "", "", 2, 4, 1, 0, 71, 0, 0, "DISABLE_BLOCK_LABEL",
	75, "ENABLE", "", "", 1, 1, 0, 0, 0, 76, 79, "",
	76, "<block-category>", "", "", 2, 2, 0, 0, 75, 77, 78, "",
	77, "", "", "", 3, 5, 1, 0, 76, 0, 0, "ENABLE_BLOCK_CAT_N",
	78, "", "", "", 2, 4, 1, 0, 75, 0, 0, "ENABLE_BLOCK_LABEL",
	79, "DELETE", "", "", 1, 1, 0, 0, 0, 80, 83, "",
	80, "<block-category>", "", "", 2, 2, 0, 0, 79, 81, 82, "",
	81, "", "", "", 3, 5, 1, 0, 80, 0, 0, "DELETE_BLOCK_CAT_N",
	82, "", "", "", 2, 4, 1, 0, 79, 0, 0, "DELETE_BLOCK_LABEL",
	83, "RENAME", "", "", 1, 1, 0, 0, 0, 84, 86, "",
	84, "", "", "", 2, 4, 0, 0, 83, 85, 0, "",
	85, "", "", "", 3, 4, 1, 0, 84, 0, 0, "RENAME_BLOCK",
	86, "EXIT", "", "", 1, 1, 1, 0, 0, 0, 87, "EXIT",
	87, "LOGOUT", "", "", 1, 1, 1, 0, 0, 0, 88, "LOGOUT",
	88, "REBOOT", "", "", 1, 1, 1, 0, 0, 0, 0, "REBOOT",
};
IdentifierName: block-category
InstanceName: 	block_cat_defs
K: FF_ERROR_CAT		V: ERROR_CAT
K: FF_GENERIC_BLOCK		V: GENERIC
K: FF_SYSTEM		V: SYSTEM
K: FF_INPUT		V: INPUT
K: FF_MONITOR		V: MONITOR
K: FF_SCHEDULE		V: SCHEDULE
K: FF_RULE		V: RULE
K: FF_CONTROLLER		V: CONROLLER
K: FF_OUTPUT		V: OUTPUT
K: LAST_BLOCK_CAT		V: OUTPUT

IdentifierName: SHOW_BLOCKS
InstanceName: 	ShowBlocks

IdentifierName: SHOW_SYSTEM
InstanceName: 	ShowSystem

IdentifierName: SHOW_BLOCK_LABEL
InstanceName: 	ShowBlockByLabel

IdentifierName: SHOW_BLOCK_CATEGORY
InstanceName: 	ShowBlocksInCategory

IdentifierName: SHOW_BLOCK_ID
InstanceName: 	ShowBlockByID

IdentifierName: MUTE_MESSAGES_ON
InstanceName: 	MessagesMute

IdentifierName: MUTE_MESSAGES_OFF
InstanceName: 	MessagesUnmute

IdentifierName: MONIOTR_START
InstanceName: 	MonitorStart

IdentifierName: MONITOR_STOP_ALL
InstanceName: 	MonitorStop

IdentifierName: MONITOR_STOP_BLOCK_LABEL
InstanceName: 	MonitorRemoveByLabel

IdentifierName: MONITOR_STOP_BLOCK_CAT_N
InstanceName: 	MonitorRemoveByBlockCatN

IdentifierName: MONITOR_BLOCK_LABEL
InstanceName: 	MonitorByLabel

IdentifierName: MONITOR_BLOCK_CAT_N
InstanceName: 	MonitorByBlockCatN

IdentifierName: CLEAR_BLOCK_LABEL
InstanceName: 	ClearBlockByLabel

IdentifierName: CLEAR_BLOCK_CAT_N
InstanceName: 	ClearBlockByBlockCatN

IdentifierName: COMMAND_BLOCK_CAT_N
InstanceName: 	SendCommandToBlockCatN

IdentifierName: COMMAND_BLOCK_LABEL
InstanceName: 	SendCommandToBlockLabel

IdentifierName: MESSAGE_DATA_BCAT_N_INT
InstanceName: 	SimIntDataMessageFromBCatN

IdentifierName: MESSAGE_DATA_BCAT_N_FLOAT
InstanceName: 	SimFloatDataMessageFromBCatN

IdentifierName: MESSAGE_DATA_LABEL_INT
InstanceName: 	SimIntDataMessageFromBlockLabel

IdentifierName: MESSAGE_DATA_LABEL_FLOAT
InstanceName: 	SimFloatDataMessageFromBlockLabel

IdentifierName: DEBUG_ON
InstanceName: 	DebugOn

IdentifierName: DEBUG_OFF
InstanceName: 	DebugOff

IdentifierName: DEBUG_TERMINAL_ON
InstanceName: 	DebugTermOn

IdentifierName: DEBUG_TERMINAL_OFF
InstanceName: 	DebugTermOff

IdentifierName: DEBUG_LEVEL
InstanceName: 	DebugSetLEvel

IdentifierName: SHOW_TIME
InstanceName: 	ShowTime

IdentifierName: SET_TIME
InstanceName: 	SetTime

IdentifierName: SHOW_DATE
InstanceName: 	ShowDate

IdentifierName: SET_DATE
InstanceName: 	SetDate

IdentifierName: CONF_CLEAR_BLOCKS
InstanceName: 	ConfClearBlocks

IdentifierName: CONF_CLEAR_ALL
InstanceName: 	ConfClearAll

IdentifierName: CONF_FILE_DEFAULT
InstanceName: 	ConfReadDefaultFile

IdentifierName: CONF_FILE_FILENAME
InstanceName: 	ConfReadFilename

IdentifierName: WRITE_DEFAULT
InstanceName: 	WriteToDefaultConfigFile

IdentifierName: WRITE_INIT
InstanceName: 	WriteEmptyDefaultConfigFile

IdentifierName: WRITE_FILENAME
InstanceName: 	WriteToFilenameConfigFile

IdentifierName: COPY_FILE_FILE
InstanceName: 	CopyConfigFileToFile

IdentifierName: DISABLE_BLOCK_CAT_N
InstanceName: 	BlockDisableByBlockCatN

IdentifierName: DISABLE_BLOCK_LABEL
InstanceName: 	BlockDisableByLabel

IdentifierName: ENABLE_BLOCK_CAT_N
InstanceName: 	BlockEnableByBlockCatN

IdentifierName: ENABLE_BLOCK_LABEL
InstanceName: 	BlockEnableByLabel

IdentifierName: DELETE_BLOCK_CAT_N
InstanceName: 	BlockDeleteByBlockCatN

IdentifierName: DELETE_BLOCK_LABEL
InstanceName: 	BlockDeleteByLabel

IdentifierName: RENAME_BLOCK
InstanceName: 	BlockRename

IdentifierName: EXIT
InstanceName: 	TerminalExit

IdentifierName: LOGOUT
InstanceName: 	TerminalLogout

IdentifierName: REBOOT
InstanceName: 	SystemReboot

