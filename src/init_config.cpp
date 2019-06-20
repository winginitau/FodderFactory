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

#ifdef FF_SIMULATOR
#include <IniFile_ff.h>
#endif

#ifdef FF_ARDUINO

#ifdef OLD_SD
#include <SD.h>
#endif //OLD SD

#ifdef NEW_SD
#include <SPI.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#endif //NEW_SD

#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h
//
// Pin numbers in templates must be constants.
const uint8_t SOFT_MISO_PIN = 12;
const uint8_t SOFT_MOSI_PIN = 11;
const uint8_t SOFT_SCK_PIN  = 13;
//
// Chip select may be constant or RAM variable.
const uint8_t SD_CHIP_SELECT_PIN = 10;

// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;
#else  // ENABLE_SOFTWARE_SPI_CLASS
#error ENABLE_SOFTWARE_SPI_CLASS must be set non-zero in SdFat/SdFatConfig.h
#endif  //ENABLE_SOFTWARE_SPI_CLASS

#endif //FF_ARDUINO

#ifdef USE_ITCH
#include "itch.h"
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
#ifdef FF_SIMULATOR
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

#ifdef FF_ARDUINO
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
#endif


void InitConfigLoadBinary(void) {
//#ifdef OLD_SD
	// Build the block list from a binary config file previously written
	//  by InitConfigSaveBinary()
	// NOTE: no existence or duplicate checking - this function assumes an empty (NULL)
	//  block list therefore DropBlockList() is called first to make sure

	#ifdef FF_ARDUINO
		EventMsg(SSS, E_INFO, M_BIN_LOAD_NOT_AVAIL);
		return;
	#endif

	#ifdef FF_SIMULATOR
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

	#ifdef FF_SIMULATOR
	FILE *fp;
	fp = fopen(CONFIG_BIN_FILENAME, "r");
	#endif

	#ifdef FF_ARDUINO
	File f;
	File *fp;
	fp = &f;
	pinMode(SS, OUTPUT);
	pinMode(10, OUTPUT);
	//pinMode(53, OUTPUT);
	#endif

	#ifdef FF_ARDUINO
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

	#endif

	while (1) {    //continue reading the file until EOF break
		b_cat = uint8_tRead(fp); //all blocks start with block category
		//DebugLog("Read b_cat");
		#ifdef FF_ARDUINO
		if (!f.available()) {
			//DebugLog("Not available - break");
			break;
		}
		#endif

		#ifdef FF_SIMULATOR
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
			b->block_cat = b_cat;
			b->block_type = uint16_tRead(fp);
			b->block_id = uint16_tRead(fp);

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
			switch (b->block_cat) {
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
		DebugLog(b->block_id, E_VERBOSE, M_BLOCK_READ_BINARY);
	};

	#ifdef FF_SIMULATOR
	fclose(fp);
	#endif
	#ifdef FF_ARDUINO
//	DebugLog("Prior f.close");
	f.close();
//	DebugLog("After f.close, prior SD.end");
	SD.end();
//	DebugLog("After SD.end");
	#endif

	// As a fresh block list, call validate and setup on each block too
	ProcessDispatcher(Validate);
	ProcessDispatcher(Setup);
	#endif //FF_SIMULATOR
}


void InitSystem(void) {

	// Set up the global system state register
	InitStateRegister();

	// set up the event log ring buffer for IPC (and debug hooks to talk to the outside world)
	EventBufferInit();

	// log event - core internal system now functional
	#ifdef FF_ARDUINO
	EventMsg(SSS, E_INFO, M_FF_AWAKE, 0, 0);
	#endif

	#ifdef FF_SIMULATOR
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

	#ifdef FF_SIMULATOR
		srand(TimeNow()); //random seed for simulator
	#endif

#ifdef FF_ARDUINO
#ifdef DEBUG_DALLAS
//		while (1) {
			InitTempSensors();
//			delay(1000);
//		}
#endif
#endif
}

uint8_t GetConfKeyIndex(uint8_t block_cat, const char* key_str) {

	uint8_t last_key = UINT8_INIT;
	uint8_t key_idx = 0; 			//see "string_consts.h" Zero is error.

	//char temp[MAX_LABEL_LENGTH];
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
			//DebugLog("Match Block Category!");
			break;
		case FF_INPUT:
			last_key = LAST_IN_CONFIG;
			while (strcmp_hal(key_str, in_config_keys[key_idx].text) != 0) {
				//DebugLog(key_str);
				//strcpy_hal(temp, in_config_keys[key_idx].text);
				//DebugLog(temp);
				key_idx++;
				if (key_idx == last_key) break;
			}
			//DebugLog("Match Block Category!");
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
			#ifdef FF_ARDUINO
				while(1);
			#endif
			#ifdef FF_SIMULATOR
				exit(-1);
			#endif
	}

	//check that we have a key that matches one of the keys strings of the block category
	/*
	#ifdef PROGMEM_BLOCK_DEFS
		char temp[MAX_LABEL_LENGTH];
		while ((strcmp_P(key_str, block_cat_defs[block_cat].conf_keys[key_idx]) != 0) && key_idx < last_key) {
			strcpy_hal(temp, block_cat_defs[block_cat].conf_keys[key_idx]);
			DebugLog(temp);
			key_idx++;
		}
	#else
		while ((strcmp(key_str, block_cat_defs[block_cat].conf_keys[key_idx]) != 0) && key_idx < last_key) {
			key_idx++;
		}
	#endif
	*/

	if (key_idx == last_key) {
		EventMsg(SSS, E_ERROR, M_CONFKEY_NOTIN_DEFS);
		HALItchWriteLnImmediate(key_str);
		return 0; //error
/*
		#ifdef FF_ARDUINO
			while(1);
		#endif
		#ifdef FF_SIMULATOR
			exit(-1);
		#endif
*/
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
			block_ptr->block_type = BlockTypeStringArrayIndex(value_str);
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
			break;
		case SYS_CONFIG_TEMP_SCALE:
			block_ptr->settings.sys.temp_scale = UnitStringArrayIndex(value_str);
			return block_ptr->settings.sys.temp_scale;
			// XXX testing threading of error results
			break;
		case SYS_CONFIG_WEEK_START:
			block_ptr->settings.sys.week_start = DayStringArrayIndex(value_str);
			break;
		case SYS_CONFIG_START_DELAY:
			block_ptr->settings.sys.start_delay = atoi(value_str);
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
		case IN_CONFIG_INTERFACE:
			block_ptr->settings.in.interface = InterfaceStringArrayIndex(value_str);
			break;
		case IN_CONFIG_IF_NUM:
			block_ptr->settings.in.if_num = atoi(value_str);
			break;
		case IN_CONFIG_LOG_RATE:
			block_ptr->settings.in.log_rate = StringToTimeValue(value_str);
			break;
		case IN_CONFIG_POLL_RATE:
			block_ptr->settings.in.poll_rate = StringToTimeValue(value_str);
			break;

		case IN_CONFIG_DATA_UNITS: {
			block_ptr->settings.in.data_units = UnitStringArrayIndex(value_str);
			if(block_ptr->settings.in.data_units == 0) {
				DebugLog(SSS, E_ERROR, M_BAD_DATA_UNITS);
			}
			return block_ptr->settings.in.data_units;
			/*
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
			*/
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
	return 1;
}

uint8_t ConfigureMONSetting(BlockNode* block_ptr, uint8_t key_idx, const char* value_str) {
	switch (key_idx) {
		case MON_CONFIG_INPUT1:
			block_ptr->settings.mon.input1 = GetBlockIDByLabel(value_str, true);
			break;
		case MON_CONFIG_INPUT2:
			block_ptr->settings.mon.input2 = GetBlockIDByLabel(value_str, true);
			break;
		case MON_CONFIG_INPUT3:
			block_ptr->settings.mon.input3 = GetBlockIDByLabel(value_str, true);
			break;
		case MON_CONFIG_INPUT4:
			block_ptr->settings.mon.input4 = GetBlockIDByLabel(value_str, true);
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
		case SCH_CONFIG_DAYS:
			if (DayStrToFlag(block_ptr->settings.sch.days, value_str) != 1) {
				DebugLog(SSS, E_WARNING, M_DAY_FLAG_EMPTY);
			}
			break;
		case SCH_CONFIG_TIME_START: {
			block_ptr->settings.sch.time_start = StringToTimeValue(value_str);
			break;
		}
		case SCH_CONFIG_TIME_END: {
			block_ptr->settings.sch.time_end = StringToTimeValue(value_str);
			break;
		}
		case SCH_CONFIG_TIME_DURATION: {
			block_ptr->settings.sch.time_duration = StringToTimeValue(value_str);
			break;
		}
		case SCH_CONFIG_TIME_REPEAT: {
			block_ptr->settings.sch.time_repeat = StringToTimeValue(value_str);
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
		case RL_CONFIG_PARAM1:
			block_ptr->settings.rl.param1 = GetBlockIDByLabel(value_str, true);
			break;
		case RL_CONFIG_PARAM2:
			block_ptr->settings.rl.param2 = GetBlockIDByLabel(value_str, true);
			break;
		case RL_CONFIG_PARAM3:
			block_ptr->settings.rl.param3 = GetBlockIDByLabel(value_str, true);
			break;
		case RL_CONFIG_PARAM_NOT:
			block_ptr->settings.rl.param_not = GetBlockIDByLabel(value_str, true);
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
		case CON_CONFIG_RULE:
			block_ptr->settings.con.rule = GetBlockIDByLabel(value_str, true);
			break;
		case CON_CONFIG_OUTPUT:
			block_ptr->settings.con.output = GetBlockIDByLabel(value_str, true);
			break;
		case CON_CONFIG_ACT_CMD: {
			block_ptr->settings.con.act_cmd = CommandStringArrayIndex(value_str);
			if(block_ptr->settings.con.act_cmd == 0) {
				DebugLog(SSS, E_ERROR, M_ACT_CMD_UNKNOWN);
			}
			return block_ptr->settings.con.act_cmd;
			break;
			/*
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
			*/
		}
		case CON_CONFIG_DEACT_CMD: {
			block_ptr->settings.con.deact_cmd = CommandStringArrayIndex(value_str);
			if(block_ptr->settings.con.deact_cmd == 0) {
				DebugLog(SSS, E_ERROR, M_DEACT_CMD_UNKNOWN);
			}
			return block_ptr->settings.con.deact_cmd;
			break;
			/*
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
			*/
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
		case OUT_CONFIG_INTERFACE:
			block_ptr->settings.out.interface = InterfaceStringArrayIndex(value_str);
			break;
		case OUT_CONFIG_IF_NUM:
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
	// Check for existence of a block identified by block_label
	// If it exists update the setting identified by key_str and value_str
	// If it does not exist, create it first and then update key / value
	// If called with key_str == NULL then just just create it and label it,
	//	or report true if already exists.

	//DebugLog(block_label);
	//DebugLog(key_str);
	//DebugLog(value_str);

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



#if defined FF_SIMULATOR

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

#ifdef FF_SIMULATOR
void HALWriteLine(FILE *f, const char* out_str) {
	fprintf(f, "%s\n", out_str);
}
#endif

#ifdef FF_ARDUINO
void HALWriteLine(SdFile *f, const char* out_str) {
	DebugLog(out_str);
	f->println(out_str);
}
#endif

#ifdef FF_SIMULATOR
void WriteTextBlock(BlockNode *b, FILE *f, uint8_t reg_only) {
#endif
#ifdef FF_ARDUINO
void WriteTextBlock(BlockNode *b, SdFile *f, uint8_t reg_only) {
#endif
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
	strcat_hal(base_str, block_cat_names[b->block_cat].text);
	strcat_hal(base_str, F(" "));
	strcat(base_str, b->block_label);
	strcat_hal(base_str, F(" "));

	// Key value pair "type" common to all blocks - build them and write them
	strcpy(out_str, base_str);
	strcat_hal(out_str, F("type "));
	strcat_hal(out_str, block_type_strings[b->block_type].text);
	HALWriteLine(f, out_str);

	// register the block labels and types only
	if (reg_only == 1 ) return;

	// Write the block status only if it is admin disabled
	// so that it persists between reboots
	if(b->status == STATUS_DISABLED_ADMIN) {
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("disable "));
		strcat_hal(out_str, F("True"));
		HALWriteLine(f, out_str);
	}

	#ifndef	EXCLUDE_DISPLAYNAME
		if (b->display_name[0] != '\0') {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("display_name \""));
			strcat(out_str, b->display_name);
			strcat(out_str, "\"");
			HALWriteLine(f, out_str);
		}
	#endif
	#ifndef EXCLUDE_DESCRIPTION
		if (b->description[0] != '\0') {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("description \""));
			strcat(out_str, b->description);
			strcat(out_str, "\"");
			HALWriteLine(f, out_str);
		}
	#endif

	switch (b->block_cat) {
	case FF_SYSTEM:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("temp_scale "));
		strcat_hal(out_str, unit_strings[b->settings.sys.temp_scale].text);
		HALWriteLine(f, out_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("language "));
		strcat_hal(out_str, language_strings[b->settings.sys.language].text);
		HALWriteLine(f, out_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("week_start "));
		strcat_hal(out_str, day_strings[b->settings.sys.week_start].text);
		HALWriteLine(f, out_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("start_delay "));
		sprintf(temp_str, "%s%d", out_str, b->settings.sys.start_delay);
		HALWriteLine(f, temp_str);

		break;

	case FF_INPUT:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("interface "));
		strcat_hal(out_str, interface_strings[b->settings.in.interface].text);
		HALWriteLine(f, out_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("if_num "));
		sprintf(temp_str, "%s%d", out_str, b->settings.in.if_num);
		HALWriteLine(f, temp_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("log_rate "));
		strcat(out_str, TimeValueToTimeString(hms_str, b->settings.in.log_rate));
		HALWriteLine(f, out_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("poll_rate "));
		strcat(out_str, TimeValueToTimeString(hms_str, b->settings.in.poll_rate));
		HALWriteLine(f, out_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("data_units "));
		strcat_hal(out_str, unit_strings[b->settings.in.data_units].text);
		HALWriteLine(f, out_str);

		// data type not properly implemented or used
		//uint8_tWrite(b->settings.in.data_type, f);
		//data_type = float

		break;
	case FF_MONITOR:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("input1 "));
		strcat(out_str, GetBlockLabelString(b->settings.mon.input1));
		HALWriteLine(f, out_str);

		if (b->settings.mon.input2 != UINT16_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("input2 "));
			strcat(out_str, GetBlockLabelString(b->settings.mon.input2));
			HALWriteLine(f, out_str);
		}

		if (b->settings.mon.input3 != UINT16_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("input3 "));
			strcat(out_str, GetBlockLabelString(b->settings.mon.input3));
			HALWriteLine(f, out_str);
		}

		if (b->settings.mon.input2 != UINT16_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("input4 "));
			strcat(out_str, GetBlockLabelString(b->settings.mon.input4));
			HALWriteLine(f, out_str);
		}

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("act_val "));
		strcat(out_str, FFFloatToCString(float_str, b->settings.mon.act_val));
		HALWriteLine(f, out_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("deact_val "));
		strcat(out_str, FFFloatToCString(float_str, b->settings.mon.deact_val));
		HALWriteLine(f, out_str);
		break;

	case FF_SCHEDULE:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("days "));
		strcat(out_str, FlagToDayStr(temp_str, b->settings.sch.days));
		HALWriteLine(f, out_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("time_start "));
		strcat(out_str, TimeValueToTimeString(hms_str, b->settings.sch.time_start));
		HALWriteLine(f, out_str);

		if (b->settings.sch.time_end != UINT32_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("time_end "));
			strcat(out_str, TimeValueToTimeString(hms_str, b->settings.sch.time_end));
			HALWriteLine(f, out_str);
		}

		if (b->settings.sch.time_duration != UINT32_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("time_duration "));
			strcat(out_str, TimeValueToTimeString(hms_str, b->settings.sch.time_duration));
			HALWriteLine(f, out_str);
		}

		if (b->settings.sch.time_repeat != UINT32_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("time_repeat "));
			strcat(out_str, TimeValueToTimeString(hms_str, b->settings.sch.time_repeat));
			HALWriteLine(f, out_str);
		}
		break;

	case FF_RULE:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("param1 "));
		strcat(out_str, GetBlockLabelString(b->settings.rl.param1));
		HALWriteLine(f, out_str);

		if (b->settings.rl.param2 != UINT16_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("param2 "));
			strcat(out_str, GetBlockLabelString(b->settings.rl.param2));
			HALWriteLine(f, out_str);
		}

		if (b->settings.rl.param3 != UINT16_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("param3 "));
			strcat(out_str, GetBlockLabelString(b->settings.rl.param3));
			HALWriteLine(f, out_str);
		}

		if (b->settings.rl.param_not != UINT16_INIT) {
			strcpy(out_str, base_str);
			strcat_hal(out_str, F("param_not "));
			strcat(out_str, GetBlockLabelString(b->settings.rl.param_not));
			HALWriteLine(f, out_str);
		}
		break;

	case FF_CONTROLLER:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("rule "));
		strcat(out_str, GetBlockLabelString(b->settings.con.rule));
		HALWriteLine(f, out_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("output "));
		strcat(out_str, GetBlockLabelString(b->settings.con.output));
		HALWriteLine(f, out_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("act_cmd "));
		strcat_hal(out_str, command_strings[b->settings.con.act_cmd].text);
		HALWriteLine(f, out_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("deact_cmd "));
		strcat_hal(out_str, command_strings[b->settings.con.deact_cmd].text);
		HALWriteLine(f, out_str);

		break;
	case FF_OUTPUT:
		strcpy(out_str, base_str);
		strcat_hal(out_str, F("interface "));
		strcat_hal(out_str, interface_strings[b->settings.out.interface].text);
		HALWriteLine(f, out_str);

		strcpy(out_str, base_str);
		strcat_hal(out_str, F("if_num "));
		sprintf(temp_str, "%s%d", out_str, b->settings.out.if_num);
		HALWriteLine(f, temp_str);

		break;

	default:

		break;
	}
}

