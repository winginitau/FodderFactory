/************************************************
 ff_outputs.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

  Output Processing
 ************************************************/

/************************************************
  Includes
************************************************/
#include <build_config.h>
#include <events.h>
#include <HAL.h>
#include <outputs.h>
#include <registry.h>
#include <string_consts.h>

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
			b->last_update = TimeNow();
			b->settings.out.command = UINT8_INIT;
			HALInitDigitalOutput(b->settings.out.if_num);
			EventMsg(b->block_id, E_DEACT);
			#ifdef DISABLE_OUTPUTS
				EventMsg(b->block_id, E_WARNING, M_OUTPUT_DISABLED);
			#else
				HALDigitalWrite (b->settings.out.if_num, DIG_LOW);
			#endif
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
					b->last_update = TimeNow();
					EventMsg(b->block_id, E_ACT);
					#ifdef DISABLE_OUTPUTS
						EventMsg(b->block_id, E_WARNING, M_OUTPUT_DISABLED);
					#else
						HALDigitalWrite (b->settings.out.if_num, DIG_HIGH);
					#endif
				}
			} else {
				if (b->active == 1) {
					b->active = 0;
					b->last_update = TimeNow();
					EventMsg(b->block_id, E_DEACT);
					#ifdef DISABLE_OUTPUTS
						EventMsg(b->block_id, E_WARNING, M_OUTPUT_DISABLED);
					#else
						HALDigitalWrite (b->settings.out.if_num, DIG_LOW);
					#endif
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

