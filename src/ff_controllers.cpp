/************************************************
 ff_inputs.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Controller Processing
 ************************************************/

/************************************************
 Includes
 ************************************************/
#include "ff_controllers.h"
#include "ff_sys_config.h"
#include "ff_string_consts.h"

#include "ff_registry.h"
#include "ff_events.h"
#include "ff_debug.h"

#include <time.h>
#include "ff_HAL.h"

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
			break;
		}

		case CON_SYSTEM: {
			break;
		}
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
					EventMsg(b->block_id, E_ACT);
					SetCommand(b->settings.con.output, b->settings.con.act_cmd);
					EventMsg(b->block_id, E_COMMAND, M_CMD_ACT);
				}
			} else {
				if (b->active == 1) {
					b->active = 0;
					b->last_update = TimeNow();
					EventMsg(b->block_id, E_DEACT);
					SetCommand(b->settings.con.output, b->settings.con.deact_cmd);
					EventMsg(b->block_id, E_COMMAND, M_CMD_DEACT);
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

