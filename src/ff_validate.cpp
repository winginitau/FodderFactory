/************************************************
 ff_validate.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Block Validaton
 ************************************************/


/************************************************
  Includes
************************************************/
//#include <Arduino.h>

#include "ff_validate.h"
#include "ff_sys_config.h"
#include "ff_string_consts.h"
#include "ff_registry.h"
#include "ff_debug.h"

//#include <string.h>

#ifdef FF_SIMULATOR
//#include <stdint.h>
#include <stdio.h>
#endif

#include <assert.h>

/************************************************
 Data Structures
 ************************************************/


/************************************************
  Globals
************************************************/


/************************************************
  Functions
************************************************/


void Validate(BlockNode* b) {
	//char debug_msg[MAX_DEBUG_LENGTH];

	//sprintf(debug_msg, "VALIDATE: CAT:%d,%d[%s][%s], TYPE:%d[%s], ID:%d, LABEL:[%s]", b->block_cat, block_cat_defs[b->block_cat].cat_id, block_cat_defs[b->block_cat].conf_section_label, block_cat_defs[b->block_cat].conf_section_key_base, b->block_type, block_type_strings[b->block_type].text, b->block_id, b->block_label);
	//DebugLog(debug_msg);

	//common attributes
		//struct BLOCK_NODE *next_block - assume ok

	//uint8_t block_cat;
		//is not null
		//is a valid category
	assert(b->block_cat < LAST_BLOCK_CAT);

	//uint16_t block_type;
		//is not null
		//is a valid type
	assert(b->block_type < LAST_BLOCK_TYPE);
	assert(b->block_type != BT_ERROR);
		//type belongs to category

	//uint16_t block_id;
		//not null
	assert(b->block_id != UINT16_INIT);
	assert(b->block_id >= SSS);
		//is unique
		//matches the block_label

	//char block_label[MAX_LABEL_LENGTH];
		//not null
	assert(b->block_label[0] != '\0');
	//is unique
		//matches block_id

	//char display_name[MAX_LABEL_LENGTH];
		//is not null
#ifndef	EXCLUDE_DISPLAYNAME
	assert(b->display_name[0] != '\0');
#endif
	//is short

	//char description[MAX_DESCR_LENGTH];
		//optional

	//uint8_t active;
		//is 0
	assert(b->active == 0);

	//uint8_t bool_val;
		//init state
	assert(b->bool_val == 0);

	//uint8_t int_val;
		//init state
	assert(b->int_val == INT32_INIT);

	//float f_val;
		//init state
	assert(b->f_val == FLOAT_INIT);

	//FFTime last_update
	assert(b->last_update == UINT32_INIT);


	//Settings

	switch(b->block_type) {
	case IN_ONEWIRE:
		assert(b->settings.in.interface == IF_ONEWIRE);
		assert(b->settings.in.if_num < UINT8_INIT);
		//assert(b->settings.in.log_rate.hour < 24);
		//assert(b->settings.in.log_rate.minute < 60);
		//assert(b->settings.in.log_rate.second < 60);
		assert(b->settings.in.data_units < LAST_UNIT);
		//uint8_t data_type;		// float, int
		//Prob derived from block type - consider dropping
		break;

	case IN_DIGITAL:
		assert(b->settings.in.interface == IF_DIG_PIN_IN);
		assert(b->settings.in.if_num < UINT8_INIT);
		//assert(b->settings.in.log_rate.hour < 24);
		//assert(b->settings.in.log_rate.minute < 60);
		//assert(b->settings.in.log_rate.second < 60);
		assert(b->settings.in.data_units == ONOFF);
		//uint8_t data_type;		// float, int
		//Prob derived from block type - consider dropping
		break;

	case MON_CONDITION_LOW:
		assert(b->settings.mon.input1 != UINT16_INIT);
		assert(b->settings.mon.input1 >= BLOCK_ID_BASE);

		assert(b->settings.mon.input2 == UINT16_INIT);
		assert(b->settings.mon.input3 == UINT16_INIT);
		assert(b->settings.mon.input4 == UINT16_INIT);
		assert(b->settings.mon.act_val > -50);
		assert(b->settings.mon.act_val < 50);
		assert(b->settings.mon.deact_val > -50);
		assert(b->settings.mon.deact_val < 50);
		assert(b->settings.mon.act_val < b->settings.mon.deact_val);
		break;

	case MON_CONDITION_HIGH:
		assert(b->settings.mon.input1 != UINT16_INIT);
		assert(b->settings.mon.input1 >= BLOCK_ID_BASE);

		assert(b->settings.mon.input2 == UINT16_INIT);
		assert(b->settings.mon.input3 == UINT16_INIT);
		assert(b->settings.mon.input4 == UINT16_INIT);
		assert(b->settings.mon.act_val > -50);
		assert(b->settings.mon.act_val < 50);
		assert(b->settings.mon.deact_val > -50);
		assert(b->settings.mon.deact_val < 50);
		assert(b->settings.mon.act_val > b->settings.mon.deact_val);
		break;

	case MON_AVERAGE_CONDITION_LOW:
		assert(b->settings.mon.input1 != UINT16_INIT);
		assert(b->settings.mon.input1 >= BLOCK_ID_BASE);

		assert(b->settings.mon.input2 != UINT16_INIT);
		assert(b->settings.mon.input2 >= BLOCK_ID_BASE);

		assert(b->settings.mon.input3 == UINT16_INIT);
		assert(b->settings.mon.input4 == UINT16_INIT);
		assert(b->settings.mon.act_val > -50);
		assert(b->settings.mon.act_val < 50);
		assert(b->settings.mon.deact_val > -50);
		assert(b->settings.mon.deact_val < 50);
		assert(b->settings.mon.act_val < b->settings.mon.deact_val);
		break;

	case MON_AVERAGE_CONDITION_HIGH:
		assert(b->settings.mon.input1 != UINT16_INIT);
		assert(b->settings.mon.input1 >= BLOCK_ID_BASE);

		assert(b->settings.mon.input2 != UINT16_INIT);
		assert(b->settings.mon.input2 >= BLOCK_ID_BASE);

		assert(b->settings.mon.input3 == UINT16_INIT);
		assert(b->settings.mon.input4 == UINT16_INIT);
		assert(b->settings.mon.act_val > -50);
		assert(b->settings.mon.act_val < 50);
		assert(b->settings.mon.deact_val > -50);
		assert(b->settings.mon.deact_val < 50);
		assert(b->settings.mon.act_val > b->settings.mon.deact_val);
		break;

	case MON_TRIGGER:
		assert(b->settings.mon.input1 != UINT16_INIT);
		assert(b->settings.mon.input1 >= BLOCK_ID_BASE);

		assert(b->settings.mon.input2 == UINT16_INIT);
		assert(b->settings.mon.input3 == UINT16_INIT);
		assert(b->settings.mon.input4 == UINT16_INIT);
		assert(b->settings.mon.act_val >= 0);
		assert(b->settings.mon.act_val <= 1);
		assert(b->settings.mon.deact_val >= 0);
		assert(b->settings.mon.deact_val <= 1);
		break;

	case SCH_START_STOP:
		assert(b->settings.sch.days[0] != UINT8_INIT);
		assert(b->settings.sch.days[1] != UINT8_INIT);
		assert(b->settings.sch.days[2] != UINT8_INIT);
		assert(b->settings.sch.days[3] != UINT8_INIT);
		assert(b->settings.sch.days[4] != UINT8_INIT);
		assert(b->settings.sch.days[5] != UINT8_INIT);
		assert(b->settings.sch.days[6] != UINT8_INIT);

		/*
		assert(b->settings.sch.time_start.hour < 24);
		assert(b->settings.sch.time_start.minute < 60);
		assert(b->settings.sch.time_start.second < 60);

		assert(b->settings.sch.time_end.hour < 24);
		assert(b->settings.sch.time_end.minute < 60);
		assert(b->settings.sch.time_end.second < 60);

		assert(b->settings.sch.time_duration.hour == UINT8_INIT);
		assert(b->settings.sch.time_duration.minute == UINT8_INIT);
		assert(b->settings.sch.time_duration.second == UINT8_INIT);

		assert(b->settings.sch.time_repeat.hour == UINT8_INIT);
		assert(b->settings.sch.time_repeat.minute == UINT8_INIT);
		assert(b->settings.sch.time_repeat.second == UINT8_INIT);
		*/
		break;

	case SCH_ONE_SHOT:
		assert(b->settings.sch.days[0] != UINT8_INIT);
		assert(b->settings.sch.days[1] != UINT8_INIT);
		assert(b->settings.sch.days[2] != UINT8_INIT);
		assert(b->settings.sch.days[3] != UINT8_INIT);
		assert(b->settings.sch.days[4] != UINT8_INIT);
		assert(b->settings.sch.days[5] != UINT8_INIT);
		assert(b->settings.sch.days[6] != UINT8_INIT);
/*
		assert(b->settings.sch.time_start.hour < 24);
		assert(b->settings.sch.time_start.minute < 60);
		assert(b->settings.sch.time_start.second < 60);

		assert(b->settings.sch.time_end.hour == UINT8_INIT);
		assert(b->settings.sch.time_end.minute == UINT8_INIT);
		assert(b->settings.sch.time_end.second == UINT8_INIT);

		assert(b->settings.sch.time_duration.hour == UINT8_INIT);
		assert(b->settings.sch.time_duration.minute == UINT8_INIT);
		assert(b->settings.sch.time_duration.second == UINT8_INIT);

		assert(b->settings.sch.time_repeat.hour == UINT8_INIT);
		assert(b->settings.sch.time_repeat.minute == UINT8_INIT);
		assert(b->settings.sch.time_repeat.second == UINT8_INIT);
	*/
		break;

	case SCH_START_DURATION_REPEAT:
		assert(b->settings.sch.days[0] != UINT8_INIT);
		assert(b->settings.sch.days[1] != UINT8_INIT);
		assert(b->settings.sch.days[2] != UINT8_INIT);
		assert(b->settings.sch.days[3] != UINT8_INIT);
		assert(b->settings.sch.days[4] != UINT8_INIT);
		assert(b->settings.sch.days[5] != UINT8_INIT);
		assert(b->settings.sch.days[6] != UINT8_INIT);
/*
		assert(b->settings.sch.time_start.hour < 24);
		assert(b->settings.sch.time_start.minute < 60);
		assert(b->settings.sch.time_start.second < 60);

		assert(b->settings.sch.time_end.hour == UINT8_INIT);
		assert(b->settings.sch.time_end.minute == UINT8_INIT);
		assert(b->settings.sch.time_end.second == UINT8_INIT);

		assert(b->settings.sch.time_duration.hour < 24);
		assert(b->settings.sch.time_duration.minute < 60);
		assert(b->settings.sch.time_duration.second < 60);

		assert(b->settings.sch.time_repeat.hour < 24);
		assert(b->settings.sch.time_repeat.minute < 60);
		assert(b->settings.sch.time_repeat.second < 60);
*/
		break;

	case RL_LOGIC_ANDNOT:
		assert(b->settings.rl.param1 != UINT16_INIT);
		assert(b->settings.rl.param1 >= BLOCK_ID_BASE);

		assert(b->settings.rl.param2 != UINT16_INIT);
		assert(b->settings.rl.param2 >= BLOCK_ID_BASE);

		assert(b->settings.rl.param3 == UINT16_INIT);

		assert(b->settings.rl.param_not != UINT16_INIT);
		assert(b->settings.rl.param_not >= BLOCK_ID_BASE);
		break;

	case RL_LOGIC_SINGLE:
		assert(b->settings.rl.param1 != UINT16_INIT);
		assert(b->settings.rl.param1 >= BLOCK_ID_BASE);

		assert(b->settings.rl.param2 == UINT16_INIT);
		assert(b->settings.rl.param3 == UINT16_INIT);
		assert(b->settings.rl.param_not == UINT16_INIT);
		break;

	case RL_LOGIC_AND:
		assert(b->settings.rl.param1 != UINT16_INIT);
		assert(b->settings.rl.param1 >= BLOCK_ID_BASE);

		assert(b->settings.rl.param2 != UINT16_INIT);
		assert(b->settings.rl.param2 >= BLOCK_ID_BASE);

		assert(b->settings.rl.param3 == UINT16_INIT);
		assert(b->settings.rl.param_not == UINT16_INIT);
		break;

	case RL_LOGIC_SINGLENOT:
		assert(b->settings.rl.param1 != UINT16_INIT);
		assert(b->settings.rl.param1 >= BLOCK_ID_BASE);

		assert(b->settings.rl.param2 == UINT16_INIT);
		assert(b->settings.rl.param3 == UINT16_INIT);

		assert(b->settings.rl.param_not != UINT16_INIT);
		assert(b->settings.rl.param_not >= BLOCK_ID_BASE);
		break;

	case CON_ONOFF:
		assert(b->settings.con.rule != UINT16_INIT);
		assert(b->settings.con.rule >= BLOCK_ID_BASE);

		assert(b->settings.con.output != UINT16_INIT);
		assert(b->settings.con.output >= BLOCK_ID_BASE);

		assert(b->settings.con.act_cmd < LAST_COMMAND);
		assert(b->settings.con.act_cmd != CMD_ERROR);
		assert(b->settings.con.deact_cmd < LAST_COMMAND);
		assert(b->settings.con.deact_cmd != CMD_ERROR);
		break;

	case CON_SYSTEM:
		assert(b->settings.con.rule != UINT16_INIT);
		assert(b->settings.con.rule >= BLOCK_ID_BASE);

		assert(b->settings.con.output != UINT16_INIT);
		assert(b->settings.con.output >= BLOCK_ID_BASE);

		assert(b->settings.con.act_cmd < LAST_COMMAND);
		assert(b->settings.con.act_cmd != CMD_ERROR);
		//assert(b->settings.con.deact_cmd < LAST_COMMAND);
		//assert(b->settings.con.deact_cmd != CMD_ERROR);
		break;

	case OUT_DIGITAL:
		assert(b->settings.out.interface != IF_ERROR);
		assert(b->settings.out.interface < LAST_INTERFACE);
		assert(b->settings.out.interface == IF_DIG_PIN_OUT);
		assert(b->settings.out.if_num < UINT8_INIT);
		break;

	case OUT_SYSTEM_CALL:
		assert(b->settings.out.interface != IF_ERROR);
		assert(b->settings.out.interface < LAST_INTERFACE);
		assert(b->settings.out.interface == IF_SYSTEM_FUNCTION);
		//assert(b->settings.out.if_num < UINT8_INIT);
		break;

	}

}


