/************************************************
 ff_registry.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Component Registry
************************************************/

/************************************************
 Includes
************************************************/
#include <build_config.h>
#include <system.h>
#include <controllers.h>
#include <debug_ff.h>
#include <display.h>
#include <inputs.h>
#include <monitors.h>
#include <outputs.h>
#include <registry.h>
#include <rules.h>
#include <schedules.h>
#include <validate.h>
#include <stdlib.h>
#include <string.h>
#include <string_consts.h>
#include <time.h>
#include <utils.h>
#include <block_common.h>


#ifndef PLATFORM_ARDUINO
#include <stdio.h>
#include <errno.h>
#endif //ndef PLATFORM_ARDUINO

#include <HAL.h>
#include <init_config.h>

#ifdef USE_ITCH
#include <out.h>
#endif

/************************************************
 Data Structures
************************************************/

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
static uint16_t block_count = 0;	// Increments to assign block_id. TODO remove once static BIDs is implemented
static BlockNode *bll = NULL;		// Block Linked List - variant record block list

#ifdef ARDUINO_LCD
	static FFStateRegister sr;
#endif //ARDUINO_LCD

/************************************************
 Functions
************************************************/

void Setup(BlockNode *b) {
	// Called on blocks to configure their initial run-time settings (not config)
	// 	and perform any preliminary setup actions.
	// Blocks that successfully complete setup will change state to STATUS_ENABLED_INIT
	//	which will allow them to be polled in the main operating loop.
	// Blocks are expected to have a status of STATUS_DISABLED_INIT before setup
	//	will be called. This provides a measure of block list consistency by
	//	avoiding run time issues such as blocks referring to other blocks that have
	//	not yet been fully configured. For example, during system load processes,
	// 	a final set post config is to move the blocks to STATUS_DISABLED_INIT. Otherwise,
	//	blocks could become operational prematurely.

	switch (b->block_cat) {
	case FF_SYSTEM:
		#ifdef UI_ATTACHED
		UpdateUI();
		#endif //UI_ATTACHED
		SystemSetup(b);
		break;
	case FF_INPUT:
		if (b->status == STATUS_DISABLED_INIT) {
			InputSetup(b);
		} else {
			EventMsg(b->block_id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
		}
		break;
	case FF_MONITOR:
		if (b->status == STATUS_DISABLED_INIT) {
			MonitorSetup(b);
		} else {
			EventMsg(b->block_id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
		}
		break;
	case FF_SCHEDULE: {
		if (b->status == STATUS_DISABLED_INIT) {
			ScheduleSetup(b);
		} else {
			EventMsg(b->block_id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
		}
	}
		break;
	case FF_RULE: {
		if (b->status == STATUS_DISABLED_INIT) {
			RuleSetup(b);
		} else {
			EventMsg(b->block_id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
		}
	}
		break;
	case FF_CONTROLLER:
		if (b->status == STATUS_DISABLED_INIT) {
			ControllerSetup(b);
		} else {
			EventMsg(b->block_id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
		}
		break;
	case FF_OUTPUT:
		if (b->status == STATUS_DISABLED_INIT) {
			OutputSetup(b);
		} else {
			EventMsg(b->block_id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
		}
		break;
	default:
		DebugLog(SSS, E_ERROR, M_SET_BCAT_NOT_MATCHED);
		break;
	}
}


void Operate(BlockNode *b) {
	// Calls the appropriate operate function on the block based on its category
	// Called from process dispatcher with each block in the list.
	// Blocks must have a status that is ENABLED (ie < INIT_DISABLED) for
	// 	operate to be called on them.
	// Under system control, blocks may only transition to an ENABLED state as follows:
	//	Block created by AddBlockNode -> STATUS_DISABLED_INIT
	//  STATUS_DISABLED_INIT -> may have Setup called (all other states reject Setup())
	//	Setup -> STATUS_ENABLED_INIT and lower.

	switch (b->block_cat) {
	case FF_SYSTEM: {
		#ifdef UI_ATTACHED
		UpdateUI();
		#endif
		SystemOperate(b);
		break;
	}
	case FF_INPUT:
		if (b->status < STATUS_DISABLED) {
			InputOperate(b);
		}
		break;
	case FF_MONITOR:
		if (b->status < STATUS_DISABLED) {
			MonitorOperate(b);
		}
		break;
	case FF_SCHEDULE:
		if (b->status < STATUS_DISABLED) {
			ScheduleOperate(b);
		}
		break;
	case FF_RULE:
		if (b->status < STATUS_DISABLED) {
			RuleOperate(b);
		}
		break;
	case FF_CONTROLLER:
		if (b->status < STATUS_DISABLED) {
			ControllerOperate(b);
		}
		break;
	case FF_OUTPUT:
		if (b->status < STATUS_DISABLED) {
			OutputOperate(b);
		}
		break;
	default:
		DebugLog(SSS, E_ERROR, M_OPR_BCAT_NOT_MATCHED);
		break;
	}
}

void InitDisable(BlockNode* b) {
	// Change blocks from their current status to
	// STATUS_DISABLED_INIT which means that Operate() will not be
	// 	called on them.
	b->status = STATUS_DISABLED_INIT;
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
	// Iterative rather than recursive to save stack space
	// Returns NULL if end of block list reached without matching
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


uint16_t GetBlockIDByLabel(const char* label, bool report_fail) {
	// Within core code this is used to ensure complete internal block list
	// consistency and should report error ie called with report_fail = true
	// Within itch this is being called to check for existence of complete
	// labels in the parser and is called with report_fail = false for normal
	// itch behaviour when looking up partial labels
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
	if (report_fail == true) {
		char msg_str[31];
		strcpy_hal(msg_str, F("ERROR: B-Label Not Found: [%s]"));
		sprintf(debug_msg, msg_str, label);
		DebugLog(debug_msg);
	}
	return M_FF_ERROR;
}

char const* GetBlockLabelString(uint16_t block_id) {

	if (block_id == BLOCK_ID_NA) {
		return "NA";
	}

	if (block_id == UINT16_INIT) {
		return "\0";
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
		// Block Label not found
		if (block_id == SSS) {
			// Possibly pre-config message
			return "SYSTEM-NOCONFIG";
		}
		char msg_str[30];
		strcpy_hal(msg_str, F("ERROR: B-ID Not Found: [%d]"));
		sprintf(debug_msg, msg_str, block_id);
		// XXX This should raise an Event Message too
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
		#ifndef	EXCLUDE_DISPLAYNAME
			new_block->display_name = NULL;
		#endif
		#ifndef EXCLUDE_DESCRIPTION
			new_block->description = NULL;
		#endif

		new_block->block_cat = block_cat;
		new_block->block_type = UINT8_INIT;

		// initialise operational, run-time block data
		new_block->active = 0;
		new_block->bool_val = 0;
		new_block->int_val = INT32_INIT;
		new_block->f_val = FLOAT_INIT;
		new_block->last_update = UINT32_INIT;
		new_block->last_logged = UINT32_INIT;
		// New blocks are disabled by default
		new_block->status = STATUS_DISABLED_INIT;

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
					new_block->settings.sys.language = ENGLISH;
					new_block->settings.sys.temp_scale = UNIT_ERROR;
					new_block->settings.sys.week_start = SUN;
					new_block->settings.sys.start_delay = 0;
					break;
				case FF_INPUT:
					new_block->settings.in.interface = IF_ERROR;
					new_block->settings.in.if_num = UINT8_INIT;
					new_block->settings.in.log_rate = TV_TYPE_INIT;
					new_block->settings.in.poll_rate = TV_TYPE_INIT;
					new_block->settings.in.data_units = UNIT_ERROR;
					new_block->settings.in.data_type = UINT8_INIT;
					break;
				case FF_MONITOR:
					new_block->settings.mon.input1 = BLOCK_ID_INIT;
					new_block->settings.mon.input2 = BLOCK_ID_INIT;
					new_block->settings.mon.input3 = BLOCK_ID_INIT;
					new_block->settings.mon.input4 = BLOCK_ID_INIT;
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
					new_block->settings.rl.param1 = BLOCK_ID_INIT;
					new_block->settings.rl.param2 = BLOCK_ID_INIT;
					new_block->settings.rl.param3 = BLOCK_ID_INIT;
					new_block->settings.rl.param_not = BLOCK_ID_INIT;
					break;
				case FF_CONTROLLER:
					new_block->settings.con.rule = BLOCK_ID_INIT;
					new_block->settings.con.output = BLOCK_ID_INIT;
					new_block->settings.con.act_cmd = CMD_ERROR;
					new_block->settings.con.deact_cmd = CMD_ERROR;
					break;
				case FF_OUTPUT:
					new_block->settings.out.interface = IF_ERROR;
					new_block->settings.out.if_num = UINT8_INIT;
					break;
			} //switch
		} // != FF_GENERIC_BLOCK
		*head_ref = new_block;
		return new_block;
	}
	return AddBlockNode(&((*head_ref)->next_block), block_cat, block_label);

}

uint8_t GetBlockCatIDByName(char* name) {
	uint8_t id = LAST_BLOCK_CAT - 1;
	char cat_name[MAX_LABEL_LENGTH];

 	for (; id > 0; id--) {
		strcpy_hal(cat_name, block_cat_names[id].text);
		if (strcasecmp(name, cat_name) == 0) {
			return id;
		}
	}
	return id;
}

uint8_t DeleteBlockByID(uint16_t bid) {
	BlockNode* walker = bll;
	BlockNode* previous = bll;
	while (walker != NULL) {
		if(walker->block_id == bid) {
			//found - delete it
			if (walker == bll) {
				//special case - head of list
				bll = walker->next_block;
				free(walker->block_label);
				free(walker);
				return 1;
			} else {
				//delete and relink previous to next
				previous->next_block = walker->next_block;
				free(walker->block_label);
				free(walker);
				return 1;
			}
		} else {
			// wasn't that one, move to next keeping track of previous
			previous = walker;
			walker = walker->next_block;
		}
	}
	return 0; //error
}

uint8_t DeleteBlockByLabel(const char *label) {
	// In an edge case of a duplicate block ID with a different label
	// this deletes based on label search rather than a label lookup
	// for ID and then delete by ID
	BlockNode* walker = bll;
	BlockNode* previous = bll;
	while (walker != NULL) {
		if(strcmp(walker->block_label, label) == 0) {
			//found - delete it
			if (walker == bll) {
				//special case - head of list
				bll = walker->next_block;
				free(walker->block_label);
				free(walker);
				return 1;
			} else {
				//delete and relink previous to next
				previous->next_block = walker->next_block;
				free(walker->block_label);
				free(walker);
				return 1;
			}
		} else {
			// wasn't that one, move to next keeping track of previous
			previous = walker;
			walker = walker->next_block;
		}
	}
	return 0; //error
}

BlockNode* AddBlock(uint8_t block_cat, const char *block_label) {
	// Low level function to add a block to the list
	// Note: no checking for duplicates or existence.
	// Use higher level function ConfigureBlock for safer application
	// Implementation: to save stack memory, iterate down the block list
	//  and only call AddBlockNode function once at the end of the list

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

char* UpdateBlockLabel(BlockNode* b, const char * block_label) {
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

#ifndef EXCLUDE_DISPLAYNAME
char* UpdateDisplayName(BlockNode* b, const char * display_name) {
	// Update the display_name of a block and adjust its memory use
	// Possible calling value combinations:
	// b->display_name == NULL and display_name != NULL - malloc (new display_name to previously NULL display_name)
	// b->display_name == NULL and display_name == NULL - do nothing (NULL display_name to existing NULL display_name)
	// b->display_name != NULL and display_name == NULL - free b->display_name (Existing display_name to be NULL'd and freed)
	// b->display_name != NULL and display_name != NULL - realloc b->display_name (Different display_name - free and re-malloc)

	if (b->display_name == NULL) {
		// not previously malloced
		if (display_name != NULL) {
			b->display_name = (char *)malloc( (strlen(display_name)+1) * sizeof(char) );
			// XXX add null check and debug throw
			strcpy(b->display_name, display_name);
		} else {
			// block_label is NULL
			return NULL;
		}
	} else {
		// b->block label already assigned
		if(display_name == NULL) {
			// update to NULL
			free(b->display_name);
			b->display_name = NULL;
			return NULL;
		} else {
			// XXX realloc fails at link time. Workaround: free and malloc again
			free(b->display_name);
			b->display_name = (char *)malloc( (strlen(display_name)+1) * sizeof(char) );
			// XXX add null check and debug throw
			strcpy(b->display_name, display_name);
		}
	}
	return b->display_name;
}
#endif

#ifndef EXCLUDE_DESCRIPTION
char* UpdateDescription(BlockNode* b, const char * description) {
	// Update the description of a block and adjust its memory use
	// Possible calling value combinations:
	// b->description == NULL and description != NULL - malloc (new description to previously NULL description)
	// b->description == NULL and description == NULL - do nothing (NULL description to existing NULL description)
	// b->description != NULL and description == NULL - free b->description (Existing description to be NULL'd and freed)
	// b->description != NULL and description != NULL - realloc b->description (Different description - free and re-malloc)

	if (b->description == NULL) {
		// not previously malloced
		if (description != NULL) {
			b->description = (char *)malloc( (strlen(description)+1) * sizeof(char) );
			// XXX add null check and debug throw
			strcpy(b->description, description);
		} else {
			// block_label is NULL
			return NULL;
		}
	} else {
		// b->block label already assigned
		if(description == NULL) {
			// update to NULL
			free(b->description);
			b->description = NULL;
			return NULL;
		} else {
			// XXX realloc fails at link time. Workaround: free and malloc again
			free(b->description);
			b->description = (char *)malloc( (strlen(description)+1) * sizeof(char) );
			// XXX add null check and debug throw
			strcpy(b->description, description);
		}
	}
	return b->description;
}
#endif

BlockNode* GetBlockNodeByLabel(BlockNode *head, const char *block_label) {
	// re-written iteratively

	BlockNode* walker;
	walker = head;
	while(walker != NULL) {
		if (strcmp(walker->block_label, block_label) == 0) {
			return walker;
		} else {
			walker = walker->next_block;
		}
	}
	return walker;
}

BlockNode* GetBlockByLabel(const char *block_label) {
	return GetBlockNodeByLabel(bll, block_label);
}

void DropBlockList(void) {
	BlockNode* block_ptr;
	BlockNode* next_ptr;

	block_ptr = bll;

	while (block_ptr != NULL) {
		next_ptr = block_ptr->next_block;
		free(block_ptr->block_label);
		#ifndef	EXCLUDE_DISPLAYNAME
			free(block_ptr->display_name);
		#endif
		#ifndef EXCLUDE_DESCRIPTION
			free(block_ptr->description);
		#endif
		free(block_ptr);
		block_ptr = next_ptr;
	}
	bll = NULL;
}


/********************************************************************************************
 * ITCH Integration Handlers
 ********************************************************************************************/

uint8_t RegLookupBlockLabel(const char* lookup_string) {
	return GetBlockIDByLabel(lookup_string, false);
}

void RegShowBlocks(void(*Callback)(const char *)) {
	BlockNode* b;
	b = bll;
	char out[MAX_LOG_LINE_LENGTH];
	char fmt_str[20];

	if (b == NULL) {
		strcpy_hal(out, F("Empty List"));
		Callback(out);
	} else {
		strcpy_hal(out, F("\tBID\tLABEL\t\tCAT\t\tTYPE"));
		Callback(out);
		while(b != NULL) {
			//strcpy_hal(fmt_str, F("\t%d\t%d\t%d\t%s"));
			strcpy_hal(fmt_str, F("\t%d\t"));
			sprintf(out, fmt_str, b->block_id);

			strcat(out, b->block_label);

			if (strlen(out) <= 20) {
				strcat_hal(out, F("\t\t"));
			} else {
				strcat_hal(out, F("\t"));
			}

			strcat_hal(out, block_cat_names[b->block_cat].text);

			if (strlen(out) > 31) {
				strcat_hal(out, F("\t"));
			} else {
				//if (strlen(out) > 29) {
					strcat_hal(out, F("\t\t"));
				//} else {
				//	strcat_hal(out, F("\t\t\t"));
				//}
			}
			strcat_hal(out, block_type_strings[b->block_type].text);

			//sprintf(out, fmt_str, b->block_id, b->block_cat, b->block_type, b->block_label);
			Callback(out);
			b = b->next_block;
		}
	}
}

void RegShowSystem(void(*Callback)(const char *)) {
	RegShowBlockByID(SSS, Callback);
}

void RegShowBlockByLabel(const char* block_label, void(*Callback)(const char *)) {
	RegShowBlockByID(GetBlockIDByLabel(block_label, true), Callback);
}

void RegShowBlockByID(uint16_t id, void(*Callback)(const char *)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char fmt_str[MAX_LABEL_LENGTH];
	//char label_str[MAX_LABEL_LENGTH];

	BlockNode *b;
	b = GetBlockByID(bll, id);

	if (b == NULL) {
		strcpy_hal(fmt_str, F("Error: %d is not a valid block ID"));
		sprintf(out_str, fmt_str, id);
		Callback(out_str);
	} else {

		switch (b->block_cat) {
			case (FF_SYSTEM):
				SystemShow(b, Callback);
				break;
			case FF_INPUT:
				InputShow(b, Callback);
				break;
			case FF_MONITOR:
				MonitorShow(b, Callback);
				break;
			case FF_SCHEDULE:
				ScheduleShow(b, Callback);
				break;
			case FF_RULE:
				RuleShow(b, Callback);
				break;
			case FF_CONTROLLER:
				ControllerShow(b, Callback);
				break;
			case FF_OUTPUT:
				OutputShow(b, Callback);
				break;
		}
	}
}

void RegSetCommandOnBlockLabel(const char* block_label, uint16_t command, void(*Callback)(const char *)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	uint16_t block_id;
	char fmt_str[35];

	block_id = GetBlockIDByLabel(block_label, true);
	if (block_id == M_FF_ERROR) {
		strcpy_hal(fmt_str, F("Block: %s not found"));
		sprintf(out_str, fmt_str, block_label);
		Callback(out_str);
	} else {
		RegSetCommandOnBlockID(block_id, command, Callback);
	}
}

void RegSetCommandOnBlockID(uint16_t id, uint16_t command, void(*Callback)(const char*)) {
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
		switch(command) {
			case CMD_OUTPUT_ON:
				SetCommand(id, command);
				EventMsg(SSS, id, E_ADMIN, M_CMD_ACT);
				break;
			case CMD_OUTPUT_OFF:
				SetCommand(id, command);
				EventMsg(SSS, id, E_ADMIN, M_CMD_DEACT);
				break;
			default:
				EventMsg(SSS, id, E_ADMIN, M_ADMIN_CMD_NOT_SUPPORTED);
				break;
		}


	}

}

void RegShowTime(void(*Callback)(const char*)) {
	char hms_str[12];
	time_t now;
	char fmt_str[9];

	now = TimeNow();
	strcpy_hal(fmt_str, F("%H:%M:%S"));
	strftime(hms_str, 12, fmt_str, localtime(&now));
	Callback(hms_str);
}

void RegSetTime(const char* time_str, void(*Callback)(const char*)) {
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

#ifdef PLATFORM_LINUX
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
#endif //PLATFORM_LINUX
#ifdef PLATFORM_ARDUINO
			    uint8_t rc = HALSetRTCTime(time_str);
			    if(rc==0) {
					strcpy_hal(out_str, F("HALSetRTCTime() successful.\n"));
			        Callback(out_str);
			    }
			    else {
					strcpy_hal(out_str, F("Error: HALSetRTCTime() failed.\n"));
			        Callback(out_str);
			    }
#endif //PLATFORM_ARDUINO

			}
		}
	}
	if (err) {
		strcpy_hal(out_str, F("Error: Time String not valid."));
		Callback(out_str);
	}

}

void RegShowDate(void(*Callback)(const char*)) {
	char ymd_str[14];
	time_t now;
	char fmt_str[9];

	now = TimeNow();
	strcpy_hal(fmt_str, F("%Y-%m-%d"));
	strftime(ymd_str, 14, fmt_str, localtime(&now));
	Callback(ymd_str);
}

void RegSetDate(const char* date_str, void(*Callback)(const char*)) {
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

#ifndef PLATFORM_ARDUINO
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
#endif //ifndef PLATFORM_ARDUINO

			}
		}
	}
	if (err) {
		strcpy_hal(out_str, F("Error: Date String not valid."));
		Callback(out_str);
	}

}

