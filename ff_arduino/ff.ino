//============================================================================
// Name        : ff_arduino.cpp
// Author      : Brendan McLearie
// Version     :
// Copyright   : Copyright Brendan McLearie
// Description :
//============================================================================


/************************************************
 Includes
 ************************************************/
#include <ff_sys_config.h>
#include <ff_string_consts.h>
#include <ff_main.h>

//#include "src/ff_utils.h"
//#include <ff_inputs.h>
//#include <ff_outputs.h>
//#include <ff_controllers.h>
//#include <ff_display.h>

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
	//TODO - remove dead and/or requested blocks from the run-time in
	// a larger continuously running system
}



#ifdef FF_ARDUINO
void setup() {
	//Power on startup or reset
#endif

#ifdef FF_SIMULATOR
int main(void) {
#endif


	//Key issue for large scalability is that the system executive (registry)
	// is presently stateful. Therefore it requires a start up / state
	// establishment phase before operation. Impractical with M or B # blocks
	// Status flag (and enum) is first attempt to move to stateless (for startup)
	// in that it provides the facility to ignore garbage data initially.

	//Moving the sr (State register) to a tolled persistence store is
	// currently seems the best way to provide a long-term stateful machine
	// though startup / init still neeeds to be handled for any blocks coming
	// online during run-time.

	// Read the binary configuration file previously created with
	// ff_config. Note, this build and the ff_config build must use exactly
	// the same pre-processor and compiler directives to ensure binary portability
	ReadProcessedConfig();

	// Set up the run-time environment
	InitSystem();
	//TODO set up remote control / data feed / radio link

	// run the Validate function on each block
	// currently uses assert() - which will bomb the run if failed on embedded
	ProcessDispatcher(Validate);
	// TODO implement assert()-like exception handling for embedded

	// while using assert, its safe to declare success (if not correctness) if we get this far
	DebugLog(SSS, E_INFO, M_DISP_VALIDATE);

	// 1. make sure each block is in an appropriate state to start execution
	//		(if not already set during block registration and configuration
	// 2. trigger callbacks to any hardware that requires an initialisation process
	// 		particularly ins and outs. Eg a ONEWIRE bus
	DebugLog(SSS, E_INFO, M_DISP_SETUP);
	ProcessDispatcher(Setup);

	// TODO would be nice to re-run validate with a post setup modality
	//ProcessDispatcher(Validate);

	// perform a single pass through the block list in operate mode
	// before handing off to a processing loop or (TODO) interrupt
	// handler. Provides opportunity for further validation during
	// development and (at scale) a de-activation filtering of blocks
	// that may cause problems in continuous run.
	DebugLog(SSS, E_INFO, M_DISP_OPER_1ST);
	ProcessDispatcher(Operate);
	DebugLog(SSS, E_INFO, M_DISP_OPER_LOOP);
	while (1) ProcessDispatcher(Operate);

	#ifdef FF_SIMULATOR
	//for (int n = 0; n < 10000; n++) {
	//	loop();
	//}
	//printf("done\n");
	while (1) loop();
	return 0;
	#endif

}

