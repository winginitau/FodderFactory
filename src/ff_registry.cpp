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

#include "ff_inputs.h"
#include "ff_monitors.h"
#include "ff_schedules.h"
#include "ff_rules.h"
#include "ff_controllers.h"
#include "ff_outputs.h"


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


typedef struct FF_STATE_REGISTER {
	//system config ad flags
	uint8_t language;
	uint8_t temperature_scale;
	uint8_t time_real_status;
	uint8_t FS_present;
	uint8_t config_valid;
	uint8_t save_event_buffer;
	UIDataSet ui_data;
} FFStateRegister;


/************************************************
 Globals
************************************************/

static FFStateRegister sr;
static uint16_t block_count = 0;
static BlockNode *bll = NULL;		//Block Linked List - variant record block list


/************************************************
 Functions
************************************************/

void Setup(BlockNode *b) {
	switch (b->block_cat) {
	case FF_SYSTEM:
		//TODO
		break;
	case FF_INPUT:
		InputSetup(b);
		break;
	case FF_MONITOR:
		MonitorSetup(b);
		break;
	case FF_SCHEDULE:
		ScheduleSetup(b);
		break;
	case FF_RULE:
		RuleSetup(b);
		break;
	case FF_CONTROLLER:
		ControllerSetup(b);
		break;
	case FF_OUTPUT:
		OutputSetup(b);
		break;
	default:
		DebugLog("ERROR: Block Category Not Matched in Setup");
		break;
	}
}

void Operate(BlockNode *b) {
	switch (b->block_cat) {
	case FF_SYSTEM:
		//TODO
		break;
	case FF_INPUT:
		InputOperate(b);
		break;
	case FF_MONITOR:
		MonitorOperate(b);
		break;
	case FF_SCHEDULE:
		ScheduleOperate(b);
		break;
	case FF_RULE:
		RuleOperate(b);
		break;
	case FF_CONTROLLER:
		ControllerOperate(b);
		break;
	case FF_OUTPUT:
		OutputOperate(b);
		break;
	default:
		DebugLog("ERROR: Block Category Not Matched in Operate");
		break;
	}
}


void ProcessDispatcher(void(*func)(BlockNode*)) {
	BlockNode* block_ptr;

	block_ptr = bll;

	while (block_ptr != NULL) {
		func(block_ptr);
		block_ptr = block_ptr->next_block;
	}
}

BlockNode* GetBlockByID(BlockNode *list_node, uint16_t block_id) {

	if(list_node == NULL) {   //empty list
		return NULL;
	} else {
		if (list_node->block_id == block_id) {
			return list_node;
		} else {
			list_node = GetBlockByID(list_node->next_block, block_id);
		}
	}
	return list_node;
}

uint8_t SetCommand(uint16_t block_id, uint8_t cmd_msg) {
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b) {
		if (b->block_cat == FF_OUTPUT) {
			b->settings.out.command = cmd_msg;
			return cmd_msg;
		} else {
			DebugLog("ERROR: (SetCommand) message sent to block that is not category FF_OUTPUT");
			return M_FF_ERROR;
		}
	} else {
		DebugLog("ERROR: (SetCommand) call to GetBlockByID returned a NULL pointer");
		return M_FF_ERROR;
	}
}

uint8_t IsActive(uint16_t block_id) {
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b) {
		return b->active;
	} else {
		DebugLog("ERROR: (IsActive) call to GetBlockByID returned a NULL pointer");
		return M_FF_ERROR;
	}
}

float GetFVal(uint16_t block_id) {
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b) {
		return b->f_val;
	} else {
		DebugLog("ERROR: (GetFVal) call to GetBlockByID returned a NULL pointer");
		return M_FF_ERROR;
	}
}

uint8_t GetBVal(uint16_t block_id) {
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b) {
		return b->bool_val;
	} else {
		DebugLog("ERROR: (GetBVal) call to GetBlockByID returned a NULL pointer");
		return M_FF_ERROR;
	}
}


