//============================================================================
// Name        : ff_arduino.cpp (and copied to ff_simulator.cpp
// Author      : Brendan McLearie
// Version     :
// Copyright   : Copyright Brendan McLearie
// Description :
//============================================================================

//XXX Architectural Matters for Resolution
//	Validation of fundamental concepts - "blocks"
//	Operational Data exchange
//		Push vs Pull or Both?
//		Text vs Binary
//		Placement of itch in above
//		Transport options - LORA, IP, Serial
//		Error correcting
//		Distributed message bussing - roll own? MQTT? Other?
//		Infosec wrappers / embedding
//	Overall Network Model
//		Janitor Core
//		Core Processing Nodes / Clusters
//		Spokes
//		Shadowing (all the way to top?)
//		How to handle HA
//		How to handle multipath redundancy
//	Provisioning and configuration model generally
//		Ref GLITCH grammar file notes and config bucket
//		Dumb start / disconnected start / online start
//		Cascading of configs
//		Reconfiguration of configs for HA and failure mode workarounds
//		Immediate management of Fodder config
//			Moving off the INI file and its binary form
//			Moving to the Cisco like version
//			Cisco model generally
//			+++ and switching between message protocol and itch (or is it the same)
//	User Tools
//		Design modelling -> Configuration management
//			Web app? Download win app? Mac app? iThing and Android apps?
//		Monitoring and management
//			Touch screen for fodder - or should it be an app? or both?
//			Genericising the user app - Design process also deploys an app?
//	Immediate needs (fodder and frustrations)
//		<done> current sensor dropout bug
//		Fodder assurance
//		Arduinos and RPIs
//			Serial interaction model
//		Getting control of the config
//		Remote tweaking and rule changes
//		Present data analysis and visualisation - for rule tuning
//		Collecting energy system data - physical interface done, need SW
//			To support decisions on heating and lighting run times
//		Priorities for which line of C code should be written next
//		In general - targets or free form prioritisation
//		If we need "project management" then we're not having fun anymore



#ifndef ARDUINO
#include <iostream>
using namespace std;
#endif

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
#include <ff_HAL.h>

#ifdef FF_CONFIG
#include <ff_ini_config.h>
#endif

#ifdef FF_SIM_PARSECONFIG
#include <ff_ini_config.h>
#endif


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
	// main() is equivalent to setup() on arduino


	//Key issue for large scalability is that the system executive (registry)
	// is presently stateful. Therefore it requires a start up / state
	// establishment phase before operation. Impractical with M or B # blocks
	// Status flag (and enum) is first attempt to move to stateless (for startup)
	// in that it provides the facility to ignore garbage data initially.

	//Moving the sr (State register) to a tolled persistence store is
	// currently seems the best way to provide a long-term stateful machine
	// though startup / init still neeeds to be handled for any blocks coming
	// online during run-time.

	// Init the Serial coms
	// TODO and sockets etc later
	// Has to be first step in setup so that config processing can debug to serial
	HALInitSerial(EVENT_SERIAL_PORT, EVENT_SERIAL_BAUDRATE);
#ifdef DEBUG
	DebugLog(F("HALInitSerial Passed"));
#endif

	// If using itch, init here because it proxies all comms on serial
	#ifdef USE_ITCH
	HALInitItch();
	#endif

	// Set up the run-time environment
	InitSystem();
	//TODO set up remote control / data feed / radio link

	// Read the config file, parse it and create a block list in memory
	#ifdef FF_SIM_PARSECONFIG
		// from INI source
		ReadAndParseConfig();	//from INI config file
	#else
	#ifdef FF_CONFIG
		ReadAndParseConfig();	//from INI config file
	#else
		// Read the binary configuration file previously created with
		// ff_config. Note, this build and the ff_config build must use exactly
		// the same pre-processor and compiler directives to ensure binary portability
		ReadProcessedConfig();
	#endif
	#endif

	// Run the Validate function on each block
	// currently uses assert() - which will bomb the run if failed on embedded
	ProcessDispatcher(Validate);
	// TODO implement assert()-like exception handling for embedded
	// while using assert, its safe to declare success (if not correctness) if we get this far
	DebugLog(SSS, E_INFO, M_DISP_VALIDATE);

	#ifdef FF_CONFIG
	//Write out a binary config file (for consumption by ReadProcessedConfig() in embedded builds
	WriteRunningConfig();
	DebugLog("Created Binary Configuration File. Done.");
	return 0;
	#endif

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


	#ifdef FF_SIMULATOR
	//for (int n = 0; n < 10000; n++) {
	//	loop();
	//}
	//printf("done\n");
	while (1) loop();
	return 0;
	#endif

	// On Arduino loop() is now called forever by the bootloader
}



