/************************************************
 ff_ini_config.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 INI Config Processing
 ************************************************/


/************************************************
  Includes
************************************************/
#include <build_config.h>
#include <datetime_ff.h>
#include <debug_ff.h>
#include <debug_ff.h>
#include <display.h>
#include <events.h>
#include <init_config.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <filesystem.h>
#include <HAL.h>
#include <registry.h>
#include <string_consts.h>
#include <utils.h>
#include <validate.h>

#ifdef PLATFORM_LINUX
#include <IniFile_ff.h>
#endif

#ifdef PLATFORM_ARDUINO
#include <SPI.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h
#else  // ENABLE_SOFTWARE_SPI_CLASS
#error ENABLE_SOFTWARE_SPI_CLASS must be set non-zero in SdFat/SdFatConfig.h
#endif  //ENABLE_SOFTWARE_SPI_CLASS
#endif //PLATFORM_ARDUINO

#ifdef USE_ITCH
#include <itch.h>
#endif

#ifdef ITCH_HEADERS_ONLY
#include "itch.h"
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
#ifdef PLATFORM_LINUX
uint8_t uint8_tRead(FILE *f) {
	uint8_t data;
	//fscanf(f, "%hhu", &data);
	fread(&data, sizeof(data), 1, f);
//	printf("%u\n", data);
	return data;
}

uint16_t uint16_tRead(FILE *f) {
	uint16_t data;
//	fscanf(f, "%hu", &data);
	fread(&data, sizeof(data), 1, f);
//	printf("%u\n", data);
	return data;
}

uint32_t uint32_tRead(FILE *f) {
	uint32_t data;
//	fscanf(f, "%u", &data);
	fread(&data, sizeof(data), 1, f);
//	printf("%u\n", data);
	return data;
}

char* LabelRead(char* data, size_t s,  FILE *f) {
	//fgets(data, s, f);
	fread(data, s, 1, f);
//	printf("%s\n", data);
	return data;
}

uint8_t* uint8_tArrayRead(uint8_t* data, size_t s,  FILE *f) {
	fread(data, s, 1, f);
	for (uint8_t i=0; i<s; i++) {
//		fscanf(f, "%hhu", &data[i]);
//		printf("%u\n", data[i]);
	}
	return data;
}

float floatRead(FILE *f) {
	float data;
	fread(&data, sizeof(data), 1, f);
//	fscanf(f, "%f", &data);
//	printf("%.2f\n ", data);
	return data;
}
#endif

#ifdef PLATFORM_ARDUINO
uint8_t uint8_tRead(File *f) {
	uint8_t data;
	//fscanf(f, "%hhu", &data);
	f->read(&data, sizeof(data));
//	printf("%u\n", data);
	return data;
}

uint16_t uint16_tRead(File *f) {
	uint16_t data;
//	fscanf(f, "%hu", &data);
	f->read(&data, sizeof(data));
//	printf("%u\n", data);
	return data;
}

uint32_t uint32_tRead(File *f) {
	uint32_t data;
//	fscanf(f, "%u", &data);
	f->read(&data, sizeof(data));
//	printf("%u\n", data);
	return data;
}

char* LabelRead(char* data, size_t s,  File *f) {
	//fgets(data, s, f);
	f->read(data, s);
//	printf("%s\n", data);
	return data;
}

uint8_t* uint8_tArrayRead(uint8_t* data, size_t s,  File *f) {
	f->read(data, s);
//	for (uint8_t i=0; i<s; i++) {
//		fscanf(f, "%hhu", &data[i]);
//		printf("%u\n", data[i]);
//	}
	return data;
}

float floatRead(File *f) {
	float data;
	f->read(&data, sizeof(data));
//	fscanf(f, "%f", &data);
//	printf("%.2f\n ", data);
	return data;
}
#endif //PLATFORM_ARDUINO


void InitConfigLoadBinary(void) {
//#ifdef OLD_SD
	// Build the block list from a binary config file previously written
	//  by InitConfigSaveBinary()
	// NOTE: no existence or duplicate checking - this function assumes an empty (NULL)
	//  block list therefore DropBlockList() is called first to make sure

	#ifdef PLATFORM_ARDUINO
		EventMsg(SSS, E_INFO, M_BIN_LOAD_NOT_AVAIL);
		return;
	#endif //PLATFORM_ARDUINO

	#ifdef PLATFORM_LINUX
	BlockNode* b;
	uint8_t b_cat; 	//used as read-ahead to test for eof before creating a new block
	char temp_label[MAX_LABEL_LENGTH];
	#ifndef	EXCLUDE_DISPLAYNAME
		char temp_display_name[MAX_DISPLAY_NAME_LENGTH];
	#endif
	#ifndef EXCLUDE_DESCRIPTION
		char temp_description[MAX_DESCR_LENGTH];
	#endif


	DropBlockList();

	#ifdef PLATFORM_LINUX
	FILE *fp;
	fp = fopen(CONFIG_BIN_FILENAME, "r");
	#endif

	#ifdef PLATFORM_ARDUINO
	File f;
	File *fp;
	fp = &f;
	pinMode(SS, OUTPUT);
	pinMode(10, OUTPUT);
	//pinMode(53, OUTPUT);
	#endif //PLATFORM_ARDUINO

	#ifdef PLATFORM_ARDUINO
	// see if the card is present and can be initialized:
	if (SD.begin(10, 11, 12, 13)) {
//	if (SD.begin(10)) {
		//strcpy_hal(temp_label, F("DEBUG INFO sd.begin"));
		//DebugLog(temp_label);
		f = SD.open(CONFIG_BIN_FILENAME, FILE_READ);
		if (!f) {
			strcpy_hal(temp_label, F("NO CONFIG"));
			DebugLog(temp_label);
			// Cannot do anything else
			while (1);
		}
	} else {
		strcpy_hal(temp_label, F("SD FAIL"));
		DebugLog(temp_label);
		while (1);
	}

	#endif //PLATFORM_ARDUINO

	while (1) {    //continue reading the file until EOF break
		b_cat = uint8_tRead(fp); //all blocks start with block category
		//DebugLog("Read b_cat");
		#ifdef PLATFORM_ARDUINO
		if (!f.available()) {
			//DebugLog("Not available - break");
			break;
		}
		#endif //PLATFORM_ARDUINO

		#ifdef PLATFORM_LINUX
		if(feof(fp)) {
//			DebugLog("EOF");
			break;
		}
		#endif

		//b = AddBlock(FF_GENERIC_BLOCK, "NEW_BLANK_BLOCK"); //add a new one
		b = AddBlock(FF_GENERIC_BLOCK, NULL); //add a new one

//DebugLog("Add block passed");
		if (b != NULL) {
			//we now have a valid block ptr pointing to a new block in the list
//DebugLog("AddBlock not NULL");
			b->cat = b_cat;
			b->type = uint16_tRead(fp);
			b->id = uint16_tRead(fp);

			//LabelRead(b->block_label, MAX_LABEL_LENGTH, fp);
			LabelRead(temp_label, MAX_LABEL_LENGTH, fp);
			UpdateBlockLabel(b, temp_label);

			// NOTE: the following excludes need to be same as when the binary was created
			#ifndef	EXCLUDE_DISPLAYNAME
				LabelRead(temp_display_name, MAX_DISPLAY_NAME_LENGTH, fp);
				UpdateDisplayName(b, temp_display_name);
			#endif
			#ifndef EXCLUDE_DESCRIPTION
				LabelRead(temp_description, MAX_DESCR_LENGTH, fp);
				UpdateDescription(b, temp_description);
			#endif
			switch (b->cat) {
				case FF_SYSTEM:
					b->settings.sys.temp_scale = uint8_tRead(fp);
					b->settings.sys.language = uint8_tRead(fp);
					b->settings.sys.week_start = uint8_tRead(fp);
					break;
				case FF_INPUT:
					b->settings.in.interface = uint8_tRead(fp);
					b->settings.in.if_num = uint8_tRead(fp);
					b->settings.in.log_rate = uint32_tRead(fp);
					b->settings.in.data_units = uint8_tRead(fp);
					b->settings.in.data_type = uint8_tRead(fp);
					break;
				case FF_MONITOR:
					b->settings.mon.input1 = uint16_tRead(fp);
					b->settings.mon.input2 = uint16_tRead(fp);
					b->settings.mon.input3 = uint16_tRead(fp);
					b->settings.mon.input4 = uint16_tRead(fp);
					b->settings.mon.act_val = floatRead(fp);
					b->settings.mon.deact_val = floatRead(fp);
					break;
				case FF_SCHEDULE:
					uint8_tArrayRead(b->settings.sch.days, 7, fp);
					b->settings.sch.time_start = uint32_tRead(fp);
					b->settings.sch.time_end = uint32_tRead(fp);
					b->settings.sch.time_duration = uint32_tRead(fp);
					b->settings.sch.time_repeat = uint32_tRead(fp);
					break;
				case FF_RULE:
					b->settings.rl.param1 = uint16_tRead(fp);
					b->settings.rl.param2 = uint16_tRead(fp);
					b->settings.rl.param3 = uint16_tRead(fp);
					b->settings.rl.param_not = uint16_tRead(fp);
					break;
				case FF_CONTROLLER:
					b->settings.con.rule = uint16_tRead(fp);
					b->settings.con.output = uint16_tRead(fp);
					b->settings.con.act_cmd = uint8_tRead(fp);
					b->settings.con.deact_cmd = uint8_tRead(fp);
					break;
				case FF_OUTPUT:
					b->settings.out.interface = uint8_tRead(fp);
					b->settings.out.if_num = uint8_tRead(fp);
					b->settings.out.command = uint8_tRead(fp);
					break;
				default:

					break;
			} //switch
		} // if b != NULL
		DebugLog(b->id, E_VERBOSE, M_BLOCK_READ_BINARY);
	};

	#ifdef PLATFORM_LINUX
	fclose(fp);
	#endif
	#ifdef PLATFORM_ARDUINO
//	DebugLog("Prior f.close");
	f.close();
//	DebugLog("After f.close, prior SD.end");
	SD.end();
//	DebugLog("After SD.end");
	#endif //PLATFORM_ARDUINO

	// As a fresh block list, call validate and setup on each block too
	BlockDispatcher(Validate);
	BlockDispatcher(Setup);
	#endif //PLATFORM_LINUX
}