uint16_t GetBlockID(const char* label) {
	BlockNode* temp;
	char debug_msg[MAX_LOG_LINE_LENGTH];

	temp = bll;

	while (temp != NULL) {
		if (strcmp(temp->block_label, label) == 0) {
			return temp->block_id;
		} else {
			temp = temp->next_block;
		}
	}
	sprintf(debug_msg, "ERROR: Block Label Not Found: [%s]", label);
	DebugLog(debug_msg);
	return M_FF_ERROR;
}

char const* GetBlockLabelString(uint16_t block_id) {
	BlockNode* temp;
	char debug_msg[MAX_LOG_LINE_LENGTH];

	temp = bll;

	while (temp != NULL) {
		if (temp->block_id == block_id) {
				return temp->block_label;
			} else {
				temp = temp->next_block;
			}
		}
		sprintf(debug_msg, "ERROR: Block ID Not Found: [%d]", block_id);
		DebugLog(debug_msg);
		return NULL;
}

BlockNode* AddBlock(BlockNode** head_ref, uint8_t block_cat, const char *block_label) {
	BlockNode* new_block;

	if (*head_ref == NULL) {   //empty list
		// common settings and setting holders for al blocks
		new_block = (BlockNode *) malloc(sizeof(BlockNode));
		new_block->next_block = NULL;

		new_block->block_cat = block_cat;
		new_block->block_type = UINT8_INIT;
		if (block_cat == FF_SYSTEM) {
			new_block->block_id = SSS;
		} else {
			new_block->block_id = BLOCK_ID_BASE + block_count;
			block_count++;
		}
		strcpy(new_block->block_label, block_label);
		new_block->display_name[0] = '\0';
		new_block->description[0] = '\0';

		// operational, run-time block data
		new_block->active = 0;
		new_block->bool_val = 0;
		new_block->int_val = UINT8_INIT;
		new_block->f_val = FLOAT_INIT;
		new_block->last_update = UINT32_INIT;
		new_block->status = STATUS_INIT;

		switch (block_cat) {
			case FF_SYSTEM:

				new_block->settings.sys.language = UINT8_INIT;
				new_block->settings.sys.temp_scale = UINT8_INIT;
				new_block->settings.sys.week_start = UINT8_INIT;
				break;
			case FF_INPUT:
				new_block->settings.in.interface = LAST_INTERFACE;
				new_block->settings.in.if_num = UINT8_INIT;
				new_block->settings.in.log_rate = 0;
				new_block->settings.in.data_units = UINT8_INIT;
				new_block->settings.in.data_type = UINT8_INIT;
				break;
			case FF_MONITOR:
				new_block->settings.mon.input1 = UINT16_INIT;
				new_block->settings.mon.input2 = UINT16_INIT;
				new_block->settings.mon.input3 = UINT16_INIT;
				new_block->settings.mon.input4 = UINT16_INIT;
				new_block->settings.mon.act_val = FLOAT_INIT;
				new_block->settings.mon.deact_val = FLOAT_INIT;
				break;
			case FF_SCHEDULE:
				for (int i = 0; i < LAST_DAY; i++ ) new_block->settings.sch.days[i] = UINT8_INIT;
				new_block->settings.sch.time_start = UINT32_INIT;
				new_block->settings.sch.time_end = UINT32_INIT;
				new_block->settings.sch.time_duration = 0;
				new_block->settings.sch.time_repeat = 0;
				break;
			case FF_RULE:
				new_block->settings.rl.param1 = UINT16_INIT;
				new_block->settings.rl.param2 = UINT16_INIT;
				new_block->settings.rl.param3 = UINT16_INIT;
				new_block->settings.rl.param_not = UINT16_INIT;
				break;
			case FF_CONTROLLER:
				new_block->settings.con.rule = UINT16_INIT;
				new_block->settings.con.output = UINT16_INIT;
				new_block->settings.con.act_cmd = UINT8_INIT;
				new_block->settings.con.deact_cmd= UINT8_INIT;
				break;
			case FF_OUTPUT:
				new_block->settings.out.interface = LAST_INTERFACE;
				new_block->settings.out.if_num = UINT8_INIT;
				break;
			}
		*head_ref = new_block;
		return new_block;
	}
	return AddBlock(&((*head_ref)->next_block), block_cat, block_label);

}


