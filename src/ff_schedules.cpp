/************************************************
 ff_schedules.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Schedule Processing
 ************************************************/


/************************************************
  Includes
************************************************/
#include "ff_schedules.h"

#include "ff_sys_config.h"
#include "ff_HAL.h"
#include "ff_string_consts.h"
#include "ff_events.h"
#include "ff_registry.h"
#include "ff_datetime.h"

#include <time.h>

#ifdef FF_SIMULATOR
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

void ScheduleSetup(BlockNode *b) {

	switch (b->block_type) {
		case SCH_START_STOP:
			b->active = 0;
			b->last_update = time(NULL);
			break;

		case SCH_ONE_SHOT:
			b->active = 0;
			b->last_update = time(NULL);
			break;

		case SCH_START_DURATION_REPEAT:
			b->active = 0;
			b->last_update = time(NULL);
			break;

		default:
			break;
	}

}

void ScheduleOperate(BlockNode *b) {

	tm* time_tm_now;
	tm* time_tm_temp;
	//TV_TYPE tv;
	time_t start_time, end_time;
	time_t time_at_zero_today;
	time_t now = time(NULL);

	time_tm_temp = localtime(&now);
	time_tm_now = localtime(&now); 		// 2x calls to localtime needed so that temp can change

	time_tm_temp->tm_hour = 0;
	time_tm_temp->tm_min = 0;
	time_tm_temp->tm_sec = 0;
	time_at_zero_today = mktime(time_tm_temp);

	switch (b->block_type) {
		case SCH_START_STOP: {
			//if we are not already active, we need to test for start time
			if (b->active == 0) {
				// does the schedule start apply to today (or yesterday)?
				//XXX
				if (b->settings.sch.days[time_tm_now->tm_wday+1]) {
					// yes it does, what time does it start?
					start_time = time_at_zero_today + b->settings.sch.time_start;
					// what time does it end?
					end_time = time_at_zero_today + b->settings.sch.time_end;
					// is the end time actually tomorrow? Adjust forward by 24 if so
					if (end_time < start_time) {
						end_time = end_time + (60 * 60 * 24);
					}
					// is it time to start or are we late to start?
					// but not past end time - no point in starting
					if (now >= start_time && now < end_time) {
						//all conditions met, go active
						b->active = 1;
						b->last_update = now;
						EventMsg(b->block_id, E_ACT);
					}
				} //not today nothing to do
			} else { //active == 1
				// if we are active then start was today or yesterday - end applies to today
				// test for end time - is it time yet to end?
				end_time = time_at_zero_today + b->settings.sch.time_end;
				if (now >= end_time) {
					// end time reached - deactivate
					b->active = 0;
					b->last_update = now;
					EventMsg(b->block_id, E_DEACT);
				}
			} // weird for a boolean state - we're not act or deact TODO exception catch
			break;
		}
		case SCH_ONE_SHOT:
			b->active = 0;
			b->last_update = time(NULL);
			break;

		case SCH_START_DURATION_REPEAT:
			b->active = 0;
			b->last_update = time(NULL);
			break;

		default:
			break;
	}

}