void RegConfigClear(void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80

	DropBlockList();
	strcpy_hal(out_str, F("Block List Reset"));
	Callback(out_str);
}


void RegConfigLoad(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	InitConfigLoad(1);
}

#ifdef RESURRECT_DEPRECIATED
void RegConfigLoadBinary(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	InitConfigLoadBinary();
}
#endif //RESURRECT_DEPRECIATED

/* 2019-06-14 Depreciated.
See Comments in init_config.cpp, function InitConfigLoadINI()
void RegConfigLoadINI(void(*Callback)(char*)) {
	(void)Callback; 		//not used
	InitConfigLoadINI();
}
*/

void RegConfigSave(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	InitConfigSave();
}

#ifdef RESURRECT_DEPRECIATED
void RegConfigSaveBinary(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	InitConfigSaveBinary();
}
#endif //RESURRECT_DEPRECIATED

void RegConfigBlockSystem(const char* param1_string, uint16_t SYS_CONFIG, const char* param2_string, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char key[MAX_LABEL_LENGTH];

	strcpy_hal(key, sys_config_keys[SYS_CONFIG].text);
	if (ConfigureBlock(FF_SYSTEM, param1_string, key, param2_string) == 0) {
		GetMessageString(out_str, M_CONFIGURE_BLOCK_ERROR);
		Callback(out_str);
		strcpy_hal(out_str, F(" >>> CONFIG system "));
		strcat(out_str, param1_string);
		strcat_hal(out_str, F(" "));
		strcat_hal(out_str, key);
		strcat_hal(out_str, F(" "));
		strcat(out_str, param2_string);
		Callback(out_str);
	}
}

