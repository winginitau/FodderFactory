/************************************************
 ff_inputs.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Input Processing
 ************************************************/


/************************************************
  Includes
************************************************/
#include <ff_sys_config.h>
#include "ff_inputs.h"
#include "ff_HAL.h"
#include "ff_string_consts.h"
#include "ff_events.h"
#include "ff_registry.h"
#include "ff_debug.h"

#ifdef FF_SIMULATOR
#endif

/************************************************
  Data Structures
************************************************/


/************************************************
  Globals
************************************************/



/************************************************
  Input Processing Functions
************************************************/

void InputSetup(BlockNode *b) {
//	DebugLog("Input Setup");
	switch (b->block_type) {
		case IN_ONEWIRE:
			float temp;
			b->last_update = TimeNow();
//			TempSensorsTakeReading();
//			Dump(b, "IN_ONEWIRE Input Setup");
			temp = GetTemperature(b->settings.in.if_num);
			// XXX temporary sanity checking until data quality schema
			// designed and implemented. For now to prevent silly data display
			// on the RPI controller. Downside - controller will always show
			// valid data even if it bollocks due to a real device failure
			if ( (temp > (float) -20) && temp < (float)80) {
				// reasonable value, update the block and report
				b->f_val = temp;
				EventMsg(b->block_id, E_DATA, M_F_READ, 0, b->f_val);
			} else {
				// Try again
				temp = GetTemperature(b->settings.in.if_num);
				if ( (temp > (float) -20) && temp < (float)80) {
					// reasonable value, update the block and report
					b->f_val = temp;
					EventMsg(b->block_id, E_DATA, M_F_READ, 0, b->f_val);
				} else {
					EventMsg(b->block_id, E_WARNING, M_BAD_TEMPERATURE_READ, 0, b->f_val);
				}
			}
			#ifdef IN_LOGRATE_OVERRIDE
			b->settings.in.log_rate = IN_LOGRATE_OVERRIDE;
			#endif
			break;
		case IN_DIGITAL:
			if (b->settings.in.interface == IF_DIG_PIN_IN) {
				HALInitDigitalInput(b->settings.in.if_num);
				b->last_update = TimeNow();
				b->bool_val = HALDigitalRead(b->settings.in.if_num);

			}
			break;
		default:
			break;
	}

}

void InputOperate(BlockNode *b) {
	time_t now = TimeNow();
	time_t next;

	switch (b->block_type) {
		case IN_ONEWIRE:
			float temp;
			next = b->last_update + b->settings.in.log_rate;
//			Dump(b, "IN_ONEWIRE Input Operate");
			if (now >= next) {
//				Dump(b, "now >=next IN_ONEWIRE Input Operate");
				b->last_update = now;
//				TempSensorsTakeReading();
//				DebugLog("Input Operate");
				temp = GetTemperature(b->settings.in.if_num);
				// XXX temporary sanity checking until data quality schema
				// designed and implemented. For now to prevent silly data display
				// on the RPI controller. Downside - controller will always show
				// valid data even if its bollocks due to a real device failure
				if ( (temp > (float) -20) && temp < (float)80) {
					// reasonable value, update the block and report
					b->f_val = temp;
					EventMsg(b->block_id, E_DATA, M_F_READ, 0, b->f_val);
				} else {
					// Try again
					temp = GetTemperature(b->settings.in.if_num);
					if ( (temp > (float) -20) && temp < (float)80) {
						// reasonable value, update the block and report
						b->f_val = temp;
						EventMsg(b->block_id, E_DATA, M_F_READ, 0, b->f_val);
					} else {
						EventMsg(b->block_id, E_WARNING, M_BAD_TEMPERATURE_READ, 0, b->f_val);
					}
				}
				//EventMsg(b->block_id, E_DATA, M_F_READ, 0, b->f_val);
			}
			break;
		case IN_DIGITAL: {
			//TODO remove log_rate test and allow asynchronous triggering
			// currently implemented with log_rate to allow random flip-flop
			// state to be generated each loop in HALDigitalRead
			next = b->last_update + b->settings.in.log_rate;
			if (now >= next) {
				uint8_t reading = HALDigitalRead(b->settings.in.if_num);
				if (reading != b->bool_val) {
					b->last_update = now;
					b->bool_val = reading;
					EventMsg(b->block_id, E_DATA, M_FLIPFLOP);
				}
			}
			break;
		}
		default:
			break;
	}
}












