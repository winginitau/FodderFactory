/************************************************
 ff_ini_config.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 INI Config Processing
 ************************************************/


/************************************************
  Includes
************************************************/
#include "ff_ini_config.h"
#include "ff_sys_config.h"
//#include "ff_HAL.h"
#include "ff_string_consts.h"
//#include "ff_events.h"
#include "ff_registry.h"
#include <string.h>
#include <stdio.h>
#include "ff_debug.h"
#include <stdlib.h>
#include "ff_datetime.h"
#include "ff_utils.h"
#include "ff_IniFile.h"

#ifdef FF_SIMULATOR
#endif

/************************************************
  Data Structures
************************************************/


/************************************************
  Globals
************************************************/



/************************************************
  Functions
************************************************/

uint8_t GetConfKeyIndex(uint8_t block_cat, const char* key_str) {

	uint8_t last_key = UINT8_INIT;
	uint8_t key_idx = 0; //see "string_consts.h" Zero is error.

	//lock the last key index to the appropriate block category

	switch (block_cat) {
		case FF_ERROR_CAT:
			DebugLog(SSS, E_STOP, M_CONFKEY_ERROR_CAT);
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
			DebugLog(SSS, E_STOP, M_CONFKEY_LAST_BCAT);
			while(1);
	}

	//check that we have a key that matches one of the keys strings of the block category

	#ifdef PROGMEM_BLOCK_DEFS
		while ((strcmp_P(key_str, block_cat_defs[block_cat].conf_keys[key_idx]) != 0) && key_idx < last_key) {
			key_idx++;
		}
	#else
		while ((strcmp(key_str, block_cat_defs[block_cat].conf_keys[key_idx]) != 0) && key_idx < last_key) {
			key_idx++;
		}
	#endif

	if (key_idx == last_key) {
		DebugLog(SSS, E_STOP, M_CONFKEY_NOTIN_DEFS);
		while(1);
	} else {
		return key_idx;
	}
}

uint8_t ConfigureCommonSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str){
	switch (key_idx) {
		case SYS_ERROR_KEY:
			// or any other (0) block cat error key
			DebugLog(SSS, E_STOP, M_KEY_IDX_ZERO);
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
#ifndef	EXCLUDE_DISPLAYNAME
			strcpy(block_ptr->display_name, value_str);
#endif
			break;
		case SYS_DESCRIPTION:
			// or case IN_DESCRIPTION
			// or case MON_DESCRIPTION:
			// or case SCH_DESCRIPTION:
			// or case RL_DESCRIPTION:
			// or case CON_DESCRIPTION:
			// or case OUT_DESCRIPTION:
#ifndef EXCLUDE_DESCRIPTION
			strcpy(block_ptr->description, value_str);
#endif
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
			DebugLog(SSS, E_ERROR, M_SYS_BAD_DEF);
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
#ifdef USE_PROGMEM
			while (u < LAST_UNIT && strcmp_P(unit_strings[u].text, value_str)) {
				u++;
			}
#else
			while (u < LAST_UNIT && strcmp(unit_strings[u].text, value_str)) {
				u++;
			}
#endif
			if (u < LAST_UNIT) {
				block_ptr->settings.in.data_units = u;
			} else {
				block_ptr->settings.in.data_units = 255;
				DebugLog(SSS, E_ERROR, M_BAD_DATA_UNITS);
				return 0;
			}
			break;
		}
		case IN_DATA_TYPE:
			//XXX so what - either float or int presently - inferred from block type, conider dropping
			break;
		default:
			DebugLog(SSS, E_ERROR, M_IN_BAD_DEF);
			return 0;
			break;
	} // switch(key_idx)
	return 1;
}

