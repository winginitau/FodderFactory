/************************************************
 ff_registry.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Component Registry
************************************************/

/************************************************
 Includes
************************************************/
#include "ff_registry.h"
#include "ff_sys_config.h"
#include "ff_string_consts.h"
#include "ff_debug.h"

#ifdef FF_SIMULATOR
#include <string.h>
#include <stdlib.h>
#endif


/************************************************
 Data Structures
************************************************/

typedef struct BLOCK_TYPE {
	char label[MAX_LABEL_LENGTH];
	uint8_t active;
} Block;



typedef struct FF_SYSTEM_SETTINGS {
	uint8_t temp_scale;
	uint8_t language;
	uint8_t week_start;
} SystemSettings;

typedef struct FF_INPUT_SETTINGS {
	uint8_t interface;
	uint8_t if_num;
	uint8_t log_rate_day;
	uint8_t log_rate_hour;
	uint8_t log_rate_minute;
	uint8_t log_rate_second;
	uint16_t log_rate_millisec;
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
	uint8_t out_digital_pin;
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
	char block_label[MAX_LABEL_LENGTH];
	char display_name[MAX_LABEL_LENGTH];
	char description[MAX_DESCR_LENGTH];
	uint8_t active;
	BlockSettings settings;
};


typedef struct FF_STATE_REGISTER {
	//system config ad flags
	uint8_t language;
	uint8_t temperature_scale;
	uint8_t time_real_status = 0;
	uint8_t FS_present = 0;
	uint8_t config_valid;
	uint8_t save_event_buffer = 0;

	//blocks

	uint8_t block_cat_offset[LAST_BLOCK_CAT];

	uint8_t block_list_size;


	uint8_t system_start;
	uint8_t system_count;

	uint8_t input_start;
	uint8_t input_count;

	uint8_t monitor_start;
	uint8_t monitor_count;

	uint8_t schedule_start;
	uint8_t schedule_count;

	uint8_t rule_start;
	uint8_t rule_count;

	uint8_t controller_start;
	uint8_t controller_count;

	uint8_t output_start;
	uint8_t output_count;

	UIDataSet ui_data;

	Block block_list[BLOCK_COUNT];
	//Block* dev_block_list;

} FFStateRegister;


/************************************************
 Globals
************************************************/

static FFStateRegister sr;
static BlockNode *bll;		//Block Linked List - variant record block list

/************************************************
 Functions
************************************************/




BlockNode* AddBlock (BlockNode *list_node, uint8_t block_cat, const char *block_label) {


	if(list_node == NULL) {   //empty list
		list_node = (BlockNode *)malloc(sizeof(BlockNode));
		if (list_node) {
			list_node->block_cat = block_cat;
			strcpy(list_node->block_label, block_label);
			list_node->active = 0;
			list_node->block_type = 255;
			list_node->next_block = NULL;
		} else {
			list_node = AddBlock(list_node->next_block, block_cat, block_label);
		}
	}
	return list_node;
}


BlockNode* GetBlock (BlockNode *list_node, uint8_t block_cat, const char *block_label) {
	BlockNode *block;

	if(list_node == NULL) {   //empty list
		return NULL;
	} else {
		if (strcmp(list_node->block_label, block_label) == 0) {
			return list_node;
		} else {
			list_node = GetBlock(list_node->next_block, block_cat, block_label);
		}
	}
	return list_node;
}

