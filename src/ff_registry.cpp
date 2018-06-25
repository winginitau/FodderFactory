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
#include <time.h>

#ifndef FF_ARDUINO
#include <stdio.h>
#include <errno.h>
#endif

#include <ff_HAL.h>

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

// XXX HACK VE Direct
time_t VE_last_polled;
time_t VE_last_logged;
TV_TYPE VE_log_rate;
TV_TYPE VE_poll_rate;
int32_t VE_soc, VE_power;
int32_t VE_voltage, VE_current;
uint8_t VE_status;

/************************************************
 Functions
************************************************/

void Setup(BlockNode *b) {
	switch (b->block_cat) {
	case FF_SYSTEM:
		//EventMsg(SSS, E_DEBUG_MSG, M_SYS_HEART_BEAT);
		//HALInitItch(); // moved to head file - needs to start first
		#ifdef FF_ARDUINO
		#ifdef FF_RPI_START_DELAY
			delay(FF_RPI_START_DELAY); //XXX kludge to allow the rpi to boot and establish ppp to the controller before sending data
		#endif
		#endif

		#ifdef UI_ATTACHED
		UpdateUI();
		#endif //UI_ATTACHED

		#ifdef VE_DIRECT
		// XXX VE Hack
		if (HALVEDirectInit()) {
			VE_last_logged = TimeNow();
			VE_last_polled = TV_TYPE_INIT;
			VE_log_rate = VE_LOG_RATE;
			VE_poll_rate = VE_POLL_RATE;
			VE_status = STATUS_ENABLED_INIT;
			EventMsg(SSS, E_INFO, M_VE_INIT);
		} else {
			VE_status = STATUS_DISABLED_ERROR;
			EventMsg(SSS, E_ERROR, M_VE_INIT_ERROR);

		}
		#endif //VE_DIRECT
		break;
	case FF_INPUT:
		InputSetup(b);
		break;
	case FF_MONITOR:

		// Hack to tweak MON_INSIDE_BOTTOM_TOO_COLD deact from 19 to 21
		//if(strcmp_hal(b->block_label, F("MON_INSIDE_BOTTOM_TOO_COLD")) == 0) {
		//	b->settings.mon.deact_val = 21;
		//	EventMsg(SSS, E_WARNING, M_HACK_MON_INSIDE_BOTTOM_TOO_COLD);
		//}

		// Hack to tweak MON_INSIDE_TOP_TOO_HOT deact from 21 to 21.x to stop circ exh race
		//if(strcmp_hal(b->block_label, F("MON_INSIDE_TOP_TOO_HOT")) == 0) {
		//	b->settings.mon.deact_val = 21.6;
		//	EventMsg(SSS, E_WARNING, M_HACK_MON_INSIDE_TOP_TOO_HOT);
		//}

		MonitorSetup(b);
		break;
	case FF_SCHEDULE:
		ScheduleSetup(b);
		break;
	case FF_RULE: {

		// XXX HACK to fix config error on RL_CIRC_IF_TOP_HOT_BOT_COLD
		//if (strcmp_hal(b->block_label, F("RL_CIRC_IF_TOP_HOT_BOT_COLD")) == 0) {
		//	b->block_type = RL_LOGIC_AND;
		//	b->settings.rl.param2 = GetBlockIDByLabel("MON_INSIDE_BOTTOM_TOO_COLD");
		//	EventMsg(SSS, E_WARNING, M_HACK_RL_CIRC_IF_TOP_HOT_BOT_COLD);
		//}

		// XXX HACK to make config change on RL_EXHAUST_IF_TOP_HOT_BOT_HOT
		//if (strcmp_hal(b->block_label, F("RL_EXHAUST_IF_TOP_HOT_BOT_HOT")) == 0) {
		//	b->block_type = RL_LOGIC_SINGLENOT;
		//	b->settings.rl.param1 = GetBlockIDByLabel("MON_INSIDE_TOP_TOO_HOT");
		//	b->settings.rl.param_not = GetBlockIDByLabel("MON_INSIDE_BOTTOM_TOO_COLD");
		//	EventMsg(SSS, E_WARNING, M_HACK_RL_EXHAUST_IF_TOP_HOT_BOT_HOT);
		//}

		RuleSetup(b);
	}
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


//XXX move to utility
uint8_t VarianceExceedsPercent(int32_t old_val, int32_t new_val, uint8_t thres_pc) {
	int32_t var_abs;
	int32_t thres_abs;
	var_abs = old_val - new_val;
	var_abs = var_abs * ((var_abs > 0) - (var_abs < 0));

	thres_abs = old_val * thres_pc / 100;
	thres_abs = thres_abs * ((thres_abs > 0) - (thres_abs < 0));

	return (var_abs > thres_abs);
}

uint8_t VarianceExceedsAbsolute(int32_t old_val, int32_t new_val, uint16_t thres_abs) {
	int32_t var_abs;
	var_abs = old_val - new_val;
	var_abs = var_abs * ((var_abs > 0) - (var_abs < 0));
	return (var_abs > thres_abs);
}

void Operate(BlockNode *b) {
	switch (b->block_cat) {
	case FF_SYSTEM: {
		//EventMsg(SSS, E_DEBUG_MSG, M_SYS_HEART_BEAT);

		#ifdef USE_ITCH
		//itch.Poll();
		HALPollItch();
		#endif
		#ifdef UI_ATTACHED
		UpdateUI();
		#endif

		#ifdef VE_DIRECT
		// XXX VE Hack
			if ((VE_status > STATUS_ERROR) && (VE_status < STATUS_DISABLED)) {
				time_t VE_now = TimeNow();
				time_t VE_next_poll;
				time_t VE_next_log;
				uint32_t new_current;
				VE_next_log = VE_last_logged + VE_log_rate;
				VE_next_poll = VE_last_polled + VE_poll_rate;
				if (VE_now >= VE_next_poll) {
					VE_last_polled = TimeNow();

					VE_soc = HALReadVEData(M_VE_SOC);
					VE_power = HALReadVEData(M_VE_POWER);
					VE_voltage = HALReadVEData(M_VE_VOLTAGE);

					new_current = HALReadVEData(M_VE_CURRENT);
					if (VarianceExceedsAbsolute(VE_current, new_current, 1000)) {
						VE_next_log = TimeNow() - 5; // set logging to earlier to trigger test
					}
					VE_current = new_current;

				}
				if (VE_now >= VE_next_log) {
					VE_last_logged = TimeNow();
					VE_status = STATUS_ENABLED_VALID_DATA;
					EventMsg(SSS, E_DATA, M_VE_SOC, VE_soc, 0);
					EventMsg(SSS, E_DATA, M_VE_VOLTAGE, VE_voltage, 0);
					EventMsg(SSS, E_DATA, M_VE_POWER, VE_power, 0);
					EventMsg(SSS, E_DATA, M_VE_CURRENT, VE_current, 0);
				}
			}
		#endif //VE_DIRECT
		break;
	}
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
	//XXX resolve design clash.
	// This fails with an error - original concept was complete internal block list
	// consistency. Now with itch this is being used to lookup partial labels
	// and is throwing debug errors, whereas the itch behaviour is normal
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
	char msg_str[31];
	strcpy_hal(msg_str, F("ERROR: B-Label Not Found: [%s]"));
	sprintf(debug_msg, msg_str, label);
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
		char msg_str[30];
		strcpy_hal(msg_str, F("ERROR: B-ID Not Found: [%d]"));
		sprintf(debug_msg, msg_str, block_id);
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
		new_block->int_val = INT32_INIT;
		new_block->f_val = FLOAT_INIT;
		new_block->last_update = UINT32_INIT;
		new_block->status = STATUS_ENABLED_INIT;

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

uint8_t RegLookupBlockLabel(char* lookup_string) {
	return GetBlockIDByLabel(lookup_string);
}

void RegShowBlocks(void(*Callback)(char *)) {
	BlockNode* b;
	b = bll;
	char out[MAX_MESSAGE_STRING_LENGTH];
	char fmt_str[20];
	if (b == NULL) {
		strcpy_hal(out, F("Empty List"));
		Callback(out);
	} else {
		strcpy_hal(out, F("\tBID\tCAT\tTYPE\tLABEL"));
		Callback(out);
		while(b != NULL) {
			strcpy_hal(fmt_str, F("\t%d\t%d\t%d\t%s"));
			sprintf(out, fmt_str, b->block_id, b->block_cat, b->block_type, b->block_label);
			Callback(out);
			b = b->next_block;
		}
	}
}

void RegShowSystem(void(*Callback)(char *)) {
	RegShowBlockByID(SSS, Callback);
}

void RegShowBlockByLabel(char* block_label, void(*Callback)(char *)) {
	RegShowBlockByID(GetBlockIDByLabel(block_label), Callback);
}

void RegShowBlockByID(uint16_t id, void (*Callback)(char *)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char fmt_str[MAX_MESSAGE_STRING_LENGTH];
	char temp_str[10];

	BlockNode *b;
	b = GetBlockByID(bll, id);

	if (b == NULL) {
		strcpy_hal(fmt_str, F("Error: %d is not a valid block ID"));
		sprintf(out_str, fmt_str, id);
		Callback(out_str);
	} else {

		strcpy_hal(out_str, F("Base Data"));
		Callback(out_str);
		strcpy_hal(fmt_str, F(" block_id:     %d"));
		sprintf(out_str, fmt_str, b->block_id);
		Callback(out_str);
		strcpy_hal(fmt_str, F(" block_cat:    %d"));
		sprintf(out_str, fmt_str, b->block_cat);
		Callback(out_str);
		strcpy_hal(fmt_str, F(" block_type:   %d"));
		sprintf(out_str, fmt_str, b->block_type);
		Callback(out_str);
		strcpy_hal(fmt_str, F(" block_label:  %s"));
		sprintf(out_str, fmt_str, b->block_label);
		Callback(out_str);

#ifndef EXCLUDE_DISPLAYNAME
		strcpy_hal(fmt_str, F(" display_name: %s"));
		sprintf(out_str, fmt_str, b->display_name);
		Callback(out_str);
#endif
#ifndef EXCLUDE_DESCRIPTION
		strcpy_hal(fmt_str, F(" description:  %s"));
		sprintf(out_str, fmt_str, b->description);
		Callback(out_str);
#endif

		strcpy_hal(out_str, F("Operational Data"));
		Callback(out_str);
		strcpy_hal(fmt_str, F(" active:       %d"));
		sprintf(out_str, fmt_str, b->active);
		Callback(out_str);
		strcpy_hal(fmt_str, F(" bool_val:     %d"));
		sprintf(out_str, fmt_str, b->bool_val);
		Callback(out_str);
		strcpy_hal(fmt_str, F(" int_val:      %ld"));
		sprintf(out_str, fmt_str, b->int_val);
		Callback(out_str);
		FFFloatToCString(temp_str, b->f_val);
		strcpy_hal(fmt_str, F(" f_val:        %s"));
		sprintf(out_str, fmt_str, temp_str);
		Callback(out_str);
		strcpy_hal(fmt_str, F(" last_update:  %lu"));
		sprintf(out_str, fmt_str, b->last_update);
		Callback(out_str);
		strcpy_hal(fmt_str, F(" status:       %d"));
		sprintf(out_str, fmt_str, b->status);
		Callback(out_str);

		strcpy_hal(out_str, F("Block Category Specific Data"));
		Callback(out_str);

		switch (b->block_cat) {
			case (FF_SYSTEM):
				strcpy_hal(out_str, F("System:"));
				Callback(out_str);
				strcpy_hal(fmt_str, F(" temp_scale:   %d"));
				sprintf(out_str, fmt_str, b->settings.sys.temp_scale);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" language:     %d"));
				sprintf(out_str, fmt_str, b->settings.sys.language);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" week_star     %d"));
				sprintf(out_str, fmt_str, b->settings.sys.week_start);
				Callback(out_str);
				break;
			case FF_INPUT:
				strcpy_hal(out_str, F("Input:"));
				Callback(out_str);
				strcpy_hal(fmt_str, F(" interface     %d"));
				sprintf(out_str, fmt_str, b->settings.in.interface);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" if_num:       %d"));
				sprintf(out_str, fmt_str, b->settings.in.if_num);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" log_rate      %lu"));
				sprintf(out_str, fmt_str, (unsigned long) b->settings.in.log_rate); //TV_TYPE
				Callback(out_str);
				strcpy_hal(fmt_str, F(" data_unit     %d"));
				sprintf(out_str, fmt_str, b->settings.in.data_units);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" data_type     %d"));
				sprintf(out_str, fmt_str, b->settings.in.data_type);		// float, int
				Callback(out_str);
				break;
			case FF_MONITOR:
				strcpy_hal(out_str, F("Monitor:"));
				Callback(out_str);
				strcpy_hal(fmt_str, F(" input1:       %d"));
				sprintf(out_str, fmt_str, b->settings.mon.input1);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" input2:       %d"));
				sprintf(out_str, fmt_str, b->settings.mon.input2);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" input3:       %d"));
				sprintf(out_str, fmt_str, b->settings.mon.input3);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" input4:       %d"));
				sprintf(out_str, fmt_str, b->settings.mon.input4);
				Callback(out_str);
				FFFloatToCString(temp_str, b->settings.mon.act_val);
				strcpy_hal(fmt_str, F(" act_val:      %s"));
				sprintf(out_str, fmt_str, temp_str);   //float
				Callback(out_str);
				FFFloatToCString(temp_str, b->settings.mon.deact_val);
				strcpy_hal(fmt_str, F(" deact_val:    %s"));
				sprintf(out_str, fmt_str, temp_str);  //float
				Callback(out_str);
				break;
			case FF_SCHEDULE: {
				strcpy_hal(out_str, F("Schedule:"));
				Callback(out_str);
				strcpy_hal(out_str, F(" days          "));
				for (uint8_t i = 0; i < 7; i++) {
					strcpy_hal(fmt_str, F("%d "));
					sprintf(temp_str, fmt_str, b->settings.sch.days[i]);
					strcat(out_str, temp_str);
				}
				Callback(out_str);
				strcpy_hal(fmt_str, F(" time_start    %lu"));
				sprintf(out_str, fmt_str, b->settings.sch.time_start);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" time_end      %lu"));
				sprintf(out_str, fmt_str, b->settings.sch.time_end);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" time_duration %lu"));
				sprintf(out_str, fmt_str, (unsigned long) b->settings.sch.time_duration);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" time_repeat   %lu"));
				sprintf(out_str, fmt_str, (unsigned long) b->settings.sch.time_repeat);
				Callback(out_str);
			}
				break;
			case FF_RULE:
				strcpy_hal(out_str, F("Rule:"));
				Callback(out_str);
				strcpy_hal(fmt_str, F(" param1:       %d"));
				sprintf(out_str, fmt_str, b->settings.rl.param1);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" param2:       %d"));
				sprintf(out_str, fmt_str, b->settings.rl.param2);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" param3:       %d"));
				sprintf(out_str, fmt_str, b->settings.rl.param3);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" param_not     %d"));
				sprintf(out_str, fmt_str, b->settings.rl.param_not);
				Callback(out_str);
				break;
			case FF_CONTROLLER:
				strcpy_hal(out_str, F("Controller:"));
				Callback(out_str);
				strcpy_hal(fmt_str, F(" rule:         %d"));
				sprintf(out_str, fmt_str, b->settings.con.rule);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" output:       %d"));
				sprintf(out_str, fmt_str, b->settings.con.output);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" act_cmd:      %d"));
				sprintf(out_str, fmt_str, b->settings.con.act_cmd);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" deact_cmd:    %d"));
				sprintf(out_str, fmt_str, b->settings.con.deact_cmd);
				Callback(out_str);
				break;
			case FF_OUTPUT:
				strcpy_hal(out_str, F("Output:"));
				Callback(out_str);
				strcpy_hal(fmt_str, F(" interface:    %d"));
				sprintf(out_str, fmt_str, b->settings.out.interface);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" if_num:       %d"));
				sprintf(out_str, fmt_str, b->settings.out.if_num);
				Callback(out_str);
				strcpy_hal(fmt_str, F(" command:      %d"));
				sprintf(out_str, fmt_str, b->settings.out.command);
				Callback(out_str);
				break;
		}
	}
}