uint8_t ConfigureMONSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case MON_INPUT1:
			block_ptr->settings.mon.input1 = GetBlockIDByLabel(value_str);
			break;
		case MON_INPUT2:
			block_ptr->settings.mon.input2 = GetBlockIDByLabel(value_str);
			break;
		case MON_INPUT3:
			block_ptr->settings.mon.input3 = GetBlockIDByLabel(value_str);
			break;
		case MON_INPUT4:
			block_ptr->settings.mon.input4 = GetBlockIDByLabel(value_str);
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
			DebugLog(SSS, E_ERROR, M_MON_BAD_DEF);
			return 0;
			break;
	} // switch(key_idx)
	return 1;
}

uint8_t ConfigureSCHSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case SCH_DAYS:
			if (DayStrToFlag(block_ptr->settings.sch.days, value_str) != 1) {
				DebugLog(SSS, E_WARNING, M_DAY_FLAG_EMPTY);
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
			DebugLog(SSS, E_ERROR, M_SCH_BAD_DEF);
			return 0;
			break;
	} // switch(key_idx)

	return 1;
}

uint8_t ConfigureRLSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case RL_PARAM_1:
			block_ptr->settings.rl.param1 = GetBlockIDByLabel(value_str);
			break;
		case RL_PARAM_2:
			block_ptr->settings.rl.param2 = GetBlockIDByLabel(value_str);
			break;
		case RL_PARAM_3:
			block_ptr->settings.rl.param3 = GetBlockIDByLabel(value_str);
			break;
		case RL_PARAM_NOT:
			block_ptr->settings.rl.param_not = GetBlockIDByLabel(value_str);
			break;
		default:
			DebugLog(SSS, E_ERROR, M_RL_BAD_DEF);
			return 0;
			break;
	} // switch(key_idx)

	return 1;
}

uint8_t ConfigureCONSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case CON_RULE:
			block_ptr->settings.con.rule = GetBlockIDByLabel(value_str);
			break;
		case CON_OUTPUT:
			block_ptr->settings.con.output = GetBlockIDByLabel(value_str);
			break;
		case CON_ACT_CMD: {
			uint8_t c = 0;
#ifdef USE_PROGMEM
			while (c < LAST_COMMAND && strcmp_P(command_strings[c].text, value_str)) {
				c++;
			}

#else
			while (c < LAST_COMMAND && strcmp(command_strings[c].text, value_str)) {
				c++;
			}
#endif
			if (c < LAST_COMMAND) {
				block_ptr->settings.con.act_cmd = c;
			} else {
				block_ptr->settings.con.act_cmd = UINT8_INIT;
				DebugLog(SSS, E_ERROR, M_ACT_CMD_UNKNOWN);
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
				DebugLog(SSS, E_ERROR, M_DEACT_CMD_UNKNOWN);
				return 0;
			}
			break;
		}
		default:
			DebugLog(SSS, E_ERROR, M_CON_BAD_DEF);
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
			DebugLog(SSS, E_ERROR, M_OUT_BAD_DEF);
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
	block_ptr = GetBlockByLabel(block_label);

	//not found - add a new block
	if (block_ptr == NULL) {
		block_ptr = AddBlock(block_cat, block_label); //add a new one
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
				DebugLog(SSS, E_ERROR, M_BAD_BLOCK_CAT);
				return_value = 0;
				break;
		} //switch(block_cat)

	} else {
		DebugLog(SSS, E_ERROR, M_ADDING_BLOCK);
		return_value = 0;
	}
	return return_value;
}



#ifdef FF_CONFIG

int uint8_tWrite(uint8_t data, FILE *f) {
	return fwrite(&data, sizeof(data), 1, f);
	//printf("%d\n", data);
	//return fprintf(f, "%d\n", data);
}

int uint16_tWrite(uint16_t data, FILE *f) {
	return fwrite(&data, sizeof(data), 1, f);
	//printf("%d\n", data);
	//return fprintf(f, "%d\n", data);
}

int uint32_tWrite(uint32_t data, FILE *f) {
	return fwrite(&data, sizeof(data), 1, f);
	//printf("%d\n", data);
	//return fprintf(f, "%d\n", data);
}