BlockNode* GetBlockByLabel(BlockNode *list_node, const char *block_label) {
	//BlockNode *block;

	if(list_node == NULL) {   //empty list
		return NULL;
	} else {
		if (strcmp(list_node->block_label, block_label) == 0) {
			return list_node;
		} else {
			list_node = GetBlockByLabel(list_node->next_block, block_label);
		}
	}
	return list_node;
}

uint8_t GetConfKeyIndex(uint8_t block_cat, const char* key_str) {

	uint8_t last_key = UINT8_INIT;
	uint8_t key_idx = 0; //see "string_consts.h" Zero is error.

	//lock the last key index to the appropriate block category

	switch (block_cat) {
		case FF_ERROR_CAT:
			DebugLog("STOP (GetConfKeyIndex) block_cat = FF_ERROR_CAT");
			while(1);
			break;
		case FF_SYSTEM:
			last_key = LAST_SYS_KEY_TYPE;
			break;
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
			DebugLog("STOP (GetConfKeyIndex) block_cat >= LAST_BLOCK_CAT");
			while(1);
	}

	//check that we have a key that matches one of the keys strings of the block category
	while ((strcmp(key_str, block_cat_defs[block_cat].conf_keys[key_idx]) != 0) && key_idx < last_key) {
		key_idx++;
	}

	if (key_idx == last_key) {
		DebugLog("STOP (GetConfKeyIndex) Key String Not Found in Block Category Definitions");
		while(1);
	} else {
		return key_idx;
	}
}

uint8_t ConfigureCommonSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str){
	switch (key_idx) {
		case SYS_ERROR_KEY:
			// or any other (0) block cat error key
			DebugLog("STOP (ConfigureCommonSetting) key_idx = 0 error key type");
			while(1);
			break;
		case SYS_TYPE:
			// or case IN_TYPE
			// or case MON_TYPE:
			// or case SCH_TYPE:
			// or case RL_TYPE:
			// or case CON_TYPE:
			// or case OUT_TYPE:
			block_ptr->block_type = BlockTypeStringArrayIndex(value_str);
			break;
		case SYS_DISPLAY_NAME:
			// or case IN_DISPLAY_NAME
			// or case MON_DISPLAY_NAME:
			// or case SCH_DISPLAY_NAME:
			// or case RL_DISPLAY_NAME:
			// or case CON_DISPLAY_NAME:
			// or case OUT_DISPLAY_NAME:
			strcpy(block_ptr->display_name, value_str);
			break;
		case SYS_DESCRIPTION:
			// or case IN_DESCRIPTION
			// or case MON_DESCRIPTION:
			// or case SCH_DESCRIPTION:
			// or case RL_DESCRIPTION:
			// or case CON_DESCRIPTION:
			// or case OUT_DESCRIPTION:
			strcpy(block_ptr->description, value_str);
			break;
		default:
			return 0; //error
			break;
	}
	return 1;
}

uint8_t ConfigureSYSSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case SYS_LANGUAGE:
			block_ptr->settings.sys.language = LanguageStringArrayIndex(value_str);
			break;
		case SYS_TEMPERATURE:
			block_ptr->settings.sys.temp_scale = UnitStringArrayIndex(value_str);
			break;
		case SYS_WEEK_START:
			block_ptr->settings.sys.week_start = DayStringArrayIndex(value_str);
			break;
		default:
			DebugLog("ERROR: In static block_cat_defs in FF_SYSTEM setting data");
			return 0;
			break;
	} // switch(key_idx)
	return 1;
}

