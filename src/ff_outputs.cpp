/************************************************
 ff_outputs.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

  Output Processing
 ************************************************/

/************************************************
  Includes
************************************************/
#include "ff_outputs.h"
#include "ff_sys_config.h"
#include "ff_events.h"
#include "ff_registry.h"
#include "ff_string_consts.h"
#include "ff_HAL.h"

#ifdef FF_SIMULATOR
#include <string.h>
#include <stdio.h>
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


void OutputSetup(BlockNode *b) {

	switch (b->block_type) {

		case OUT_DIGITAL: {
			b->active = 0;
			b->last_update = time(NULL);
			b->settings.out.command = UINT8_INIT;
			HALInitDigitalOutput(b->settings.out.if_num);
			break;
		}
		case OUT_SYSTEM_CALL: {
			break;
		}
		default:
			;
	}


}

void OutputOperate(BlockNode *b) {

	switch (b->block_type) {

		case OUT_DIGITAL: {
			if (b->settings.out.command == CMD_OUTPUT_ON) {
				if (b->active == 0) {
					b->active = 1;
					b->last_update = time(NULL);
					EventMsg(b->block_id, E_ACT);
					HALDigitalWrite (b->settings.out.if_num, DIG_HIGH);
				}
			} else {
				if (b->active == 1) {
					b->active = 0;
					b->last_update = time(NULL);
					EventMsg(b->block_id, E_DEACT);
					HALDigitalWrite (b->settings.out.if_num, DIG_LOW);
				}
			}

			break;
		}
		case OUT_SYSTEM_CALL: {
			break;
		}
		default:
			break;
	}

}