int LabelWrite(char* data, size_t s,  FILE *f) {
	return fwrite(data, s, 1, f);
	//puts(data);
	//printf("%s\n", data);
	//return fwrite(data, s, 1, f);
}

int uint8_tArrayWrite(uint8_t* data, size_t s,  FILE *f) {
	return fwrite(data, s, 1, f);
	//for (uint i=0; i<s; i++) {
	//	printf("%d\n", *(data+i));
	//	fprintf(f, "%d\n", *(data+i));
	//}
	//return 1;
}

int floatWrite(float data, FILE *f) {
	return fwrite(&data, sizeof(data) , 1, f);
	//printf("%.2f\n", data);
	//return fprintf(f, "%.2f\n", data);
}

void WriteBlock(BlockNode *b, FILE *f) {

	uint8_tWrite(b->block_cat, f);
	uint16_tWrite(b->block_type, f);
	uint16_tWrite(b->block_id, f);
	LabelWrite(b->block_label, MAX_LABEL_LENGTH, f);
	#ifndef	EXCLUDE_DISPLAYNAME
	LabelWrite(b->display_name, MAX_LABEL_LENGTH, f);
	#endif
	#ifndef EXCLUDE_DESCRIPTION
	LabelWrite(b->description, MAX_DESCR_LENGTH, f);
	#endif

	switch (b->block_cat) {
	case FF_SYSTEM:
		uint8_tWrite(b->settings.sys.temp_scale, f);
		uint8_tWrite(b->settings.sys.language, f);
		uint8_tWrite(b->settings.sys.week_start, f);
		break;
	case FF_INPUT:
		uint8_tWrite(b->settings.in.interface, f);
		uint8_tWrite(b->settings.in.if_num, f);
		uint32_tWrite(b->settings.in.log_rate, f);
		uint8_tWrite(b->settings.in.data_units, f);
		uint8_tWrite(b->settings.in.data_type, f);
		break;
	case FF_MONITOR:
		uint16_tWrite(b->settings.mon.input1, f);
		uint16_tWrite(b->settings.mon.input2, f);
		uint16_tWrite(b->settings.mon.input3, f);
		uint16_tWrite(b->settings.mon.input4, f);
		floatWrite(b->settings.mon.act_val, f);
		floatWrite(b->settings.mon.deact_val, f);
		break;
	case FF_SCHEDULE:
		uint8_tArrayWrite(b->settings.sch.days, 7, f);
		uint32_tWrite(b->settings.sch.time_start, f);
		uint32_tWrite(b->settings.sch.time_end, f);
		uint32_tWrite(b->settings.sch.time_duration, f);
		uint32_tWrite(b->settings.sch.time_repeat, f);
		break;
	case FF_RULE:
		uint16_tWrite(b->settings.rl.param1, f);
		uint16_tWrite(b->settings.rl.param2, f);
		uint16_tWrite(b->settings.rl.param3, f);
		uint16_tWrite(b->settings.rl.param_not, f);
		break;
	case FF_CONTROLLER:
		uint16_tWrite(b->settings.con.rule, f);
		uint16_tWrite(b->settings.con.output, f);
		uint8_tWrite(b->settings.con.act_cmd, f);
		uint8_tWrite(b->settings.con.deact_cmd, f);
		break;
	case FF_OUTPUT:
		uint8_tWrite(b->settings.out.interface, f);
		uint8_tWrite(b->settings.out.if_num, f);
		uint8_tWrite(b->settings.out.command, f);
		break;
	default:

		break;
	}

}


void FileIODispatcher(void(*func)(BlockNode*, FILE*), FILE* file) {
	BlockNode* block_ptr;

	block_ptr = GetBlockListHead();

	while (block_ptr != NULL) {
		func(block_ptr, file);
		block_ptr = block_ptr->next_block;
	}
}

