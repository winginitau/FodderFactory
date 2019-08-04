/************************************************
 ff_schedules.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Schedule Processing
 ************************************************/


/************************************************
  Includes
************************************************/
#include <build_config.h>
//#include <datetime_ff.h>
#include <debug_ff.h>
#include <events.h>
#include <HAL.h>
#include <registry.h>
#include <schedules.h>
#include <time.h>
#include <string.h>
#include <string_consts.h>
#include <block_common.h>
#include <utils.h>

#ifdef PLATFORM_LINUX
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

	switch (b->type) {
		case SCH_START_STOP:
			b->active = 0;
			//b->last_update = TimeNow();
			b->status = STATUS_ENABLED_INIT;
			break;

		case SCH_ONE_SHOT:
			// XXX set up Evnt Message Bus subscriber, publisher model
			// XXX one-shot kludgy to implement using activation logic as the activation is instantaneous
			// it may not been seen by recipients unless it is held activated for a period
			// in which case debounce logic would then need to be implemented.
			b->active = 0;
			//b->last_update = TimeNow();
			b->status = STATUS_ENABLED_INIT;
			break;

		case SCH_START_DURATION_REPEAT: {
			char fmt_str[32];
			b->active = 0;
			//b->last_update = TimeNow();
			if (b->settings.sch.time_duration >= b->settings.sch.time_repeat) {
				char log_message[MAX_LOG_LINE_LENGTH];
				strcpy_hal(fmt_str, F("[%s] WARNING Duration >= Repeat"));
				sprintf(log_message, fmt_str, b->label);
				DebugLog(log_message);
			}
			b->status = STATUS_ENABLED_INIT;
		}
			break;

		default:
			EventMsg(SSS, E_ERROR, M_UNKNOWN_BLOCK_TYPE);
			break;
	}

}

void ScheduleOperate(BlockNode *b) {
	tm* now_tm;
	tm zero_tm = {0, 0, 0, 0, 0, 0, 0, 0, 0};

	time_t start_t;
	time_t end_t;
	time_t zero_today_t;
	time_t now_t;

	now_t = TimeNow();
	now_tm = localtime(&now_t);

	zero_tm.tm_year = now_tm->tm_year;
	zero_tm.tm_mon = now_tm->tm_mon;
	zero_tm.tm_mday = now_tm->tm_mday;
	zero_tm.tm_hour = 0;
	zero_tm.tm_min = 0;
	zero_tm.tm_sec = 0;

	zero_today_t = mktime(&zero_tm);


	uint8_t target_state = UINT8_INIT;
	uint8_t today_num = now_tm->tm_wday;
	uint8_t yesterday_num = ((today_num - 1) + 7) % 7;

	switch (b->type) {
		case SCH_START_STOP: {

			// convert start and end times to time_t values
			start_t = zero_today_t + b->settings.sch.time_start;
			end_t = zero_today_t + b->settings.sch.time_end;

			// Determine the pattern - a schedule contained within today (end >= start) or one that
			//	crosses midnight (end < start).

			if (end_t >= start_t) { //schedule contained within this 24hr period
				// is today an active day?
				//XXX align day num conversion
				if (b->settings.sch.days[today_num] == 1) {
					// Now could be in 1 of 3 periods
					//	1 - before start time (start > now && end >= now)
					//	2 - during the active period (start < now && end >= now)
					//	3 - after the active period (start < now && end < now)
					if (start_t > now_t && end_t >= now_t) target_state = 0;
					if (start_t <= now_t && end_t > now_t) target_state = 1;
					if (start_t <= now_t && end_t < now_t) target_state = 0;
				}
			} else {
				if (end_t < start_t) {
					//schedule must be a midnight crossing pattern
					// Now could be in 1 of 3 periods
					//	1 - before end time today where it started yesterday
					//	2 - after end time and not yet time to start again
					//	3 - in the next active period that will continue past the next midnight

					// was yesterday an active day? (ie. active period started yesterday)
					if (b->settings.sch.days[yesterday_num] == 1) {
						// start an active period from yesterday that should be still running
						if (start_t >= now_t && end_t > now_t) target_state = 1;
						// time to turn it off?
						if (start_t >= now_t && end_t < now_t) target_state = 0;
					}
					// is today active?
					if (b->settings.sch.days[today_num]) {
						if (start_t <= now_t && end_t < now_t) target_state = 1;
					}
				}
			}
				if (target_state == 1 && b->active == 0) {				//all conditions met, go active
					b->active = 1;
					//b->last_update = now;
					EventMsg(b->id, E_ACT);
				}
				if (target_state == 0 && b->active == 1) { //deact
					b->active = 0;
					//b->last_update = now;
					EventMsg(b->id, E_DEACT);
				}

			break;
		}
		case SCH_ONE_SHOT: {

			// convert start and end times to time_t values
			start_t = zero_today_t + b->settings.sch.time_start;
			end_t = start_t + ONE_SHOT_DURATION; 	// accommodate loop delays longer than 1 second

			if (b->settings.sch.days[today_num] == 1) {
				if (start_t <= now_t && end_t >= now_t) {
					target_state = 1;
				} else target_state = 0;
			} else target_state = 0;

			if (target_state == 1 && b->active == 0) {				//all conditions met, go active
				b->active = 1;
				//b->last_update = now;
				EventMsg(b->id, E_ACT);
				//XXX work around for reset min max without having a full block sequence in the config
				if (strcmp(b->label, RESET_MINMAX_SCH_BLOCK) == 0) {
					EventMsg(b->id, SSS, E_COMMAND, CMD_RESET_MINMAX);
				}
			}
			if (target_state == 0 && b->active == 1) { //deact
				b->active = 0;
				//b->last_update = now;
				EventMsg(b->id, E_DEACT);
			}
			break;
		}
		case SCH_START_DURATION_REPEAT: {
			TV_TYPE last_start_time;
			TV_TYPE sched_start;
			TV_TYPE repeat;
			TV_TYPE secs_today_now;
			TV_TYPE last_start_num;

			secs_today_now = now_t - zero_today_t;

			sched_start = b->settings.sch.time_start;
			repeat = b->settings.sch.time_repeat;

			last_start_num = (secs_today_now - sched_start) / repeat;
			last_start_time = sched_start + (last_start_num * repeat);

			start_t = last_start_time + zero_today_t;
			end_t = start_t + b->settings.sch.time_duration;

			if (b->settings.sch.days[today_num] == 1) {
				if (start_t <= now_t && end_t > now_t) {
					target_state = 1;
				} else target_state = 0;
			} else target_state = 0;

			if (target_state == 1 && b->active == 0) {				//all conditions met, go active
				b->active = 1;
				//b->last_update = now;
				EventMsg(b->id, E_ACT);
			}

			if (target_state == 0 && b->active == 1) { 				//deact
				b->active = 0;
				//b->last_update = now;
				EventMsg(b->id, E_DEACT);
			}
			break;
		}
		default:
			EventMsg(SSS, E_ERROR, M_UNKNOWN_BLOCK_TYPE);
			break;
	}
}

