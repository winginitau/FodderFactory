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
#include "ff_utils.h"

#ifdef FF_SIMULATOR
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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
	char interface[MAX_LABEL_LENGTH];
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
	uint16_t block_id;
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
static uint16_t block_count = 0;

/************************************************
 Functions
************************************************/


uint16_t GetBlockID(const char* label) {
	BlockNode* temp;

	temp = bll;

	while (temp != NULL) {
		if (strcmp(temp->block_label, label) == 0) {
			return temp->block_id;
		} else {
			temp = temp->next_block;
		}
	}
	DebugLog("ERROR: Block Label Not Found");
	return 65535;
}


BlockNode* AddBlock (BlockNode** head_ref, uint8_t block_cat, const char *block_label) {
	BlockNode* new_block;

	if(*head_ref == NULL) {   //empty list
		new_block = (BlockNode *)malloc(sizeof(BlockNode));
		new_block->block_cat = block_cat;
		strcpy(new_block->block_label, block_label);
		new_block->block_id = block_count;
		block_count++;
		new_block->active = 0;
		new_block->block_type = 255;
		new_block->next_block = NULL;
		*head_ref = new_block;
		return new_block;
	}
	return AddBlock(&((*head_ref)->next_block), block_cat, block_label);

}


BlockNode* GetBlock (BlockNode *list_node, uint8_t block_cat, const char *block_label) {
	//BlockNode *block;

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

uint8_t ConfigureBlock(uint8_t block_cat, const char *block_label, const char *key_str, const char *value_str) {
	BlockNode *block_ptr;
	uint8_t return_value = 1;  //error by exception
	uint8_t last_key = 255;
	//uint16_t block_id = 0;

	block_ptr = GetBlock(bll, block_cat, block_label);

	if (block_ptr == NULL) {
		block_ptr = AddBlock(&bll, block_cat, block_label); //add a new one
	}
	if (block_ptr != NULL) {

		//assume we now have a valid block_ptr pointing to a categorised and labelled block

		if(key_str == NULL) {	//registration only nothing more to do.
			return return_value;
		}

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

		while ((strcmp(key_str, block_cat_defs[block_cat].conf_keys[key_idx]) != 0) && key_idx < last_key) {
			key_idx++;
		}
		if (key_idx == last_key) {
			DebugLog("ERROR (ConFigureBlock) Key String Not Found in Block Category Definitions");
			return_value = 0;
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

			} else { //key_idx specific to category ie. > IN_DESCRIPTION

				switch (block_cat) {
				case FF_SYSTEM:
					//uint8_t temp_scale;
					//uint8_t language;
					//uint8_t week_start;
					break;

				case FF_INPUT:
					switch (key_idx) {

					case IN_INTERFACE:
						strcpy(block_ptr->settings.in.interface, value_str);
						break;

					case IN_IF_NUM:
						block_ptr->settings.in.if_num = atoi(value_str);
						break;

					case IN_LOG_RATE:
						block_ptr->settings.in.log_rate = StringToFFTime(value_str);
						break;

					case IN_DATA_UNITS: {
						uint8_t u = 0;
						while (u < LAST_UNIT_SCALE && strcmp(unit_strings[u].text[ENGLISH], value_str)) {
							u++;
						}
						if (u < LAST_UNIT_SCALE) {
							block_ptr->settings.in.data_units = u;
						} else {
							block_ptr->settings.in.data_units = 255;
							DebugLog("ERROR: Missing or malformed input data_units in config file");
							return_value = 0;
						}
						break;
					}

					case IN_DATA_TYPE:
						//so what - either float or int presently - inferred from block type, conider dropping
						break;
					default:
						DebugLog("ERROR: In static block_cat_defs in FF_INPUT setting data");
						return_value = 0;
						break;
					} // switch(key_idx)
					break; //switch (block_cat);


				case FF_MONITOR:
					switch (key_idx) {

					case MON_INPUT1:
						block_ptr->settings.mon.input1 = GetBlockID(value_str);
						break;
					case MON_INPUT2:
						block_ptr->settings.mon.input2 = GetBlockID(value_str);
						break;
					case MON_INPUT3:
						block_ptr->settings.mon.input3 = GetBlockID(value_str);
						break;
					case MON_INPUT4:
						block_ptr->settings.mon.input4 = GetBlockID(value_str);
						break;
					case MON_ACT_VAL:
						sscanf(value_str, "%f", &(block_ptr->settings.mon.act_val));
						break;
					case MON_DEACT_VAL:
						sscanf(value_str, "%f", &(block_ptr->settings.mon.deact_val));
						break;
					default:
						DebugLog("ERROR: In static block_cat_defs in FF_MONITOR setting data");
						return_value = 0;
						break;
					} // switch(key_idx)
					break; //switch (block_cat);

				case FF_SCHEDULE:
					switch (key_idx) {

					case SCH_DAYS:
						if (DayStrToFlag(block_ptr->settings.sch.days, value_str) != 1) {
							DebugLog("WARNING: No Days Found Converting Day String to Flag");
						}
						break;
					case SCH_TIME_START:
						block_ptr->settings.sch.time_start = StringToFFTime(value_str);
						break;
					case SCH_TIME_END:
						block_ptr->settings.sch.time_end = StringToFFTime(value_str);
						break;
					case SCH_TIME_DURATION:
						block_ptr->settings.sch.time_duration = StringToFFTime(value_str);
						break;
					case SCH_TIME_REPEAT:
						block_ptr->settings.sch.time_repeat = StringToFFTime(value_str);
						break;
					default:
						DebugLog("ERROR: In static block_cat_defs in FF_SCHEDULE setting data");
						return_value = 0;
						break;
					} // switch(key_idx)
					break; //switch (block_cat);

				case FF_RULE:
					switch (key_idx) {

					case RL_PARAM_1:
						block_ptr->settings.rl.param1 = GetBlockID(value_str);
						break;
					case RL_PARAM_2:
						block_ptr->settings.rl.param2 = GetBlockID(value_str);
						break;
					case RL_PARAM_3:
						block_ptr->settings.rl.param3 = GetBlockID(value_str);
						break;
					case RL_PARAM_NOT:
						block_ptr->settings.rl.param_not = GetBlockID(value_str);
						break;
					default:
						DebugLog("ERROR: In static block_cat_defs in FF_RULE setting data");
						return_value = 0;
						break;
					} // switch(key_idx)
					break; //switch (block_cat);

				case FF_CONTROLLER:
					switch (key_idx) {

					case CON_RULE:
						block_ptr->settings.con.rule = GetBlockID(value_str);
						break;
					case CON_OUTPUT:
						block_ptr->settings.con.output = GetBlockID(value_str);
						break;
					case CON_ACT_CMD: {
						uint8_t c = 0;
						while (c < LAST_COMMAND && strcmp(command_strings[c], value_str)) {
							c++;
						}
						if (c < LAST_COMMAND) {
							block_ptr->settings.con.act_cmd = c;
						} else {
							block_ptr->settings.con.act_cmd = 255;
							DebugLog("ERROR: Valid controller ACT_CMD string not defined in config");
							return_value = 0;
						}
						break;
					}
					case CON_DEACT_CMD: {
						uint8_t c = 0;
						while (c < LAST_COMMAND && strcmp(command_strings[c], value_str)) {
							c++;
						}
						if (c < LAST_COMMAND) {
							block_ptr->settings.con.deact_cmd = c;
						} else {
							block_ptr->settings.con.deact_cmd = 255;
							DebugLog("ERROR: Valid controller DEACT_CMD string not defined in config");
							return_value = 0;
						}
						break;
					}
					default:
						DebugLog("ERROR: In static block_cat_defs in FF_CONTROLLER setting data");
						return_value = 0;
						break;

					} // switch(key_idx)
					break; //switch (block_cat);

				case FF_OUTPUT:
					switch (key_idx) {

					case OUT_DIGITAL_PIN:
						block_ptr->settings.out.out_digital_pin = atoi(value_str);
						break;

					//uint8_t out_digital_pin;
					default:
						DebugLog("ERROR: In static block_cat_defs in FF_OUTPUT setting data");
						return_value = 0;
						break;

					} // switch(key_idx)
					break; //switch (block_cat);

				default:
					DebugLog("ERROR: (ConfigureBlock) Invalid Block Category");
					return_value = 0;
					break;
				} //switch(block_cat)
			}; //else : key_idx spec to cat
		} //else - key was found in cat idx
	} else {
		DebugLog("ERROR: (ConfigureBlock) Finding or Adding Block ");
		return_value = 0;
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