void RegConfigBlockInput(const char* param1_string, uint16_t IN_CONFIG, const char* param2_string, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char key[MAX_LABEL_LENGTH];

	strcpy_hal(key, in_config_keys[IN_CONFIG].text);
	if (ConfigureBlock(FF_INPUT, param1_string, key, param2_string) == 0) {
		GetMessageString(out_str, M_CONFIGURE_BLOCK_ERROR);
		Callback(out_str);
		strcpy_hal(out_str, F(" >>> CONFIG input "));
		strcat(out_str, param1_string);
		strcat_hal(out_str, F(" "));
		strcat_hal(out_str, key);
		strcat_hal(out_str, F(" "));
		strcat(out_str, param2_string);
		Callback(out_str);
	}
}

void RegConfigBlockMonitor(const char* param1_string, uint16_t MON_CONFIG, const char* param2_string, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char key[MAX_LABEL_LENGTH];

	strcpy_hal(key, mon_config_keys[MON_CONFIG].text);
	if (ConfigureBlock(FF_MONITOR, param1_string, key, param2_string) == 0) {
		GetMessageString(out_str, M_CONFIGURE_BLOCK_ERROR);
		Callback(out_str);
		strcpy_hal(out_str, F(" >>> CONFIG monitor "));
		strcat(out_str, param1_string);
		strcat_hal(out_str, F(" "));
		strcat_hal(out_str, key);
		strcat_hal(out_str, F(" "));
		strcat(out_str, param2_string);
		Callback(out_str);
	}
}

