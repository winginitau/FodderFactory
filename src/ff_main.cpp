/************************************************
 ff_main.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Main System Routines
************************************************/

/************************************************
  Includes
************************************************/
#include "ff_registry.h"  //first include for C global dec and init of main block list (bll) and dec of state register (sr)

#include "ff_sys_config.h"
#include "ff_events.h"
#include "ff_string_consts.h"

#include "ff_debug.h"
#include "ff_display.h"
#include "ff_filesystem.h"


#include "ff_IniFile.h"

#include <string.h>


/************************************************
  Data Structures
************************************************/



/************************************************
  Globals
************************************************/



/************************************************
  Functions
************************************************/


char* GetINIError(uint8_t e, char* msg_buf) {

	strcpy(msg_buf, ini_error_strings[e].text);
	return msg_buf;
}

void InitConfFile(IniFile* cf) {
	char key_value[INI_FILE_MAX_LINE_LENGTH];
	if (!cf->open()) {
		DebugLog(SSS, E_ERROR, M_NO_CONFIG, 0, 0);
		// Cannot do anything else
		while (1)
			;
	}
	// Check the file is valid. This can be used to warn if any lines
	// are longer than the buffer.
	if (!cf->validate(key_value, INI_FILE_MAX_LINE_LENGTH)) {
		DebugLog(SSS, E_ERROR, M_CONFIG_NOT_VALID, 0, 0);
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
	char debug_msg[MAX_DEBUG_LENGTH];
	char ini_error_string[MAX_LABEL_LENGTH];

	InitConfFile(&cf);

	//Read and process the system block for
	//global settings - language and scale etc
	block_cat = (FF_SYSTEM);
	last_key = LAST_SYS_KEY_TYPE;

	DebugLog("[CONFIG] Reading and Processing Configuration File");
	DebugLog("[CONFIG] Processing [system] Block");


	for (int key = 1; key < last_key; key++) {  //see string_consts.h - 0 reserved for error types.
		strcpy(block_section_key, block_cat_defs[block_cat].conf_keys[key]);

		if (cf.getValue("system", block_section_key, key_value, INI_FILE_MAX_LINE_LENGTH)) {
			if (ConfigureBlock(FF_SYSTEM, "system", block_section_key, key_value)) {
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
	// them in the registry vis the ConfigureBlock call.
	// Note they can't be fully configured
	// in this iteration because blocks refer to other blocks
	// so we need the full list of block identifiers registered first
	// Calls to ConfigureBlock with NULL for setting::value pair
	// trigger registration only.

	block_cat = (FF_SYSTEM + 1); 			//general block cat defs always follow the reserved system cat.
	bl = 1; 								//block list start from "1" in config file "input1=" etc
	uint8_t last_found = 0;					//set flag on first read error
											//TODO this can fail if lists are not numbered correctly
	DebugLog("[CONFIG] Registering Config Blocks and Labels");


	for (; block_cat < LAST_BLOCK_CAT; block_cat++) { //iterate through each block category
		last_found = 0;
		while (bl < MAX_BLOCKS_PER_CATEGORY && !last_found) { 		// a b_cat<bl> = <Label> is still to process (potentially)

			strcpy(list_section, block_cat_defs[block_cat].conf_section_label);	//list_section
			sprintf(list_section_key, "%s%d", block_cat_defs[block_cat].conf_section_key_base, bl);	//list_section_key
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
				if( cf.getError() == errorKeyNotFound) {  // this is expected once key list end is found
					sprintf(debug_msg, "[CONFIG] Registered %d [%s]", bl-1, block_cat_defs[block_cat].conf_section_label);
				} else {
					sprintf(debug_msg, "[CONFIG] LAST [%s] REACHED - [%s]: %s ", block_cat_defs[block_cat].conf_section_label, list_section_key, GetINIError(cf.getError(), ini_error_string));
				}
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

	DebugLog("[CONFIG] Processing Each Block Configuration");

	for (; block_cat < LAST_BLOCK_CAT; block_cat++) { //iterate through each block category
		last_found = 0;
		while (bl < MAX_BLOCKS_PER_CATEGORY && !last_found) { 		// a b_cat<bl> = <Label> is still to process (potentially)

			strcpy(list_section, block_cat_defs[block_cat].conf_section_label);	//list_section
			sprintf(list_section_key, "%s%d", block_cat_defs[block_cat].conf_section_key_base, bl);	//list_section_key
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

					strcpy(block_section_key, block_cat_defs[block_cat].conf_keys[key]);

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
				if( cf.getError() == errorKeyNotFound) {
					sprintf(debug_msg, "[CONFIG] Processed %d [%s]", bl-1, block_cat_defs[block_cat].conf_section_label);
				} else {
					sprintf(debug_msg, "[CONFIG] LAST [%s] REACHED - [%s]: %s ", block_cat_defs[block_cat].conf_section_label, list_section_key, GetINIError(cf.getError(), ini_error_string));
				}
				DebugLog(debug_msg);
				last_found = 1;
				bl = 1;
			}
		}
	}

	cf.close();
}


void InitSystem(void) {
	//Read the config file, parse it and create a block list in memory
	//XXX consider if Block0(SYSTEM) can be the state register?
	ReadAndParseConfig();

	// Set up the global system state register
	InitStateRegister();

	// set up the event log ring buffer for IPC (and debug hooks to talk to the outside world)
	EventBufferInit();

	// log event - core internal system now functional
#ifdef FF_ARDUINO
	EventMsg(SSS, INFO, M_FF_AWAKE, 0, 0);
#endif
#ifdef FF_SIMULATOR
	EventMsg(SSS, E_INFO, M_SIM);
#endif

#ifdef DEBUG_SERIAL
	EventMsg(SSS, DEBUG_MSG, M_D_SERIAL, 0, 0);
#endif

	// get some real time
	// set sys time or fallback to 00:00
	// log it
	InitRTC();				// set up the Real Time Clock

	// debug / console / serial / LCD UI set via ff_sys_config.h
	// at compile time.

	// setup real UI (LCD, TFT, QT, None)
	//TODO - extend beyond LCD
	InitUI(); 				// set up UI arrangements

	// TODO display some timed status pages

	// check for a file system
	InitFileSystem(); // DEPR placeholder - no longer needed

	// Any other global setup routines that are handled at
	// compile time, above, or or during system block configuration

	#ifdef FF_SIMULATOR
		srand(time(NULL)); //random seed for simulator
	#endif
}




