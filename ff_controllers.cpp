/************************************************
 ff_inputs.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Controller Processing
 ************************************************/

/************************************************
 Includes
 ************************************************/


#include "ff_sys_config.h"

#include "ff_controllers.h"
//#include "ff_utils.h"
#include "ff_datetime.h"
#include "ff_inputs.h"
#include "ff_string_consts.h"
#include "ff_outputs.h"
#include "ff_registry.h"
#include "ff_events.h"

#ifdef FF_ARDUINO
#include <Arduino.h>
#endif

#ifdef FF_SIMULATOR
#include <string.h>
#endif

/************************************************
 Data Structures
 ************************************************/
typedef struct CONTROL_BLOCK {
	uint8_t control_no;
	char label[MAX_LABEL_LENGTH];
	uint8_t type;
	//char descr[MAX_DESCR_LENGTH];
	char input[MAX_LABEL_LENGTH];
	char output[MAX_LABEL_LENGTH];
	uint8_t act_command;
	uint8_t deact_command;
	uint8_t currently_active;
	float low_val;
	float high_val;
	uint8_t sched_day;
	uint8_t sched_hour;
	uint8_t sched__minute;
	uint8_t sched_second;
	uint8_t sched_done_today;
	FFDateTime last_action;
} ControlBlock;

/************************************************
 Globals
 ************************************************/

ControlBlock controls[CONTROL_COUNT];

/************************************************
 Input Processing Functions
 ************************************************/