uint8_t ConfigureINSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case IN_INTERFACE:
			block_ptr->settings.in.interface = InterfaceStringArrayIndex(value_str);
			break;
		case IN_IF_NUM:
			block_ptr->settings.in.if_num = atoi(value_str);
			break;
		case IN_LOG_RATE: {
			//tm time_tm;
			//strptime(value_str, "%H:%M:%S", &time_tm);
			block_ptr->settings.in.log_rate = StrToTV(value_str);
			break;
		}
		case IN_DATA_UNITS: {
			uint8_t u = 0;
			while (u < LAST_UNIT && strcmp(unit_strings[u].text[ENGLISH], value_str)) {
				u++;
			}
			if (u < LAST_UNIT) {
				block_ptr->settings.in.data_units = u;
			} else {
				block_ptr->settings.in.data_units = 255;
				DebugLog("ERROR: Missing or malformed input data_units in config file");
				return 0;
			}
			break;
		}
		case IN_DATA_TYPE:
			//so what - either float or int presently - inferred from block type, conider dropping
			break;
		default:
			DebugLog("ERROR: In static block_cat_defs in FF_INPUT setting data");
			return 0;
			break;
	} // switch(key_idx)
	return 1;
}

uint8_t ConfigureMONSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
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
			// check first for IN_DIGITAL boolean values
			if (strcmp(value_str, "HIGH") == 0) {
				block_ptr->settings.mon.act_val = 1;
			} else {
				if (strcmp(value_str, "LOW") == 0) {
					block_ptr->settings.mon.act_val = 0;
				} else {
					// must be a numeric float value
					sscanf(value_str, "%f", &(block_ptr->settings.mon.act_val));
				}
			}
			break;
		case MON_DEACT_VAL:
			// check first for IN_DIGITAL boolean values
			if (strcmp(value_str, "HIGH") == 0) {
				block_ptr->settings.mon.deact_val = 1;
			} else {
				if (strcmp(value_str, "LOW") == 0) {
					block_ptr->settings.mon.deact_val = 0;
				} else {
					// must be a numeric float value
					sscanf(value_str, "%f", &(block_ptr->settings.mon.deact_val));
				}
			}
			break;
		default:
			DebugLog("ERROR: In static block_cat_defs in FF_MONITOR setting data");
			return 0;
			break;
	} // switch(key_idx)
	return 1;
}

uint8_t ConfigureSCHSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case SCH_DAYS:
			if (DayStrToFlag(block_ptr->settings.sch.days, value_str) != 1) {
				DebugLog("WARNING: No Days Found Converting Day String to Flag");
			}
			break;
		case SCH_TIME_START: {
			block_ptr->settings.sch.time_start = StrToTV(value_str);
			break;
		}
		case SCH_TIME_END: {
			block_ptr->settings.sch.time_end = StrToTV(value_str);
			break;
		}
		case SCH_TIME_DURATION: {
			block_ptr->settings.sch.time_duration = StrToTV(value_str);
			break;
		}
		case SCH_TIME_REPEAT: {
			block_ptr->settings.sch.time_repeat = StrToTV(value_str);
			break;
		}
		default:
			DebugLog("ERROR: In static block_cat_defs in FF_SCHEDULE setting data");
			return 0;
			break;
	} // switch(key_idx)

	return 1;
}

uint8_t ConfigureRLSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
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
			return 0;
			break;
	} // switch(key_idx)

	return 1;
}

