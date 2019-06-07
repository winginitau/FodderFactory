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
			break;
	}

}