void ProcessController(int i) {

	switch (controls[i].type) {

	case SCHED_DAILY_ON:
		//no input just a scheduled time

		if (controls[i].sched_done_today == 0) {   //ie it hasn't yet been done
			FFDateTime now, sched;
			now = FFDTNow();
			sched.year = now.year;
			sched.month = now.month;
			sched.day = now.day; 	//daily controller - do it today
			sched.hour = controls[i].sched_hour;
			sched.minute = controls[i].sched__minute;
			sched.second = controls[i].sched_second;

			if (FFDTGrEq(now, sched)) {
				SetOutputCommand(controls[i].output, OUTPUT_ON);
				controls[i].sched_done_today = 1;
				controls[i].last_action = now;
				EventMsg(i + GetBlockTypeOffset(FF_CONTROLLER), SCHED_DAILY_ON, M_CONTROL_ACTIVATED, 0,
						0);
			}
		}
		break;
	case SCHED_DAILY_OFF:
		//no input just a scheduled time

		if (controls[i].sched_done_today == 0) {   //ie it hasn't yet been done
			FFDateTime now, sched;
			now = FFDTNow();
			sched.year = now.year;
			sched.month = now.month;
			sched.day = now.day; 	//daily controller - do it today
			sched.hour = controls[i].sched_hour;
			sched.minute = controls[i].sched__minute;
			sched.second = controls[i].sched_second;

			if (FFDTGrEq(now, sched)) {
				SetOutputCommand(controls[i].output, OUTPUT_OFF);
				controls[i].sched_done_today = 1;
				controls[i].last_action = now;
				EventMsg(i + GetBlockTypeOffset(FF_CONTROLLER), SCHED_DAILY_OFF, M_CONTROL_ACTIVATED,
						0, 0);
			}
		}
		break;
	case CONDITION_HIGH:
		// if not on then act, if already on do nothing, if on and now low deact

		if (InputCurrentFVal(controls[i].input) >= controls[i].high_val) {
			if (controls[i].currently_active == 0) {   //ie not active
				SetOutputCommand(controls[i].output, controls[i].act_command);
				controls[i].currently_active = 1;
				controls[i].last_action = FFDTNow();
				EventMsg(i + GetBlockTypeOffset(FF_CONTROLLER), CONDITION_HIGH, M_CONTROL_ACTIVATED, 0,
						0);
			}
		} else { //not exceed - turn off if already on
			if (controls[i].currently_active == 1) {   //ie is active
				SetOutputCommand(controls[i].output, controls[i].deact_command);
				controls[i].currently_active = 0;
				controls[i].last_action = FFDTNow();
				EventMsg(i + GetBlockTypeOffset(FF_CONTROLLER), CONDITION_HIGH, M_CONTROL_DEACT,
						0, 0);
			}
		}
		break;
	case CONDITION_LOW:

		if (InputCurrentFVal(controls[i].input) <= controls[i].low_val) {
			if (controls[i].currently_active == 0) {   //ie not active
				SetOutputCommand(controls[i].output, controls[i].act_command);
				controls[i].currently_active = 1;
				controls[i].last_action = FFDTNow();
				EventMsg(i + GetBlockTypeOffset(FF_CONTROLLER), CONDITION_LOW, M_CONTROL_ACTIVATED, 0,
						0);
			}
		} else { //not exceed - turn off if already on
			if (controls[i].currently_active == 1) {   //ie is active
				SetOutputCommand(controls[i].output, controls[i].deact_command);
				controls[i].currently_active = 0;
				controls[i].last_action = FFDTNow();
				EventMsg(i + GetBlockTypeOffset(FF_CONTROLLER), CONDITION_LOW, M_CONTROL_DEACT,
						0, 0);
			}
		}
		break;
	case TRIGGER_ON:
		//TODO
		break;
	case TRIGGER_OFF:
		//TODO
		break;
	case SYSTEM_MSG:
		if (controls[i].sched_done_today == 0) {   //ie it hasn't yet been done
			FFDateTime now, sched;
			now = FFDTNow();
			sched.year = now.year;
			sched.month = now.month;
			sched.day = now.day; 	//daily controller - do it today
			sched.hour = controls[i].sched_hour;
			sched.minute = controls[i].sched__minute;
			sched.second = controls[i].sched_second;

			if (FFDTGrEq(now, sched)) {
				//TODO currently done by a Log message and picked up as a special device "SYSTEM"
				controls[i].sched_done_today = 1;
				controls[i].last_action = now;
				EventMsg(i + GetBlockTypeOffset(FF_CONTROLLER), RESET_MIN_MAX, M_MIN_MAX_CLEAR,	0, 0);
			}
		}
		break;
	case CLEAR_DONE:
		if (controls[i].sched_done_today == 0) {   //ie it hasn't yet been done
			FFDateTime now, sched;
			now = FFDTNow();
			sched.year = now.year;
			sched.month = now.month;
			sched.day = now.day; 	//daily controller - do it today
			sched.hour = controls[i].sched_hour;
			sched.minute = controls[i].sched__minute;
			sched.second = controls[i].sched_second;

			if (FFDTGrEq(now, sched)) {
				for (int d = 0; d < CONTROL_COUNT; d++) {
					controls[d].sched_done_today = 0;
				}
				controls[i].sched_done_today = 1;
				controls[i].last_action = now;
				EventMsg(i + GetBlockTypeOffset(FF_CONTROLLER), CLEAR_DONE,	M_MIDNIGHT_DONE, 0, 0);
			}
		}
		break;
	default:
		// if nothing else matches, do the default
		// default is optional
		break;
	}
}