void ScheduleShow(BlockNode *b, void(Callback(const char *))) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	char fmt_str[MAX_LABEL_LENGTH];
	char label_str[MAX_LABEL_LENGTH];

	CommonShow(b, Callback);

	strcpy_hal(out_str, F("Schedule:"));
	Callback(out_str);

	strcpy_hal(out_str, F(" days:         "));
	FlagToDayStr(label_str, b->settings.sch.days);
	strcat(out_str, label_str);
	strcat_hal(out_str, F(" ( "));
	for (uint8_t i = 0; i < 7; i++) {
		strcpy_hal(fmt_str, F("%d "));
		sprintf(label_str, fmt_str, b->settings.sch.days[i]);
		strcat(out_str, label_str);
	}
	strcat_hal(out_str, F(")"));
	Callback(out_str);

	strcpy_hal(fmt_str, F(" time_start:   %s (%lu)"));
	TimeValueToTimeString(label_str, b->settings.sch.time_start);
	sprintf(out_str, fmt_str, label_str, (unsigned long)b->settings.sch.time_start);
	Callback(out_str);

	if (b->settings.sch.time_end == TV_TYPE_INIT) {
		strcpy_hal(out_str, F(" time_end:     TV_TYPE_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" time_end:     %s (%lu)"));
		TimeValueToTimeString(label_str, b->settings.sch.time_end);
		sprintf(out_str, fmt_str, label_str, (unsigned long)b->settings.sch.time_end);
	}
	Callback(out_str);

	if(b->settings.sch.time_duration == TV_TYPE_INIT) {
		strcpy_hal(out_str, F(" time_duration:TV_TYPE_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" time_duration:%s (%lu)"));
		TimeValueToTimeString(label_str, b->settings.sch.time_duration);
		sprintf(out_str, fmt_str, label_str, (unsigned long)b->settings.sch.time_duration);
	}
	Callback(out_str);

	if(b->settings.sch.time_repeat == TV_TYPE_INIT) {
		strcpy_hal(out_str, F(" time_repeat:  TV_TYPE_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" time_repeat:  %s (%lu)"));
		TimeValueToTimeString(label_str, b->settings.sch.time_repeat);
		sprintf(out_str, fmt_str, label_str, (unsigned long)b->settings.sch.time_repeat);
	}
	Callback(out_str);
}



