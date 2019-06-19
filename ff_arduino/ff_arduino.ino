//============================================================================
// Name        : ff_arduino.cpp (and copied to ff_simulator.cpp
// Author      : Brendan McLearie
// Version     :
// Copyright   : Copyright Brendan McLearie
// Description :
//============================================================================

// TODO Architectural Matters for Resolution
//	Operational Data exchange
//		Push vs Pull or Both?
//		Text vs Binary
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
//		Add in non-terminal config mode
//		Move to Binary protocol
//	User Tools
//		Design modelling -> Configuration management
//			Web app? Download win app? Mac app? iThing and Android apps?
//		Monitoring and management
//			Touch screen for fodder - or should it be an app? or both?
//			Genericising the user app - Design process also deploys an app?
//	Immediate needs (fodder and frustrations)
//		Arduinos and RPIs
//			<WIP> Serial interaction model
//		<WIP> Getting control of the config
//		Move Victron VE to a library and incorporate as an input blocl
//		Collecting energy system data - physical interface done, need SW



/************************************************
 Includes
 ************************************************/
#include <build_config.h>
#include <string_consts.h>
#include <validate.h>
#include <registry.h>
#include <debug_ff.h>
#include <stdio.h>
#include <HAL.h>
#include <init_config.h>

/************************************************
 Main Functions
 ************************************************/

void loop() {
	// Operate each block forever
	ProcessDispatcher(Operate);
	//TODO - remove dead and/or requested blocks from the run-time in
	// a larger continuously running system
	#ifdef USE_ITCH
		HALPollItch();
	#endif
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
	// though startup / init still needs to be handled for any blocks coming
	// online during run-time.
	// 2019-06 - mostly addressed with status levels and conditional code.

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

	// Set up the basic run-time environment - state vector, event buffer, rtc, etc
	InitSystem();
	// TODO Consider moving to setup function on the system block

	//Load config from parsable text file replaces loading from binary.
	DebugLog("Calling InitConfigLoad");
	InitConfigLoad(1);

	// Run the Validate function on each block
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

	// TODO would re-run validate with a post setup modality
	// Checking for valid array index values
	//ProcessDispatcher(Validate);

	// perform a single pass through the block list in operate mode
	// before handing off to a processing loop or. This provides a
	// Convenient stop point for development.
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
	// On Arduino loop() is now called automatically forever
}