void WriteRunningConfig(void) {
	FILE *outfile;
	outfile = fopen(BIN_CONFIG_FILENAME, "w");
	FileIODispatcher(WriteBlock, outfile);
	fclose(outfile);
}

#endif //FF_CONFIG


#ifdef USE_PROGMEM
char* GetINIError(uint8_t e, char* msg_buf) {
	//SimpleStringArray temp;
	char temp[MAX_MESSAGE_STRING_LENGTH];
	//XXX on review temp.text is unitialised and sizeof(temp) shouldn't get the string
	strcpy_P (temp, ini_error_strings[e].text);
	strcpy(msg_buf, temp);
	return msg_buf;
}
#else
char* GetINIError(uint8_t e, char* msg_buf) {

	strcpy(msg_buf, ini_error_strings[e].text);
	return msg_buf;
}
#endif


void InitConfFile(IniFile* cf) {
	char key_value[INI_FILE_MAX_LINE_LENGTH];
	if (!cf->open()) {
		DebugLog(SSS, E_STOP, M_NO_CONFIG);
		// Cannot do anything else
		while (1)
			;
	}
	// Check the file is valid. This can be used to warn if any lines
	// are longer than the buffer.
	if (!cf->validate(key_value, INI_FILE_MAX_LINE_LENGTH)) {
		DebugLog(SSS, E_STOP, M_CONFIG_NOT_VALID);
		// Cannot do anything else
		while (1)
			;
	}
}


