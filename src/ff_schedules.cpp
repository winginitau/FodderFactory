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
#include "ff_debug.h"

#include <time.h>

#ifdef FF_SIMULATOR
#include <stdio.h>
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
			// XXX set up Evnt Message Bus subscriber, publisher model
			// XXX one-shot cludgy to implement using activation logic as the activation is instantaneous
			// it may not been seen by recipients unless it is held activated for a period
			// in which case debounce logic would then need to be implemented.
			b->active = 0;
			b->last_update = time(NULL);
			break;

		case SCH_START_DURATION_REPEAT:
			b->active = 0;
			b->last_update = time(NULL);
			if (b->settings.sch.time_duration >= b->settings.sch.time_repeat) {
				char log_message[MAX_DEBUG_LENGTH];
				sprintf(log_message, "[%s] WARNING Duration >= Repeat", b->block_label);
				DebugLog(log_message);
			}
			break;

		default:
			break;
	}

}

void ScheduleOperate(BlockNode *b) {
	tm* now_tm;
	tm* temp_tm;

	time_t start;
	time_t end;
	time_t zero_today;

	time_t now;
	time_t temp;

	temp = time(NULL);
	temp_tm = localtime(&temp);

	temp_tm->tm_hour = 0;
	temp_tm->tm_min = 0;
	temp_tm->tm_sec = 0;
	zero_today = mktime(temp_tm);

	now = time(NULL);
	now_tm = localtime(&now);


	uint8_t target_state = UINT8_INIT;
	uint8_t today_num = now_tm->tm_wday;
	uint8_t yesterday_num = ((today_num - 1) + 7) % 7;


	switch (b->block_type) {
		case SCH_START_STOP: {

			// convert start and end times to time_t values
			start = zero_today + b->settings.sch.time_start;
			end = zero_today + b->settings.sch.time_end;

			// Determine the pattern - a schedule contained within today (end >= start) or one that
			//	crosses midnight (end < start).

			if (end >= start) { //schedule contained within this 24hr period
				// is today an active day?
				//XXX align day num conversion
				if (b->settings.sch.days[today_num] == 1) {
					// Could be in 1 of 3 periods
					//	1 - before start time (start > now && end >= now)
					//	2 - during the active period (start < now && end >= now)
					//	3 - after the active period (start < now && end < now)
					if (start > now && end >= now) target_state = 0;
					if (start <= now && end > now) target_state = 1;
					if (start <= now && end < now) target_state = 0;
				}
			} else {
				if (end < start) {
					//schedule must cross midnight
					// Could be in 1 of 3 periods
					//	1 - yesterday was active and it started yesterday and is still running
					//	2 - if it was active yesterday it is now finished, and not yet time to start again
					//	3 - in the next active period that will continue past the next midnight

					// was yesterday an active day? (ie. active period started yesterday)
					if (b->settings.sch.days[yesterday_num] == 1) {
						// start an active period from yesterday that should be still running
						if (start >= now && end > now) target_state = 1;
						// time to turn it off?
						if (start >= now && end < now) target_state = 0;
					}
					// is today active?
					if (b->settings.sch.days[today_num]) {
						if (start <= now && end < now) target_state = 1;
					}
				}
			}
				if (target_state == 1 && b->active == 0) {				//all conditions met, go active
					b->active = 1;
					b->last_update = now;
					EventMsg(b->block_id, E_ACT);
				}
				if (target_state == 0 && b->active == 1) { //deact
					b->active = 0;
					b->last_update = now;
					EventMsg(b->block_id, E_DEACT);
				}

			break;
		}
		case SCH_ONE_SHOT:
			//XXX to do - see comments in setup
			b->active = 0;
			b->last_update = time(NULL);
			break;

		case SCH_START_DURATION_REPEAT: {

			TV_TYPE last_start_time;
			TV_TYPE sched_start;
			TV_TYPE repeat;
			TV_TYPE time_now;
			TV_TYPE last_start_num;

			time_now = (now_tm->tm_sec) + (now_tm->tm_min * 60) + (now_tm->tm_hour * 60 * 60);

			sched_start = b->settings.sch.time_start;
			repeat = b->settings.sch.time_repeat;


			last_start_num = (time_now - sched_start) / repeat;
			last_start_time = sched_start + (last_start_num * repeat);

			start = last_start_time + zero_today;
			end = start + b->settings.sch.time_duration;

			if (b->settings.sch.days[today_num] == 1) {
				if (start <= now && end > now) {
					target_state = 1;
				} else target_state = 0;
			} else target_state = 0;

			if (target_state == 1 && b->active == 0) {				//all conditions met, go active
				b->active = 1;
				b->last_update = now;
				EventMsg(b->block_id, E_ACT);
			}
			if (target_state == 0 && b->active == 1) { //deact
				b->active = 0;
				b->last_update = now;
				EventMsg(b->block_id, E_DEACT);
			}
			break;
		}
		default:
			break;
	}

}


