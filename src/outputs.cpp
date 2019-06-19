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
#include <block_common.h>

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
			b->status = STATUS_ENABLED_INIT;
			break;
		}
		case OUT_SYSTEM_CALL: {
			b->status = STATUS_ENABLED_INIT;
			break;
		}
		default:
			EventMsg(SSS, E_ERROR, M_UNKNOWN_BLOCK_TYPE);
			break;
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
			EventMsg(SSS, E_ERROR, M_UNKNOWN_BLOCK_TYPE);
			break;
	}

}

void OutputShow(BlockNode *b, void(Callback(char *))) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	char fmt_str[MAX_LABEL_LENGTH];
	char label_str[MAX_LABEL_LENGTH];

	CommonShow(b, Callback);

	strcpy_hal(out_str, F("Output:"));
	Callback(out_str);

	strcpy_hal(fmt_str, F(" interface:    %s (%d)"));
	strcpy_hal(label_str, interface_strings[b->settings.out.interface].text);
	sprintf(out_str, fmt_str, label_str, b->settings.out.interface);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" if_num:       %d"));
	sprintf(out_str, fmt_str, b->settings.out.if_num);
	Callback(out_str);

	if (b->settings.out.command == CMD_INIT) {
		strcpy_hal(out_str, F(" command:      CMD_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" command:      %s (%d)"));
		strcpy_hal(label_str, command_strings[b->settings.out.command].text);
		sprintf(out_str, fmt_str, label_str, b->settings.out.command);
	}
	Callback(out_str);
}