#ifdef RESURRECT_DEPRECIATED
void WriteBinaryBlock(BlockNode *b, FILE *f, uint8_t _option) {
	// Called by FileIODispatcher on each block to write their
	//  config to a binary file
	// Note strict adherence to cross platform compatible types

	uint8_tWrite(b->block_cat, f);
	uint16_tWrite(b->block_type, f);
	uint16_tWrite(b->block_id, f);
	LabelWrite(b->block_label, MAX_LABEL_LENGTH, f);
	#ifndef	EXCLUDE_DISPLAYNAME
	LabelWrite(b->display_name, MAX_DISPLAY_NAME_LENGTH, f);
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
#endif //RESURRECT_DEPRECIATED

#ifdef FF_SIMULATOR
void FileIODispatcher(void(*func)(BlockNode*, FILE*, uint8_t), FILE* file, uint8_t option) {
#endif
#ifdef FF_ARDUINO
void FileIODispatcher(void(*func)(BlockNode*, SdFile*, uint8_t), SdFile* file, uint8_t option) {
#endif
	BlockNode* block_ptr;

	block_ptr = GetBlockListHead();

	while (block_ptr != NULL) {
		func(block_ptr, file, option);
		block_ptr = block_ptr->next_block;
	}
}

void InitConfigSave(void) {
	// Write the running config to a text file in the form of parse-able
	//  CONFIG statements that can be read by itch in buffer stuffing mode

	#ifdef FF_SIMULATOR
	FILE *fp;
	fp = fopen(CONFIG_TXT_FILENAME, "w");
	#endif

	#ifdef FF_ARDUINO
	SdFile f;
	//File *fp;
	//fp = &f;
	//pinMode(SS, OUTPUT);
	//pinMode(10, OUTPUT);
	//pinMode(53, OUTPUT);
	#endif

	#ifdef FF_ARDUINO
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
	#endif

	#ifdef FF_SIMULATOR
		// First write block registrations only (option = 1)
		FileIODispatcher(WriteTextBlock, fp, 1);
		// Then write the rest of the settings (option = 0)
		FileIODispatcher(WriteTextBlock, fp, 0);
	#endif

	#ifdef FF_ARDUINO
		// First write block registrations only (option = 1)
		FileIODispatcher(WriteTextBlock, &f, 1);
		// Then write the rest of the settings (option = 0)
		FileIODispatcher(WriteTextBlock, &f, 0);
	#endif

	#ifdef FF_SIMULATOR
	fclose(fp);
	#endif
	#ifdef FF_ARDUINO
	f.flush();
	f.close();
	//sd.end();
	#endif

}

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

	#ifdef FF_SIMULATOR
	FILE *outfile;
	outfile = fopen(CONFIG_BIN_FILENAME, "w");
	FileIODispatcher(WriteBinaryBlock, outfile, 0);
	fclose(outfile);
	#endif
	#ifdef FF_ARDUINO
		EventMsg(SSS, E_INFO, M_BIN_SAVE_NOT_AVAIL);
	#endif
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


#ifdef FF_SIMULATOR
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
		#ifdef FF_ARDUINO
			while (1);
		#endif
		#ifdef FF_SIMULATOR
			exit(-1);
		#endif
	}
}
#endif //FF_SIMULATOR

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

    	//DebugLog(wl[0]);
    	//DebugLog(wl[1]);
    	//DebugLog(wl[2]);
    	//DebugLog(wl[3]);
    	//DebugLog(wl[4]);

    	ConfigureBlock(GetBlockCatIDByName(wl[1]), wl[2], wl[3], wl[4]);
    }

}