void RegConfigBlockSchedule(const char* param1_string, uint16_t SCH_CONFIG, const char* param2_string, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char key[MAX_LABEL_LENGTH];

	strcpy_hal(key, sch_config_keys[SCH_CONFIG].text);
	if (ConfigureBlock(FF_SCHEDULE, param1_string, key, param2_string) == 0) {
		GetMessageString(out_str, M_CONFIGURE_BLOCK_ERROR);
		Callback(out_str);
		strcpy_hal(out_str, F(" >>> CONFIG schedule "));
		strcat(out_str, param1_string);
		strcat_hal(out_str, F(" "));
		strcat_hal(out_str, key);
		strcat_hal(out_str, F(" "));
		strcat(out_str, param2_string);
		Callback(out_str);
	}
}

void RegConfigBlockRule(const char* param1_string, uint16_t RL_CONFIG, const char* param2_string, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char key[MAX_LABEL_LENGTH];

	strcpy_hal(key, rl_config_keys[RL_CONFIG].text);
	if (ConfigureBlock(FF_RULE, param1_string, key, param2_string) == 0) {
		GetMessageString(out_str, M_CONFIGURE_BLOCK_ERROR);
		Callback(out_str);
		strcpy_hal(out_str, F(" >>> CONFIG rule "));
		strcat(out_str, param1_string);
		strcat_hal(out_str, F(" "));
		strcat_hal(out_str, key);
		strcat_hal(out_str, F(" "));
		strcat(out_str, param2_string);
		Callback(out_str);
	}
}

