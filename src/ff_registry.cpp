/************************************************
 ff_registry.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Component Registry
************************************************/

/************************************************
 Includes
************************************************/
#include <ff_sys_config.h>
#include "ff_registry.h"
#include "ff_string_consts.h"
#include "ff_debug.h"
#include "ff_utils.h"
#include "ff_display.h"

#include "ff_inputs.h"
#include "ff_monitors.h"
#include "ff_schedules.h"
#include "ff_rules.h"
#include "ff_controllers.h"
#include "ff_outputs.h"

#include <stdlib.h>
#include <string.h>

#ifdef FF_SIMULATOR
#include <stdio.h>
#endif

#include "ff_HAL.h"

#ifdef USE_ITCH
//#include "itch.h"
#endif

/************************************************
 Data Structures
************************************************/

//typedef struct BLOCK_TYPE {
//	char label[MAX_LABEL_LENGTH];
//	uint8_t active;
//} Block;


typedef struct FF_STATE_REGISTER {
	//system config and flags
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
		//HALInitItch();
		#ifdef FF_ARDUINO
		#ifdef FF_RPI_START_DELAY
			delay(FF_RPI_START_DELAY); //XXX kludge to allow the rpi to boot and establish ppp to the controller before sending data
		#endif
		#endif

		#ifdef UI_ATTACHED
		UpdateUI();
		#endif //UI_ATTACHED

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
		#ifdef USE_ITCH
		//itch.Poll();
		HALPollItch();
		#endif
		#ifdef UI_ATTACHED
		UpdateUI();
		#endif
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

BlockNode* GetLastBlockAddr(void) {
	BlockNode* block_ptr;

	block_ptr = bll;

	if (block_ptr != NULL) {
		while (block_ptr->next_block != NULL) {
			block_ptr = block_ptr->next_block;
		}
		return block_ptr;
	} else {
		return NULL;
	}
}

BlockNode* GetBlockByID(BlockNode *head, uint16_t block_id) {
	// re-written iteratively

	BlockNode* walker;
	walker = head;
	while(walker != NULL) {
		if(walker->block_id == block_id) {
			return walker;
		} else {
			walker = walker->next_block;
		}
	}
	return walker;
}
/*
	if(head == NULL) {   //empty list
		return NULL;
	} else {
		if (head->block_id == block_id) {
			return head;
		} else {
			head = GetBlockByID(head->next_block, block_id);
		}
	}
	return head;
*/


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


uint16_t GetBlockIDByLabel(const char* label) {
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

	if (block_id == BLOCK_ID_NA) {
		return "NA";
	}
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

	if (*head_ref == NULL) {   //empty list or end of list - add the block
		// common settings and setting holders for all blocks
		new_block = (BlockNode *) malloc(sizeof(BlockNode));
		if (new_block == NULL) {
			DebugLog(SSS, E_STOP, M_ADDBLOCK_ERROR);
			while (1)
				;
		} else {
//			DebugLog("(AddBlock) malloc OK");
		}
		new_block->next_block = NULL;
		new_block->block_label = NULL;
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

			//strcpy(new_block->block_label, block_label);

			if(block_label != NULL) {
				// XXX add null checks
				new_block->block_label = (char *)malloc( (strlen(block_label)+1) * sizeof(char) );
				strcpy(new_block->block_label, block_label);
			} else {
				new_block->block_label = NULL;
			}

			#ifndef	EXCLUDE_DISPLAYNAME
			//new_block->display_name[0] = '\0';
			new_block->display_name = NULL;
			#endif
			#ifndef EXCLUDE_DESCRIPTION
			//new_block->description[0] = '\0';
			new_block->description = NULL;
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
	//to save stack memory, iterate down the block list
	//and only call AddBlockNode function
	//once at the end of the list

	if (bll == NULL) {
		return AddBlockNode(&bll, block_cat, block_label);
	} else {

		BlockNode* walker = bll;
		while(walker->next_block != NULL) {
			walker = walker->next_block;
		}
		return AddBlockNode(&(walker->next_block), block_cat, block_label);
	}
}

char* UpdateBlockLabel(BlockNode* b, char * block_label) {
	// Update the label of a block and adjust its memory use
	// Possible calling value combinations:
	// b->label == NULL and block_label != NULL - malloc (new label to previously NULL label)
	// b->label == NULL and block_label == NULL - do nothing (NULL label to existing NULL label)
	// b->label != NULL and block_label == NULL - free b->label (Existing label to be NULL'd and freed)
	// b->label != NULL and block_label != NULL - realloc b->label (Different label - free and re-malloc)

	if (b->block_label == NULL) {
		// not previously malloced
		if (block_label != NULL) {
			b->block_label = (char *)malloc( (strlen(block_label)+1) * sizeof(char) );
			// XXX add null check and debug throw
			strcpy(b->block_label, block_label);
		} else {
			// block_label is NULL
			return NULL;
		}
	} else {
		// b->block label already assigned
		if(block_label == NULL) {
			// update to NULL
			free(b->block_label);
			b->block_label = NULL;
			return NULL;
		} else {
			// XXX realloc fails at link time. Workaround: free and malloc again
			free(b->block_label);
			b->block_label = (char *)malloc( (strlen(block_label)+1) * sizeof(char) );
			// XXX add null check and debug throw
			strcpy(b->block_label, block_label);
		}
	}
	return b->block_label;
}

BlockNode* GetBlockNodeByLabel(BlockNode *head, const char *block_label) {
	// re-written iteratively

	BlockNode* walker;
	walker = head;
	while(walker != NULL) {
		if (strcmp(head->block_label, block_label) == 0) {
			return walker;
		} else {
			walker = walker->next_block;
		}
	}
	return walker;
}
/*
	if(head == NULL) {   //empty list
		return NULL;
	} else {
		if (strcmp(head->block_label, block_label) == 0) {
			return head;
		} else {
			head = GetBlockNodeByLabel(head->next_block, block_label);
		}
	}
	return head;
*/

BlockNode* GetBlockByLabel(const char *block_label) {
	return GetBlockNodeByLabel(bll, block_label);
}

/********************************************************************************************
 * ITCH Integration Handlers
 ********************************************************************************************/

void RegShowBlocks(void(*Callback)(char *)) {
	BlockNode* b;
	b = bll;
	char out[MAX_MESSAGE_STRING_LENGTH];

	if (b == NULL) {
		strcpy(out, "Empty List");
		Callback(out);
	} else {
		sprintf(out, "\tBID\tCAT\tTYPE\tLABEL");
		Callback(out);
		while(b != NULL) {
			sprintf(out, "\t%d\t%d\t%d\t%s", b->block_id, b->block_cat, b->block_type, b->block_label);
			Callback(out);
			b = b->next_block;
		}
	}
}

void RegShowBlockByID(uint16_t id, void(*Callback)(char *)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char temp_str[10];

	BlockNode *b;
	b = GetBlockByID(bll, id);

	sprintf(out_str, "Base Data");
	Callback(out_str);
	sprintf(out_str, " block_id:     %d", b->block_id);
	Callback(out_str);
	sprintf(out_str, " block_cat:    %d", b->block_cat);
	Callback(out_str);
	sprintf(out_str, " block_type:   %d", b->block_type);
	Callback(out_str);
	sprintf(out_str, " block_label:  %s", b->block_label);
	Callback(out_str);

	#ifndef EXCLUDE_DISPLAYNAME
	sprintf(out_str, " display_name: %s", b->display_name);
	Callback(out_str);
#endif
#ifndef EXCLUDE_DESCRIPTION
	sprintf(out_str, " description:  %s", b->description);
	Callback(out_str);
#endif

	sprintf(out_str, "Operational Data");
	Callback(out_str);
	sprintf(out_str, " active:       %d", b->active);
	Callback(out_str);
	sprintf(out_str, " bool_val:     %d", b->bool_val);
	Callback(out_str);
	sprintf(out_str, " int_val:      %d", b->int_val);
	Callback(out_str);
	FFFloatToCString(temp_str, b->f_val);
	sprintf(out_str, " f_val:        %s", temp_str);
	Callback(out_str);
	sprintf(out_str, " last_update:  %lu", b->last_update);
	Callback(out_str);
	sprintf(out_str, " status:       %d", b->status);
	Callback(out_str);

	sprintf(out_str, "Block Category Specific Data");
	Callback(out_str);

	switch (b->block_cat) {
		case (FF_SYSTEM):
			sprintf(out_str, "System:");
			Callback(out_str);
			sprintf(out_str, " temp_scale:   %d", b->settings.sys.temp_scale);
			Callback(out_str);
			sprintf(out_str, " language:     %d", b->settings.sys.language);
			Callback(out_str);
			sprintf(out_str, " week_star     %d", b->settings.sys.week_start);
			Callback(out_str);
			break;
		case FF_INPUT:
			sprintf(out_str, "Input:");
			Callback(out_str);
			sprintf(out_str, " interface     %d", b->settings.in.interface);
			Callback(out_str);
			sprintf(out_str, " if_num:       %d", b->settings.in.if_num);
			Callback(out_str);
			sprintf(out_str, " log_rate      %lu", b->settings.in.log_rate); //TV_TYPE
			Callback(out_str);
			sprintf(out_str, " data_unit     %d", b->settings.in.data_units);
			Callback(out_str);
			sprintf(out_str, " data_type     %d", b->settings.in.data_type);		// float, int
			Callback(out_str);
		break;
		case FF_MONITOR:
			sprintf(out_str, "Monitor:");
			Callback(out_str);
			sprintf(out_str, " input1:       %d", b->settings.mon.input1);
			Callback(out_str);
			sprintf(out_str, " input2:       %d", b->settings.mon.input2);
			Callback(out_str);
			sprintf(out_str, " input3:       %d", b->settings.mon.input3);
			Callback(out_str);
			sprintf(out_str, " input4:       %d", b->settings.mon.input4);
			Callback(out_str);
			FFFloatToCString(temp_str, b->settings.mon.act_val);
			sprintf(out_str, " act_val:      %s", temp_str);   //float
			Callback(out_str);
			FFFloatToCString(temp_str, b->settings.mon.deact_val);
			sprintf(out_str, " deact_val:    %s", temp_str);  //float
			Callback(out_str);
		break;
		case FF_SCHEDULE: {
			sprintf(out_str, "Schedule:");
			Callback(out_str);
			sprintf(out_str, " days          ");
			for(uint8_t i = 0; i < 7; i++) {
				sprintf(temp_str, "%d ", b->settings.sch.days[i]);
				strcat(out_str, temp_str);
			}
			Callback(out_str);
			sprintf(out_str, " time_start    %lu", b->settings.sch.time_start);
			Callback(out_str);
			sprintf(out_str, " time_end      %lu", b->settings.sch.time_end);
			Callback(out_str);
			sprintf(out_str, " time_duration %lu", b->settings.sch.time_duration);
			Callback(out_str);
			sprintf(out_str, " time_repeat   %lu", b->settings.sch.time_repeat);
			Callback(out_str);
		}
		break;
		case FF_RULE:
			sprintf(out_str, "Rule:");
			Callback(out_str);
			sprintf(out_str, " param1:       %d", b->settings.rl.param1);
			Callback(out_str);
			sprintf(out_str, " param2:       %d", b->settings.rl.param2);
			Callback(out_str);
			sprintf(out_str, " param3:       %d", b->settings.rl.param3);
			Callback(out_str);
			sprintf(out_str, " param_not     %d", b->settings.rl.param_not);
			Callback(out_str);
		break;
		case FF_CONTROLLER:
			sprintf(out_str, "Controller:");
			Callback(out_str);
			sprintf(out_str, " rule:         %d", b->settings.con.rule);
			Callback(out_str);
			sprintf(out_str, " output:       %d", b->settings.con.output);
			Callback(out_str);
			sprintf(out_str, " act_cmd:      %d", b->settings.con.act_cmd);
			Callback(out_str);
			sprintf(out_str, " deact_cmd:    %d", b->settings.con.deact_cmd);
			Callback(out_str);
		break;
		case FF_OUTPUT:
			sprintf(out_str, "Output:");
			Callback(out_str);
			sprintf(out_str, " interface:    %d", b->settings.out.interface);
			Callback(out_str);
			sprintf(out_str, " if_num:       %d", b->settings.out.if_num);
			Callback(out_str);
			sprintf(out_str, " command:      %d", b->settings.out.command);
			Callback(out_str);
		break;
	}
}


void RegSendCommandToBlockLabel(char* block_label, uint16_t command, void(*Callback)(char *)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	uint16_t block_id;

	block_id = GetBlockIDByLabel(block_label);
	if (block_id == M_FF_ERROR) {
		sprintf(out_str, "Block: %s not found", block_label);
		Callback(out_str);
	} else {
		//XXX range check command
		sprintf(out_str, "Sending %d to Block: %s", command, block_label);
		Callback(out_str);
		SetCommand(block_id, command);
		EventMsg(SSS, block_id, E_COMMAND, command);
	}

}

/********************************************************************************************
 * State Register and UI Data Set Functions
 ********************************************************************************************/


UIDataSet* GetUIDataSet(void) {
	return &sr.ui_data;
}

void UpdateStateRegister(uint16_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val) {

	//TODO include further registry block logic update here

	//TODO redo - remove hard coding perhaps
	const char* src_label;
	src_label = GetBlockLabelString(source);

	//Update UI Data Fields with raw values and update MIN and MAX fields if they have changed
	if (src_label) {
		if (strcmp(src_label, DISPLAY_INSIDE_SOURCE_BLOCK) == 0) {
			sr.ui_data.inside_current = f_val;
			if (f_val < sr.ui_data.inside_min) {
				sr.ui_data.inside_min = f_val;
				sr.ui_data.inside_min_dt = TimeNow();
			}
			if (f_val > sr.ui_data.inside_max) {
				sr.ui_data.inside_max = f_val;
				sr.ui_data.inside_max_dt = TimeNow();
			}
		}

		if (strcmp(src_label, DISPLAY_OUTSIDE_SOURCE_BLOCK) == 0) {
			sr.ui_data.outside_current = f_val;
			if (f_val < sr.ui_data.outside_min) {
				sr.ui_data.outside_min = f_val;
				sr.ui_data.outside_min_dt = TimeNow();
			}
			if (f_val > sr.ui_data.outside_max) {
				sr.ui_data.outside_max = f_val;
				sr.ui_data.outside_max_dt = TimeNow();
			}
		}

		if (strcmp(src_label, DISPLAY_WATER_SOURCE_BLOCK) == 0) {
			sr.ui_data.water_current = f_val;
			if (f_val < sr.ui_data.water_min) {
				sr.ui_data.water_min = f_val;
				sr.ui_data.water_min_dt = TimeNow();
			}
			if (f_val > sr.ui_data.water_max) {
				sr.ui_data.water_max = f_val;
				sr.ui_data.water_max_dt = TimeNow();
			}
		}

	} else { //src_label Null
		DebugLog(SSS, E_ERROR, M_SR_BAD_ID);
	}

	if ( (msg_type == E_COMMAND) && (msg_str == CMD_RESET_MINMAX) ) {
		sr.ui_data.inside_min = sr.ui_data.inside_current;
		sr.ui_data.inside_min_dt = TimeNow();
		sr.ui_data.inside_max = sr.ui_data.inside_current;
		sr.ui_data.inside_max_dt = TimeNow();

		sr.ui_data.outside_min = sr.ui_data.outside_current;
		sr.ui_data.outside_min_dt = TimeNow();
		sr.ui_data.outside_max = sr.ui_data.outside_current;
		sr.ui_data.outside_max_dt = TimeNow();

		sr.ui_data.water_min = sr.ui_data.water_current;
		sr.ui_data.water_min_dt = TimeNow();
		sr.ui_data.water_max = sr.ui_data.water_current;
		sr.ui_data.water_max_dt = TimeNow();

		DebugLog(SSS, E_INFO, M_SR_MINMAX_RESET);
	}

	//TODO - decide: flags for lights and heater on UI?
	// Not used for now
	//sr.ui_data.light_flag = 0;
	//sr.ui_data.water_heater_flag = 0;
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
	sr.ui_data.inside_min_dt = TimeNow();
	sr.ui_data.inside_max = -50;
	sr.ui_data.inside_max_dt = TimeNow();

	sr.ui_data.outside_current = 0;
	sr.ui_data.outside_min = 100;
	sr.ui_data.outside_min_dt = TimeNow();
	sr.ui_data.outside_max = -50;
	sr.ui_data.outside_max_dt = TimeNow();

	sr.ui_data.water_current = 0;
	sr.ui_data.water_min = 100;
	sr.ui_data.water_min_dt = TimeNow();
	sr.ui_data.water_max = -50;
	sr.ui_data.water_max_dt = TimeNow();

	//TODO these will go
	//sr.ui_data.light_flag = 0;
	//sr.ui_data.water_heater_flag = 0;

}






