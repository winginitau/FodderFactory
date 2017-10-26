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
#include "ff_datetime.h"

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
	FFTime log_rate;
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
	FFTime time_start;
	FFTime time_end;
	FFTime time_duration;
	FFTime time_repeat;
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
	struct BLOCK_NODE *next_block;
	uint8_t block_cat;
	uint16_t block_type;
	uint16_t block_id;
	char block_label[MAX_LABEL_LENGTH];
	char display_name[MAX_LABEL_LENGTH];
	char description[MAX_DESCR_LENGTH];
	uint8_t active;
	uint8_t bool_val;
	uint8_t int_val;
	float f_val;
	FFTime last_update;
	BlockSettings settings;
};

typedef struct BLOCK_NODE BlockNode;


typedef struct UI_DATA_SET {
	float inside_current;
	float inside_min;
	FFDateTime inside_min_dt;
	float inside_max;
	FFDateTime inside_max_dt;

	float outside_current;
	float outside_min;
	FFDateTime outside_min_dt;
	float outside_max;
	FFDateTime outside_max_dt;

	float water_current;
	float water_min;
	FFDateTime water_min_dt;
	float water_max;
	FFDateTime water_max_dt;

	uint8_t light_flag;				//current state of lights
	uint8_t water_heater_flag;		//current state of water heater
} UIDataSet;



/************************************************
 Function Prototypes
************************************************/

void Operate(BlockNode *b);

void Setup(BlockNode *b);

void ProcessDispatcher(void(*f)(BlockNode*));

BlockNode* GetBlock (BlockNode *list_node, uint8_t block_cat, const char *block_label);

uint8_t ConfigureBlock(uint8_t block_cat, const char *block_label, const char *key_str, const char *value_str);

char const* GetBlockLabelString(int idx);

void SetBlockLabelString(uint8_t block_type, int idx, const char* label);

//uint8_t GetBlockIndexByLabel (const char * label);

//void SaveEventBuffer(void);

void UpdateStateRegister(uint8_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val);

uint8_t GetBlockTypeOffset (uint8_t block_type);

UIDataSet* GetUIDataSet(void);

void InitStateRegister(void);


#endif /* SRC_FF_REGISTRY_H_ */
