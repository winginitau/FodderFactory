/************************************************
 ff_registry.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Component Registry
************************************************/
#ifndef SRC_FF_REGISTRY_H_
#define SRC_FF_REGISTRY_H_

/************************************************
 Includes
************************************************/
#include <ff_sys_config.h>
#include <time.h>
#include <stdint.h>

/************************************************
 Public and Forward Data Structure Declarations
************************************************/


typedef struct FF_SYSTEM_SETTINGS {
	uint8_t temp_scale;
	uint8_t language;
	uint8_t week_start;
} SystemSettings;

typedef struct FF_INPUT_SETTINGS {
	uint8_t interface;
	uint8_t if_num;
	TV_TYPE log_rate;
	uint8_t data_units;
	uint8_t data_type;		// float, int
} InputSettings;

typedef struct FF_MONITOR_SETTINGS {
	uint16_t input1;
	uint16_t input2;
	uint16_t input3;
	uint16_t input4;
	float act_val;
	float deact_val;
} MonitorSettings;

typedef struct FF_SCHED_SETTINGS {
	uint8_t days[7];
	time_t time_start;
	time_t time_end;
	TV_TYPE time_duration;
	TV_TYPE time_repeat;
} ScheduleSettings;

typedef struct FF_RULE_SETTINGS {
	uint16_t param1;
	uint16_t param2;
	uint16_t param3;
	uint16_t param_not;
} RuleSettings;

typedef struct FF_CONTROLLER_SETTINGS {
	uint16_t rule;
	uint16_t output;
	uint8_t act_cmd;
	uint8_t deact_cmd;
} ControllerSettings;

typedef struct FF_OUTPUT_SETTINGS {
	uint8_t interface;
	uint8_t if_num;
	uint8_t command;
} OutputSettings;


typedef union BLOCK_SETTINGS {
	SystemSettings sys;
	InputSettings in;
	MonitorSettings mon;
	ScheduleSettings sch;
	RuleSettings rl;
	ControllerSettings con;
	OutputSettings out;
} BlockSettings;

struct BLOCK_NODE {
	// common configuration data
	// note: these types are locked to config file keys strings
	// see ff_string_consts.h
	struct BLOCK_NODE *next_block;
	uint8_t block_cat;
	uint16_t block_type;
	uint16_t block_id;
	//char block_label[MAX_LABEL_LENGTH];
	char *block_label;

	#ifndef EXCLUDE_DISPLAYNAME
	char display_name[MAX_LABEL_LENGTH];
#endif
#ifndef EXCLUDE_DESCRIPTION
	char description[MAX_DESCR_LENGTH];
#endif

	// common operational, run-time data
	// not tied to config, though must be initialised in AddNewBlock()
	uint8_t active;		// boolean flag - is the block activated or deactivated TODO consider renaming
	uint8_t bool_val;	// generic holder for a boolean value (representing some real world status)
	int32_t int_val;	// generic holder of an int value (representing some real world status)
	float f_val;		// generic holder of a float value (representing some real world status)
	time_t last_update;	// the time the block was last updated / operated / etc
	uint8_t status;		// generic status flag (eg for MON cat to indicate of the last input read was valid)

	// union of block_type specific settings
	// also tied to config file key strings
	BlockSettings settings;
};

typedef struct BLOCK_NODE BlockNode;


typedef struct UI_DATA_SET {
	float inside_current;
	float inside_min;
	time_t inside_min_dt;
	float inside_max;
	time_t inside_max_dt;

	float outside_current;
	float outside_min;
	time_t outside_min_dt;
	float outside_max;
	time_t outside_max_dt;

	float water_current;
	float water_min;
	time_t water_min_dt;
	float water_max;
	time_t water_max_dt;

	//TODO not being used - delete?
	//uint8_t light_flag;				//current state of lights
	//uint8_t water_heater_flag;		//current state of water heater
} UIDataSet;



/************************************************
 Function Prototypes
************************************************/

void Operate(BlockNode *b);
void Setup(BlockNode *b);

void ProcessDispatcher(void(*f)(BlockNode*));

uint8_t SetCommand(uint16_t block_id, uint8_t cmd_msg);
uint8_t IsActive(uint16_t block_id);
uint8_t GetBVal(uint16_t block_id);
float GetFVal(uint16_t block_id);
uint16_t GetBlockIDByLabel(const char* label);
char const* GetBlockLabelString(uint16_t block_id);
void SetBlockLabelString(uint8_t block_type, int idx, const char* label);
BlockNode* GetLastBlockAddr(void);
BlockNode* GetBlockListHead(void);
BlockNode* GetBlockByID(BlockNode *list_node, uint16_t block_id);
BlockNode* GetBlockByLabel(const char *block_label);
BlockNode* AddBlock(uint8_t block_cat, const char *block_label);
//BlockNode* AddBlockNode(BlockNode** head_ref, uint8_t block_cat, const char *block_label);
char* UpdateBlockLabel(BlockNode* b, char * block_label);


uint8_t RegLookupBlockLabel(char* lookup_string);
void RegShowBlocks(void(*Callback)(char *));
void RegShowSystem(void(*Callback)(char *));
void RegShowBlockByID(uint16_t, void(*Callback)(char *));
void RegShowBlockByLabel(char* block_label, void(*Callback)(char *));
void RegSendCommandToBlockLabel(char* block_label, uint16_t command, void(*Callback)(char *));
void RegSendCommandToBlockID(uint16_t id, uint16_t command, void(*Callback)(char*));
void RegShowTime(void(*Callback)(char*));
void RegSetTime(char* time_str, void(*Callback)(char*));
void RegShowDate(void(*Callback)(char*));
void RegSetDate(char* date_str, void(*Callback)(char*));
void RegSystemReboot(void(*Callback)(char*));
void RegBlockIDOn(uint16_t block_id, void(*Callback)(char*));
void RegBlockIDOff(uint16_t block_id, void(*Callback)(char*));


void UpdateStateRegister(uint16_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val);
UIDataSet* GetUIDataSet(void);
void InitStateRegister(void);


#endif /* SRC_FF_REGISTRY_H_ */