void RegConfigBlockController(const char* param1_string, uint16_t CON_CONFIG, const char* param2_string, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char key[MAX_LABEL_LENGTH];

	strcpy_hal(key, con_config_keys[CON_CONFIG].text);
	if (ConfigureBlock(FF_CONTROLLER, param1_string, key, param2_string) == 0) {
		GetMessageString(out_str, M_CONFIGURE_BLOCK_ERROR);
		Callback(out_str);
		strcpy_hal(out_str, F(" >>> CONFIG controller "));
		strcat(out_str, param1_string);
		strcat_hal(out_str, F(" "));
		strcat_hal(out_str, key);
		strcat_hal(out_str, F(" "));
		strcat(out_str, param2_string);
		Callback(out_str);
	}
}

void RegConfigBlockOutput(const char* param1_string, uint16_t OUT_CONFIG, const char* param2_string, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char key[MAX_LABEL_LENGTH];

	strcpy_hal(key, out_config_keys[OUT_CONFIG].text);
	if (ConfigureBlock(FF_OUTPUT, param1_string, key, param2_string) == 0) {
		GetMessageString(out_str, M_CONFIGURE_BLOCK_ERROR);
		Callback(out_str);
		strcpy_hal(out_str, F(" >>> CONFIG output "));
		strcat(out_str, param1_string);
		strcat_hal(out_str, F(" "));
		strcat_hal(out_str, key);
		strcat_hal(out_str, F(" "));
		strcat(out_str, param2_string);
		Callback(out_str);
	}
}