void InitSystem(void) {
	// Set up the global system state register
	InitStateRegister();
	// set up the event log ring buffer for IPC (and debug hooks to talk to the outside world)
	EventBufferInit();
	// log event - core internal system now functional
	#ifdef PLATFORM_ARDUINO
		EventMsg(SSS, E_INFO, M_FF_AWAKE, 0, 0);
	#endif //PLATFORM_ARDUINO
	#ifdef PLATFORM_LINUX
	EventMsg(SSS, E_INFO, M_SIM);
	#endif

	#ifdef DEBUG_SERIAL
	EventMsg(SSS, E_VERBOSE, M_D_SERIAL, 0, 0);
	#endif

	InitRTC();				// set up the Real Time Clock and synch system time to rtc

	#ifdef UI_ATTACHED
	// setup real UI (LCD, TFT, QT, None)
	//TODO - extend beyond LCD
	InitUI(); 				// set up UI arrangements
	// TODO display some timed status pages
	#endif

	// check for a file system
	InitFileSystem(); // DEPR placeholder - no longer really needed

	#ifdef PLATFORM_LINUX
		srand(TimeNow()); //random seed for simulator
	#endif

#ifdef PLATFORM_ARDUINO
#ifdef DEBUG_DALLAS
//		while (1) {
			InitTempSensors();
//			delay(1000);
//		}
#endif
#endif //PLATFORM_ARDUINO
}

uint8_t GetConfKeyIndex(uint8_t block_cat, const char* key_str) {
	// Search the relevant CAT_config_strings for the key__str
	//  and return the index (which is the related enum).
	// If block_cat invalid, report error and STOP - should never happen.
	// If the key_str can't be found in the relevant string list,
	//	report and return 0 (which is the CAT_CONFIG_ERROR key in each CAT)

	uint8_t last_key = UINT8_INIT;
	uint8_t key_idx = 0; 			//see "string_consts.h" Zero is error.

	//lock the last key index to the appropriate block category
	switch (block_cat) {
		case FF_ERROR_CAT:
			DebugLog(SSS, E_STOP, M_CONFKEY_ERROR_CAT);
			while(1);
			break;
		case FF_SYSTEM:
			last_key = LAST_SYS_CONFIG;
			while (strcmp_hal(key_str, sys_config_keys[key_idx].text) != 0) {
				key_idx++;
				if (key_idx == last_key) break;
			}
			break;
		case FF_INTERFACE:
			last_key = LAST_IF_CONFIG;
			while (strcmp_hal(key_str, if_config_keys[key_idx].text) != 0) {
				key_idx++;
				if (key_idx == last_key) break;
			}
			break;
		case FF_INPUT:
			last_key = LAST_IN_CONFIG;
			while (strcmp_hal(key_str, in_config_keys[key_idx].text) != 0) {
				key_idx++;
				if (key_idx == last_key) break;
			}
			break;
		case FF_MONITOR:
			last_key = LAST_MON_CONFIG;
			while (strcmp_hal(key_str, mon_config_keys[key_idx].text) != 0) {
				key_idx++;
				if (key_idx == last_key) break;
			}
			break;
		case FF_SCHEDULE:
			last_key = LAST_SCH_CONFIG;
			while (strcmp_hal(key_str, sch_config_keys[key_idx].text) != 0) {
				key_idx++;
				if (key_idx == last_key) break;
			}
			break;
		case FF_RULE:
			last_key = LAST_RL_CONFIG;
			while (strcmp_hal(key_str, rl_config_keys[key_idx].text) != 0) {
				key_idx++;
				if (key_idx == last_key) break;
			}
			break;
		case FF_CONTROLLER:
			last_key = LAST_CON_CONFIG;
			while (strcmp_hal(key_str, con_config_keys[key_idx].text) != 0) {
				key_idx++;
				if (key_idx == last_key) break;
			}
			break;
		case FF_OUTPUT:
			last_key = LAST_OUT_CONFIG;
			while (strcmp_hal(key_str, out_config_keys[key_idx].text) != 0) {
				key_idx++;
				if (key_idx == last_key) break;
			}
			break;
		default:
			DebugLog(SSS, E_STOP, M_CONFKEY_LAST_BCAT);
			#ifdef PLATFORM_ARDUINO
				while(1);
			#endif //PLATFORM_ARDUINO
			#ifdef PLATFORM_LINUX
				exit(-1);
			#endif
	}

	if (key_idx == last_key) {
		EventMsg(SSS, E_ERROR, M_CONFKEY_NOTIN_DEFS);
		/// TODO this should probably become a DebugMessage
		HALItchWriteLnImmediate(key_str);
		return 0; //error
	} else {
		return key_idx;
	}
}

uint8_t ConfigureCommonSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str){
	switch (key_idx) {
		case SYS_CONFIG_ERROR:
			// or any other (0) block cat error key
			EventMsg(SSS, E_ERROR, M_KEY_IDX_ZERO);
			return 0;
			break;
		case SYS_CONFIG_TYPE:
			// or case IN_TYPE
			// or case MON_TYPE:
			// or case SCH_TYPE:
			// or case RL_TYPE:
			// or case CON_TYPE:
			// or case OUT_TYPE:
			block_ptr->type = BlockTypeStringArrayIndex(value_str);
			if (block_ptr->type == BT_ERROR) return 0;
			break;
		case SYS_CONFIG_DISABLE:
			// or case IN_DISABLE
			// or case MON_DISABLE:
			// or case SCH_DISABLE:
			// or case RL_DISABLE:
			// or case CON_DISABLE:
			// or case OUT_DISABLE:
			if (strcasecmp_hal(value_str, F("True")) == 0) {
				block_ptr->status = STATUS_DISABLED_ADMIN;
			} else {
				if (strcasecmp_hal(value_str, F("False")) == 0) {
					block_ptr->status = STATUS_DISABLED_INIT;
				} else {
					return 0; //error
				}
			}
			break;

		case SYS_CONFIG_DISPLAY_NAME:
			// or case IN_DISPLAY_NAME
			// or case MON_DISPLAY_NAME:
			// or case SCH_DISPLAY_NAME:
			// or case RL_DISPLAY_NAME:
			// or case CON_DISPLAY_NAME:
			// or case OUT_DISPLAY_NAME:
#ifndef	EXCLUDE_DISPLAYNAME
			UpdateDisplayName(block_ptr, value_str);
#endif
			break;
		case SYS_CONFIG_DESCRIPTION:
			// or case IN_DESCRIPTION
			// or case MON_DESCRIPTION:
			// or case SCH_DESCRIPTION:
			// or case RL_DESCRIPTION:
			// or case CON_DESCRIPTION:
			// or case OUT_DESCRIPTION:
#ifndef EXCLUDE_DESCRIPTION
			UpdateDescription(block_ptr, value_str);
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
		case SYS_CONFIG_LANGUAGE:
			block_ptr->settings.sys.language = LanguageStringArrayIndex(value_str);
			 // Although English = 0 which is default, returning 0 will produce error
			return 1;
			break;
		case SYS_CONFIG_TEMP_SCALE:
			block_ptr->settings.sys.temp_scale = UnitStringArrayIndex(value_str);
			return block_ptr->settings.sys.temp_scale;
			break;
		case SYS_CONFIG_WEEK_START:
			block_ptr->settings.sys.week_start = DayStringArrayIndex(value_str);
			// Day strings don't have an ERROR_DAY
			// TODO fix
			return 1;
			break;
		case SYS_CONFIG_START_DELAY:
			block_ptr->settings.sys.start_delay = atoi(value_str);
			return 1;
			break;
		default:
			DebugLog(SSS, E_ERROR, M_SYS_BAD_DEF);
			return 0;
			break;
	} // switch(key_idx)
	return 0;
}