uint8_t ConfigureBlock(uint8_t block_cat, const char *block_label,
		const char *key_str, const char *value_str) {
	BlockNode *block_ptr;
	uint8_t return_value = 0;  //error
	uint8_t last_key = 255;

	block_ptr = GetBlock(bll, block_cat, block_label);

	if (block_ptr == NULL) {
		block_ptr = AddBlock(bll, block_cat, block_label); //add a new one
	}
	if (block_ptr != NULL) {
		return_value = 1;

		//assume we now have a valid block_ptr pointing to a categorised and labelled block


		//lock the last key index to the appropriate block category
		switch (block_cat) {
		case FF_INPUT:
			last_key = LAST_IN_KEY_TYPE;
			break;
		case FF_MONITOR:
			last_key = LAST_MON_KEY_TYPE;
			break;
		case FF_SCHEDULE:
			last_key = LAST_SCH_KEY_TYPE;
			break;
		case FF_RULE:
			last_key = LAST_RL_KEY_TYPE;
			break;
		case FF_CONTROLLER:
			last_key = LAST_CON_KEY_TYPE;
			break;
		case FF_OUTPUT:
			last_key = LAST_OUT_KEY_TYPE;
			break;

		default:
			;
		}

		//check that we have a key that matches one of the keys of the block category
		uint8_t key_idx = 0;
		while ((strcmp(key_str, block_cat_defs[block_cat].conf_keys[key_idx])
				!= 0) && key_idx < last_key) {
			key_idx++;
		}
		if (key_idx == last_key) {
			DebugLog(
					"ERROR (ConFigureBlock) Key String Not Found in Block Category Definitions");
		} else {
			//we have a match to a valid key

			//check for keys that are common to all block categories
			if (key_idx <= IN_DESCRIPTION) {
				switch (key_idx) {
				case IN_TYPE:
					// or case MON_TYPE:
					// or case SCH_TYPE:
					// or case RL_TYPE:
					// or case CON_TYPE:
					// or case OUT_TYPE:
					block_ptr->block_type = SimpleStringArrayIndex(block_type_strings, value_str);
					break;
				case IN_DISPLAY_NAME:
					// or case MON_DISPLAY_NAME:
					// or case SCH_DISPLAY_NAME:
					// or case RL_DISPLAY_NAME:
					// or case CON_DISPLAY_NAME:
					// or case OUT_DISPLAY_NAME:
					strcpy(block_ptr->display_name, value_str);
					break;
				case IN_DESCRIPTION:
					// or case MON_DESCRIPTION:
					// or case SCH_DESCRIPTION:
					// or case RL_DESCRIPTION:
					// or case CON_DESCRIPTION:
					// or case OUT_DESCRIPTION:
					strcpy(block_ptr->description, value_str);
					break;
				default:
					break;
				} //switch key_idx

			} else { //key_idx specific to category

				switch (block_cat) {
				case FF_SYSTEM:
					//uint8_t temp_scale;
					//uint8_t language;
					//uint8_t week_start;
					break;

				case FF_INPUT:
					switch (key_idx) {
					case IN_INTERFACE:
					case IN_IF_NUM:
					case IN_LOG_RATE_DAY:
					case IN_LOG_RATE_HOUR:
					case IN_LOG_RATE_MINUTE:
					case IN_LOG_RATE_SECOND:
					case IN_LOG_RATE_MILLISEC:
					case IN_DATA_UNITS:
					case IN_DATA_TYPE:
					default:
						break;
					} // switch(key_idx)
					break; //switch (block_cat);

					/*
					 uint8_t interface;
					 uint8_t if_num;
					 uint8_t log_rate_day;
					 uint8_t log_rate_hour;
					 uint8_t log_rate_minute;
					 uint8_t log_rate_second;
					 uint16_t log_rate_millisec;
					 uint8_t data_units;
					 uint8_t data_type;		// float, int
					 */

				case FF_MONITOR:
					uint16_t input1;
					uint16_t input2;
					uint16_t input3;
					uint16_t input4;
					float act_val;
					float deact_val;
					break;

				case FF_SCHEDULE:
					uint8_t days[7];
					FFTime time_start;
					FFTime time_end;
					FFTime time_duration;
					FFTime time_repeat;
					break;

				case FF_RULE:
					uint16_t param1;
					uint16_t param2;
					uint16_t param3;
					uint16_t param_not;
					break;

				case FF_CONTROLLER:
					uint16_t rule;
					uint16_t output;
					uint8_t act_cmd;
					uint8_t deact_cmd;
					break;

				case FF_OUTPUT:
					uint8_t out_digital_pin;
					break;
				default:
					DebugLog("ERROR: (ConfigureBlock) Invalid Block Category");
					return_value = 0;
					break;
				} //switch(block_cat)
			}; //else : key_idx spec to cat
		} //else - key was found in cat idx
	} else {
		DebugLog("ERROR: (ConfigureBlock) Finding or Adding Block ");
	}
	return return_value;
}



char const* GetBlockLabelString(int idx) {
	return sr.block_list[idx].label;
}

void SetBlockLabelString(uint8_t block_cat, int idx, const char* label) {
	switch (block_cat) {
	case FF_SYSTEM:
		idx = idx + sr.system_start;
		break;
	case FF_INPUT:
		idx = idx + sr.input_start;
		break;
	case FF_MONITOR:
		idx = idx + sr.monitor_start;
		break;
	case FF_SCHEDULE:
		idx = idx + sr.schedule_start;
		break;
	case FF_RULE:
		idx = idx + sr.rule_start;
		break;
	case FF_CONTROLLER:
		idx = idx + sr.controller_start;
		break;
	case FF_OUTPUT:
		idx = idx + sr.output_start;
		break;
	}
	strcpy(sr.block_list[idx].label, label);
	sr.block_list[idx].active = 0;
}

uint8_t GetBlockIndexByLabel (const char * label) {
	int i = 0;
	while (i < sr.block_list_size && !strcmp(sr.block_list[i].label, label)) {
		i++;
	}
	return i;
}

uint8_t GetBlockTypeOffset (uint8_t block_type) {
	uint8_t offset;
	switch (block_type) {
	case FF_SYSTEM:
		offset=sr.system_start;
		break;
	case FF_INPUT:
		offset=sr.input_start;
		break;
	case FF_MONITOR:
		offset=sr.monitor_start;
		break;
	case FF_SCHEDULE:
		offset=sr.schedule_start;
		break;
	case FF_RULE:
		offset=sr.rule_start;
		break;
	case FF_CONTROLLER:
		offset=sr.controller_start;
		break;
	case FF_OUTPUT:
		offset=sr.output_start;
		break;
	}
	return offset;
}

