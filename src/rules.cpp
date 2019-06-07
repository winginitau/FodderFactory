/************************************************
 ff_schedules.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Rule Processing
************************************************/


/************************************************
  Includes
************************************************/
#include <build_config.h>
#include <datetime_ff.h>
#include <debug_ff.h>
#include <events.h>
#include <HAL.h>
#include <registry.h>
#include <schedules.h>
#include <string_consts.h>
#include <time.h>

#ifdef FF_SIMULATOR
//#include <stdio.h>
#endif

/************************************************
  Data Structures
************************************************/


/************************************************
  Globals
************************************************/


/************************************************
  Schedule Processing Functions
************************************************/

void RuleSetup(BlockNode *b) {

	switch (b->block_type) {

		case RL_LOGIC_ANDNOT: {
			b->active = 0;
			b->last_update = TimeNow();
			EventMsg(b->block_id, E_DEACT);
			b->status = STATUS_ENABLED_INIT;
			break;
		}

		case RL_LOGIC_SINGLE: {
			b->active = 0;
			b->last_update = TimeNow();
			EventMsg(b->block_id, E_DEACT);
			b->status = STATUS_ENABLED_INIT;
			break;
		}

		case RL_LOGIC_AND: {
			b->active = 0;
			b->last_update = TimeNow();
			EventMsg(b->block_id, E_DEACT);
			b->status = STATUS_ENABLED_INIT;
			break;
		}

		case RL_LOGIC_SINGLENOT: {
			b->active = 0;
			b->last_update = TimeNow();
			EventMsg(b->block_id, E_DEACT);
			b->status = STATUS_ENABLED_INIT;
			break;
		}
		default:
			break;

	}

}


void RuleOperate(BlockNode *b) {
	switch (b->block_type) {

		case RL_LOGIC_ANDNOT: {
			if (IsActive(b->settings.rl.param1) && IsActive(b->settings.rl.param2) && (IsActive(b->settings.rl.param_not) == 0)) {
				if (b->active == 0) {
					b->active = 1;
					b->last_update = TimeNow();
					EventMsg(b->block_id, E_ACT);
				}
			} else {
				if (b->active == 1) {
					b->active = 0;
					b->last_update = TimeNow();
					EventMsg(b->block_id, E_DEACT);
				}
			}
			break;
		}

		case RL_LOGIC_SINGLE: {
			if (IsActive(b->settings.rl.param1)) {
				if (b->active == 0) {
					b->active = 1;
					b->last_update = TimeNow();
					EventMsg(b->block_id, E_ACT);
				}
			} else {
				if (b->active == 1) {
					b->active = 0;
					b->last_update = TimeNow();
					EventMsg(b->block_id, E_DEACT);
				}
			}
			break;
		}

		case RL_LOGIC_AND: {
			if (IsActive(b->settings.rl.param1) && IsActive(b->settings.rl.param2)) {
				if (b->active == 0) {
					b->active = 1;
					b->last_update = TimeNow();
					EventMsg(b->block_id, E_ACT);
				}
			} else {
				if (b->active == 1) {
					b->active = 0;
					b->last_update = TimeNow();
					EventMsg(b->block_id, E_DEACT);
				}
			}
			break;
		}

		case RL_LOGIC_SINGLENOT: {
			if (IsActive(b->settings.rl.param1) && (IsActive(b->settings.rl.param_not) == 0)) {
				if (b->active == 0) {
					b->active = 1;
					b->last_update = TimeNow();
					EventMsg(b->block_id, E_ACT);
				}
			} else {
				if (b->active == 1) {
					b->active = 0;
					b->last_update = TimeNow();
					EventMsg(b->block_id, E_DEACT);
				}
			}
			break;
		}
		default:
			break;

	}

}




