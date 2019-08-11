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
#include <interfaces.h>
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
static InterfaceNode *ill = NULL;


#ifdef ARDUINO_LCD
	static FFStateRegister sr;
#endif //ARDUINO_LCD

/************************************************
 Functions
************************************************/

void BlockDispatcher(void (*func)(BlockNode*)) {
	BlockNode* block_ptr;

	block_ptr = bll;

	while (block_ptr != NULL) {
		func(block_ptr);
		block_ptr = block_ptr->next;
	}
}

void InterfaceDispatcher(void (*func)(InterfaceNode*)) {
	InterfaceNode* interface_ptr;

	interface_ptr = ill;

	while (interface_ptr != NULL) {
		func(interface_ptr);
		interface_ptr = interface_ptr->next;
	}
}

void SetupInterface(InterfaceNode* b) {
	if (b->status == STATUS_DISABLED_INIT) {
		switch(b->type) {
			case IF_ONEWIRE_BUS:
				InterfaceONEWIRE_BUS_Setup(b);
				break;
			case IF_DS1820B:
				InterfaceDS1820B_Setup(b);
		}
	} else {
		EventMsg(b->id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
	}
}

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

	switch (b->cat) {
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
			EventMsg(b->id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
		}
		break;
	case FF_MONITOR:
		if (b->status == STATUS_DISABLED_INIT) {
			MonitorSetup(b);
		} else {
			EventMsg(b->id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
		}
		break;
	case FF_SCHEDULE: {
		if (b->status == STATUS_DISABLED_INIT) {
			ScheduleSetup(b);
		} else {
			EventMsg(b->id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
		}
	}
		break;
	case FF_RULE: {
		if (b->status == STATUS_DISABLED_INIT) {
			RuleSetup(b);
		} else {
			EventMsg(b->id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
		}
	}
		break;
	case FF_CONTROLLER:
		if (b->status == STATUS_DISABLED_INIT) {
			ControllerSetup(b);
		} else {
			EventMsg(b->id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
		}
		break;
	case FF_OUTPUT:
		if (b->status == STATUS_DISABLED_INIT) {
			OutputSetup(b);
		} else {
			EventMsg(b->id, E_WARNING, M_SETUP_ON_BLOCK_NOT_DISABLED_INIT);
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

	switch (b->cat) {
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

void InitDisableInterface(InterfaceNode * b) {
	// Change blocks from their current status to
	// STATUS_DISABLED_INIT which means that Operate() will not be
	// 	called on them.
	b->status = STATUS_DISABLED_INIT;
}
void InitDisable(BlockNode* b) {
	// Change blocks from their current status to
	// STATUS_DISABLED_INIT which means that Operate() will not be
	// 	called on them.
	b->status = STATUS_DISABLED_INIT;
}

BlockNode* GetLastBlockAddr(void) {
	BlockNode* block_ptr;

	block_ptr = bll;

	if (block_ptr != NULL) {
		while (block_ptr->next != NULL) {
			block_ptr = block_ptr->next;
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
		if(walker->id == block_id) {
			return walker;
		} else {
			walker = walker->next;
		}
	}
	return walker;
}

InterfaceNode* GetInterfaceByID(InterfaceNode *head, uint16_t block_id) {
	// Iterative rather than recursive to save stack space
	// Returns NULL if end of block list reached without matching
	InterfaceNode* walker;
	walker = head;
	while(walker != NULL) {
		if(walker->id == block_id) {
			return walker;
		} else {
			walker = walker->next;
		}
	}
	return walker;
}

InterfaceNode* GetInterfaceByID(uint16_t id) {
	InterfaceNode* walker;
	walker = ill;
	while(walker != NULL) {
		if(walker->id == id) {
			return walker;
		} else {
			walker = walker->next;
		}
	}
	return walker;
}

uint16_t GetInterfaceIDByLabel(const char* label) {
	// Returns 0 if end of block list reached without matching
	InterfaceNode* walker;
	walker = ill;
	while(walker != NULL) {
		if(strcmp(walker->label, label) == 0) {
			return walker->id;
		} else {
			walker = walker->next;
		}
	}
	return 0;
}

uint8_t SetCommand(uint16_t block_id, uint8_t cmd_msg) {
	BlockNode *b;
	b = GetBlockByID(bll, block_id);
	if (b) {
		if (b->cat == FF_OUTPUT) {
			b->settings.out.command = cmd_msg;
			return cmd_msg;
		} else {
			DebugLog(b->id, E_ERROR, M_CMD_TO_BLOCK_NOT_OUTPUT);
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
	BlockNode* b;
	InterfaceNode* i;
	char debug_msg[MAX_LOG_LINE_LENGTH];

	b = bll;
	i = ill;

	while (b != NULL) {
		if (strcmp(b->label, label) == 0) {
			return b->id;
		} else {
			b = b->next;
		}
	}
	while (i != NULL) {
		if (strcmp(i->label, label) == 0) {
			return i->id;
		} else {
			i = i->next;
		}
	}

	if (report_fail == true) {
		char msg_str[31];
		strcpy_hal(msg_str, F("ERROR: B-Label Not Found: [%s]"));
		sprintf(debug_msg, msg_str, label);
		DebugLog(debug_msg);
		EventMsg(SSS, E_ERROR, M_BLOCK_ID_NOT_FOUND);
	}
	return M_FF_ERROR;
}

char const* GetBlockLabelString(uint16_t block_id) {
	// Return a pointer to the label string of the block
	// identified by block_id

	BlockNode* b;
	InterfaceNode *i;
	//char debug_msg[MAX_LOG_LINE_LENGTH];
	//char msg_str[30];
	b = bll;
	i = ill;

	/*
	if (block_id == BLOCK_ID_NA) {
		return "NA";
	}

	if (block_id == UINT16_INIT) {
		return "\0";
	}
	 */

	while (b != NULL) {
		if (b->id == block_id) {
				return b->label;
		}
		b = b->next;
	}

	// not found in bll

	while (i != NULL) {
		if (i->id == block_id) {
				return i->label;
		}
		i = i->next;
	}

	/*
	// Block Label not found
	if (block_id == SSS) {
		// Possibly pre-config message
		//return "SYSTEM-NOCONFIG";
		//XXX
		return NULL;
	}
	*/

	/*
	strcpy_hal(msg_str, F("ERROR: B-ID Not Found: [%d]"));
	sprintf(debug_msg, msg_str, block_id);
	DebugLog(debug_msg);
	EventMsg(SSS, E_ERROR, M_BLOCK_ID_NOT_FOUND);
	*/
	return NULL;
}


BlockNode* GetBlockListHead(void) {
	return bll;
}

InterfaceNode* GetInterfaceListHead(void) {
	return ill;
}

BlockNode* NewBlockNode(uint8_t block_cat, const char *block_label) {
	BlockNode* new_block;

	// common settings and setting holders for all blocks
	new_block = (BlockNode *) malloc(sizeof(BlockNode));
	if (new_block == NULL) {
		DebugLog(SSS, E_STOP, M_ADDBLOCK_ERROR);
		while (1)
			;
	} else {
		//			DebugLog("(AddBlock) malloc OK");
	}
	new_block->next = NULL;
	new_block->label = NULL;
	#ifndef	EXCLUDE_DISPLAYNAME
		new_block->display_name = NULL;
	#endif
	#ifndef EXCLUDE_DESCRIPTION
		new_block->description = NULL;
	#endif

	new_block->cat = block_cat;
	new_block->type = UINT8_INIT;

	// initialise operational, run-time block data
	new_block->active = 0;
	new_block->bool_val = 0;
	new_block->int_val = INT32_INIT;
	new_block->f_val = FLOAT_INIT;
	//new_block->last_update = UINT32_INIT;
	//new_block->last_logged = UINT32_INIT;
	// New blocks are disabled by default
	new_block->status = STATUS_DISABLED_INIT;

	if (block_cat != FF_GENERIC_BLOCK) {

		if (block_cat == FF_SYSTEM) {
			new_block->id = SSS;
		} else {
			new_block->id = BLOCK_ID_BASE + block_count;
			block_count++;
		}

		if(block_label != NULL) {
			// XXX add null checks
			new_block->label = (char *)malloc( (strlen(block_label)+1) * sizeof(char) );
			strcpy(new_block->label, block_label);
		} else {
			new_block->label = NULL;
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
				new_block->settings.in.interface = BLOCK_ID_INIT;
				//new_block->settings.in.if_num = UINT8_INIT;
				new_block->settings.in.log_rate = UINT16_INIT;
				new_block->settings.in.poll_rate = UINT16_INIT;
				new_block->settings.in.last_polled = UINT32_INIT;
				new_block->settings.in.last_logged = UINT32_INIT;

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
				new_block->settings.out.command = CMD_INIT;
				break;
		} //switch
	} // != FF_GENERIC_BLOCK

	return new_block;
}

InterfaceNode* NewInterfaceNode(uint8_t if_type, const char *label) {
	InterfaceNode* new_if;

	new_if = (InterfaceNode *) malloc(sizeof(InterfaceNode));
	if (new_if == NULL) {
		DebugLog(SSS, E_STOP, M_ADDBLOCK_ERROR);
		while (1)
			;
	}
	new_if->next = NULL;
	new_if->label = NULL;
	#ifndef	EXCLUDE_DISPLAYNAME
	new_if->display_name = NULL;
	#endif
	#ifndef EXCLUDE_DESCRIPTION
	new_if->description = NULL;
	#endif

	new_if->type = if_type;
	new_if->status = STATUS_DISABLED_INIT;

	new_if->id = BLOCK_ID_BASE + block_count;
	block_count++;

	if (label != NULL) {
		// XXX add null checks
		new_if->label = (char *) malloc((strlen(label) + 1) * sizeof(char));
		strcpy(new_if->label, label);
	} else {
		new_if->label = NULL;
	}

	switch (new_if->type) {
		case IF_WIFI: {
			//"IF_WIFI"
			InterfaceWIFI* ifs; 	// IF Settings
			ifs = (InterfaceWIFI *)malloc(sizeof(InterfaceWIFI));
			if (ifs) {
				//ifs->gateway_staic = {0, 0, 0, 0};
				//ifs->ip_static = {0, 0, 0, 0};
				//ifs->netmask_static = {0, 0, 0, 0};
				//ifs->static_address = 0;
				//ifs->wifi_ssid = "";
				//ifs->wpa_psk = "";
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_ETHERNET: {
			//"IF_ETHERNET"
			InterfaceETHERNET* ifs; 	// IF Settings
			ifs = (InterfaceETHERNET *)malloc(sizeof(InterfaceETHERNET));
			if (ifs) {
				//ifs->ip_gateway = (0, 0, 0, 0);
				//ifs->ip_static = {0, 0, 0, 0};
				//ifs->ip_netmask = {0, 0, 0, 0};
				ifs->static_address = 0;
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_PWM_IN: {
			//"IF_PWM_IN"
			InterfacePWM_IN* ifs; 	// IF Settings
			ifs = (InterfacePWM_IN *)malloc(sizeof(InterfacePWM_IN));
			if (ifs) {
				// Field initialisers here
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_PWM_OUT: {
			//"IF_PWM_OUT"
			InterfacePWM_OUT* ifs; 	// IF Settings
			ifs = (InterfacePWM_OUT *)malloc(sizeof(InterfacePWM_OUT));
			if (ifs) {
				// Field initialisers here
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_PPM_IN: {
			//"IF_PPM_IN"
			InterfacePPM_IN* ifs; 	// IF Settings
			ifs = (InterfacePPM_IN *)malloc(sizeof(InterfacePPM_IN));
			if (ifs) {
				// Field initialisers here
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_PPM_OUT: {
			//"IF_PPM_OUT"
			InterfacePPM_OUT* ifs; 	// IF Settings
			ifs = (InterfacePPM_OUT *)malloc(sizeof(InterfacePPM_OUT));
			if (ifs) {
				// Field initialisers here
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_DS1820B: {
			//"IF_DS1820B"
			InterfaceDS1820B* ifs; 	// IF Settings
			ifs = (InterfaceDS1820B *)malloc(sizeof(InterfaceDS1820B));
			if (ifs) {
				// Field initialisers here
				ifs->bus_pin = UINT8_INIT;
				DallasAddressStringToArray("00:00:00:00:00:00:00:00", ifs->dallas_address);
				ifs->in_progress = 0;
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_ONEWIRE_BUS: {
			//"IF_ONEWIRE_BUS"
			InterfaceONEWIRE_BUS* ifs; 	// IF Settings
			ifs = (InterfaceONEWIRE_BUS *)malloc(sizeof(InterfaceONEWIRE_BUS));
			if (ifs) {
				ifs->bus_pin = UINT8_INIT;
				ifs->device_count = UINT8_INIT;
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_DIG_PIN_IN: {
			//"IF_DIG_PIN_IN"
			InterfaceDIG_PIN_IN* ifs; 	// IF Settings
			ifs = (InterfaceDIG_PIN_IN *)malloc(sizeof(InterfaceDIG_PIN_IN));
			if (ifs) {
				// Field initialisers here
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_DIG_PIN_OUT: {
			//"IF_DIG_PIN_OUT"
			InterfaceDIG_PIN_OUT* ifs; 	// IF Settings
			ifs = (InterfaceDIG_PIN_OUT *)malloc(sizeof(InterfaceDIG_PIN_OUT));
			if (ifs) {
				// Field initialisers here
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_VED_VOLTAGE: {
			//"IF_VED_VOLTAGE"
			InterfaceVED_VOLTAGE* ifs; 	// IF Settings
			ifs = (InterfaceVED_VOLTAGE *)malloc(sizeof(InterfaceVED_VOLTAGE));
			if (ifs) {
				// Field initialisers here
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_VED_CURRENT: {
			//"IF_VED_CURRENT"
			InterfaceVED_CURRENT* ifs; 	// IF Settings
			ifs = (InterfaceVED_CURRENT *)malloc(sizeof(InterfaceVED_CURRENT));
			if (ifs) {
				// Field initialisers here
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_VED_POWER: {
			//"IF_VED_POWER"
			InterfaceVED_POWER* ifs; 	// IF Settings
			ifs = (InterfaceVED_POWER *)malloc(sizeof(InterfaceVED_POWER));
			if (ifs) {
				// Field initialisers here
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_VED_SOC: {
			//"IF_VED_SOC"
			InterfaceVED_SOC* ifs; 	// IF Settings
			ifs = (InterfaceVED_SOC *)malloc(sizeof(InterfaceVED_SOC));
			if (ifs) {
				// Field initialisers here
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		case IF_HW_SERIAL: {
			//"IF_VED_SOC"
			InterfaceHW_SERIAL* ifs; 	// IF Settings
			ifs = (InterfaceHW_SERIAL *)malloc(sizeof(InterfaceHW_SERIAL));
			if (ifs) {
				// Field initialisers here
				new_if->IFSettings = ifs;
			} else {
				EventMsg(SSS, E_STOP, M_ADDIFS_MALLOC_NULL);
				#ifdef PLATFORM_ARDUINO
					while(1);
				#endif
				#ifdef PLATFORM_LINUX
					exit(-1);
				#endif
			}
			break;
		}
		default:
			return 0; // catch-all error
	}
	return new_if;
}

uint8_t GetBlockCatIdxByBID(uint16_t bid) {
	BlockNode* b;
	InterfaceNode* i;

	b = bll;
	i = ill;

	while (b != NULL) {
		if (b->id == bid) {
			return b->cat;
		} else {
			b = b->next;
		}
	}
	while (i != NULL) {
		if (i->id == bid)  {
			return FF_INTERFACE;
		} else {
			i = i->next;
		}
	}
	return FF_ERROR_CAT;
}

uint8_t GetBlockCatIdxByBlockCatName(char* name) {
	uint8_t id = LAST_BLOCK_CAT - 1;
	char cat_name[MAX_LABEL_LENGTH];

 	for (; id > 0; id--) {
		strcpy_hal(cat_name, block_cat_strings[id].text);
		if (strcasecmp(name, cat_name) == 0) {
			return id;
		}
	}
	return id;
}

uint8_t DeleteBlockByID(uint16_t bid) {
	BlockNode* bwalker = bll;
	BlockNode* bprevious = bll;

	InterfaceNode* iwalker = ill;
	InterfaceNode* iprevious = ill;

	// Check normal block list first
	while (bwalker != NULL) {
		if(bwalker->id == bid) {
			//found - delete it
			if (bwalker == bll) {
				//special case - head of list
				bll = bwalker->next;
				free(bwalker->label);
				free(bwalker);
				return 1;
			} else {
				//delete and relink previous to next
				bprevious->next = bwalker->next;
				free(bwalker->label);
				free(bwalker);
				return 1;
			}
		} else {
			// wasn't that one, move to next keeping track of bprevious
			bprevious = bwalker;
			bwalker = bwalker->next;
		}
	}
	if (bwalker == NULL) {
		// Search interface list
		while (iwalker != NULL) {
			if(iwalker->id == bid) {
				//found - delete it
				if (iwalker == ill) {
					//special case - head of list
					ill = iwalker->next;
					free(iwalker->label);
					free(iwalker->IFSettings);
					free(iwalker);
					return 1;
				} else {
					//delete and relink previous to next
					iprevious->next = iwalker->next;
					free(iwalker->label);
					free(iwalker->IFSettings);
					free(iwalker);
					return 1;
				}
			} else {
				// wasn't that one, move to next keeping track of previous
				iprevious = iwalker;
				iwalker = iwalker->next;
			}
		}

	}
	return 0; // ID not found
}

uint8_t DeleteBlockByLabel(const char *label) {
	// In an edge case of a duplicate block ID with a different label
	// this deletes based on label search rather than a label lookup
	// for ID and then delete by ID
	BlockNode* bwalker = bll;
	BlockNode* bprevious = bll;

	InterfaceNode* iwalker = ill;
	InterfaceNode* iprevious = ill;

	// Check normal block list first
	while (bwalker != NULL) {
		if(strcmp(bwalker->label, label) == 0) {
			//found - delete it
			if (bwalker == bll) {
				//special case - head of list
				bll = bwalker->next;
				free(bwalker->label);
				free(bwalker);
				return 1;
			} else {
				//delete and relink previous to next
				bprevious->next = bwalker->next;
				free(bwalker->label);
				free(bwalker);
				return 1;
			}
		} else {
			// wasn't that one, move to next keeping track of previous
			bprevious = bwalker;
			bwalker = bwalker->next;
		}
		if (bwalker == NULL) {
			// search the interface list
			while (iwalker != NULL) {
				if(strcmp(iwalker->label, label) == 0) {
					//found - delete it
					if (iwalker == ill) {
						//special case - head of list
						ill = iwalker->next;
						free(iwalker->label);
						free(iwalker);
						return 1;
					} else {
						//delete and relink previous to next
						iprevious->next = iwalker->next;
						free(iwalker->label);
						free(iwalker);
						return 1;
					}
				} else {
					// wasn't that one, move to next keeping track of previous
					iprevious = iwalker;
					iwalker = iwalker->next;
				}
			}
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
		bll = NewBlockNode(block_cat, block_label);
		return bll;
	} else {

		BlockNode* walker = bll;
		while(walker->next != NULL) {
			walker = walker->next;
		}
		walker->next = NewBlockNode(block_cat, block_label);
		return walker->next;
	}
}

InterfaceNode* AddInterface(uint8_t if_type, const char *label) {
	// Low level function to add an interface to the list
	// Note: no checking for duplicates or existence.
	// Use higher level function ConfigureIFBlock for safer application
	// Implementation: to save stack memory, iterate down the block list
	//  and only call AddBlockNode function once at the end of the list

	if (ill == NULL) {
		ill = NewInterfaceNode(if_type, label);
		return ill;
	} else {

		InterfaceNode* walker = ill;
		while(walker->next != NULL) {
			walker = walker->next;
		}
		walker->next = NewInterfaceNode(if_type, label);
		return walker->next;
	}
}

char* UpdateBlockLabel(BlockNode* b, const char * block_label) {
	// Update the label of a block and adjust its memory use
	// Possible calling value combinations:
	// b->label == NULL and block_label != NULL - malloc (new label to previously NULL label)
	// b->label == NULL and block_label == NULL - do nothing (NULL label to existing NULL label)
	// b->label != NULL and block_label == NULL - free b->label (Existing label to be NULL'd and freed)
	// b->label != NULL and block_label != NULL - realloc b->label (Different label - free and re-malloc)

	if (b->label == NULL) {
		// not previously malloced
		if (block_label != NULL) {
			b->label = (char *)malloc( (strlen(block_label)+1) * sizeof(char) );
			// XXX add null check and debug throw
			strcpy(b->label, block_label);
		} else {
			// block_label is NULL
			return NULL;
		}
	} else {
		// b->block label already assigned
		if(block_label == NULL) {
			// update to NULL
			free(b->label);
			b->label = NULL;
			return NULL;
		} else {
			// XXX realloc fails at link time. Workaround: free and malloc again
			free(b->label);
			b->label = (char *)malloc( (strlen(block_label)+1) * sizeof(char) );
			// XXX add null check and debug throw
			strcpy(b->label, block_label);
		}
	}
	return b->label;
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
		if (strcmp(walker->label, block_label) == 0) {
			return walker;
		} else {
			walker = walker->next;
		}
	}
	return walker;
}

BlockNode* GetBlockByLabel(const char *block_label) {
	return GetBlockNodeByLabel(bll, block_label);
}

InterfaceNode* GetInterfaceNodeByLabel(InterfaceNode *head, const char* label) {
	InterfaceNode* walker;
	walker = head;
	while(walker != NULL) {
		if (strcmp(walker->label, label) == 0) {
			return walker;
		} else {
			walker = walker->next;
		}
	}
	return walker;
}

InterfaceNode* GetInterfaceByLabel(const char* label) {
	return GetInterfaceNodeByLabel(ill, label);
}

uint16_t GetInterfaceWithMatchingDallas(uint8_t addr[8]) {
	// Scan the interface list searching IF_DS1820B types
	// compare the address and if the same return the block_id
	// else return 0
	InterfaceNode *b;
	InterfaceDS1820B *ifs;
	b = ill;
	while (b != NULL) {
		if (b->type == IF_DS1820B) {
			ifs = (InterfaceDS1820B *)b->IFSettings;
			if (DallasCompare(addr, ifs->dallas_address) == 0) {
				return b->id;
			}
		}
		b = b->next;
	}
	return 0;
}


void DropBlockList(void) {
	BlockNode* block_ptr;
	BlockNode* next_ptr;

	block_ptr = bll;

	while (block_ptr != NULL) {
		next_ptr = block_ptr->next;
		free(block_ptr->label);
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

void DropInterfaceList(void) {
	InterfaceNode* block_ptr;
	InterfaceNode* next_ptr;

	block_ptr = ill;

	while (block_ptr != NULL) {
		next_ptr = block_ptr->next;
		free(block_ptr->label);
		free(block_ptr->IFSettings);
		#ifndef	EXCLUDE_DISPLAYNAME
			free(block_ptr->display_name);
		#endif
		#ifndef EXCLUDE_DESCRIPTION
			free(block_ptr->description);
		#endif
		free(block_ptr);
		block_ptr = next_ptr;
	}
	ill = NULL;
}

/********************************************************************************************
 * ITCH Integration Handlers
 ********************************************************************************************/

uint8_t RegLookupBlockLabel(const char* lookup_string) {
	return GetBlockIDByLabel(lookup_string, false);
}

void RegShowBlocks(void(*Callback)(const char *)) {
	// XXX this whole business of calculating field widths and manipulating
	// endless strings is complete overkill merely to optimise a diagnostic
	// display.
	// Error checking on valid ranges is however critical - especially
	//  when reading out blocks that have a config file mismatched to firmware
	//  version.

	BlockNode* b;
	InterfaceNode* i;

	char out[MAX_LOG_LINE_LENGTH];
	char fmt[9];

	uint8_t w_id = 0;
	uint8_t w_label = 0;
	uint8_t w_cat = 0;
	uint8_t w_type = 0;
	uint8_t w_status = 0;
	uint8_t w_active = 0;

	// Calculate min field widths
	// Common to all
	w_id = 4;		// nnnn
	w_active = 3;	// "OFF: For Heading "ACT"

	w_cat = strlen_hal(block_cat_strings[FF_INTERFACE].text);

	i = ill;
	while (i != NULL) {
		// Label is in RAM
		if (strlen(i->label) > w_label) {
			w_label = strlen(i->label);
		}
		if (i->type < LAST_INTERFACE) {
			if (strlen_hal(interface_type_strings[i->type].text) > w_type) {
				w_type = strlen_hal(interface_type_strings[i->type].text);
			}
		} else {
			w_type = strlen_hal(interface_type_strings[IF_ERROR].text);
		}

		if (i->status < LAST_STATUS) {
			if (strlen_hal(status_strings[i->status].text) > w_status) {
				w_status = strlen_hal(status_strings[i->status].text);
			}
		} else {
			w_status = strlen_hal(status_strings[STATUS_ERROR].text);
		}

		i = i->next;
	}

	b = bll;
	while (b != NULL) {
		if (b->cat < LAST_BLOCK_CAT) {
			if (strlen_hal(block_cat_strings[b->cat].text) > w_cat) {
				w_cat = strlen_hal(block_cat_strings[b->cat].text);
			}
		} else {
			w_cat = strlen_hal(block_cat_strings[FF_ERROR_CAT].text);
		}

		// label is in RAM
		if (strlen(b->label) > w_label) {
			w_label = strlen(b->label);
		}

		if (b->type < LAST_BLOCK_TYPE) {
			if (strlen_hal(block_type_strings[b->type].text) > w_type) {
				w_type = strlen_hal(block_type_strings[b->type].text);
			}
		} else {
			w_type = strlen_hal(block_type_strings[BT_ERROR].text);
		}

		if (b->status < LAST_STATUS) {
			if (strlen_hal(status_strings[b->status].text) > w_status) {
				w_status = strlen_hal(status_strings[b->status].text);
			}
		} else {
			w_status = strlen_hal(status_strings[LAST_STATUS].text);
		}

		b = b->next;
	}

	b = bll;
	i = ill;
	if ((b != NULL) || (i != NULL)) {
		strcpy_hal(out, F("\t"));
		strcatpad_hal(out, F("BID"), (w_id +2));
		strcatpad_hal(out, F("LABEL"), (w_label +2));
		strcatpad_hal(out, F("ACT"), (w_active +2));
		strcatpad_hal(out, F("CAT"), (w_cat +2));
		strcatpad_hal(out, F("TYPE"), (w_type +2));
		strcatpad_hal(out, F("STATUS"), (w_status +2));
		Callback(out);
	}

	i = ill;
	if (i == NULL) {
		strcpy_hal(out, F("No Interfaces"));
		Callback(out);
	} else {
		while (i != NULL) {
			// id is in RAM on all platforms. fmt from PROGMEM on AVR
			strcpy_hal(fmt, F("\t%-4d  "));
			sprintf(out, fmt, i->id);
			// TODO - write a proper int to string func that pads

			// label is in RAM
			strcatpad(out, i->label, w_label+2);

			// Active is NA for Interfaces
			strcatpad_hal(out, F("NA"), w_active+2);

			// Category string in PROGMEM on AVR
			strcatpad_hal(out, block_cat_strings[FF_INTERFACE].text, w_cat+2);

			// type string in PROGMEM on AVR
			if (i->type < LAST_INTERFACE) {
				strcatpad_hal(out, interface_type_strings[i->type].text, w_type+2);
			} else {
				strcatpad_hal(out, interface_type_strings[IF_ERROR].text, w_type+2);
			}

			// status string in PROGMEM on AVR
			if (i->status < LAST_STATUS) {
				strcatpad_hal(out, status_strings[i->status].text, w_status);
			} else {
				strcatpad_hal(out, status_strings[STATUS_ERROR].text, w_status);
			}

			Callback(out);
			i = i->next;
		}
	}

	b = bll;
	if (b == NULL) {
		strcpy_hal(out, F("No Blocks"));
		Callback(out);
	} else {
		while(b != NULL) {
			// id is in RAM on all platforms. fmt from PROGMEM on AVR
			strcpy_hal(fmt, F("\t%-4d  "));
			sprintf(out, fmt, b->id);

			// label is in RAM
			strcatpad(out, b->label, w_label+2);

			// Active is NA for Interfaces
			if (b->active) {
				strcatpad_hal(out, F("ON"), w_active+2);
			} else {
				strcatpad_hal(out, F("OFF"), w_active+2);
			}

			// Category string in PROGMEM on AVR
			if (b->cat < LAST_BLOCK_CAT) {
				strcatpad_hal(out, block_cat_strings[b->cat].text, w_cat+2);
			} else {
				strcatpad_hal(out, block_cat_strings[FF_ERROR_CAT].text, w_cat+2);
			}

			// type string in PROGMEM on AVR
			if (b->type < LAST_BLOCK_TYPE) {
				strcatpad_hal(out, block_type_strings[b->type].text, w_type+2);
			} else {
				strcatpad_hal(out, block_type_strings[BT_ERROR].text, w_type+2);
			}

			// status string in PROGMEM on AVR
			if (b->status < LAST_STATUS) {
				strcatpad_hal(out, status_strings[b->status].text, w_status);
			} else {
				strcatpad_hal(out, status_strings[STATUS_ERROR].text, w_status);
			}

			Callback(out);
			b = b->next;
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

	BlockNode *b;
	InterfaceNode *i;

	uint8_t bcat = GetBlockCatIdxByBID(id);

	if (bcat == FF_ERROR_CAT) {
		strcpy_hal(fmt_str, F("Error: %d is not a valid block ID"));
		sprintf(out_str, fmt_str, id);
		Callback(out_str);
	} else {
		switch (bcat) {
			case FF_INTERFACE:
				i = GetInterfaceByID(ill, id);
				InterfaceShow(i, Callback);
				break;
			case FF_SYSTEM:
				b = GetBlockByID(bll, id);
				SystemShow(b, Callback);
				break;
			case FF_INPUT:
				b = GetBlockByID(bll, id);
				InputShow(b, Callback);
				break;
			case FF_MONITOR:
				b = GetBlockByID(bll, id);
				MonitorShow(b, Callback);
				break;
			case FF_SCHEDULE:
				b = GetBlockByID(bll, id);
				ScheduleShow(b, Callback);
				break;
			case FF_RULE:
				b = GetBlockByID(bll, id);
				RuleShow(b, Callback);
				break;
			case FF_CONTROLLER:
				b = GetBlockByID(bll, id);
				ControllerShow(b, Callback);
				break;
			case FF_OUTPUT:
				b = GetBlockByID(bll, id);
				OutputShow(b, Callback);
				break;
		}
	}
}

void RegShowMemory(void(*Callback)(const char *)) {
	(void)Callback;
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	GetMemPointers(out_str);
	ITCHWriteLine(out_str);
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

void RegIFOneWireScanBID(int16_t param1_int, void(*Callback)(const char*)) {
	(void)Callback; // not used
	InterfaceNode *i;
	i = GetInterfaceByID(ill, param1_int);
	if (i) {
		ScanAndMatchDallasSensors(i, 1);
	} else {
		ITCHWriteLine(F("Unknown Interface ID"));
	}
}

void RegIFOneWireScanLabel(char* BLOCK_LABEL, void(*Callback)(const char*)) {
	(void)Callback; // not used
	uint16_t id;
	id = GetInterfaceIDByLabel(BLOCK_LABEL);
	if (id) {
		RegIFOneWireScanBID(id, WriteLineCallback);
	} else {
		ITCHWriteLine(F("Unknown Interface Label"));
	}
}

void RegIFOneWireAssignBID(int16_t param1_int, int16_t param2_int, void(*Callback)(const char*)) {
	(void)Callback; // not used
	InterfaceNode *s;
	InterfaceNode *t;
	s = GetInterfaceByID(ill, param1_int);
	t = GetInterfaceByID(ill, param2_int);
	if (s && t) {
		AssignDallasSensor(s, t, 1);
	} else {
		ITCHWriteLine(F("Unknown Interface ID"));
	}
}

void RegIFDS1820BRead(int16_t param1_int, void(*Callback)(const char*)) {
	(void)Callback;

	char out[MAX_LOG_LINE_LENGTH];
	float tempC = 0.0;
	uint8_t result;
	result = InterfaceDS1820B_Read(&tempC, param1_int, 1);
	ITCHWrite(F("Result Code: "));
	Serial.println(result);
	FFFloatToCString(out, tempC);
	ITCHWrite(F("Temperature C: "));
	ITCHWriteLine(out);
}

void RegIFDS1820BTest(int16_t param1_int, void(*Callback)(const char*)) {
	(void)Callback;

	char out[MAX_LOG_LINE_LENGTH];
	float tempC = 0.0;
	uint8_t result = 0;
	uint16_t tries = 500;

	while ( (result != 1) && (tries > 0) ) {
		result = InterfaceDS1820B_Read(&tempC, param1_int, 0);
		Serial.print(result);
		tries --;
	}
	if (result == 1) {
		FFFloatToCString(out, tempC);
		ITCHWrite(F("Temperature C: "));
		ITCHWriteLine(out);
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
			        sprintf(out_str, "System stime() successful.");
			        Callback(out_str);
			    }
			    else {
			        sprintf(out_str, "Error: stime() failed, errno = %d: %s",errno, strerror(errno));
			        Callback(out_str);
			    }
#endif //PLATFORM_LINUX
#ifdef PLATFORM_ARDUINO
			    uint8_t rc = HALSetRTCTime(time_str);
			    if(rc==0) {
					strcpy_hal(out_str, F("HALSetRTCTime() successful"));
			        Callback(out_str);
			    }
			    else {
					strcpy_hal(out_str, F("Error: HALSetRTCTime() failed"));
			        Callback(out_str);
			    }
#endif //PLATFORM_ARDUINO

			}
		}
	}
	if (err) {
		strcpy_hal(out_str, F("Error: Time String not valid"));
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
					strcpy(out_str, "System stime() successful.");
			        Callback(out_str);
			    }
			    else {
			        sprintf(out_str, "Error: stime() failed, errno = %d: %s", errno, strerror(errno));
			        Callback(out_str);
			    }
#else
			    uint8_t rc = HALSetRTCDate(date_str);
			    if(rc==0) {
			    	strcpy_hal(out_str, F("HALSetRTCDate() successful"));
			        Callback(out_str);
			    }
			    else {
					strcpy_hal(out_str, F("Error: HALSetRTCDate() failed"));
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

	DropInterfaceList();
	strcpy_hal(out_str, F("Interface List Reset"));
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

void RegConfigShow(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	InitConfigShow();
}

#ifdef RESURRECT_DEPRECIATED
void RegConfigSaveBinary(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	InitConfigSaveBinary();
}
#endif //RESURRECT_DEPRECIATED

void RegConfigInterface(const char* param1_string, uint16_t IF_CONFIG, const char* param2_string, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char key[MAX_LABEL_LENGTH];

	strcpy_hal(key, if_config_keys[IF_CONFIG].text);
	if (ConfigureInterface(param1_string, key, param2_string) == 0) {
		GetMessageString(out_str, M_CONFIGURE_BLOCK_ERROR);
		Callback(out_str);
		strcpy_hal(out_str, F(" >>> CONFIG system "));
		strcat(out_str, param1_string);
		strcat_hal(out_str, F(" "));
		strcat(out_str, key);
		strcat_hal(out_str, F(" "));
		strcat(out_str, param2_string);
		Callback(out_str);
	}
}

void RegConfigBlockSystem(const char* param1_string, uint16_t SYS_CONFIG, const char* param2_string, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];	// 80
	char key[MAX_LABEL_LENGTH];

	strcpy_hal(key, sys_config_keys[SYS_CONFIG].text);
	if (ConfigureBlock(FF_SYSTEM, param1_string, key, param2_string) == 0) {
		GetMessageString(out_str, M_CONFIGURE_BLOCK_ERROR);
		Callback(out_str);
		strcpy_hal(out_str, F(" >>> CONFIG interface "));
		strcat(out_str, param1_string);
		strcat_hal(out_str, F(" "));
		strcat(out_str, key);
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
		strcat(out_str, key);
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
		strcat(out_str, key);
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
		strcat(out_str, key);
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
		strcat(out_str, key);
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
		strcat(out_str, key);
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
		strcat(out_str, key);
		strcat_hal(out_str, F(" "));
		strcat(out_str, param2_string);
		Callback(out_str);
	}
}


void RegInitSetupAll(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	InterfaceDispatcher(SetupInterface);
	BlockDispatcher(Setup);
}

void RegInitSetupBID(uint16_t block_id, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	BlockNode *b;
	InterfaceNode *i;
	b = GetBlockByID(bll, block_id);
	if (b != NULL) {
		Setup(b);
	} else {
		i = GetInterfaceByID(ill, block_id);
		if (i != NULL) {
			SetupInterface(i);
		} else {
			strcpy_hal(out_str, F("Unknown Block ID"));
			Callback(out_str);
		}
	}
}

void RegInitValidateAll(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	InterfaceDispatcher(ValidateInterface);
	BlockDispatcher(Validate);
}

void RegInitValidateBID(uint16_t block_id, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	BlockNode *b;
	InterfaceNode *i;
	b = GetBlockByID(bll, block_id);
	if (b != NULL) {
		Validate(b);
	} else {
		i = GetInterfaceByID(ill, block_id);
		if (i != NULL) {
			ValidateInterface(i);
		} else {
			strcpy_hal(out_str, F("Unknown Block ID"));
			Callback(out_str);
		}
	}
}

void RegInitDisableAll(void(*Callback)(const char*)) {
	(void)Callback; 		//not used
	InterfaceDispatcher(InitDisableInterface);
	BlockDispatcher(InitDisable);
}

void RegInitDisableBID(uint16_t block_id, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	BlockNode *b;
	InterfaceNode *i;
	b = GetBlockByID(bll, block_id);
	if (b != NULL) {
		InitDisable(b);
	} else {
		i = GetInterfaceByID(ill, block_id);
		if (i != NULL) {
			InitDisableInterface(i);
		} else {
			strcpy_hal(out_str, F("Unknown Block ID"));
			Callback(out_str);
		}
	}
}

void RegAdminDisableBID(uint16_t block_id, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	BlockNode *b;
	InterfaceNode *i;
	b = GetBlockByID(bll, block_id);
	if (b != NULL) {
		b->status = STATUS_DISABLED_ADMIN;
	} else {
		i = GetInterfaceByID(ill, block_id);
		if (i != NULL) {
			b->status = STATUS_DISABLED_ADMIN;
		} else {
			strcpy_hal(out_str, F("Unknown Block ID"));
			Callback(out_str);
		}
	}
}

void RegAdminEnableBID(uint16_t block_id, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	BlockNode *b;
	InterfaceNode *i;
	b = GetBlockByID(bll, block_id);
	if (b != NULL) {
		b->status = STATUS_DISABLED_INIT;
	} else {
		i = GetInterfaceByID(ill, block_id);
		if (i != NULL) {
			b->status = STATUS_DISABLED_INIT;
		} else {
			strcpy_hal(out_str, F("Unknown Block ID"));
			Callback(out_str);
		}
	}
}

void RegConfigDeleteBID(uint16_t block_id, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	if (DeleteBlockByID(block_id) == 1) {
		strcpy_hal(out_str, F("Block / Interface Deleted"));
		Callback(out_str);
	} else {
		strcpy_hal(out_str, F("DeleteBlockByID Reported an Error"));
		Callback(out_str);
	}
}

void RegConfigDeleteBlockLabel(const char* BLOCK_LABEL, void(*Callback)(const char*)) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	if (DeleteBlockByLabel(BLOCK_LABEL) == 1) {
		strcpy_hal(out_str, F("Block / Interface Deleted"));
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


#ifdef PLATFORM_ARDUINO
char* GetMemPointers(char* str) {
/* This function places the current value of the heap and stack pointers in the
 * variables. You can call it from any place in your code and save the data for
 * outputting or displaying later. This allows you to check at different parts of
 * your program flow.
 * The stack pointer starts at the top of RAM and grows downwards. The heap pointer
 * starts just above the static variables etc. and grows upwards. SP should always
 * be larger than HP or you'll be in big trouble! The smaller the gap, the more
 * careful you need to be. Julian Gall 6-Feb-2009.
 */
	uint8_t * heapptr, * stackptr, *bll_tail;

	bll_tail = (uint8_t *)GetLastBlockAddr();
	stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
	heapptr = stackptr;                     // save value of heap pointer
	free(stackptr);      // free up the memory again (sets stackptr to 0)
	stackptr =  (uint8_t *)(SP);           // save value of stack pointer
	char fmt_str[33];
	strcpy_hal(fmt_str, F("ILL: %d  BLL: %d  SP: %d  HP: %d"));
	sprintf(str, fmt_str, (uint16_t)ill, (uint16_t)bll_tail, (uint16_t)stackptr, (uint16_t)heapptr);
	return str;
}
#endif //PLATFORM_ARDUINO