void ReadAndParseConfig(void) {
	const char f_name[] = CONFIG_FILENAME;
#ifdef FF_ARDUINO
	IniFile cf(f_name);
#endif
#ifdef FF_SIMULATOR
	char f_mode[] = "r\0";
	IniFile cf(f_name, f_mode);
#endif

	char list_section[MAX_LABEL_LENGTH];		// [inputs] [controllers] etc
	char list_section_key[MAX_LABEL_LENGTH];	// input7=    controller3=
	char block_section[MAX_LABEL_LENGTH];
	char block_section_key[MAX_LABEL_LENGTH];
	char key_value[INI_FILE_MAX_LINE_LENGTH];	//to read value into
	uint8_t bl;									//iterator - block list within block category
	uint8_t block_count = 0;					//used for information display only
	uint8_t last_key = 0;
	uint8_t block_cat = FF_SYSTEM;				//block_cat 0 will always be Error - start from system
	char debug_msg[MAX_LOG_LINE_LENGTH];
	char ini_error_string[MAX_LABEL_LENGTH];

	InitConfFile(&cf);

	//Read and process the system block for
	//global settings - language and scale etc
	block_cat = (FF_SYSTEM);
	last_key = LAST_SYS_KEY_TYPE;

	//DebugLog(SSS, E_INFO, M_RP_CONFIG);
	//DebugLog(SSS, E_VERBOSE, M_PROC_SYS_BLK);


	for (int key = 1; key < last_key; key++) {  //see string_consts.h - 0 reserved for error types.
		#ifdef PROGMEM_BLOCK_DEFS
			strcpy_P(block_section_key, block_cat_defs[block_cat].conf_keys[key]);
		#else
			strcpy(block_section_key, block_cat_defs[block_cat].conf_keys[key]);
		#endif
		if (cf.getValue("system", block_section_key, key_value, INI_FILE_MAX_LINE_LENGTH)) {
			if (ConfigureBlock(FF_SYSTEM, "SYSTEM", block_section_key, key_value)) {
				sprintf(debug_msg, "[CONFIG][SYSTEM][%s] = %s", block_section_key, key_value);
				DebugLog(debug_msg);
			} else {
				sprintf(debug_msg, "[CONFIG] FAILED [SYSTEM][%s]: %s ", block_section_key, key_value);
				DebugLog(debug_msg);
			}
		} else {
			if( cf.getError() != errorKeyNotFound) {
				sprintf(debug_msg, "ERROR [CONFIG][SYSTEM][%s]: %s ", block_section_key, GetINIError(cf.getError(), ini_error_string));
				DebugLog(debug_msg);
				//handle error - could be ok if key not required for that block sub type
			}
		}
	}

	// Read each of the block category list sections
	// getting all the user defined labels and registering
	// them in the registry via the ConfigureBlock call.
	// Note they can't be fully configured
	// in this iteration because blocks refer to other blocks
	// so we need the full list of block identifiers registered first
	// Calls to ConfigureBlock with NULL for setting::value pair
	// trigger registration only.

	block_cat = (FF_SYSTEM + 1); 			//general block cat defs always follow the reserved system cat.
	bl = 1; 								//block list start from "1" in config file "input1=" etc
	uint8_t last_found = 0;					//set flag on first read error
											//TODO this can fail if lists are not numbered correctly
	DebugLog(SSS, E_VERBOSE, M_CONF_REG_BLKS);


	for (; block_cat < LAST_BLOCK_CAT; block_cat++) { //iterate through each block category
		last_found = 0;
		while (bl < MAX_BLOCKS_PER_CATEGORY && !last_found) { 		// a b_cat<bl> = <Label> is still to process (potentially)
			#ifdef PROGMEM_BLOCK_DEFS
				strcpy_P(list_section, block_cat_defs[block_cat].conf_section_label);	//list_section
				sprintf_P(list_section_key, "%S%d", block_cat_defs[block_cat].conf_section_key_base, bl);	//list_section_key
			#else
				strcpy(list_section, block_cat_defs[block_cat].conf_section_label);	//list_section
				sprintf(list_section_key, "%s%d", block_cat_defs[block_cat].conf_section_key_base, bl);	//list_section_key
			#endif
			key_value[0] = '\0';

			if (cf.getValue(list_section, list_section_key, key_value,
			INI_FILE_MAX_LINE_LENGTH)) {

				sprintf(debug_msg, "[%s][%s] = %s", list_section, list_section_key, key_value);
				DebugLog(debug_msg);

				//////////////////// label registration only to allow block cross references when registering the settings later
				if (ConfigureBlock(block_cat, key_value, NULL, NULL)) {	//register the block
					block_count++;
//					sprintf(debug_msg, "Registered: block_cat=[%d] label=[%s]", block_cat, key_value);
//					DebugLog(debug_msg);
				} else {
					sprintf(debug_msg, "[CONFIG] REGISTRATION FAILED [%s][%s][%s]", list_section, list_section_key, key_value);
					DebugLog(debug_msg);
				}
				////////////////////

				bl++;

			} else {
				#ifdef PROGMEM_BLOCK_DEFS
				if( cf.getError() == errorKeyNotFound) {  // this is expected once key list end is found
					sprintf_P(debug_msg, "[CONFIG] Registered %d [%S]", bl-1, block_cat_defs[block_cat].conf_section_label);
				} else {
					sprintf_P(debug_msg, "[CONFIG] LAST [%s] REACHED - [%S]: %s ", block_cat_defs[block_cat].conf_section_label, list_section_key, GetINIError(cf.getError(), ini_error_string));
				}
				#else
				if( cf.getError() == errorKeyNotFound) {  // this is expected once key list end is found
					sprintf(debug_msg, "[CONFIG] Registered %d [%s]", bl-1, block_cat_defs[block_cat].conf_section_label);
				} else {
					sprintf(debug_msg, "[CONFIG] LAST [%s] REACHED - [%s]: %s ", block_cat_defs[block_cat].conf_section_label, list_section_key, GetINIError(cf.getError(), ini_error_string));
				}
				#endif
				DebugLog(debug_msg);
				last_found = 1;
				bl = 1;
			}
		}
	}

	// We now have a block list registered but no settings
	// set up to go second pass and get all the details
	// updating the registry with ConfigureBlock calls.

	block_cat = (FF_SYSTEM + 1);
	bl = 1; 		//block list start from "1" in config file "input1=" etc

	DebugLog(SSS, E_VERBOSE, M_CONF_BLKS);

	for (; block_cat < LAST_BLOCK_CAT; block_cat++) { //iterate through each block category
		last_found = 0;
		while (bl < MAX_BLOCKS_PER_CATEGORY && !last_found) { 		// a b_cat<bl> = <Label> is still to process (potentially)
			#ifdef PROGMEM_BLOCK_DEFS
				strcpy_P(list_section, block_cat_defs[block_cat].conf_section_label);	//list_section
				sprintf_P(list_section_key, "%S%d", block_cat_defs[block_cat].conf_section_key_base, bl);	//list_section_key
			#else
				strcpy(list_section, block_cat_defs[block_cat].conf_section_label);	//list_section
				sprintf(list_section_key, "%s%d", block_cat_defs[block_cat].conf_section_key_base, bl);	//list_section_key
			#endif
			key_value[0] = '\0';

			if (cf.getValue(list_section, list_section_key, key_value, INI_FILE_MAX_LINE_LENGTH)) {

//				sprintf(debug_msg, "[%s][%s] = %s", list_section, list_section_key, key_value);
//				DebugLog(debug_msg);

				strcpy(block_section, key_value); //look for a block section with that label

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
				for (int key = 1; key < last_key; key++) {
					#ifdef PROGMEM_BLOCK_DEFS
						strcpy_P(block_section_key, block_cat_defs[block_cat].conf_keys[key]);
					#else
						strcpy(block_section_key, block_cat_defs[block_cat].conf_keys[key]);
					#endif
					if (cf.getValue(block_section, block_section_key, key_value,
					INI_FILE_MAX_LINE_LENGTH)) {

						if (ConfigureBlock(block_cat, block_section, block_section_key, key_value)) {
//							sprintf(debug_msg, "CONFIGURED [%s][%s][%s][%s] = %s", list_section, list_section_key, block_section, block_section_key, key_value);
//							DebugLog(debug_msg);
						} else {
							sprintf(debug_msg, "[CONFIG] CONFIG FAILED [%s][%s][%s][%s]: %s ", list_section, list_section_key, block_section, block_section_key, key_value);
							DebugLog(debug_msg);
						}

					} else {
						if( cf.getError() != errorKeyNotFound) {
							sprintf(debug_msg, "[CONFIG] ERROR [%s][%s][%s][%s]: %s ", list_section, list_section_key, block_section, block_section_key, GetINIError(cf.getError(), ini_error_string));
							DebugLog(debug_msg);
							//handle error - could be ok if key not required for that block sub type
						}
					}
				}

				bl++;

			} else {
				#ifdef PROGMEM_BLOCK_DEFS
				if( cf.getError() == errorKeyNotFound) {
					sprintf_P(debug_msg, "[CONFIG] Processed %d [%S]", bl-1, block_cat_defs[block_cat].conf_section_label);
				} else {
					sprintf_P(debug_msg, "[CONFIG] LAST [%S] REACHED - [%s]: %s ", block_cat_defs[block_cat].conf_section_label, list_section_key, GetINIError(cf.getError(), ini_error_string));
				}
				#else
				if( cf.getError() == errorKeyNotFound) {
					sprintf(debug_msg, "[CONFIG] Processed %d [%s]", bl-1, block_cat_defs[block_cat].conf_section_label);
				} else {
					sprintf(debug_msg, "[CONFIG] LAST [%s] REACHED - [%s]: %s ", block_cat_defs[block_cat].conf_section_label, list_section_key, GetINIError(cf.getError(), ini_error_string));
				}
				#endif
				DebugLog(debug_msg);
				last_found = 1;
				bl = 1;
			}
		}
	}

	cf.close();
}




