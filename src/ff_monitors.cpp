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
#include "ff_debug.h"

#ifdef FF_SIMULATOR
#include <stdio.h>
#endif

#include "ff_HAL.h"
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
			b->last_update = TimeNow();
			break;

		case MON_CONDITION_HIGH:
			b->active = 0;
			b->last_update = TimeNow();
			break;

		case MON_AVERAGE_CONDITION_LOW:
			b->active = 0;
			b->last_update = TimeNow();
			break;

		case MON_AVERAGE_CONDITION_HIGH:
			b->active = 0;
			b->last_update = TimeNow();
			break;

		case MON_TRIGGER:
			b->active = 0;
			b->last_update = TimeNow();
			break;

		default:
			break;
	}

}

void MonitorOperate(BlockNode *b) {

	uint8_t start_status = b->status;

	switch (b->block_type) {

		case MON_CONDITION_LOW: {
			float fval;
			fval = GetFVal(b->settings.mon.input1);
			if (fval == FLOAT_INIT) {
				b->status = STATUS_ENABLED_INVALID_DATA;
				// nothing more to do
			} else {
				b->status = STATUS_ENABLED_VALID_DATA;
				if (fval <= b->settings.mon.act_val) {
					if (b->active == 0) {
						b->active = 1;
						b->last_update = TimeNow();
						EventMsg(b->block_id, E_ACT);
					}
				}
				if (fval >= b->settings.mon.deact_val) {
					if (b->active == 1) {
						b->active = 0;
						b->last_update = TimeNow();
						EventMsg(b->block_id, E_DEACT);
					}
				}
			}
			break;
		}

		case MON_CONDITION_HIGH: {
			float fval;
			fval = GetFVal(b->settings.mon.input1);
			if (fval == FLOAT_INIT) {
				b->status = STATUS_ENABLED_INVALID_DATA;
				// nothing more to do
			} else {
				b->status = STATUS_ENABLED_VALID_DATA;
				if (GetFVal(b->settings.mon.input1) >= b->settings.mon.act_val) {
					if (b->active == 0) {
						b->active = 1;
						b->last_update = TimeNow();
						EventMsg(b->block_id, E_ACT);
					}
				}
				if (GetFVal(b->settings.mon.input1) <= b->settings.mon.deact_val) {
					if (b->active == 1) {
						b->active = 0;
						b->last_update = TimeNow();
						EventMsg(b->block_id, E_DEACT);
					}
				}
			}
			break;
		}
		case MON_AVERAGE_CONDITION_LOW: {
			float fval1, fval2;
			fval1 = GetFVal(b->settings.mon.input1);
			fval2 = GetFVal(b->settings.mon.input2);
			if (fval1 == FLOAT_INIT || fval2 == FLOAT_INIT) {
				b->status = STATUS_ENABLED_INVALID_DATA;
				// nothing more to do
			} else {
				b->status = STATUS_ENABLED_VALID_DATA;
				float ave;
				ave = (fval1 + fval2) / 2;
				if (ave <= b->settings.mon.act_val) {
					if (b->active == 0) {
						b->active = 1;
						b->last_update = TimeNow();
						EventMsg(b->block_id, E_ACT);
					}
				}
				if (ave >= b->settings.mon.deact_val) {
					if (b->active == 1) {
						b->active = 0;
						b->last_update = TimeNow();
						EventMsg(b->block_id, E_DEACT);
					}
				}
			}
			break;
		}
		case MON_AVERAGE_CONDITION_HIGH: {
			float fval1, fval2;
			fval1 = GetFVal(b->settings.mon.input1);
			fval2 = GetFVal(b->settings.mon.input2);
			if (fval1 == FLOAT_INIT || fval2 == FLOAT_INIT) {
				b->status = STATUS_ENABLED_INVALID_DATA;
				// nothing more to do
			} else {
				b->status = STATUS_ENABLED_VALID_DATA;
				float ave;
				ave = (fval1 + fval2) / 2;
				if (ave >= b->settings.mon.act_val) {
					if (b->active == 0) {
						b->active = 1;
						b->last_update = TimeNow();
						EventMsg(b->block_id, E_ACT);
					}
				}
				if (ave <= b->settings.mon.deact_val) {
					if (b->active == 1) {
						b->active = 0;
						b->last_update = TimeNow();
						EventMsg(b->block_id, E_DEACT);
					}
				}
			}
			break;
		}
		case MON_TRIGGER: {
			uint8_t bool_val;
			bool_val = GetBVal(b->settings.mon.input1);
			if (bool_val == UINT8_INIT) {
				b->status = STATUS_ENABLED_INVALID_DATA;
				// nothing more to do
			} else {
				b->status = STATUS_ENABLED_VALID_DATA;
				if (bool_val == (uint8_t)b->settings.mon.act_val) {
					if (b->active == 0) {
						b->active = 1;
						b->last_update = TimeNow();
						EventMsg(b->block_id, E_ACT);
					}
				}
				if (bool_val == (uint8_t)b->settings.mon.deact_val) {
					if (b->active == 1) {
						b->active = 0;
						b->last_update = TimeNow();
						EventMsg(b->block_id, E_DEACT);
					}
				}
			}
			break;
		}
		default:
			break;
	}
	if(b->status != start_status) {
		char debug_msg[MAX_LOG_LINE_LENGTH];
#ifdef USE_PROGMEM
		//XXX
		SimpleStringArray s_str;
		memcpy_P (&s_str, &status_strings[b->status], sizeof(s_str));
		sprintf(debug_msg, "[%s]Changed Status to: %s", b->block_label, s_str.text);
#else
		sprintf(debug_msg, "[%s]Changed Status to: %s", b->block_label, status_strings[b->status].text);
#endif
		DebugLog(debug_msg);
	}

}