UIDataSet* GetUIDataSet(void) {
	return &sr.ui_data;
}

void UpdateStateRegister(uint8_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val) {

	//TODO include further registry block logic update here

	//TODO optimise
	if (strcmp(sr.block_list[source].label, "INSIDE_TOP_TEMP") == 0) {
		sr.ui_data.inside_current = f_val;
		if (f_val < sr.ui_data.inside_min) {
			sr.ui_data.inside_min = f_val;
			sr.ui_data.inside_min_dt = FFDTNow();
		}
		if (f_val > sr.ui_data.inside_max) {
			sr.ui_data.inside_max = f_val;
			sr.ui_data.inside_max_dt = FFDTNow();
		}
	}

	if (strcmp(sr.block_list[source].label, "OUTSIDE_TEMP") == 0) {
		sr.ui_data.outside_current = f_val;
		if (f_val < sr.ui_data.outside_min) {
			sr.ui_data.outside_min = f_val;
			sr.ui_data.outside_min_dt = FFDTNow();
		}
		if (f_val > sr.ui_data.outside_max) {
			sr.ui_data.outside_max = f_val;
			sr.ui_data.outside_max_dt = FFDTNow();
		}
	}

	if (strcmp(sr.block_list[source].label, "WATER_TEMP") == 0) {
			sr.ui_data.water_current = f_val;
			if (f_val < sr.ui_data.water_min) {
				sr.ui_data.water_min = f_val;
				sr.ui_data.water_min_dt = FFDTNow();
			}
			if (f_val > sr.ui_data.water_max) {
				sr.ui_data.water_max = f_val;
				sr.ui_data.water_max_dt = FFDTNow();
			}
	}

	if (strcmp(sr.block_list[source].label, "RESET_MIN_MAX") == 0) {
				sr.ui_data.inside_min = sr.ui_data.inside_current;
				sr.ui_data.inside_min_dt = FFDTNow();
				sr.ui_data.inside_max = sr.ui_data.inside_current;
				sr.ui_data.inside_max_dt = FFDTNow();

				sr.ui_data.outside_min = sr.ui_data.outside_current;
				sr.ui_data.outside_min_dt = FFDTNow();
				sr.ui_data.outside_max = sr.ui_data.outside_current;
				sr.ui_data.outside_max_dt = FFDTNow();

				sr.ui_data.water_min = sr.ui_data.water_current;
				sr.ui_data.water_min_dt = FFDTNow();
				sr.ui_data.water_max = sr.ui_data.water_current;
				sr.ui_data.water_max_dt = FFDTNow();
	}

	sr.ui_data.light_flag = 0;
	sr.ui_data.water_heater_flag = 0;
}

void InitStateRegister(void) {
	sr.language = ENGLISH;
	sr.temperature_scale = DEG_C;
	sr.time_real_status = 0; 	//false at this stage
	sr.FS_present = 0; 			//false at this stage
	sr.config_valid = 0;		//false at this stage
	sr.save_event_buffer = 0;

//TODO mod code due to config parsing
	sr.block_list_size = BLOCK_COUNT;
	sr.system_start = 0;
	sr.system_count = 1; 		//manual for now - ref sys_config define

	strcpy(sr.block_list[sr.system_start].label, "SYSTEM");
	strcpy(sr.block_list[sr.block_list_size-1].label, "LAST_BLOCK"); //size declared +1

	sr.input_start = sr.system_start + sr.system_count;
	sr.input_count = INPUT_COUNT;
	sr.monitor_start = sr.input_start + sr.input_count;
	sr.monitor_count = 0; ///for now
	sr.schedule_start = sr.monitor_start + sr.monitor_count;
	sr.schedule_count = 0; //for now
	sr.rule_start = sr.schedule_start + sr.schedule_count;
	sr.rule_count = 0; //for now
	sr.controller_start = sr.rule_start + sr.rule_count;
	sr.controller_count = CONTROL_COUNT;
	sr.output_start = sr.controller_start + sr.controller_count;
	sr.output_count = OUTPUT_COUNT;

//initialise the min and max counters used for UI display
	sr.ui_data.inside_current = 0;
	sr.ui_data.inside_min = 100;
	sr.ui_data.inside_min_dt = FFDTNow();
	sr.ui_data.inside_max = -50;
	sr.ui_data.inside_max_dt = FFDTNow();

	sr.ui_data.outside_current = 0;
	sr.ui_data.outside_min = 100;
	sr.ui_data.outside_min_dt = FFDTNow();
	sr.ui_data.outside_max = -50;
	sr.ui_data.outside_max_dt = FFDTNow();

	sr.ui_data.water_current = 0;
	sr.ui_data.water_min = 100;
	sr.ui_data.water_min_dt = FFDTNow();
	sr.ui_data.water_max = -50;
	sr.ui_data.water_max_dt = FFDTNow();

	//TODO these will go
	sr.ui_data.light_flag = 0;
	sr.ui_data.water_heater_flag = 0;

	bll = NULL;
}






