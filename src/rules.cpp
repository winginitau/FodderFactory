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
#include <block_common.h>

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
			EventMsg(SSS, E_ERROR, M_UNKNOWN_BLOCK_TYPE);
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
			EventMsg(SSS, E_ERROR, M_UNKNOWN_BLOCK_TYPE);
			break;

	}

}

void RuleShow(BlockNode *b, void(Callback(char *))) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	char fmt_str[MAX_LABEL_LENGTH];
	const char* label_ptr;

	CommonShow(b, Callback);

	strcpy_hal(out_str, F("Rule:"));
	Callback(out_str);

	if (b->settings.rl.param1 == BLOCK_ID_INIT) {
		strcpy_hal(out_str, F(" param1:       BLOCK_ID_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" param1:       %s (%d)"));
		label_ptr = GetBlockLabelString(b->settings.rl.param1);
		sprintf(out_str, fmt_str, label_ptr, b->settings.rl.param1);
	}
	Callback(out_str);

	if (b->settings.rl.param2 == BLOCK_ID_INIT) {
		strcpy_hal(out_str, F(" param2:       BLOCK_ID_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" param2:       %s (%d)"));
		label_ptr = GetBlockLabelString(b->settings.rl.param2);
		sprintf(out_str, fmt_str, label_ptr, b->settings.rl.param2);
	}
	Callback(out_str);

	if (b->settings.rl.param3 == BLOCK_ID_INIT) {
		strcpy_hal(out_str, F(" param3:       BLOCK_ID_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" param3:       %s (%d)"));
		label_ptr = GetBlockLabelString(b->settings.rl.param3);
		sprintf(out_str, fmt_str, label_ptr, b->settings.rl.param3);
	}
	Callback(out_str);

	if (b->settings.rl.param_not == BLOCK_ID_INIT) {
		strcpy_hal(out_str, F(" param_not:       BLOCK_ID_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" param_not     %s (%d)"));
		label_ptr = GetBlockLabelString(b->settings.rl.param_not);
		sprintf(out_str, fmt_str, label_ptr, b->settings.rl.param_not);
	}
	Callback(out_str);
}