void SetupControllers(void) {
	int i = 0;
	//TODO temp until reading loop done
	//TODO - read a config file once we have SD memory

	controls[i].control_no = C1_CONTROL_NO;
	strcpy(controls[i].label, C1_LABEL);
	controls[i].type = C1_TYPE;
	//strcpy(controls[i].descr, C1_DESCR);
	strcpy(controls[i].input, C1_INPUT);
	strcpy(controls[i].output, C1_OUTPUT);
	controls[i].act_command = C1_ACT_COMMAND;
	controls[i].deact_command = C1_DEACT_COMMAND;
	controls[i].low_val = C1_LOW_VAL;
	controls[i].high_val = C1_HIGH_VAL;
	controls[i].sched_day = C1_SCHED_DAY;
	controls[i].sched_hour = C1_SCHED_HOUR;
	controls[i].sched__minute = C1_SCHED_MINUTE;
	controls[i].sched_second = C1_SCHED_SECOND;
	controls[i].sched_done_today = 0;
	controls[i].currently_active = 0;

	SetBlockLabelString(FF_CONTROLLER, i, controls[i].label);

	i = 1; 	//TODO temp until reading loop done

	controls[i].control_no = C2_CONTROL_NO;
	strcpy(controls[i].label, C2_LABEL);
	controls[i].type = C2_TYPE;
	//strcpy(controls[i].descr, C2_DESCR);
	strcpy(controls[i].input, C2_INPUT);
	strcpy(controls[i].output, C2_OUTPUT);
	controls[i].act_command = C2_ACT_COMMAND;
	controls[i].deact_command = C2_DEACT_COMMAND;
	controls[i].low_val = C2_LOW_VAL;
	controls[i].high_val = C2_HIGH_VAL;
	controls[i].sched_day = C2_SCHED_DAY;
	controls[i].sched_hour = C2_SCHED_HOUR;
	controls[i].sched__minute = C2_SCHED_MINUTE;
	controls[i].sched_second = C2_SCHED_SECOND;
	controls[i].sched_done_today = 0;
	controls[i].currently_active = 0;

	SetBlockLabelString(FF_CONTROLLER, i, controls[i].label);

	i = 2; 	//TODO temp until reading loop done

	controls[i].control_no = C3_CONTROL_NO;
	strcpy(controls[i].label, C3_LABEL);
	controls[i].type = C3_TYPE;
	//strcpy(controls[i].descr, C3_DESCR);
	strcpy(controls[i].input, C3_INPUT);
	strcpy(controls[i].output, C3_OUTPUT);
	controls[i].act_command = C3_ACT_COMMAND;
	controls[i].deact_command = C3_DEACT_COMMAND;
	controls[i].low_val = C3_LOW_VAL;
	controls[i].high_val = C3_HIGH_VAL;
	controls[i].sched_day = C3_SCHED_DAY;
	controls[i].sched_hour = C3_SCHED_HOUR;
	controls[i].sched__minute = C3_SCHED_MINUTE;
	controls[i].sched_second = C3_SCHED_SECOND;
	controls[i].sched_done_today = 0;
	controls[i].currently_active = 0;

	SetBlockLabelString(FF_CONTROLLER, i, controls[i].label);

	i = 3; 	//TODO temp until reading loop done

	controls[i].control_no = C4_CONTROL_NO;
	strcpy(controls[i].label, C4_LABEL);
	controls[i].type = C4_TYPE;
	//strcpy(controls[i].descr, C4_DESCR);
	strcpy(controls[i].input, C4_INPUT);
	strcpy(controls[i].output, C4_OUTPUT);
	controls[i].act_command = C4_ACT_COMMAND;
	controls[i].deact_command = C4_DEACT_COMMAND;
	controls[i].low_val = C4_LOW_VAL;
	controls[i].high_val = C4_HIGH_VAL;
	controls[i].sched_day = C4_SCHED_DAY;
	controls[i].sched_hour = C4_SCHED_HOUR;
	controls[i].sched__minute = C4_SCHED_MINUTE;
	controls[i].sched_second = C4_SCHED_SECOND;
	controls[i].sched_done_today = 0;
	controls[i].currently_active = 0;

	SetBlockLabelString(FF_CONTROLLER, i, controls[i].label);

	i = 4; 	//TODO temp until reading loop done

	controls[i].control_no = C5_CONTROL_NO;
	strcpy(controls[i].label, C5_LABEL);
	controls[i].type = C5_TYPE;
	//strcpy(controls[i].descr, C5_DESCR);
	strcpy(controls[i].input, C5_INPUT);
	strcpy(controls[i].output, C5_OUTPUT);
	controls[i].act_command = C5_ACT_COMMAND;
	controls[i].deact_command = C5_DEACT_COMMAND;
	controls[i].low_val = C5_LOW_VAL;
	controls[i].high_val = C5_HIGH_VAL;
	controls[i].sched_day = C5_SCHED_DAY;
	controls[i].sched_hour = C5_SCHED_HOUR;
	controls[i].sched__minute = C5_SCHED_MINUTE;
	controls[i].sched_second = C5_SCHED_SECOND;
	controls[i].sched_done_today = 0;
	controls[i].currently_active = 0;

	SetBlockLabelString(FF_CONTROLLER, i, controls[i].label);

	i = 5; 	//TODO temp until reading loop done

	controls[i].control_no = C6_CONTROL_NO;
	strcpy(controls[i].label, C6_LABEL);
	controls[i].type = C6_TYPE;
	//strcpy(controls[i].descr, C6_DESCR);
	strcpy(controls[i].input, C6_INPUT);
	strcpy(controls[i].output, C6_OUTPUT);
	controls[i].act_command = C6_ACT_COMMAND;
	controls[i].deact_command = C6_DEACT_COMMAND;
	controls[i].low_val = C6_LOW_VAL;
	controls[i].high_val = C6_HIGH_VAL;
	controls[i].sched_day = C6_SCHED_DAY;
	controls[i].sched_hour = C6_SCHED_HOUR;
	controls[i].sched__minute = C6_SCHED_MINUTE;
	controls[i].sched_second = C6_SCHED_SECOND;
	controls[i].sched_done_today = 0;
	controls[i].currently_active = 0;

	SetBlockLabelString(FF_CONTROLLER, i, controls[i].label);

	i = 6; 	//TODO temp until reading loop done

	controls[i].control_no = C7_CONTROL_NO;
	strcpy(controls[i].label, C7_LABEL);
	controls[i].type = C7_TYPE;
	//strcpy(controls[i].descr, C7_DESCR);
	strcpy(controls[i].input, C7_INPUT);
	strcpy(controls[i].output, C7_OUTPUT);
	controls[i].act_command = C7_ACT_COMMAND;
	controls[i].deact_command = C7_DEACT_COMMAND;
	controls[i].low_val = C7_LOW_VAL;
	controls[i].high_val = C7_HIGH_VAL;
	controls[i].sched_day = C7_SCHED_DAY;
	controls[i].sched_hour = C7_SCHED_HOUR;
	controls[i].sched__minute = C7_SCHED_MINUTE;
	controls[i].sched_second = C7_SCHED_SECOND;
	controls[i].sched_done_today = 0;
	controls[i].currently_active = 0;

	SetBlockLabelString(FF_CONTROLLER, i, controls[i].label);

	i = 7; 	//TODO temp until reading loop done

	controls[i].control_no = C8_CONTROL_NO;
	strcpy(controls[i].label, C8_LABEL);
	controls[i].type = C8_TYPE;
	//strcpy(controls[i].descr, C8_DESCR);
	strcpy(controls[i].input, C8_INPUT);
	strcpy(controls[i].output, C8_OUTPUT);
	controls[i].act_command = C8_ACT_COMMAND;
	controls[i].deact_command = C8_DEACT_COMMAND;
	controls[i].low_val = C8_LOW_VAL;
	controls[i].high_val = C8_HIGH_VAL;
	controls[i].sched_day = C8_SCHED_DAY;
	controls[i].sched_hour = C8_SCHED_HOUR;
	controls[i].sched__minute = C8_SCHED_MINUTE;
	controls[i].sched_second = C8_SCHED_SECOND;
	controls[i].sched_done_today = 0;
	controls[i].currently_active = 0;

	SetBlockLabelString(FF_CONTROLLER, i, controls[i].label);

	EventMsg(SSS, INFO, M_SETTING_CONTROLLERS, 0, 0);
	for (i = 0; i < CONTROL_COUNT; i++) {
		ProcessController(i);
	}
	EventMsg(SSS, INFO, M_CONTROLLERS_INIT, 0, 0);
}

void RunControllers(void) {
	for (int i = 0; i < CONTROL_COUNT; i++) {
		ProcessController(i);
	}
}