void InitConfigLoad(uint8_t how) {
	// Build the block list from a text config file previously written
	//  by InitConfigSave()
	// Read each line of the file and pass it to ITCH::StuffAndProcess() which will
	//	use the restuff buffer to insert the line into the parsing stream and then
	//	poll itch until it is consumed.
	// NOTE: no existence or duplicate checking - this function assumes an empty (NULL)
	//  block list therefore DropBlockList() is called first to make sure

	// XXX this whole routine and subs needs error handling

//DebugLog("GOOD");
	#ifndef USE_ITCH
		(void)how;		// Not relevent if not using itch;
	#endif

	#ifdef FF_SIMULATOR
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

	#endif // FF_SIMULATOR


	#ifdef FF_ARDUINO
	#ifdef OLD_SD
	size_t lin_len = MAX_LABEL_LENGTH + 19 + MAX_DESCR_LENGTH;
	char line[lin_len];
	File f;
	DebugLog("GOOD");
	pinMode(SS, OUTPUT);
	pinMode(10, OUTPUT);
	//pinMode(53, OUTPUT);

	// see if the card is present and can be initialized:
	if (SD.begin(10, 11, 12, 13)) {
		//	if (SD.begin(10)) {
		f = SD.open(CONFIG_TXT_FILENAME, FILE_READ);
		DebugLog("GOOD");
		if (!f) {
			char temp_label[MAX_LABEL_LENGTH];
			strcpy_hal(temp_label, F("NO CONFIG"));
			DebugLog(temp_label);
			// Cannot do anything else
			while (1);
		}
	} else {
		char temp_label[MAX_LABEL_LENGTH];
		strcpy_hal(temp_label, F("SD FAIL"));
		DebugLog(temp_label);
		while (1);
	}
	#ifdef USE_ITCH
		if (how == 0) {
			HALItchSetBufferStuffMode();
		}
	#endif //USE_ITCH

	while (1) {    //continue reading the file until EOF break

		DebugLog("GOOD");
		f.readBytesUntil('\n', line, lin_len);
		DebugLog("GOOD");
		#ifdef USE_ITCH
			DebugLog(line);
			if (how == 0) {
				HALItchStuffAndProcess(line);
			} else {
				ConfigParse(line);
			}
		#else
			DebugLog(line);
			DebugLog("GOOD");
			ConfigParse(line);
			DebugLog("GOOD");
		#endif //USE_ITCH

		if (!f.available()) { //EOF
			break;
		}
	}
	f.close();
	SD.end();

	#ifdef USE_ITCH
		if (how == 0) {
			HALItchSetTextDataMode();
		}
	#endif //USE_ITCH
	#endif // OLD_SD

	#ifdef NEW_SD
//CONFIG schedule SCH_WATER_BOT type SCH_START_DURATION_REPEAT
	//size_t lin_len = MAX_LABEL_LENGTH + 19 + MAX_DESCR_LENGTH;
	//size_t lin_len = 120
	char *line_ptr;
	char ch;
	char line[120];
	SdFile f;
	//DebugLog("GOOD");

	// see if the card is present and can be initialized:
	if (!sd.begin(SD_CHIP_SELECT_PIN)) {
		char temp_label[MAX_LABEL_LENGTH];
		strcpy_hal(temp_label, F("SD BEGIN FAILED"));
		DebugLog(temp_label);
		// Cannot do anything else
		while (1);
		//sd.initErrorHalt();
	}

	if (!f.open(CONFIG_TXT_FILENAME, O_READ)) {
		//char temp_label[MAX_LABEL_LENGTH];
		EventMsg(SSS, E_ERROR, NO_CONFIG_FILE);
		//strcpy_hal(temp_label, F("F.OPEN CONFIG FAILED"));
		//DebugLog(temp_label);
		return;
		//while (1);
		//sd.errorHalt(F("open failed"));
	}

	//DebugLog("GOOD");

	#ifdef USE_ITCH
	if (how == 0) {
		HALItchSetBufferStuffMode();
	}
	#endif //USE_ITCH

	while (1) {    //continue reading the file until EOF break
		//DebugLog("GOOD");
		line_ptr = line;
		*line_ptr = '\0';

		// Get first char
		ch = f.read();
		// Discard inital blank lines and extra \n \r from previous line
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
			//DebugLog(line);
			ch = f.read();
		}
		// ch was 0 or \n or \r, line complete continue
		*line_ptr = '\0';
		//DebugLog("GOOD");
		#ifdef USE_ITCH
			DebugLog(line);
			if (how == 0) {
				HALItchStuffAndProcess(line);
			} else {
				ConfigParse(line);
			}
		#else
			DebugLog(line);
			//DebugLog("GOOD");
			ConfigParse(line);
			//DebugLog("GOOD");
		#endif //USE_ITCH

		if (!f.available()) { //EOF
			break;
		}
	}
	f.close();
	//sd.end();

	#ifdef USE_ITCH
		if (how == 0) {
			HALItchSetTextDataMode();
		}
	#endif //USE_ITCH

	#endif //NEW_SD
	#endif //FF_ARDUINO

	EventMsg(SSS, E_INFO, M_CONFIG_LOAD_BY_DIRECT_PARSE);
}

/*
2019-06-14 INI files depreciated and block_cat_defs denormalised into config keys
To retain this code it will need some re-writing to use the config keys glitch
generated structures.

void InitConfigLoadINI(void) {
#ifdef FF_SIMULATOR
	// Create or update the block list and each block settings from stanzas and key / value
	//	pairs in an INI file identified by CONFIG_FILENAME
	// First read all the block label identifiers heading each block category section
	//  and register the blocks (to ensure block cross reference consistency
	// Then read each block section and set the key / value pairs
	// May be safely called without dropping the block list first in which case
	//  key value pairs will update the existing blocks.

	const char f_name[] = CONFIG_INI_FILENAME;
	#ifdef FF_ARDUINO
		IniFile cf(f_name);
	#endif
	#ifdef FF_SIMULATOR
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
#endif //FF_SIMULATOR

#ifdef FF_ARDUINO
	EventMsg(SSS, E_INFO, M_INI_LOAD_NOT_AVAIL);
#endif
}

*/