uint8_t ConfigureINSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case IN_CONFIG_INTERFACE:

			// XXX Transition until all INs use interfaces
			if (block_ptr->type == IN_ONEWIRE) {
				block_ptr->settings.in.interface = GetBlockIDByLabel(value_str, true);
				return block_ptr->settings.in.interface;
			} else {
				block_ptr->settings.in.interface = InterfaceTypeStringArrayIndex(value_str);
				return block_ptr->settings.in.interface;
			}
			break;

		case IN_CONFIG_IF_NUM:
			block_ptr->settings.in.if_num = atoi(value_str);
			return block_ptr->settings.in.if_num;
			break;

		case IN_CONFIG_LOG_RATE:
			block_ptr->settings.in.log_rate = StringToTimeValue(value_str);
			return block_ptr->settings.in.log_rate;
			break;
		case IN_CONFIG_POLL_RATE:
			block_ptr->settings.in.poll_rate = StringToTimeValue(value_str);
			return block_ptr->settings.in.poll_rate;
			break;
		case IN_CONFIG_DATA_UNITS: {
			block_ptr->settings.in.data_units = UnitStringArrayIndex(value_str);
			if(block_ptr->settings.in.data_units == 0) {
				DebugLog(SSS, E_ERROR, M_BAD_DATA_UNITS);
			}
			return block_ptr->settings.in.data_units;
			break;
		}
		case IN_CONFIG_DATA_TYPE:
			//XXX so what - either float or int presently - inferred from block type, consider dropping
			break;
		default:
			DebugLog(SSS, E_ERROR, M_IN_BAD_DEF);
			return 0;
			break;
	} // switch(key_idx)
	return 0;
}

uint8_t ConfigureMONSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case MON_CONFIG_INPUT1:
			block_ptr->settings.mon.input1 = GetBlockIDByLabel(value_str, true);
			return block_ptr->settings.mon.input1;
			break;
		case MON_CONFIG_INPUT2:
			block_ptr->settings.mon.input2 = GetBlockIDByLabel(value_str, true);
			return block_ptr->settings.mon.input2;
			break;
		case MON_CONFIG_INPUT3:
			block_ptr->settings.mon.input3 = GetBlockIDByLabel(value_str, true);
			return block_ptr->settings.mon.input3;
			break;
		case MON_CONFIG_INPUT4:
			block_ptr->settings.mon.input4 = GetBlockIDByLabel(value_str, true);
			return block_ptr->settings.mon.input4;
			break;
		case MON_CONFIG_ACT_VAL:
			// check first for IN_DIGITAL boolean values
			if (strcmp_hal(value_str, F("HIGH")) == 0) {
				block_ptr->settings.mon.act_val = 1;
			} else {
				if (strcmp_hal(value_str, F("LOW")) == 0) {
					block_ptr->settings.mon.act_val = 0;
				} else {
					// must be a numeric float value
					//sscanf(value_str, "%f", &(block_ptr->settings.mon.act_val));
					block_ptr->settings.mon.act_val = atof(value_str);
				}
			}
			return 1;
			break;
		case MON_CONFIG_DEACT_VAL:
			// check first for IN_DIGITAL boolean values
			if (strcmp(value_str, "HIGH") == 0) {
				block_ptr->settings.mon.deact_val = 1;
			} else {
				if (strcmp(value_str, "LOW") == 0) {
					block_ptr->settings.mon.deact_val = 0;
				} else {
					// must be a numeric float value
					//sscanf(value_str, "%f", &(block_ptr->settings.mon.deact_val));
					block_ptr->settings.mon.deact_val = atof(value_str);
				}
			}
			return 1;
			break;
		default:
			DebugLog(SSS, E_ERROR, M_MON_BAD_DEF);
			return 0;
			break;
	} // switch(key_idx)
	return 0;
}

uint8_t ConfigureSCHSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case SCH_CONFIG_DAYS:
			if (DayStrToFlag(block_ptr->settings.sch.days, value_str) != 1) {
				DebugLog(SSS, E_WARNING, M_DAY_FLAG_EMPTY);
				return 0; //error
			}
			return 1;
			break;
		case SCH_CONFIG_TIME_START: {
			block_ptr->settings.sch.time_start = StringToTimeValue(value_str);
			if (block_ptr->settings.sch.time_start != UINT32_INIT) {
				return 1;
			} else return 0; // error
			break;
		}
		case SCH_CONFIG_TIME_END: {
			block_ptr->settings.sch.time_end = StringToTimeValue(value_str);
			if (block_ptr->settings.sch.time_end != UINT32_INIT) {
				return 1;
			} else return 0; // error
			break;
		}
		case SCH_CONFIG_TIME_DURATION: {
			block_ptr->settings.sch.time_duration = StringToTimeValue(value_str);
			if (block_ptr->settings.sch.time_duration != UINT32_INIT) {
				return 1;
			} else return 0; // error
			break;
		}
		case SCH_CONFIG_TIME_REPEAT: {
			block_ptr->settings.sch.time_repeat = StringToTimeValue(value_str);
			if (block_ptr->settings.sch.time_repeat != UINT32_INIT) {
				return 1;
			} else return 0; // error
			break;
		}
		default:
			DebugLog(SSS, E_ERROR, M_SCH_BAD_DEF);
			return 0;
			break;
	} // switch(key_idx)
	return 0;
}

uint8_t ConfigureRLSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case RL_CONFIG_PARAM1:
			block_ptr->settings.rl.param1 = GetBlockIDByLabel(value_str, true);
			return block_ptr->settings.rl.param1;
			break;
		case RL_CONFIG_PARAM2:
			block_ptr->settings.rl.param2 = GetBlockIDByLabel(value_str, true);
			return block_ptr->settings.rl.param2;
			break;
		case RL_CONFIG_PARAM3:
			block_ptr->settings.rl.param3 = GetBlockIDByLabel(value_str, true);
			return block_ptr->settings.rl.param3;
			break;
		case RL_CONFIG_PARAM_NOT:
			block_ptr->settings.rl.param_not = GetBlockIDByLabel(value_str, true);
			return block_ptr->settings.rl.param_not;
			break;
		default:
			DebugLog(SSS, E_ERROR, M_RL_BAD_DEF);
			return 0;
			break;
	} // switch(key_idx)
	return 0;
}

uint8_t ConfigureCONSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case CON_CONFIG_RULE:
			block_ptr->settings.con.rule = GetBlockIDByLabel(value_str, true);
			return block_ptr->settings.con.rule;
			break;
		case CON_CONFIG_OUTPUT:
			block_ptr->settings.con.output = GetBlockIDByLabel(value_str, true);
			return block_ptr->settings.con.output;
			break;
		case CON_CONFIG_ACT_CMD: {
			block_ptr->settings.con.act_cmd = CommandStringArrayIndex(value_str);
			if(block_ptr->settings.con.act_cmd == 0) {
				DebugLog(SSS, E_ERROR, M_ACT_CMD_UNKNOWN);
				return 0;
			}
			return block_ptr->settings.con.act_cmd;
			break;
		}
		case CON_CONFIG_DEACT_CMD: {
			block_ptr->settings.con.deact_cmd = CommandStringArrayIndex(value_str);
			if(block_ptr->settings.con.deact_cmd == 0) {
				DebugLog(SSS, E_ERROR, M_DEACT_CMD_UNKNOWN);
				return 0;
			}
			return block_ptr->settings.con.deact_cmd;
			break;
		}
		default:
			DebugLog(SSS, E_ERROR, M_CON_BAD_DEF);
			return 0;
			break;
	} // switch(key_idx)
	return 0;
}

uint8_t ConfigureOUTSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case OUT_CONFIG_INTERFACE:
			block_ptr->settings.out.interface = InterfaceTypeStringArrayIndex(value_str);
			return block_ptr->settings.out.interface;
			break;
		case OUT_CONFIG_IF_NUM:
			block_ptr->settings.out.if_num = atoi(value_str);
			return block_ptr->settings.out.if_num;
			break;
		default:
			DebugLog(SSS, E_ERROR, M_OUT_BAD_DEF);
			return 0;
			break;
	} // switch(key_idx)
	return 0;
}

