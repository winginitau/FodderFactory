/************************************************
 ff_monitors.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

  Monitor Block Routines
 ************************************************/

/************************************************
  Includes
************************************************/

#include <build_config.h>
#include <debug_ff.h>
#include <events.h>
#include <monitors.h>
#include <monitors.h>
#include <registry.h>
#include <string_consts.h>
#include <block_common.h>
#include <utils.h>

#ifdef PLATFORM_LINUX
#include <stdio.h>
#endif

#include <HAL.h>
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
			b->status = STATUS_ENABLED_INIT;
			break;

		case MON_CONDITION_HIGH:
			b->active = 0;
			b->last_update = TimeNow();
			b->status = STATUS_ENABLED_INIT;
			break;

		case MON_AVERAGE_CONDITION_LOW:
			b->active = 0;
			b->last_update = TimeNow();
			b->status = STATUS_ENABLED_INIT;
			break;

		case MON_AVERAGE_CONDITION_HIGH:
			b->active = 0;
			b->last_update = TimeNow();
			b->status = STATUS_ENABLED_INIT;
			break;

		case MON_TRIGGER:
			b->active = 0;
			b->last_update = TimeNow();
			b->status = STATUS_ENABLED_INIT;
			break;

		default:
			EventMsg(SSS, E_ERROR, M_UNKNOWN_BLOCK_TYPE);
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
			EventMsg(SSS, E_ERROR, M_UNKNOWN_BLOCK_TYPE);
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

void MonitorShow(BlockNode *b, void(Callback(const char *))) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	char fmt_str[MAX_LABEL_LENGTH];
	char label_str[MAX_LABEL_LENGTH];
	const char* label_ptr;

	CommonShow(b, Callback);

	strcpy_hal(out_str, F("Monitor:"));
	Callback(out_str);

	if (b->settings.mon.input1 == BLOCK_ID_INIT) {
		strcpy_hal(out_str, F(" input1:       BLOCK_ID_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" input1:       %s (%d)"));
		label_ptr = GetBlockLabelString(b->settings.mon.input1);
		sprintf(out_str, fmt_str, label_ptr, b->settings.mon.input1);
	}
	Callback(out_str);

	if (b->settings.mon.input2 == BLOCK_ID_INIT) {
		strcpy_hal(out_str, F(" input2:       BLOCK_ID_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" input2:       %s (%d)"));
		label_ptr = GetBlockLabelString(b->settings.mon.input2);
		sprintf(out_str, fmt_str, label_ptr, b->settings.mon.input2);
	}
	Callback(out_str);

	if (b->settings.mon.input3 == BLOCK_ID_INIT) {
		strcpy_hal(out_str, F(" input3:       BLOCK_ID_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" input3:       %s (%d)"));
		label_ptr = GetBlockLabelString(b->settings.mon.input3);
		sprintf(out_str, fmt_str, label_ptr, b->settings.mon.input3);
	}
	Callback(out_str);

	if (b->settings.mon.input4 == BLOCK_ID_INIT) {
		strcpy_hal(out_str, F(" input4:       BLOCK_ID_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" input4:       %s (%d)"));
		label_ptr = GetBlockLabelString(b->settings.mon.input4);
		sprintf(out_str, fmt_str, label_ptr, b->settings.mon.input4);
	}
	Callback(out_str);

	FFFloatToCString(label_str, b->settings.mon.act_val);
	strcpy_hal(fmt_str, F(" act_val:      %s"));
	sprintf(out_str, fmt_str, label_str);   //float
	Callback(out_str);

	FFFloatToCString(label_str, b->settings.mon.deact_val);
	strcpy_hal(fmt_str, F(" deact_val:    %s"));
	sprintf(out_str, fmt_str, label_str);  //float
	Callback(out_str);

}
