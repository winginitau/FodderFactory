/************************************************
 ff_schedules.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Schedule Processing
 ************************************************/


/************************************************
  Includes
************************************************/
#include <ff_sys_config.h>
#include "ff_schedules.h"

#include "ff_HAL.h"
#include "ff_string_consts.h"
#include "ff_events.h"
#include "ff_registry.h"
#include "ff_datetime.h"
#include "ff_debug.h"

#include <time.h>
#include <string.h>

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
			b->last_update = TimeNow();
			break;

		case SCH_ONE_SHOT:
			// XXX set up Evnt Message Bus subscriber, publisher model
			// XXX one-shot kludgy to implement using activation logic as the activation is instantaneous
			// it may not been seen by recipients unless it is held activated for a period
			// in which case debounce logic would then need to be implemented.
			b->active = 0;
			b->last_update = TimeNow();
			break;

		case SCH_START_DURATION_REPEAT: {
			char fmt_str[32];
			b->active = 0;
			b->last_update = TimeNow();
			if (b->settings.sch.time_duration >= b->settings.sch.time_repeat) {
				char log_message[MAX_LOG_LINE_LENGTH];
				strcpy_hal(fmt_str, F("[%s] WARNING Duration >= Repeat"));
				sprintf(log_message, fmt_str, b->block_label);
				DebugLog(log_message);
			}
		}
			break;

		default:
			break;
	}

}

void ScheduleOperate(BlockNode *b) {
	tm* now_tm;
	tm zero_tm;

	time_t start;
	time_t end;
	time_t zero_today;
	time_t now;

	now = TimeNow();
	now_tm = localtime(&now);

//	D("now", now);
//	D("now_tm", now_tm);


	zero_tm.tm_year = now_tm->tm_year;
	zero_tm.tm_mon = now_tm->tm_mon;
	zero_tm.tm_mday = now_tm->tm_mday;
	zero_tm.tm_hour = 0;
	zero_tm.tm_min = 0;
	zero_tm.tm_sec = 0;

//	D("zero_locatime", &zero_tm);

	zero_today = mktime(&zero_tm);


//	D("zero_today", zero_today);
//	D("zero_locatime", &zero_tm);


	//now = TimeNow();
	//now_tm = localtime(&now);


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
					// Now could be in 1 of 3 periods
					//	1 - before start time (start > now && end >= now)
					//	2 - during the active period (start < now && end >= now)
					//	3 - after the active period (start < now && end < now)
					if (start > now && end >= now) target_state = 0;
					if (start <= now && end > now) target_state = 1;
					if (start <= now && end < now) target_state = 0;
				}
			} else {
				if (end < start) {
					//schedule must be a midnight crossing pattern
					// Now could be in 1 of 3 periods
					//	1 - before end time today where it started yesterday
					//	2 - after end time and not yet time to start again
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
		case SCH_ONE_SHOT: {

			// convert start and end times to time_t values
			start = zero_today + b->settings.sch.time_start;
			end = start + ONE_SHOT_DURATION; 	// accommodate loop delays longer than 1 second

			if (b->settings.sch.days[today_num] == 1) {
				if (start <= now && end >= now) {
					target_state = 1;
				} else target_state = 0;
			} else target_state = 0;

			if (target_state == 1 && b->active == 0) {				//all conditions met, go active
				b->active = 1;
				b->last_update = now;
				EventMsg(b->block_id, E_ACT);
				//XXX work around for reset min max without having a full block sequence in the config
				if (strcmp(b->block_label, RESET_MINMAX_SCH_BLOCK) == 0) {
					EventMsg(b->block_id, SSS, E_COMMAND, CMD_RESET_MINMAX);
				}
			}
			if (target_state == 0 && b->active == 1) { //deact
				b->active = 0;
				b->last_update = now;
				EventMsg(b->block_id, E_DEACT);
			}
			break;
		}
		case SCH_START_DURATION_REPEAT: {

			// XXX HACK
			if(strcmp_hal(b->block_label, F("SCH_WATERING_BOTTOM_SCHEDULE")) == 0) {
				uint16_t mon_id;
				mon_id = GetBlockIDByLabel("MON_INSIDE_BOTTOM_TOO_COLD");
				if(IsActive(mon_id)) {
					b->settings.sch.time_duration = 60;
				} else {
					b->settings.sch.time_duration = 20;
				}
			}

			TV_TYPE last_start_time;
			TV_TYPE sched_start;
			TV_TYPE repeat;
			TV_TYPE secs_today_now;
			TV_TYPE last_start_num;

//			secs_today_now = (now_tm->tm_sec) + (now_tm->tm_min * 60) + (now_tm->tm_hour * 60 * 60);
			secs_today_now = now - zero_today;

			sched_start = b->settings.sch.time_start;
			repeat = b->settings.sch.time_repeat;

			last_start_num = (secs_today_now - sched_start) / repeat;
			last_start_time = sched_start + (last_start_num * repeat);

			start = last_start_time + zero_today;
			end = start + b->settings.sch.time_duration;

//			D("secs_today_now", secs_today_now);
//			D("sched_start", sched_start);
//			D("repeat", repeat);

//			D("zero_today", zero_today);
//			D("last_start_num", last_start_num);
//			D("last_start_time", last_start_time);

//			D("start", start);
//			D("end", end);

//			D("now", now);
//			D("now_tm", now_tm);


			//D("last_start_num", last_start_num);

			if (b->settings.sch.days[today_num] == 1) {
				if (start <= now && end > now) {
					target_state = 1;
				} else target_state = 0;
			} else target_state = 0;

			if (target_state == 1 && b->active == 0) {				//all conditions met, go active
				b->active = 1;
				b->last_update = now;
				EventMsg(b->block_id, E_ACT);
				//XXX HACK
				if(strcmp_hal(b->block_label, F("SCH_WATERING_BOTTOM_SCHEDULE")) == 0) {
					EventMsg(SSS, E_WARNING, M_HACK_SCH_WATERING_BOTTOM_SCHEDULE);
				}
			}

			if (target_state == 0 && b->active == 1) { //deact
				b->active = 0;
				b->last_update = now;
				EventMsg(b->block_id, E_DEACT);
				//XXX HACK
				if(strcmp_hal(b->block_label, F("SCH_WATERING_BOTTOM_SCHEDULE")) == 0) {
					EventMsg(SSS, E_WARNING, M_HACK_SCH_WATERING_BOTTOM_SCHEDULE);
				}
			}
			break;
		}
		default:
			break;
	}

}


