/************************************************
 ff_schedules.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Rule Processing
************************************************/


/************************************************
  Includes
************************************************/
#include "ff_schedules.h"

#include "ff_sys_config.h"
//#include "ff_HAL.h"
#include "ff_string_consts.h"
#include "ff_events.h"
#include "ff_registry.h"
#include "ff_datetime.h"
#include "ff_debug.h"

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
			b->last_update = time(NULL);
			break;
		}

		case RL_LOGIC_SINGLE: {
			b->active = 0;
			b->last_update = time(NULL);
			break;
		}

		case RL_LOGIC_AND: {
			b->active = 0;
			b->last_update = time(NULL);
			break;
		}

		case RL_LOGIC_SINGLENOT: {
			b->active = 0;
			b->last_update = time(NULL);
			break;
		}
		default:
			break;

	}

}


void RuleOperate(BlockNode *b) {
	switch (b->block_type) {

		case RL_LOGIC_ANDNOT: {
			b->active = 0;
			b->last_update = time(NULL);
			break;
		}

		case RL_LOGIC_SINGLE: {
			b->active = 0;
			b->last_update = time(NULL);
			break;
		}

		case RL_LOGIC_AND: {
			b->active = 0;
			b->last_update = time(NULL);
			break;
		}

		case RL_LOGIC_SINGLENOT: {
			b->active = 0;
			b->last_update = time(NULL);
			break;
		}
		default:
			break;

	}

}