void RegSendCommandToBlockLabel(char* block_label, uint16_t command, void(*Callback)(char *)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	uint16_t block_id;
	char fmt_str[35];

	block_id = GetBlockIDByLabel(block_label);
	if (block_id == M_FF_ERROR) {
		strcpy_hal(fmt_str, F("Block: %s not found"));
		sprintf(out_str, fmt_str, block_label);
		Callback(out_str);
	} else {
		//XXX range check command
		strcpy_hal(fmt_str, F("Sending %d to Block: %s"));
		sprintf(out_str, fmt_str, command, block_label);
		Callback(out_str);
		SetCommand(block_id, command);
		EventMsg(SSS, block_id, E_COMMAND, command);
	}
}

void RegSendCommandToBlockID(uint16_t id, uint16_t command, void(*Callback)(char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80

	BlockNode *b;
	b = GetBlockByID(bll, id);
	char fmt_str[35];

	if (b == NULL) {
		strcpy_hal(fmt_str, F("Error: %d is not a valid block ID"));
		sprintf(out_str, fmt_str, id);
		Callback(out_str);
	} else {
		//XXX range check command
		strcpy_hal(fmt_str, F("Sending %d to Block ID: %d"));
		sprintf(out_str, fmt_str, command, id);
		Callback(out_str);
		SetCommand(id, command);
		EventMsg(SSS, id, E_COMMAND, command);
	}

}

void RegShowTime(void(*Callback)(char*)) {
	char hms_str[12];
	time_t now;
	char fmt_str[9];

	now = TimeNow();
	strcpy_hal(fmt_str, F("%H:%M:%S"));
	strftime(hms_str, 12, fmt_str, localtime(&now));
	Callback(hms_str);
}

void RegSetTime(char* time_str, void(*Callback)(char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char tok_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char *tok;
	uint8_t err = 1;
	uint8_t ton;

	strcpy(tok_str, time_str);
	tok = strtok(tok_str, ":");
	if (tok) ton = atoi(tok);
	if(tok && (ton < 24)) {
		tok = strtok(0, ":");
		if (tok) ton = atoi(tok);
		if(tok && (ton < 60)) {
			tok = strtok(0, ":");
			if (tok) ton = atoi(tok);
			if(tok && (ton < 60)) {
				err = 0;
				strcpy_hal(out_str, F("Time string is valid"));
				Callback(out_str);

#ifndef FF_ARDUINO
			    struct tm *tmptr;
			    time_t now;
			    time_t t;
				int rc;
				now = TimeNow();
				tmptr = localtime(&now);
			    strptime(time_str, "%H:%M:%S", tmptr);
			    t = mktime(tmptr);
			    rc = stime(&t);
			    if(rc==0) {
			        sprintf(out_str, "System stime() successful.\n");
			        Callback(out_str);
			    }
			    else {
			        sprintf(out_str, "Error: stime() failed, errno = %d: %s\n",errno, strerror(errno));
			        Callback(out_str);
			    }
#else
			    uint8_t rc = HALSetRTCTime(time_str);
			    if(rc==0) {
					strcpy_hal(out_str, F("HALSetRTCTime() successful.\n"));
			        Callback(out_str);
			    }
			    else {
					strcpy_hal(out_str, F("Error: HALSetRTCTime() failed.\n"));
			        Callback(out_str);
			    }
#endif //ifndef FF_ARDUINO

			}
		}
	}
	if (err) {
		strcpy_hal(out_str, F("Error: Time String not valid."));
		Callback(out_str);
	}

}

void RegShowDate(void(*Callback)(char*)) {
	char ymd_str[14];
	time_t now;
	char fmt_str[9];

	now = TimeNow();
	strcpy_hal(fmt_str, F("%Y-%m-%d"));
	strftime(ymd_str, 14, fmt_str, localtime(&now));
	Callback(ymd_str);
}

void RegSetDate(char* date_str, void(*Callback)(char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char tok_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char *tok;
	uint8_t err = 1;
	uint16_t ton;

	strcpy(tok_str, date_str);
	tok = strtok(tok_str, "-");
	if (tok) ton = atol(tok_str);
	if(tok && (ton > 2000)) {
		tok = strtok(0, "-");
		if (tok) ton = atoi(tok);
		if(tok && (ton <= 12)) {
			tok = strtok(0, "-");
			if (tok) ton = atoi(tok);
			if(tok && (ton <= 31)) {
				err = 0;
				strcpy_hal(out_str, F("Date string is valid"));
				Callback(out_str);

#ifndef FF_ARDUINO
			    struct tm *tmptr;
			    time_t now;
			    time_t t;
				int rc;
				now = TimeNow();
				tmptr = localtime(&now);
			    strptime(date_str, "%Y-%m-%d", tmptr);
			    t = mktime(tmptr);
			    rc = stime(&t);
			    if(rc==0) {
					strcpy(out_str, "System stime() successful.\n");
			        Callback(out_str);
			    }
			    else {
			        sprintf(out_str, "Error: stime() failed, errno = %d: %s\n", errno, strerror(errno));
			        Callback(out_str);
			    }
#else
			    uint8_t rc = HALSetRTCDate(date_str);
			    if(rc==0) {
			    	strcpy_hal(out_str, F("HALSetRTCDate() successful.\n"));
			        Callback(out_str);
			    }
			    else {
					strcpy_hal(out_str, F("Error: HALSetRTCDate() failed.\n"));
			        Callback(out_str);
			    }
#endif //ifndef FF_ARDUINO

			}
		}
	}
	if (err) {
		strcpy_hal(out_str, F("Error: Date String not valid."));
		Callback(out_str);
	}

}

void RegSystemReboot(void(*Callback)(char*)) {
	HALReboot();
	Callback(NULL); //stop compiler complaining
}

void RegBlockIDOn(uint16_t block_id, void(*Callback)(char*)) {
	RegSendCommandToBlockID(block_id, CMD_OUTPUT_ON, Callback);
}

void RegBlockIDOff(uint16_t block_id, void(*Callback)(char*)) {
	RegSendCommandToBlockID(block_id, CMD_OUTPUT_OFF, Callback);
}

/********************************************************************************************
 * State Register and UI Data Set Functions
 ********************************************************************************************/


UIDataSet* GetUIDataSet(void) {
	return &sr.ui_data;
}

void UpdateStateRegister(uint16_t source, uint8_t msg_type, uint8_t msg_str, int32_t i_val, float f_val) {

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






