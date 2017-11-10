//============================================================================
// Name        : ff_config.cpp
// Author      : Brendan McLearie
// Version     :
// Copyright   : Copyright Brendan McLearie
// Description :
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



	InitSystem();
	ProcessDispatcher(Validate);
	WriteRunningConfig();

	//XXX THIS BUILD - Write Config Call
	//XXX move most of "main" to new config library

	// while using assert, its safe to declare success (if not correctness) if we get this far
	DebugLog(SSS, E_INFO, M_DISP_VALIDATE);

//	DebugLog(SSS, E_INFO, M_DISP_SETUP);
//	ProcessDispatcher(Setup);

//	DebugLog(SSS, E_INFO, M_DISP_OPER_1ST);
//	ProcessDispatcher(Operate);
//	DebugLog(SSS, E_INFO, M_DISP_OPER_LOOP);



	return 0;

}