uint8_t ConfigureCONSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case CON_RULE:
			block_ptr->settings.con.rule = GetBlockID(value_str);
			break;
		case CON_OUTPUT:
			block_ptr->settings.con.output = GetBlockID(value_str);
			break;
		case CON_ACT_CMD: {
			uint8_t c = 0;
			while (c < LAST_COMMAND && strcmp(command_strings[c].text, value_str)) {
				c++;
			}
			if (c < LAST_COMMAND) {
				block_ptr->settings.con.act_cmd = c;
			} else {
				block_ptr->settings.con.act_cmd = UINT8_INIT;
				DebugLog("ERROR: Valid controller ACT_CMD string not defined in config");
				return 0;
			}
			break;
		}
		case CON_DEACT_CMD: {
			uint8_t c = 0;
			while (c < LAST_COMMAND && strcmp(command_strings[c].text, value_str)) {
				c++;
			}
			if (c < LAST_COMMAND) {
				block_ptr->settings.con.deact_cmd = c;
			} else {
				block_ptr->settings.con.deact_cmd = UINT8_INIT;
				DebugLog("ERROR: Valid controller DEACT_CMD string not defined in config");
				return 0;
			}
			break;
		}
		default:
			DebugLog("ERROR: In static block_cat_defs in FF_CONTROLLER setting data");
			return 0;
			break;
	} // switch(key_idx)
	return 1;
}

uint8_t ConfigureOUTSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case OUT_INTERFACE:
			block_ptr->settings.out.interface = InterfaceStringArrayIndex(value_str);
			break;
		case OUT_IF_NUM:
			block_ptr->settings.out.if_num = atoi(value_str);
			break;
		default:
			DebugLog("ERROR: In static block_cat_defs in FF_OUTPUT setting data");
			return 0;
			break;
	} // switch(key_idx)
	return 1;
}

uint8_t ConfigureBlock(uint8_t block_cat, const char *block_label, const char *key_str, const char *value_str) {
	BlockNode *block_ptr;
	uint8_t return_value = 1;  //error by exception
	uint8_t key_idx = 0; //see "string_consts.h" Zero is error.

	// First search for an existing block with that label
	block_ptr = GetBlockByLabel(bll, block_label);

	//not found - add a new block
	if (block_ptr == NULL) {
		block_ptr = AddBlock(&bll, block_cat, block_label); //add a new one
	}
	if (block_ptr != NULL) {
		//we now have a valid block_ptr pointing to a categorised and labelled block

		//are we just registering or updating details?
		if (key_str == NULL) {		//registration only nothing more to do.
			return return_value;
		}

		key_idx = GetConfKeyIndex(block_cat, key_str);

		if (key_idx <= SYS_DESCRIPTION) {
			return (ConfigureCommonSetting(block_ptr, key_idx, value_str));
		}

		//key idx must be in range of defined keys
		//but they are different for each category
		switch (block_cat) {
			case FF_SYSTEM:
				return ConfigureSYSSetting(block_ptr, key_idx, value_str);
				break; //switch (block_cat);
			case FF_INPUT:
				return ConfigureINSetting(block_ptr, key_idx, value_str);
				break; //switch (block_cat);
			case FF_MONITOR:
				return ConfigureMONSetting(block_ptr, key_idx, value_str);
				break; //switch (block_cat);
			case FF_SCHEDULE:
				return ConfigureSCHSetting(block_ptr, key_idx, value_str);
				break; //switch (block_cat);
			case FF_RULE:
				return ConfigureRLSetting(block_ptr, key_idx, value_str);
				break; //switch (block_cat);
			case FF_CONTROLLER:
				return ConfigureCONSetting(block_ptr, key_idx, value_str);
				break; //switch (block_cat);
			case FF_OUTPUT:
				return ConfigureOUTSetting(block_ptr, key_idx, value_str);
				break; //switch (block_cat);
			default:
				DebugLog("ERROR: (ConfigureBlock) Invalid Block Category");
				return_value = 0;
				break;
		} //switch(block_cat)

	} else {
		DebugLog("ERROR: (ConfigureBlock) Finding or Adding Block ");
		return_value = 0;
	}
	return return_value;
}






UIDataSet* GetUIDataSet(void) {
	return &sr.ui_data;
}