uint8_t ConfigureIFS_WIFI(InterfaceWIFI* ifs, uint8_t key_idx, const char* value_str) {
	switch(key_idx) {
		case IF_CONFIG_STATIC_ADDR:
			//"static_address"
			if (strcasecmp_hal(value_str, F("True")) == 0) {
				ifs->static_address = 1;
				return 1;
			} else {
				if (strcasecmp_hal(value_str, F("False")) == 0) {
					ifs->static_address = 0;
					return 1;
				} else {
					EventMsg(SSS, E_ERROR, M_CONFIFS_WIFI_STATIC_NOT_BOOL);
					return 0; //error
				}
			}
			break;
		case IF_CONFIG_IP_STATIC:
			//"ip_static"
			break;

		case IF_CONFIG_IP_NETMASK:
			//"ip_netmask"
			break;

		case IF_CONFIG_IP_GATEWAY:
			//"ip_gateway"
			break;

		case IF_CONFIG_WIFI_SSID:
			//"wifi_ssid"
			break;

		case IF_CONFIG_WPS_PSK:
			//"wps_psk"
			break;
		default:
			EventMsg(SSS, E_ERROR, M_CONFIFS_KEY_INVALID_FOR_TYPE);
			return 0;
			break;
	}
	return 0; //error
}

uint8_t ConfigureIFS_ETHERNET(InterfaceETHERNET* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_ETHERNET"
	(void) ifs;
	(void) key_idx;
	(void) value_str;
	return 0; //error
}

uint8_t ConfigureIFS_PWM_IN(InterfacePWM_IN* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_PWM_IN"
	(void) ifs;
	(void) key_idx;
	(void) value_str;
	return 0; //error
}

uint8_t ConfigureIFS_PWM_OUT(InterfacePWM_OUT* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_PWM_OUT"
	(void) ifs;
	(void) key_idx;
	(void) value_str;
	return 0; //error
}

uint8_t ConfigureIFS_PPM_IN(InterfacePPM_IN* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_PPM_IN"
	(void) ifs;
	(void) key_idx;
	(void) value_str;
	return 0; //error
}

uint8_t ConfigureIFS_PPM_OUT(InterfacePPM_OUT* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_PPM_OUT"
	(void) ifs;
	(void) key_idx;
	(void) value_str;
	return 0; //error
}

uint8_t ConfigureIFS_DS1820B(InterfaceDS1820B* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_ONEWIRE_DEVICE"
	switch(key_idx) {
		case IF_CONFIG_BUS_PIN:
			ifs->bus_pin = atoi(value_str);
			//"bus_pin"
			return 1;
			break;
		case IF_CONFIG_DALLAS_ADDR:
			//"dallas_address"
			if (DallasAddressStringToArray(value_str, ifs->dallas_address) == 0) {
				EventMsg(SSS, E_ERROR, M_INVALID_DALLAS_ADDR);
				return 0;
			}
			return 1;
			break;
		default:
			break;
	}
	return 0; //error
}

uint8_t ConfigureIFS_ONEWIRE_BUS(InterfaceONEWIRE_BUS* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_ONEWIRE_BUS"
	switch(key_idx) {
		case IF_CONFIG_BUS_PIN:
			//"bus_pin"
			ifs->bus_pin = atoi(value_str);
			return 1;
			break;
		case IF_CONFIG_DEVICE_COUNT:
			//"device_count"
			ifs->device_count = atoi(value_str);
			return 1;
			break;
		default:
			break;
	}
	return 0; //error
}

uint8_t ConfigureIFS_DIG_PIN_IN(InterfaceDIG_PIN_IN* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_DIG_PIN_IN"
	(void) ifs;
	(void) key_idx;
	(void) value_str;
	return 0; //error
}

uint8_t ConfigureIFS_DIG_PIN_OUT(InterfaceDIG_PIN_OUT* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_DIG_PIN_OUT"
	(void) ifs;
	(void) key_idx;
	(void) value_str;
	return 0; //error
}

uint8_t ConfigureIFS_VED_VOLTAGE(InterfaceVED_VOLTAGE* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_VED_VOLTAGE"
	(void) ifs;
	(void) key_idx;
	(void) value_str;
	return 0; //error
}

uint8_t ConfigureIFS_VED_CURRENT(InterfaceVED_CURRENT* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_VED_CURRENT"
	(void) ifs;
	(void) key_idx;
	(void) value_str;
	return 0; //error
}

uint8_t ConfigureIFS_VED_POWER(InterfaceVED_POWER* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_VED_POWER"
	(void) ifs;
	(void) key_idx;
	(void) value_str;
	return 0; //error
}

uint8_t ConfigureIFS_VED_SOC(InterfaceVED_SOC* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_VED_SOC"
	(void) ifs;
	(void) key_idx;
	(void) value_str;
	return 0; //error
}

uint8_t ConfigureIFS_HW_SERIAL(InterfaceHW_SERIAL* ifs, uint8_t key_idx, const char* value_str) {
	//"IF_HW_SERIAL"
	(void) ifs;
	(void) key_idx;
	(void) value_str;
	switch(key_idx) {
		case IF_CONFIG_PORT_NUM:
			//"port_num"
			break;
		case IF_CONFIG_SPEED:
			//"speed"
			break;
		default:
			break;
	}
	return 0; //error
}