void RegInitSetupAll(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	ProcessDispatcher(Setup);
}

void RegInitSetupBID(uint16_t block_id, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b != NULL) {
		Setup(b);
	} else {
		strcpy_hal(out_str, F("Unknown Block ID"));
		Callback(out_str);
	}
}

void RegInitValidateAll(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	ProcessDispatcher(Validate);
}

void RegInitValidateBID(uint16_t block_id, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b != NULL) {
		Validate(b);
	} else {
		strcpy_hal(out_str, F("Unknown Block ID"));
		Callback(out_str);
	}
}

void RegInitDisableAll(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	ProcessDispatcher(InitDisable);
}

void RegInitDisableBID(uint16_t block_id, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b != NULL) {
		InitDisable(b);
	} else {
		strcpy_hal(out_str, F("Unknown Block ID"));
		Callback(out_str);
	}
}

void RegAdminDisableBID(uint16_t block_id, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b != NULL) {
		b->status = STATUS_DISABLED_ADMIN;
	} else {
		strcpy_hal(out_str, F("Unknown Block ID"));
		Callback(out_str);
	}
}

void RegAdminEnableBID(uint16_t block_id, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b != NULL) {
		b->status = STATUS_DISABLED_INIT;
	} else {
		strcpy_hal(out_str, F("Unknown Block ID"));
		Callback(out_str);
	}
}