void UpdateStateRegister(uint16_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val) {

	//TODO include further registry block logic update here

	//TODO redo - remove hard coding perhaps
	const char* src_label;
	src_label = GetBlockLabelString(source);

	if (src_label) {
		if (strcmp(src_label, "INSIDE_TOP_TEMP") == 0) {
			sr.ui_data.inside_current = f_val;
			if (f_val < sr.ui_data.inside_min) {
				sr.ui_data.inside_min = f_val;
				sr.ui_data.inside_min_dt = time(NULL);
			}
			if (f_val > sr.ui_data.inside_max) {
				sr.ui_data.inside_max = f_val;
				sr.ui_data.inside_max_dt = time(NULL);
			}
		}

		if (strcmp(src_label, "OUTSIDE_TEMP") == 0) {
			sr.ui_data.outside_current = f_val;
			if (f_val < sr.ui_data.outside_min) {
				sr.ui_data.outside_min = f_val;
				sr.ui_data.outside_min_dt = time(NULL);
			}
			if (f_val > sr.ui_data.outside_max) {
				sr.ui_data.outside_max = f_val;
				sr.ui_data.outside_max_dt = time(NULL);
			}
		}

		if (strcmp(src_label, "WATER_TEMP") == 0) {
			sr.ui_data.water_current = f_val;
			if (f_val < sr.ui_data.water_min) {
				sr.ui_data.water_min = f_val;
				sr.ui_data.water_min_dt = time(NULL);
			}
			if (f_val > sr.ui_data.water_max) {
				sr.ui_data.water_max = f_val;
				sr.ui_data.water_max_dt = time(NULL);
			}
		}

		if (strcmp(src_label, "RESET_MIN_MAX") == 0) {
			sr.ui_data.inside_min = sr.ui_data.inside_current;
			sr.ui_data.inside_min_dt = time(NULL);
			sr.ui_data.inside_max = sr.ui_data.inside_current;
			sr.ui_data.inside_max_dt = time(NULL);

			sr.ui_data.outside_min = sr.ui_data.outside_current;
			sr.ui_data.outside_min_dt = time(NULL);
			sr.ui_data.outside_max = sr.ui_data.outside_current;
			sr.ui_data.outside_max_dt = time(NULL);

			sr.ui_data.water_min = sr.ui_data.water_current;
			sr.ui_data.water_min_dt = time(NULL);
			sr.ui_data.water_max = sr.ui_data.water_current;
			sr.ui_data.water_max_dt = time(NULL);
		}
	} else { //src_label Null
		DebugLog("ERROR Updating State Register - Source ID does not match a valid Label");
	}

	sr.ui_data.light_flag = 0;
	sr.ui_data.water_heater_flag = 0;
}

//TODO - should be moved after config parsing
void InitStateRegister(void) {

	DebugLog("[INIT] Initialising Persistence Store - State Register");

	sr.language = ENGLISH;
	sr.temperature_scale = CELSIUS;
	sr.time_real_status = 0; 	//false at this stage
	sr.FS_present = 0; 			//false at this stage
	sr.config_valid = 0;		//false at this stage
	sr.save_event_buffer = 0;


//TODO mod code due to config parsing
/*
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
*/


//initialise the min and max counters used for UI display
	sr.ui_data.inside_current = 0;
	sr.ui_data.inside_min = 100;
	sr.ui_data.inside_min_dt = time(NULL);
	sr.ui_data.inside_max = -50;
	sr.ui_data.inside_max_dt = time(NULL);

	sr.ui_data.outside_current = 0;
	sr.ui_data.outside_min = 100;
	sr.ui_data.outside_min_dt = time(NULL);
	sr.ui_data.outside_max = -50;
	sr.ui_data.outside_max_dt = time(NULL);

	sr.ui_data.water_current = 0;
	sr.ui_data.water_min = 100;
	sr.ui_data.water_min_dt = time(NULL);
	sr.ui_data.water_max = -50;
	sr.ui_data.water_max_dt = time(NULL);

	//TODO these will go
	sr.ui_data.light_flag = 0;
	sr.ui_data.water_heater_flag = 0;


}