uint8_t ConfigureIFSpecificSetting(InterfaceNode* if_ptr, uint8_t key_idx, const char* value_str) {
	// Set the interface config setting (key_idx) to the value_str converted
	//  to its appropriate run-time type and setting.
	// Assumes: if_ptr points to a valid interface in the ill.
	// Assumes: if_ptr->type is set to a valid interface type
	// Assumes: key_idx is a valid interface _specific_ config key (enumerated)

	switch (if_ptr->type) {
		case IF_WIFI: {
			//"IF_WIFI"
			InterfaceWIFI* ifs; 	// IF Settings
			ifs = (InterfaceWIFI *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_WIFI(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_ETHERNET: {
			//"IF_ETHERNET"
			InterfaceETHERNET* ifs; 	// IF Settings
			ifs = (InterfaceETHERNET *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_ETHERNET(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_PWM_IN: {
			//"IF_PWM_IN"
			InterfacePWM_IN* ifs; 	// IF Settings
			ifs = (InterfacePWM_IN *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_PWM_IN(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_PWM_OUT: {
			//"IF_PWM_OUT"
			InterfacePWM_OUT* ifs; 	// IF Settings
			ifs = (InterfacePWM_OUT *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_PWM_OUT(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_PPM_IN: {
			//"IF_PPM_IN"
			InterfacePPM_IN* ifs; 	// IF Settings
			ifs = (InterfacePPM_IN *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_PPM_IN(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_PPM_OUT: {
			//"IF_PPM_OUT"
			InterfacePPM_OUT* ifs; 	// IF Settings
			ifs = (InterfacePPM_OUT *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_PPM_OUT(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_DS1820B: {
			//"IF_ONEWIRE_DEVICE"
			InterfaceDS1820B* ifs; 	// IF Settings
			ifs = (InterfaceDS1820B *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_DS1820B(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_ONEWIRE_BUS: {
			//"IF_ONEWIRE_BUS"
			InterfaceONEWIRE_BUS* ifs; 	// IF Settings
			ifs = (InterfaceONEWIRE_BUS *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_ONEWIRE_BUS(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_DIG_PIN_IN: {
			//"IF_DIG_PIN_IN"
			InterfaceDIG_PIN_IN* ifs; 	// IF Settings
			ifs = (InterfaceDIG_PIN_IN *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_DIG_PIN_IN(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_DIG_PIN_OUT: {
			//"IF_DIG_PIN_OUT"
			InterfaceDIG_PIN_OUT* ifs; 	// IF Settings
			ifs = (InterfaceDIG_PIN_OUT *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_DIG_PIN_OUT(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_VED_VOLTAGE: {
			//"IF_VED_VOLTAGE"
			InterfaceVED_VOLTAGE* ifs; 	// IF Settings
			ifs = (InterfaceVED_VOLTAGE *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_VED_VOLTAGE(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_VED_CURRENT: {
			//"IF_VED_CURRENT"
			InterfaceVED_CURRENT* ifs; 	// IF Settings
			ifs = (InterfaceVED_CURRENT *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_VED_CURRENT(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_VED_POWER: {
			//"IF_VED_POWER"
			InterfaceVED_POWER* ifs; 	// IF Settings
			ifs = (InterfaceVED_POWER *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_VED_POWER(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_VED_SOC: {
			//"IF_VED_SOC"
			InterfaceVED_SOC* ifs; 	// IF Settings
			ifs = (InterfaceVED_SOC *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_VED_SOC(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		case IF_HW_SERIAL: {
			//"IF_VED_SOC"
			InterfaceHW_SERIAL* ifs; 	// IF Settings
			ifs = (InterfaceHW_SERIAL *)if_ptr->IFSettings;
			if (ifs) {
				return ConfigureIFS_HW_SERIAL(ifs, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONFIFS_NOT_ALLOCATED);
			}
			break;
		}
		default:
			return 0; // catch-all error
	}
	return 0; // catch-all error
}

uint8_t ConfigureIFSetting(InterfaceNode* if_ptr, uint8_t key_idx, const char* value_str) {
	// Set the interface config setting (key_idx) to the value_str converted
	//  to its appropriate run-time type and setting.
	// Assumes: if_ptr points to a valid interface in the ill.
	// Assumes: key_idx is a valid interface config key (enumerated)
	// Prerequisite:
	//	For Settings that are interface-type specific if_ptr->type must be set and
	//	be a valid type. Report and return error if not

	switch(key_idx) {
		case IF_CONFIG_TYPE:
			//"type"
			if_ptr->type = InterfaceTypeStringArrayIndex(value_str);
			if(if_ptr->type == IF_ERROR) {
				EventMsg(SSS, E_ERROR, M_CONF_IF_UNKOWN_IF_TYPE);
				return 0; //error
			} else {
				return 1;
			}
			break;
		case IF_CONFIG_DISABLE:
			//"disable"
			if (strcasecmp_hal(value_str, F("True")) == 0) {
				if_ptr->status = STATUS_DISABLED_ADMIN;
				return 1;
			} else {
				if (strcasecmp_hal(value_str, F("False")) == 0) {
					if_ptr->status = STATUS_DISABLED_INIT;
					return 1;
				} else {
					EventMsg(SSS, E_ERROR, M_CONF_IF_DISABLED_NOT_BOOLEAN);
					return 0; //error
				}
			}
			break;
		case IF_CONFIG_DISPLAY_NAME:
			//"display_name"
			#ifndef	EXCLUDE_DISPLAYNAME
			UpdateDisplayName(block_ptr, value_str);
			return 1;
			#endif
			break;
		case IF_CONFIG_DESCRIPTION:
			//"description"
			#ifndef EXCLUDE_DESCRIPTION
			UpdateDescription(block_ptr, value_str);
			return 1;
			#endif
			break;
		default:
			// Must be interface-type specific setting (or error)
			if((if_ptr->type > IF_ERROR) && (if_ptr->type < LAST_INTERFACE)) {
				// a valid type has been set
				return ConfigureIFSpecificSetting(if_ptr, key_idx, value_str);
			} else {
				EventMsg(SSS, E_ERROR, M_CONF_IF_TYPE_NOT_SET);
				return 0;
			}
			break;
	}
	return 0; // catch-all error
}

uint8_t ConfigureInterface(const char *label, const char *key_str, const char *value_str) {
	// Check for existence of an interface identified by label
	// If it exists update the setting identified by key_str and value_str
	// If it does not exist, create it first and then update key / value
	// If called with key_str == NULL then just just create it and label it,
	//	or report true if already exists.

	InterfaceNode *if_ptr;
	uint8_t key_idx = 0; 		//see "string_consts.h" Zero is error.

	// First search for an existing interface with that label
	if_ptr = GetInterfaceByLabel(label);

	//not found - add a new block
	if (if_ptr == NULL) {
		key_idx = GetConfKeyIndex(FF_INTERFACE, key_str);
		// interface type must be specified if adding a new block
		if (key_idx == IF_CONFIG_TYPE) {
			// value_str must be a valid interface type string
			key_idx = InterfaceTypeStringArrayIndex(value_str);
			if (key_idx != IF_ERROR) {
				if_ptr = AddInterface(key_idx, label); //add a new one and label it
				// nothing more to do
				return 1; //non-zero - good
			} else {
				EventMsg(SSS, E_ERROR, M_CONF_IF_UNKOWN_IF_TYPE);
				return 0; //error
			}
		} else {
			EventMsg(SSS, E_ERROR, M_IF_TYPE_FIRST_CONFIG);
			return 0;
		}
	}
	if (if_ptr != NULL) {
		// if_ptr is pointing to a labelled block
		// that has valid if_type and is initialised

		// Get the index (related enum) of the key_str)
		key_idx = GetConfKeyIndex(FF_INTERFACE, key_str);

		if(key_idx == 0) {
			// not found in key list
			return 0; //error
		}

		if(key_idx < LAST_IF_CONFIG) {
			// Valid key str
			return ConfigureIFSetting(if_ptr, key_idx, value_str);
		} else {
			// redundant check if > LAST_IF_CONFIG then GetConfKeyIndex() is broken
			return 0;
		}
	} else {
		EventMsg(SSS, E_ERROR, M_ADDING_BLOCK);
		return 0;
	}
	return 0; // execution should never get here
}

uint8_t ConfigureBlock(uint8_t block_cat, const char *block_label, const char *key_str, const char *value_str) {
	// Check for existence of a block identified by block_label
	// If it exists update the setting identified by key_str and value_str
	// If it does not exist, create it first and then update key / value
	// If called with key_str == NULL then just just create it and label it,
	//	or report true if already exists.

	BlockNode *block_ptr;
	uint8_t return_value = 1;  	//error by exception
	uint8_t key_idx = 0; 		//see "string_consts.h" Zero is error.

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

		if (key_idx <= SYS_CONFIG_DESCRIPTION) {
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

#ifdef PLATFORM_LINUX

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
	return fwrite(data, s, (size_t)1, f);
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

#endif


#ifdef PLATFORM_LINUX
void HALWriteLine(FILE *f, const char* out_str, uint8_t show_only) {
	if (show_only) {
		HALItchWriteLnImmediate(out_str);
	} else {
		fprintf(f, "%s\n", out_str);
	}
}
#endif

#ifdef PLATFORM_ARDUINO
void HALWriteLine(SdFile *f, const char* out_str, uint8_t show_only) {
	//DebugLog(out_str);
	if (show_only) {
		HALItchWriteLnImmediate(out_str);
	} else {
		f->println(out_str);
	}
}
#endif //PLATFORM_ARDUINO

#ifdef PLATFORM_LINUX
void WriteBlock(BlockNode *b, FILE *f, uint8_t reg_only, uint8_t show_only) {
#endif
#ifdef PLATFORM_ARDUINO
void WriteBlock(BlockNode *b, SdFile *f, uint8_t reg_only, uint8_t show_only) {
#endif //PLATFORM_ARDUINO
	// Called by FileIODispatcher on each block to write their
	//  config to an itch parsable text file. To read and parse
	//	the file, all blocks that are referenced by other blocks
	//	need to be registered first, before settings are applied
	//	that may otherwise refer to non-existent blocks.

	char base_str[MAX_LABEL_LENGTH + 19];					// "CONFIG CONTROLLER "(19)
	char out_str[MAX_LABEL_LENGTH + 19 + MAX_DESCR_LENGTH];
	char hms_str[9];
	char float_str[6];
	char temp_str[MAX_LABEL_LENGTH + 19 + MAX_DESCR_LENGTH];

	// set up base string for all "CONFIG <block_cat> <block_label> "
	strcpy_hal(base_str, F("CONFIG "));
	strcat_hal(base_str, block_cat_strings[b->cat].text);
	strcat_hal(base_str, F(" "));
	strcat(base_str, b->label);
	strcat_hal(base_str, F(" "));

	// Key value pair "type" common to all blocks - build them and write them
	strcpy(out_str, base_str);
	strcat_hal(out_str, F("type "));
	strcat_hal(out_str, block_type_strings[b->type].text);
	HALWriteLine(f, out_str, show_only);

	// register the block labels and types only
	if (reg_only == 1 ) return;

	// Write the block status only if it is admin disabled
	// so that it persists between reboots
	if(b->status == STATUS_DISABLED_ADMIN) {
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("disable "));
		strcat_hal(out_str, F("True"));
		HALWriteLine(f, out_str, show_only);
	}

	#ifndef	EXCLUDE_DISPLAYNAME
		if (b->display_name[0] != '\0') {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("display_name \""));
			strcat(out_str, b->display_name);
			strcat(out_str, "\"");
			HALWriteLine(f, out_str, show_only);
		}
	#endif
	#ifndef EXCLUDE_DESCRIPTION
		if (b->description[0] != '\0') {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("description \""));
			strcat(out_str, b->description);
			strcat(out_str, "\"");
			HALWriteLine(f, out_str, show_only);
		}
	#endif

	switch (b->cat) {
	case FF_SYSTEM:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("temp_scale "));
		strcat_hal(out_str, unit_strings[b->settings.sys.temp_scale].text);
		HALWriteLine(f, out_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("language "));
		strcat_hal(out_str, language_strings[b->settings.sys.language].text);
		HALWriteLine(f, out_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("week_start "));
		strcat_hal(out_str, day_strings[b->settings.sys.week_start].text);
		HALWriteLine(f, out_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("start_delay "));
		sprintf(temp_str, "%s%d", out_str, b->settings.sys.start_delay);
		HALWriteLine(f, temp_str, show_only);

		break;
	case FF_INPUT:

		// XXX Transition until all INs use interfaces
		if(b->type == IN_ONEWIRE) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("interface "));
			strcat(out_str, GetBlockLabelString(b->settings.in.interface));
			HALWriteLine(f, out_str, show_only);
		} else {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("interface "));
			strcat_hal(out_str, interface_type_strings[b->settings.in.interface].text);
			HALWriteLine(f, out_str, show_only);
		}

		// XXX Transition until all INs use interfaces
		if (b->type != IN_ONEWIRE) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("if_num "));
			sprintf(temp_str, "%s%d", out_str, b->settings.in.if_num);
			HALWriteLine(f, temp_str, show_only);
		}

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("log_rate "));
		strcat(out_str, TimeValueToTimeString(hms_str, b->settings.in.log_rate));
		HALWriteLine(f, out_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("poll_rate "));
		strcat(out_str, TimeValueToTimeString(hms_str, b->settings.in.poll_rate));
		HALWriteLine(f, out_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("data_units "));
		strcat_hal(out_str, unit_strings[b->settings.in.data_units].text);
		HALWriteLine(f, out_str, show_only);

		// data type not properly implemented or used
		//uint8_tWrite(b->settings.in.data_type, f);
		//data_type = float

		break;
	case FF_MONITOR:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("input1 "));
		strcat(out_str, GetBlockLabelString(b->settings.mon.input1));
		HALWriteLine(f, out_str, show_only);

		if (b->settings.mon.input2 != UINT16_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("input2 "));
			strcat(out_str, GetBlockLabelString(b->settings.mon.input2));
			HALWriteLine(f, out_str, show_only);
		}

		if (b->settings.mon.input3 != UINT16_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("input3 "));
			strcat(out_str, GetBlockLabelString(b->settings.mon.input3));
			HALWriteLine(f, out_str, show_only);
		}

		if (b->settings.mon.input2 != UINT16_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("input4 "));
			strcat(out_str, GetBlockLabelString(b->settings.mon.input4));
			HALWriteLine(f, out_str, show_only);
		}

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("act_val "));
		strcat(out_str, FFFloatToCString(float_str, b->settings.mon.act_val));
		HALWriteLine(f, out_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("deact_val "));
		strcat(out_str, FFFloatToCString(float_str, b->settings.mon.deact_val));
		HALWriteLine(f, out_str, show_only);
		break;

	case FF_SCHEDULE:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("days "));
		strcat(out_str, FlagToDayStr(temp_str, b->settings.sch.days));
		HALWriteLine(f, out_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("time_start "));
		strcat(out_str, TimeValueToTimeString(hms_str, b->settings.sch.time_start));
		HALWriteLine(f, out_str, show_only);

		if (b->settings.sch.time_end != UINT32_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("time_end "));
			strcat(out_str, TimeValueToTimeString(hms_str, b->settings.sch.time_end));
			HALWriteLine(f, out_str, show_only);
		}

		if (b->settings.sch.time_duration != UINT32_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("time_duration "));
			strcat(out_str, TimeValueToTimeString(hms_str, b->settings.sch.time_duration));
			HALWriteLine(f, out_str, show_only);
		}

		if (b->settings.sch.time_repeat != UINT32_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("time_repeat "));
			strcat(out_str, TimeValueToTimeString(hms_str, b->settings.sch.time_repeat));
			HALWriteLine(f, out_str, show_only);
		}
		break;

	case FF_RULE:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("param1 "));
		strcat(out_str, GetBlockLabelString(b->settings.rl.param1));
		HALWriteLine(f, out_str, show_only);

		if (b->settings.rl.param2 != UINT16_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("param2 "));
			strcat(out_str, GetBlockLabelString(b->settings.rl.param2));
			HALWriteLine(f, out_str, show_only);
		}

		if (b->settings.rl.param3 != UINT16_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("param3 "));
			strcat(out_str, GetBlockLabelString(b->settings.rl.param3));
			HALWriteLine(f, out_str, show_only);
		}

		if (b->settings.rl.param_not != UINT16_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("param_not "));
			strcat(out_str, GetBlockLabelString(b->settings.rl.param_not));
			HALWriteLine(f, out_str, show_only);
		}
		break;

	case FF_CONTROLLER:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("rule "));
		strcat(out_str, GetBlockLabelString(b->settings.con.rule));
		HALWriteLine(f, out_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("output "));
		strcat(out_str, GetBlockLabelString(b->settings.con.output));
		HALWriteLine(f, out_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("act_cmd "));
		strcat_hal(out_str, command_strings[b->settings.con.act_cmd].text);
		HALWriteLine(f, out_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("deact_cmd "));
		strcat_hal(out_str, command_strings[b->settings.con.deact_cmd].text);
		HALWriteLine(f, out_str, show_only);

		break;
	case FF_OUTPUT:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("interface "));
		strcat_hal(out_str, interface_type_strings[b->settings.out.interface].text);
		HALWriteLine(f, out_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("if_num "));
		sprintf(temp_str, "%s%d", out_str, b->settings.out.if_num);
		HALWriteLine(f, temp_str, show_only);

		break;
	default:
		break;
	}
}

#ifdef PLATFORM_LINUX
void WriteInterface(InterfaceNode *b, FILE *f, uint8_t reg_only, uint8_t show_only) {
#endif
#ifdef PLATFORM_ARDUINO
void WriteInterface(InterfaceNode *b, SdFile *f, uint8_t reg_only, uint8_t show_only) {
#endif //PLATFORM_ARDUINO
	// Called by FileIODispatcher on each block to write their
	//  config to an itch parsable text file. To read and parse
	//	the file, all blocks that are referenced by other blocks
	//	need to be registered first, before settings are applied
	//	that may otherwise refer to non-existent blocks.

	char base_str[MAX_LABEL_LENGTH + 19];					// "CONFIG CONTROLLER "(19)
	char out_str[MAX_LABEL_LENGTH + 19 + MAX_DESCR_LENGTH];
	//char hms_str[9];
	//char float_str[6];
	char temp_str[MAX_LABEL_LENGTH + 19 + MAX_DESCR_LENGTH];

	// set up base string for all "CONFIG <block_cat> <block_label> "
	strcpy_hal(base_str, F("CONFIG "));
	strcat_hal(base_str, block_cat_strings[FF_INTERFACE].text);
	strcat_hal(base_str, F(" "));
	strcat(base_str, b->label);
	strcat_hal(base_str, F(" "));

	// Key value pair "type" common to all blocks - build them and write them
	strcpy(out_str, base_str);
	strcat_hal(out_str, F("type "));
	strcat_hal(out_str, interface_type_strings[b->type].text);
	HALWriteLine(f, out_str, show_only);

	// register the block labels and types only
	if (reg_only == 1 ) return;

	// Write the block status only if it is admin disabled
	// so that it persists between reboots
	if(b->status == STATUS_DISABLED_ADMIN) {
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("disable "));
		strcat_hal(out_str, F("True"));
		HALWriteLine(f, out_str, show_only);
	}

	#ifndef	EXCLUDE_DISPLAYNAME
		if (b->display_name[0] != '\0') {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("display_name \""));
			strcat(out_str, b->display_name);
			strcat(out_str, "\"");
			HALWriteLine(f, out_str, show_only);
		}
	#endif
	#ifndef EXCLUDE_DESCRIPTION
		if (b->description[0] != '\0') {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("description \""));
			strcat(out_str, b->description);
			strcat(out_str, "\"");
			HALWriteLine(f, out_str, show_only);
		}
	#endif

	switch (b->type) {
	case IF_ONEWIRE_BUS: {
		InterfaceONEWIRE_BUS *ifs;
		ifs = (InterfaceONEWIRE_BUS *)b->IFSettings;

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("bus_pin "));
		sprintf(temp_str, "%s%d", out_str, ifs->bus_pin);
		HALWriteLine(f, temp_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("device_count "));
		sprintf(temp_str, "%s%d", out_str, ifs->device_count);
		HALWriteLine(f, temp_str, show_only);
	}
		break;
	case IF_DS1820B: {
		InterfaceDS1820B *ifs;
		ifs = (InterfaceDS1820B *)b->IFSettings;

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("bus_pin "));
		sprintf(temp_str, "%s%d", out_str, ifs->bus_pin);
		HALWriteLine(f, temp_str, show_only);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("dallas_address "));
		DallasAddressArrayToString(temp_str, ifs->dallas_address);
		strcat(out_str, temp_str);
		HALWriteLine(f, out_str, show_only);
	}
		break;

	default:
		ITCHWrite(F("(WriteInterface) Interface Type "));
		ITCHWrite(interface_type_strings[b->type].text);
		ITCHWriteLine(F(" Not Yet Implemented"));
		break;
	}
}


#ifdef PLATFORM_LINUX
void FileIOBlockDispatcher(void(*func)(BlockNode*, FILE*, uint8_t, uint8_t), FILE* file, uint8_t option, uint8_t show) {
#endif
#ifdef PLATFORM_ARDUINO
void FileIOBlockDispatcher(void(*func)(BlockNode*, SdFile*, uint8_t, uint8_t), SdFile* file, uint8_t option, uint8_t show) {
#endif //PLATFORM_ARDUINO
	BlockNode* block_ptr;

	block_ptr = GetBlockListHead();

	while (block_ptr != NULL) {
		func(block_ptr, file, option, show);
		block_ptr = block_ptr->next;
	}
}

#ifdef PLATFORM_LINUX
void FileIOInterfaceDispatcher(void(*func)(InterfaceNode*, FILE*, uint8_t, uint8_t), FILE* file, uint8_t option, uint8_t show) {
#endif
#ifdef PLATFORM_ARDUINO
void FileIOInterfaceDispatcher(void(*func)(InterfaceNode*, SdFile*, uint8_t, uint8_t), SdFile* file, uint8_t option, uint8_t show) {
#endif //PLATFORM_ARDUINO
	InterfaceNode* ptr;

	ptr = GetInterfaceListHead();

	while (ptr != NULL) {
		func(ptr, file, option, show);
		ptr = ptr->next;
	}
}

void InitConfigShow(void) {
	#ifdef PLATFORM_LINUX
	FILE *fp = NULL;
	// First write block registrations only (option = 1)
	FileIOInterfaceDispatcher(WriteInterface, fp, 1, 1);
	FileIOBlockDispatcher(WriteBlock, fp, 1, 1);
	// Then write the rest of the settings (option = 0)
	FileIOInterfaceDispatcher(WriteInterface, fp, 0, 1);
	FileIOBlockDispatcher(WriteBlock, fp, 0, 1);
	#endif

	#ifdef PLATFORM_ARDUINO
	SdFile f;
	// First write block registrations only (option = 1)
	FileIOInterfaceDispatcher(WriteInterface, &f, 1, 1);
	FileIOBlockDispatcher(WriteBlock, &f, 1, 1);
	// Then write the rest of the settings (option = 0)
	FileIOInterfaceDispatcher(WriteInterface, &f, 0, 1);
	FileIOBlockDispatcher(WriteBlock, &f, 0, 1);
	#endif //PLATFORM_ARDUINO
}

void InitConfigSave(void) {
	// Write the running config to a text file in the form of parse-able
	//  CONFIG statements that can be read by itch in buffer stuffing mode
	//  or natively by InitConfigLoad()

	#ifdef PLATFORM_LINUX
	FILE *fp;
	fp = fopen(CONFIG_TXT_FILENAME, "w");
	#endif

	#ifdef PLATFORM_ARDUINO
	// Pin numbers in templates must be constants.
	const uint8_t SOFT_MISO_PIN = 12;
	const uint8_t SOFT_MOSI_PIN = 11;
	const uint8_t SOFT_SCK_PIN  = 13;
	// Chip select may be constant or RAM variable.
	const uint8_t SD_CHIP_SELECT_PIN = 10;
	// SdFat software SPI template
	SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;
	SdFile f;
	#endif //PLATFORM_ARDUINO

	#ifdef PLATFORM_ARDUINO
	// see if the card is present and can be initialized:
	if (!sd.begin(SD_CHIP_SELECT_PIN)) {
		char temp_label[MAX_LABEL_LENGTH];
		strcpy_hal(temp_label, F("SD BEGIN FAILED"));
		DebugLog(temp_label);
		// Cannot do anything else
		while (1);
		//sd.initErrorHalt();
	}

	if (!f.open(CONFIG_TXT_FILENAME, O_WRITE | O_CREAT | O_TRUNC)) {
		char temp_label[MAX_LABEL_LENGTH];
		strcpy_hal(temp_label, F("F.OPEN CONFIG FAILED"));
		DebugLog(temp_label);
		while (1);
		//sd.errorHalt(F("open failed"));
	}
	#endif //PLATFORM_ARDUINO

	#ifdef PLATFORM_LINUX
		// First write block registrations only (option = 1)
		FileIOInterfaceDispatcher(WriteInterface, fp, 1, 0);
		FileIOBlockDispatcher(WriteBlock, fp, 1, 0);
		// Then write the rest of the settings (option = 0)
		FileIOInterfaceDispatcher(WriteInterface, fp, 0, 0);
		FileIOBlockDispatcher(WriteBlock, fp, 0, 0);
	#endif

	#ifdef PLATFORM_ARDUINO
		// First write block registrations only (option = 1)
		FileIOInterfaceDispatcher(WriteInterface, &f, 1, 0);
		FileIOBlockDispatcher(WriteBlock, &f, 1, 0);
		// Then write the rest of the settings (option = 0)
		FileIOInterfaceDispatcher(WriteInterface, &f, 0, 0);
		FileIOBlockDispatcher(WriteBlock, &f, 0, 0);
	#endif //PLATFORM_ARDUINO

	#ifdef PLATFORM_LINUX
	fclose(fp);
	#endif
	#ifdef PLATFORM_ARDUINO
	f.flush();
	f.close();
	#endif //PLATFORM_ARDUINO
}

#ifdef PLATFORM_LINUX
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
		#ifdef PLATFORM_ARDUINO
			while (1);
		#endif //PLATFORM_ARDUINO
		#ifdef PLATFORM_LINUX
			exit(-1);
		#endif
	}
}
#endif //PLATFORM_LINUX

void ConfigParse(char* buf) {
	// Put the tokens present in the line into word_list[]
	// Treat anything in double quotes ("...") as a single token
	// Assumes: there is at most one single string designated by double quotes
	// Assumes: that any quoted string is at the end of the line
	// Any number of tokens may precede the quoted string.

	char* wl[10];  			//XXX randomly selected max number of words
	for (uint8_t i = 0 ; i < 10; i++) {
		wl[i] = NULL;
	}
    uint8_t word_count = 0;

    // Check if the buffer contains a double quote ("...") delimited string
    char *pre_quotes;
    char *in_quotes;
    uint8_t quotes = 0;

    pre_quotes = strtok(buf, "\"");
    if (pre_quotes != NULL) {
    	// its not an empty string
    	in_quotes = strtok(NULL, "\"");
    	if(in_quotes != NULL) {
    		// quoted string now pointed to by in_quotes
    		// with either eol or the next \" being null
    		quotes = 1;
    		//strcpy(quoted_string_buf, in_quotes);
    	}
    }
    // buf still points to the start of string.
    // buf now with the first \" turned to null - ie end of string
    // or buf is unmodified if \" wasn't found.
    int i = 0;
    wl[i] = strtok(buf, TOKEN_DELIM);
    while (wl[i] != NULL) {
        i++;
        wl[i] = strtok(0, TOKEN_DELIM);
    }
    if(quotes) {
    	//strcpy(word_list[i], quoted_string_buf);
    	wl[i] = in_quotes;
    	i++;
    }
    word_count = i;

    if (word_count == 5) {
    	uint8_t cat = GetBlockCatIdxByBlockCatName(wl[1]);
    	if (cat == FF_INTERFACE) {
    		ConfigureInterface(wl[2], wl[3], wl[4]);
    	} else {
    		ConfigureBlock(cat, wl[2], wl[3], wl[4]);
    	}
    }

}

void InitConfigLoad(uint8_t how) {
	// Build the block list from a text config file previously written
	//  by InitConfigSave()
	// Read each line of the file and either:
	//	how = 0: pass it to ITCH::StuffAndProcess() which will
	//		use the restuff buffer to insert the line into the parsing stream and then
	//		poll itch until it is consumed.
	//	how = 1: Use native ConfigParse()
	// NOTE: no existence or duplicate checking - this function assumes an empty (NULL)
	//  block list therefore DropBlockList() is called first to make sure

	// XXX this whole routine and subs needs error handling

	#ifndef USE_ITCH
		(void)how;		// Not relevent if not using itch;
	#endif

	#ifdef PLATFORM_LINUX
	char* line_ptr = NULL;
	size_t len = 0;

	FILE *fp;
	fp = fopen(CONFIG_TXT_FILENAME, "r");

	if (!fp) {
		EventMsg(SSS, E_ERROR, NO_CONFIG_FILE);
		return;
	}
	if (how == 0) {
		HALItchSetBufferStuffMode();
	}

	while (1) {    //continue reading the file until EOF break
		getline(&line_ptr, &len, fp);
		//fgets(&line, fp);
		//fscanf(fp, "%s\n", line);

		if(feof(fp)) {
			break;
		}

		uint8_t slen;
		slen = (uint8_t)strlen(line_ptr);
		if (slen > 0) {
			if ( (line_ptr[slen-1] == '\n') || (line_ptr[slen-1] == '\r')) {
				line_ptr[slen-1] = '\0';
			}
		}
		if (slen > 1) {
			if ( (line_ptr[slen-2] == '\n') || (line_ptr[slen-2] == '\r')) {
				line_ptr[slen-2] = '\0';
			}
		}

		#ifdef USE_ITCH
			if (how == 0) {
				HALItchStuffAndProcess(line_ptr);
			} else {
				DebugLog(line_ptr);
				ConfigParse(line_ptr);
			}

		#endif
			free(line_ptr);
			line_ptr = NULL;
			len = 0;
	}
	if(line_ptr) free(line_ptr);
	line_ptr = NULL;
	len = 0;
	if (fp) fclose(fp);

	if (how == 0) {
		HALItchSetTextDataMode();
	}
	#endif // PLATFORM_LINUX

	#ifdef PLATFORM_ARDUINO
	// Pin numbers in templates must be constants.
	const uint8_t SOFT_MISO_PIN = 12;
	const uint8_t SOFT_MOSI_PIN = 11;
	const uint8_t SOFT_SCK_PIN  = 13;
	// Chip select may be constant or RAM variable.
	const uint8_t SD_CHIP_SELECT_PIN = 10;
	// SdFat software SPI template
	SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;

	//CONFIG schedule SCH_WATER_BOT type SCH_START_DURATION_REPEAT
	//size_t lin_len = MAX_LABEL_LENGTH + 19 + MAX_DESCR_LENGTH;
	//size_t lin_len = 120
	char *line_ptr;
	char ch;
	char line[120]; //XXX Trim? How to determine size?
	SdFile f;

	// see if the card is present and can be initialized:
	if (!sd.begin(SD_CHIP_SELECT_PIN)) {
		//char temp_label[MAX_LABEL_LENGTH];
		//strcpy_hal(temp_label, F("SD BEGIN FAILED"));
		EventMsg(SSS, E_ERROR, M_SD_BEGIN_FAIL);
		return;
	}

	// Try to open the default config file
	if (!f.open(CONFIG_TXT_FILENAME, O_READ)) {
		EventMsg(SSS, E_ERROR, NO_CONFIG_FILE);
		return;
	}

	#ifdef USE_ITCH
	if (how == 0) {
		HALItchSetBufferStuffMode();
	}
	#endif //USE_ITCH

	while (1) {    //continue reading the file until EOF break
		line_ptr = line;
		*line_ptr = '\0';

		// Get first char
		ch = f.read();
		// Discard initial blank lines and extra \n \r from previous line
		while ( (ch == '\r') || (ch == '\n')) {
			//ignore it and read again
			ch = f.read();
		}
		// only place normal chars in the buffer up to \r or \n
		while ((ch > 0) && (ch != '\n') && (ch != '\r')) {
			// add to line
			*line_ptr = ch;
			line_ptr++;
			*line_ptr = '\0';
			ch = f.read();
		}
		// ch was 0 or \n or \r, line complete continue
		*line_ptr = '\0';

		#ifdef USE_ITCH
			DebugLog(line);
			if (how == 0) {
				HALItchStuffAndProcess(line);
			} else {
				ConfigParse(line);
			}
		#else
			DebugLog(line);
			ConfigParse(line);
		#endif //USE_ITCH

		if (!f.available()) { //EOF
			break;
		}
	}
	f.close();

	#ifdef USE_ITCH
		if (how == 0) {
			HALItchSetTextDataMode();
		}
	#endif //USE_ITCH

	#endif //PLATFORM_ARDUINO

	EventMsg(SSS, E_INFO, M_CONFIG_LOAD_BY_DIRECT_PARSE);
}


#ifdef RESURRECT_DEPRECIATED
void WriteBinaryBlock(BlockNode *b, FILE *f, uint8_t _option) {
	// Called by FileIODispatcher on each block to write their
	//  config to a binary file
	// Note strict adherence to cross platform compatible types

	uint8_tWrite(b->cat, f);
	uint16_tWrite(b->type, f);
	uint16_tWrite(b->block_id, f);
	LabelWrite(b->label, MAX_LABEL_LENGTH, f);
	#ifndef	EXCLUDE_DISPLAYNAME
	LabelWrite(b->display_name, MAX_DISPLAY_NAME_LENGTH, f);
	#endif
	#ifndef EXCLUDE_DESCRIPTION
	LabelWrite(b->description, MAX_DESCR_LENGTH, f);
	#endif

	switch (b->cat) {
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
#endif //RESURRECT_DEPRECIATED


#ifdef RESURRECT_DEPRECIATED
void InitConfigSaveBinary(void) {
	// Write the running config (block list) to a binary file which can be
	//  more efficiently ingested in an avr environment by not needing the
	//  overhead of the INI file processing functions
	// Note the need for strict adherence to cross platform compatible types
	// Note the need for the same pre-proc directives between an instance writing
	//  a binary config file (normally linux) and the instance reading it later
	//  (eg avr). In particular EXCLUDE_DESCRIPTION and EXCLUDE_DISPLAYNAME
	//  change the size and field locations within the file.

	#ifdef PLATFORM_LINUX
	FILE *outfile;
	outfile = fopen(CONFIG_BIN_FILENAME, "w");
	FileIOBlockDispatcher(WriteBinaryBlock, outfile, 0);
	fclose(outfile);
	#endif
	#ifdef PLATFORM_ARDUINO
		EventMsg(SSS, E_INFO, M_BIN_SAVE_NOT_AVAIL);
	#endif //PLATFORM_ARDUINO
}
#endif //RESURRECT_DEPRECIATED

/*
2019-06-15 - INI Config Depreciated
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
*/


/*
2019-06-14 INI files depreciated and block_cat_defs denormalised into config keys
To retain this code it will need some re-writing to use the config keys glitch
generated structures.

void InitConfigLoadINI(void) {
#ifdef PLATFORM_LINUX
	// Create or update the block list and each block settings from stanzas and key / value
	//	pairs in an INI file identified by CONFIG_FILENAME
	// First read all the block label identifiers heading each block category section
	//  and register the blocks (to ensure block cross reference consistency
	// Then read each block section and set the key / value pairs
	// May be safely called without dropping the block list first in which case
	//  key value pairs will update the existing blocks.

	const char f_name[] = CONFIG_INI_FILENAME;
	#ifdef PLATFORM_ARDUINO
		IniFile cf(f_name);
	#endif
	#ifdef PLATFORM_LINUX
		char f_mode[] = "r\0";
		IniFile cf(f_name, f_mode);
	#endif

	char list_section[MAX_LABEL_LENGTH];		// [inputs] [controllers] etc
	char list_section_key[MAX_LABEL_LENGTH+2];	// input7=    controller3=
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
	last_key = LAST_SYS_CONFIG;

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
			if( cf.getError() != IniFile::errorKeyNotFound) {
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

				sprintf(debug_msg, "Registered [%s][%s] = %s", list_section, list_section_key, key_value);
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
				if( cf.getError() == IniFile::errorKeyNotFound) {  // this is expected once key list end is found
					sprintf_P(debug_msg, "[CONFIG] Registered %d [%S]", bl-1, block_cat_defs[block_cat].conf_section_label);
				} else {
					sprintf_P(debug_msg, "[CONFIG] LAST [%s] REACHED - [%S]: %s ", block_cat_defs[block_cat].conf_section_label, list_section_key, GetINIError(cf.getError(), ini_error_string));
				}
				#else
				if( cf.getError() == IniFile::errorKeyNotFound) {  // this is expected once key list end is found
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
					last_key = LAST_IN_CONFIG;
					break;
				case FF_MONITOR:
					last_key = LAST_MON_CONFIG;
					break;
				case FF_SCHEDULE:
					last_key = LAST_SCH_CONFIG;
					break;
				case FF_RULE:
					last_key = LAST_RL_CONFIG;
					break;
				case FF_CONTROLLER:
					last_key = LAST_CON_CONFIG;
					break;
				case FF_OUTPUT:
					last_key = LAST_OUT_CONFIG;
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
						if( cf.getError() != IniFile::errorKeyNotFound) {
							sprintf(debug_msg, "[CONFIG] ERROR [%s][%s][%s][%s]: %s ", list_section, list_section_key, block_section, block_section_key, GetINIError(cf.getError(), ini_error_string));
							DebugLog(debug_msg);
							//handle error - could be ok if key not required for that block sub type
						}
					}
				}

				bl++;

			} else {
				#ifdef PROGMEM_BLOCK_DEFS
				if( cf.getError() == IniFile::errorKeyNotFound) {
					sprintf_P(debug_msg, "[CONFIG] Processed %d [%S]", bl-1, block_cat_defs[block_cat].conf_section_label);
				} else {
					sprintf_P(debug_msg, "[CONFIG] LAST [%S] REACHED - [%s]: %s ", block_cat_defs[block_cat].conf_section_label, list_section_key, GetINIError(cf.getError(), ini_error_string));
				}
				#else
				if( cf.getError() == IniFile::errorKeyNotFound) {
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
#endif //PLATFORM_LINUX

#ifdef PLATFORM_ARDUINO
	EventMsg(SSS, E_INFO, M_INI_LOAD_NOT_AVAIL);
#endif
}

*/


