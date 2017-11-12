/************************************************
 ff_main.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Main System Routines
************************************************/

/************************************************
  Includes
************************************************/
#include "ff_main.h"
#include "ff_registry.h"  //first include for C global dec and init of main block list (bll) and dec of state register (sr)
#include "ff_sys_config.h"
#include "ff_string_consts.h"
#include <stdio.h>
#include <stdlib.h>

#include "ff_events.h"
#include "ff_debug.h"
#include "ff_display.h"
#include "ff_filesystem.h"

#ifdef FF_ARDUINO
#include "SD.h"
#endif

//#include "ff_IniFile.h"

//#include <string.h>


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
	printf("%u\n", data);
	return data;
}

uint16_t uint16_tRead(FILE *f) {
	uint16_t data;
//	fscanf(f, "%hu", &data);
	fread(&data, sizeof(data), 1, f);
	printf("%u\n", data);
	return data;
}

uint32_t uint32_tRead(FILE *f) {
	uint32_t data;
//	fscanf(f, "%u", &data);
	fread(&data, sizeof(data), 1, f);
	printf("%u\n", data);
	return data;
}

char* LabelRead(char* data, size_t s,  FILE *f) {
	//fgets(data, s, f);
	fread(data, s, 1, f);
	printf("%s\n", data);
	return data;
}

uint8_t* uint8_tArrayRead(uint8_t* data, size_t s,  FILE *f) {
	fread(data, s, 1, f);
	for (uint8_t i=0; i<s; i++) {
//		fscanf(f, "%hhu", &data[i]);
		printf("%u\n", data[i]);
	}
	return data;
}

float floatRead(FILE *f) {
	float data;
	fread(&data, sizeof(data), 1, f);
//	fscanf(f, "%f", &data);
	printf("%.2f\n ", data);
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


void ReadProcessedConfig(void) {
	BlockNode* b;
	uint8_t b_cat; 	//used as read-ahead to test for eof before creating a new block

#ifdef FF_SIMULATOR
	FILE *fp;
	fp = fopen(BIN_CONFIG_FILENAME, "r");
#endif
#ifdef FF_ARDUINO
	File f;
	File *fp;
	pinMode(SS, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(53, OUTPUT);
	// see if the card is present and can be initialized:
	if (SD.begin(10, 11, 12, 13)) {
		//DebugLog("DEBUG INFO sd.begin"); //cant use EventMsg
		f = SD.open(BIN_CONFIG_FILENAME, FILE_READ);
	}
	fp = &f;
#endif

	while (1) {
		b_cat = uint8_tRead(fp); //all blocks start with block category
//		DebugLog("Read b_cat");
#ifdef FF_ARDUINO
		if (!f.available()) {
//			DebugLog("Not available - break");
			break;
		}
#endif
#ifdef FF_SIMULATOR
		if(feof(fp)) {
			DebugLog("EOF");
			break;
		}
#endif
		b = AddBlock(FF_GENERIC_BLOCK, "NEW_BLANK_BLOCK"); //add a new one
//		DebugLog("Add block passed");
		if (b != NULL) {
			//we now have a valid block ptr pointing to a new block in the list
//			DebugLog("AddBlock not NULL");
			b->block_cat = b_cat;
			b->block_type = uint16_tRead(fp);
			b->block_id = uint16_tRead(fp);
			LabelRead(b->block_label, MAX_LABEL_LENGTH, fp);
#ifndef	EXCLUDE_DISPLAYNAME
			LabelRead(b->display_name, MAX_LABEL_LENGTH, fp);
#endif
#ifndef EXCLUDE_DESCRIPTION
			LabelRead(b->description, MAX_DESCR_LENGTH, fp);
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
	f.close();
	SD.end();
#endif
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

	// setup real UI (LCD, TFT, QT, None)
	//TODO - extend beyond LCD
	InitUI(); 				// set up UI arrangements

	// TODO display some timed status pages

	// check for a file system
	InitFileSystem(); // DEPR placeholder - no longer really needed

	#ifdef FF_SIMULATOR
		srand(time(NULL)); //random seed for simulator
	#endif
}




