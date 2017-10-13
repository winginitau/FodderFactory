/************************************************
 ff_main.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Main System Routines
************************************************/

/************************************************
  Includes
************************************************/
#include "ff_sys_config.h"
#include "ff_events.h"
#include "ff_string_consts.h"
#include "ff_registry.h"
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

	strcpy(msg_buf, ini_error_strings[e]);
	return msg_buf;
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
	//uint8_t total_block_count = 0;
	//char section_label[MAX_LABEL_LENGTH];
	//char base_key[MAX_LABEL_LENGTH];

	char target_key[MAX_LABEL_LENGTH];			//to search for
	char key_value[INI_FILE_MAX_LINE_LENGTH];	//to read value into
	char char_num[4];							//to add to key strings to find how many are defined
	int bl;										//iterator - block list within block category

	if (!cf.open()) {
		DebugLog(SSS, ERROR, M_NO_CONFIG, 0, 0);
		// Cannot do anything else
		while (1);
	}
	// Check the file is valid. This can be used to warn if any lines
	// are longer than the buffer.
	if (!cf.validate(key_value, INI_FILE_MAX_LINE_LENGTH)) {
		DebugLog(SSS, ERROR, M_CONFIG_NOT_VALID, 0, 0);
		// Cannot do anything else
		while (1);
	}

	/*  errorNoError = 0,
    errorFileNotFound,
    errorFileNotOpen,
    errorBufferTooSmall,
    errorSeekError,
    errorSectionNotFound,
    errorKeyNotFound,
    errorEndOfFile,
    errorUnknownError,
    */

	//TODO read system block and set language and scale

	char debug_msg[MAX_LABEL_LENGTH * 3];
	char ini_error_string[MAX_LABEL_LENGTH];

	//Block type 0 will always be SYSTEM - special case - start from b_cat=1

	for (int b_cat = 1; b_cat < LAST_BLOCK_CAT; b_cat++) {    //iterate each block category
		int block_count = 0;

		bl = 1; 	//block list start from "1" in config file
		uint8_t last_found = 0;
		while (bl < MAX_BLOCKS_PER_CATEGORY && !last_found) {
		//for (bl = 1; bl < MAX_BLOCKS_PER_CATEGORY; bl++) {  //check up to MAX for defined blocks per category

			strcpy(target_key, block_cat_defs[b_cat].conf_section_key_base);
			sprintf(char_num, "%d", bl);
			strcat(target_key, char_num);
			key_value[0] = '\0';

			if (cf.getValue(block_cat_defs[b_cat].conf_section_label, target_key, key_value, INI_FILE_MAX_LINE_LENGTH)) {

				sprintf(debug_msg, "[%s][%s] = %s", block_cat_defs[b_cat].conf_section_label, target_key, key_value);
				DebugLog(debug_msg);
				block_count++;
				bl++;

			} else {
				sprintf(debug_msg, "ERROR[%s][%s]: %s ",block_cat_defs[b_cat].conf_section_label, target_key, GetINIError(cf.getError(), ini_error_string));
				DebugLog(debug_msg);
				last_found = 1;
			}
		}
	}
	cf.close();


/*
	const size_t bufferLen = 150;
	char buffer[bufferLen];

	// Check the file is valid. This can be used to warn if any lines
	// are longer than the buffer.
	if (!cf.validate(buffer, bufferLen)) {
		printf("ini file ");
		printf("%s", cf.getFilename());
		printf(" not valid.\n ");
		printErrorMessage(cf.getError());
		// Cannot do anything else
		while (1)
			;
	}

	// Fetch a value from a key which is present
	if (cf.getValue("controllers", "controller2", buffer, bufferLen)) {
		printf("section and entry valid: ");
		printf("%s\n", buffer);
	} else {
		printf("Could not read section and key value, error was ");
		printf("d%\n", cf.getError());
		printErrorMessage(cf.getError());
	}

	// Try fetching a value from a missing key (but section is present)
	if (cf.getValue("controllers", "nosuchkey", buffer, bufferLen)) {
		printf("section 'controllers' has an entry 'nosuchkey' with value ");
		printf("%S\n", buffer);
	} else {
		printf(
				"Could not read 'nosuchkey' from section 'controllers', error was ");
		printErrorMessage(cf.getError());
	}

	// Try fetching a key from a section which is not present
	if (cf.getValue("nosuchsection", "controller2", buffer, bufferLen)) {
		printf(
				"section 'nosuchsection' has an entry 'controller2' with value ");
		printf("%S\n", buffer);
	} else {
		printf(
				"Could not read 'controller2' from section 'nosuchsection', error was ");
		printErrorMessage(cf.getError());
	}

	// Fetch a boolean value
	bool allowPut; // variable where result will be stored
	bool found = cf.getValue("/upload", "allow put", buffer, bufferLen,
			allowPut);
	if (found) {
		printf("The value of 'allow put' in section '/upload' is ");
		// Print value, converting boolean to a string
		printf(allowPut ? "TRUE" : "FALSE");
	} else {
		printf("Could not get the value of 'allow put' in section '/upload': ");
		printErrorMessage(cf.getError());
	}
*/



}


void InitSystem(void) {
	// Got here - brain awake, need some memory
	InitStateRegister();

	// set up the event log ring buffer (and debug hooks) to talk to the outside (somehow)
	EventBufferInit();

	// log event - we're alive (minimally)
#ifdef FF_ARDUINO
	EventMsg(SSS, INFO, M_FF_AWAKE, 0, 0);
#endif
#ifdef FF_SIMULATOR
	EventMsg(SSS, INFO, M_SIM, 0, 0);
#endif

#ifdef DEBUG_SERIAL
	EventMsg(SSS, DEBUG_MSG, M_D_SERIAL, 0, 0);
#endif

	// get some real time
	// set sys time or fallback to 00:00
	// log it
	InitRTC();				// set up the Real Time Clock

	// milestone - we can now run (though without config)
	// safe now to talk to the UI - set it up
	InitUI(); 				// set up UI arrangements
	// TODO display some timed status pages
	// check for a file system
	InitFileSystem(); //placeholder - not needed

	//TODO
	//ReadAndParseConfig();


	// check for config file

	// parse config file
	//		inputs
	//		monitors (inc mins and maxes)
	//		controllers
	//		outputs

}




