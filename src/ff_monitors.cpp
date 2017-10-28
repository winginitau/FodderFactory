/************************************************
 ff_monitors.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

  Monitor Block Routines
 ************************************************/

/************************************************
  Includes
************************************************/

#include "ff_monitors.h"
#include "ff_sys_config.h"
#include "ff_string_consts.h"
#include "ff_registry.h"
#include "ff_events.h"
#include "ff_monitors.h"

/************************************************
  Data Structures
************************************************/


/************************************************
  Globals
************************************************/


/************************************************
  Functions
************************************************/

void MonitorSetup(BlockNode *b) {

	switch (b->block_type) {
		case MON_CONDITION_LOW:
			b->active = 0;

			break;
		case MON_CONDITION_HIGH:
			b->active = 0;
			break;

		case MON_AVERAGE_CONDITION_LOW:
			b->active = 0;
			break;

		case MON_AVERAGE_CONDITION_HIGH:
			b->active = 0;
			break;

		case MON_TRIGGER:
			b->active = 0;
			break;

		default:
			break;
	}

}

void MonitorOperate(BlockNode *b) {
	switch (b->block_type) {
		case MON_CONDITION_LOW:
			if (GetFVal(b->settings.mon.input1) <= b->settings.mon.act_val) {
				if (b->active == 0) {
					b->active = 1;
					b->last_update = time(NULL);
					EventMsg(b->block_id, E_ACT);
				}
			}
			if (GetFVal(b->settings.mon.input1) >= b->settings.mon.deact_val) {
				if (b->active == 1) {
					b->active = 0;
					b->last_update = time(NULL);
					EventMsg(b->block_id, E_DEACT);
				}
			}
			break;
		case MON_CONDITION_HIGH:
			break;
		case MON_AVERAGE_CONDITION_LOW:
			break;
		case MON_AVERAGE_CONDITION_HIGH:
			break;
		case MON_TRIGGER:
			break;
		default:
			break;
	}

}
