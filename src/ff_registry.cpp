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
		DebugLog(SSS, E_ERROR, M_SET_BCAT_NOT_MATCHED);
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
		DebugLog(SSS, E_ERROR, M_OPR_BCAT_NOT_MATCHED);
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
			DebugLog(b->block_id, E_ERROR, M_CMD_TO_BLOCK_NOT_OUTPUT);
			return M_FF_ERROR;
		}
	} else {
		DebugLog(SSS, E_ERROR, M_CMD_BID_NULL);
		return M_FF_ERROR;
	}
}

uint8_t IsActive(uint16_t block_id) {
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b) {
		return b->active;
	} else {
		DebugLog(SSS, E_ERROR, M_IA_NULL);
		return M_FF_ERROR;
	}
}

float GetFVal(uint16_t block_id) {
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b) {
		return b->f_val;
	} else {
		DebugLog(SSS, E_ERROR, M_GFV_NULL);
		return M_FF_ERROR;
	}
}

uint8_t GetBVal(uint16_t block_id) {
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b) {
		return b->bool_val;
	} else {
		DebugLog(SSS, E_ERROR, M_GBV_NULL);
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
	sprintf(debug_msg, "ERROR: B-Label Not Found: [%s]", label);
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
		sprintf(debug_msg, "ERROR: B-ID Not Found: [%d]", block_id);
		DebugLog(debug_msg);
		return NULL;
}


BlockNode* GetBlockListHead(void) {
	return bll;
}

BlockNode* AddBlockNode(BlockNode** head_ref, uint8_t block_cat, const char *block_label) {
	BlockNode* new_block;

	if (*head_ref == NULL) {   //empty list
		// common settings and setting holders for al blocks
		new_block = (BlockNode *) malloc(sizeof(BlockNode));
		if (new_block == NULL) {
			DebugLog("STOP: (AddBlock) malloc returned NULL");
			while (1)
				;
		} else
			DebugLog("(AddBlock) malloc OK");
		new_block->next_block = NULL;

		new_block->block_cat = block_cat;
		new_block->block_type = UINT8_INIT;

		// initialise operational, run-time block data
		new_block->active = 0;
		new_block->bool_val = 0;
		new_block->int_val = UINT8_INIT;
		new_block->f_val = FLOAT_INIT;
		new_block->last_update = UINT32_INIT;
		new_block->status = STATUS_INIT;

		if (block_cat != FF_GENERIC_BLOCK) {

			if (block_cat == FF_SYSTEM) {
				new_block->block_id = SSS;
			} else {
				new_block->block_id = BLOCK_ID_BASE + block_count;
				block_count++;
			}
			strcpy(new_block->block_label, block_label);
#ifndef	EXCLUDE_DISPLAYNAME
			new_block->display_name[0] = '\0';
#endif
#ifndef EXCLUDE_DESCRIPTION
			new_block->description[0] = '\0';
#endif

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
					for (int i = 0; i < LAST_DAY; i++)
						new_block->settings.sch.days[i] = UINT8_INIT;
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
					new_block->settings.con.deact_cmd = UINT8_INIT;
					break;
				case FF_OUTPUT:
					new_block->settings.out.interface = LAST_INTERFACE;
					new_block->settings.out.if_num = UINT8_INIT;
					break;
			} //switch
		} // != FF_GENERIC_BLOCK
		*head_ref = new_block;
		return new_block;
	}
	return AddBlockNode(&((*head_ref)->next_block), block_cat, block_label);

}

BlockNode* AddBlock(uint8_t block_cat, const char *block_label) {
	return AddBlockNode(&bll, block_cat, block_label);
}

BlockNode* GetBlockNodeByLabel(BlockNode *list_node, const char *block_label) {
	//BlockNode *block;

	if(list_node == NULL) {   //empty list
		return NULL;
	} else {
		if (strcmp(list_node->block_label, block_label) == 0) {
			return list_node;
		} else {
			list_node = GetBlockNodeByLabel(list_node->next_block, block_label);
		}
	}
	return list_node;
}

BlockNode* GetBlockByLabel(const char *block_label) {
	return GetBlockNodeByLabel(bll, block_label);
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
		DebugLog(SSS, E_ERROR, M_SR_BAD_ID);
	}

	sr.ui_data.light_flag = 0;
	sr.ui_data.water_heater_flag = 0;
}


void InitStateRegister(void) {

	DebugLog(SSS, E_INFO, M_INI_SR);

	sr.language = ENGLISH;
	sr.temperature_scale = CELSIUS;
	sr.time_real_status = 0; 	//false at this stage
	sr.FS_present = 0; 			//false at this stage
	sr.config_valid = 0;		//false at this stage
	sr.save_event_buffer = 0;



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