void RegAdminDeleteBID(uint16_t block_id, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	if (DeleteBlockByID(block_id) == 1) {
		strcpy_hal(out_str, F("Block Deleted"));
		Callback(out_str);
	} else {
		strcpy_hal(out_str, F("DeleteBlockByID Reported an Error"));
		Callback(out_str);
	}
}

void RegAdminDeleteBlockLabel(const char* BLOCK_LABEL, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	if (DeleteBlockByLabel(BLOCK_LABEL) == 1) {
		strcpy_hal(out_str, F("Block Deleted"));
		Callback(out_str);
	} else {
		strcpy_hal(out_str, F("DeleteBlockByLabel Reported an Error"));
		Callback(out_str);
	}
}


void RegSystemReboot(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	HALReboot();
}

void RegAdminCmdOnBID(uint16_t block_id, void(*Callback)(const char*)) {
	RegSetCommandOnBlockID(block_id, CMD_OUTPUT_ON, Callback);
}

void RegAdminCmdOffBID(uint16_t block_id, void(*Callback)(const char*)) {
	RegSetCommandOnBlockID(block_id, CMD_OUTPUT_OFF, Callback);
}

/********************************************************************************************
 * State Register and UI Data Set Functions
 ********************************************************************************************/

#ifdef ARDUINO_LCD
UIDataSet* GetUIDataSet(void) {
	return &sr.ui_data;
}
#endif //ARDUINO_LCD

void UpdateStateRegister(uint16_t source, uint8_t msg_type, uint8_t msg_str, int32_t i_val, float f_val) {

	(void)i_val;	// not used on the LCD

	//TODO include further registry block logic update here
	//TODO redo - remove hard coding perhaps **** INDEED
	//2019-06 Prob not needed for now - this was only used for the LCD display

#ifdef ARDUINO_LCD
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
#else //!ARDUINO_LCD
	(void)source;
	(void)msg_type;
	(void)msg_str;
	(void)i_val;
	(void)f_val;
#endif //ARDUINO_LCD
}

void InitStateRegister(void) {
#ifdef ARDUINO_LCD
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
#endif //ARDUINO_LCD
}






