/************************************************
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com
 ************************************************/

//VERSION - SEE "ff_sys_config.h"



/************************************************
 Includes
 ************************************************/
#include "src/ff_sys_config.h"

#ifdef FF_ARDUINO
#include <Arduino.h>
#endif

//#include "src/ff_utils.h"
#include "src/ff_inputs.h"
#include "src/ff_outputs.h"
#include "src/ff_controllers.h"
#include "src/ff_display.h"

#include "src/ff_main.h"


/************************************************
 Main Functions
 ************************************************/

#ifdef FF_ARDUINO
void setup() {
	//Power on startup or reset
#endif

#ifdef FF_SIMULATOR
int main(void) {
#endif

	InitSystem();
	//TODO 					// set up remote control / data feed / radio link

	SetupInputs();			// sensors, triggers, external data feeds (eg from battery mon)
	SetupOutputs();			// relays and data connections to turn devices on/off and talk to external systems
	SetupControllers();		// rules and logic - when and why

	#ifdef FF_SIMULATOR
	while (1) loop;
	return 0;
	#endif

}

void loop() {
	PollInputs(); 			// based on their config they may need to take a measurement and write it to a log
	RunControllers(); 		// does something need to happen? Store in control register, set a command register
	PollOutputs();     		// check command registers and change outputs accordingly
	UpdateUI();             // Check for User Input and update screens etc
	//TODO					// update radio / feed
	//SaveEventBuffer();		// write recent event to file - move to being a system scheduler and controller
	//delay(1000); //for dev and debug
}