//============================================================================
// Name        : ff_simulator.cpp
// Author      : Brendan McLearie
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

using namespace std;

/************************************************
 Includes
 ************************************************/



#include <ff_sys_config.h>
#include <ff_string_consts.h>

#ifdef FF_ARDUINO
//#include <Arduino.h>
#endif

//#include "src/ff_utils.h"
#include <ff_inputs.h>
#include <ff_outputs.h>
#include <ff_controllers.h>
#include <ff_display.h>
#include <ff_main.h>
#include <ff_validate.h>
#include <ff_registry.h>
#include <ff_debug.h>
#include <stdio.h>
#include <ff_ini_config.h>


/************************************************
 Main Functions
 ************************************************/

void loop() {
	// Operate each block forever
	ProcessDispatcher(Operate);

}


#ifdef FF_ARDUINO
void setup() {
	//Power on startup or reset
#endif

#ifdef FF_SIMULATOR
int main(void) {
#endif


	// Read the config file, parse it and create a block list in memory
	//XXX consider if Block0(SYSTEM) can be the state register?

#ifdef FF_SIM_PARSECONFIG
	ReadAndParseConfig();	//from INI config file
#else
	ReadProcessedConfig();
#endif

	// set up the runtime environment
	InitSystem();
	// run validate over eaach block
	ProcessDispatcher(Validate);
	DebugLog(SSS, E_INFO, M_DISP_VALIDATE);
	// write a binary config file (for consumption by ReadProcessedConfig() in embedded builds
	//WriteRunningConfig();
	//DebugLog("Created Binary Configuration File. Done.");

	if(1) { //0 or 1 to include for testing
		DebugLog(SSS, E_INFO, M_DISP_SETUP);
		ProcessDispatcher(Setup);

		DebugLog(SSS, E_INFO, M_DISP_OPER_1ST);
		ProcessDispatcher(Operate);
		DebugLog(SSS, E_INFO, M_DISP_OPER_LOOP);

		while (1) loop();
	}
	return 0;

}
