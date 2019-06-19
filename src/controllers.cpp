/************************************************
 ff_inputs.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Controller Processing
 ************************************************/

/************************************************
 Includes
 ************************************************/
#include <build_config.h>
#include <controllers.h>
#include <debug_ff.h>
#include <events.h>
#include <HAL.h>
#include <registry.h>
#include <string_consts.h>
#include <time.h>
#include <block_common.h>

#ifdef FF_ARDUINO
#include <Arduino.h>
#endif


/************************************************
 Data Structures
************************************************/


/************************************************
 Globals
************************************************/


/************************************************
 Functions
************************************************/

void ControllerSetup(BlockNode *b) {

	switch (b->block_type) {
		case CON_ONOFF: {
			b->active = 0;
			b->last_update = TimeNow();
			b->status = STATUS_ENABLED_INIT;
		}
			break;

		case CON_SYSTEM: {
			b->status = STATUS_ENABLED_INIT;
		}
			break;
		default:
			EventMsg(SSS, E_ERROR, M_UNKNOWN_BLOCK_TYPE);
			break;
	}


}


void ControllerOperate(BlockNode *b) {

	switch (b->block_type) {

		case CON_ONOFF: {
			if (IsActive(b->settings.con.rule)) {
				if (b->active == 0) {
					b->active = 1;
					b->last_update = TimeNow();
					// XXX Resolve: using event messages to send commands or not?
					SetCommand(b->settings.con.output, b->settings.con.act_cmd);
					EventMsg(b->block_id, b->settings.con.output, E_COMMAND, M_CMD_ACT);
				}
			} else {
				if (b->active == 1) {
					b->active = 0;
					b->last_update = TimeNow();
					SetCommand(b->settings.con.output, b->settings.con.deact_cmd);
					EventMsg(b->block_id, b->settings.con.output, E_COMMAND, M_CMD_DEACT);
				}
			}
			break;
		}

		case CON_SYSTEM: {
			break;
		}
		default:
			EventMsg(SSS, E_ERROR, M_UNKNOWN_BLOCK_TYPE);
			break;
	}

}

void ControllerShow(BlockNode *b, void(Callback(char *))) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	char fmt_str[MAX_LABEL_LENGTH];
	char label_str[MAX_LABEL_LENGTH];
	const char* label_ptr;

	CommonShow(b, Callback);

	strcpy_hal(out_str, F("Controller:"));
	Callback(out_str);

	if (b->settings.con.rule == BLOCK_ID_INIT) {
		strcpy_hal(out_str, F(" rule:         BLOCK_ID_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" rule:         %s (%d)"));
		label_ptr = GetBlockLabelString(b->settings.con.rule);
		sprintf(out_str, fmt_str, label_ptr, b->settings.con.rule);
	}
	Callback(out_str);

	if (b->settings.con.output == BLOCK_ID_INIT) {
		strcpy_hal(out_str, F(" output:       BLOCK_ID_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" output:       %s (%d)"));
		label_ptr = GetBlockLabelString(b->settings.con.output);
		sprintf(out_str, fmt_str, label_ptr, b->settings.con.output);
	}
	Callback(out_str);

	strcpy_hal(fmt_str, F(" act_cmd:      %s (%d)"));
	strcpy_hal(label_str, command_strings[b->settings.con.act_cmd].text);
	sprintf(out_str, fmt_str, label_str, b->settings.con.act_cmd);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" deact_cmd:    %s (%d)"));
	strcpy_hal(label_str, command_strings[b->settings.con.deact_cmd].text);
	sprintf(out_str, fmt_str, label_str, b->settings.con.deact_cmd);
	Callback(out_str);
}
