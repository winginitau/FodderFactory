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

#include <build_config.h>
#include <debug_ff.h>
#include <registry.h>
#include <string_consts.h>
#include <validate.h>
#include <HAL.h>
#include <utils.h>

//#include <string.h>

#ifdef PLATFORM_LINUX
//#include <stdint.h>
#include <stdio.h>
#endif

#include <assert.h>
#include <stdint.h>
#include <events.h>
#include <build_config.h>

/************************************************
 Data Structures
 ************************************************/


/************************************************
  Globals
************************************************/


/************************************************
  Functions
************************************************/

void ASSERT_ARDUINO(uint8_t result, BlockNode* b) {
	if (!result) {
		EventMsg(b->id, E_ERROR, M_ASSERT_FAILED);
		b->status = STATUS_DISABLED_ASSERT;
		//RegShowBlockByID(b->id, HALItchWriteLnImmediate);
	}
}

void ASSERT_INT_ARDUINO(uint8_t result, InterfaceNode* b) {
	if (!result) {
		EventMsg(b->id, E_ERROR, M_ASSERT_FAILED);
		b->status = STATUS_DISABLED_ASSERT;
		//RegShowBlockByID(b->id, HALItchWriteLnImmediate);
	}
}

void ValidateInterface(InterfaceNode* b) {

	// Assume success (ASSERTS will negate if it fails)
	b->status = STATUS_DISABLED_INIT;

	//uint16_t block_type;
	ASSERT_INT(b->type < LAST_INTERFACE, b);
	ASSERT_INT(b->type != IF_ERROR, b);
	// TODO type belongs to category

	//uint16_t block_id;
	ASSERT_INT(b->id != UINT16_INIT, b);
	ASSERT_INT(b->id >= SSS, b);
	// TODO is unique

	//char block_label[MAX_LABEL_LENGTH];
	ASSERT_INT(b->label != NULL, b);
	ASSERT_INT(b->label[0] != '\0', b);

	#ifndef	EXCLUDE_DISPLAYNAME
	//char display_name[MAX_LABEL_LENGTH];
	ASSERT_INT(b->display_name[0] != '\0', b);
	// TODO is short
	#endif

	//char description[MAX_DESCR_LENGTH];
	// TODO checks

	//uint8_t active;
	//ASSERT(b->active == 0, b);

	switch (b->type) {
		case IF_ONEWIRE_BUS: {
			InterfaceONEWIRE_BUS *ifs;
			ifs = (InterfaceONEWIRE_BUS *)b->IFSettings;
			ASSERT_INT(ifs->bus_pin < UINT8_INIT, b);
			ASSERT_INT(ifs->bus_pin > 0, b);
			ASSERT_INT(ifs->device_count < UINT8_INIT, b);
			//ASSERT_INT(ifs->bus_pin > 0, b);
		}
		break;
		case IF_DS1820B: {
			InterfaceDS1820B *ifs;
			ifs = (InterfaceDS1820B *)b->IFSettings;
			ASSERT_INT(ifs->bus_pin < UINT8_INIT, b);
			ASSERT_INT(ifs->bus_pin > 0, b);

			uint8_t init_address[8] = {0, 0, 0, 0, 0, 0, 0, 0};
			ASSERT_INT(DallasCompare(ifs->dallas_address, init_address) != 0, b);
			ASSERT_INT(ifs->in_progress == 0, b);
		}

	}
}

void Validate(BlockNode* b) {
#ifdef DEBUG
	char debug_msg[MAX_DEBUG_LENGTH];
	//sprintf(debug_msg, "Asserts: CAT:%d,%d[%s][%s], TYPE:%d[%s], ID:%d, LABEL:[%s]", b->block_cat, block_cat_defs[b->block_cat].cat_id, block_cat_defs[b->block_cat].conf_section_label, block_cat_defs[b->block_cat].conf_section_key_base, b->block_type, block_type_strings[b->block_type].text, b->block_id, b->block_label);
	sprintf(debug_msg, "Asserts: cat:%d, type:%d, id:%d, label:[%s]", b->cat, b->type, b->id, b->label);
	DebugLog(debug_msg);
#endif

	// Assume success (ASSERTS will negate if it fails)
	b->status = STATUS_DISABLED_INIT;

	//common attributes
	//struct BLOCK_NODE *next_block - assume ok

	//uint8_t block_cat;
	ASSERT(b->cat != FF_ERROR_CAT, b);
	ASSERT(b->cat < LAST_BLOCK_CAT, b);

	//uint16_t block_type;
	ASSERT(b->type < LAST_BLOCK_TYPE, b);
	ASSERT(b->type != BT_ERROR, b);
	// TODO type belongs to category

	//uint16_t block_id;
	ASSERT(b->id != UINT16_INIT, b);
	ASSERT(b->id >= SSS, b);
	// TODO is unique

	//char block_label[MAX_LABEL_LENGTH];
	ASSERT(b->label != NULL, b);
	ASSERT(b->label[0] != '\0', b);

	#ifndef	EXCLUDE_DISPLAYNAME
	//char display_name[MAX_LABEL_LENGTH];
	ASSERT(b->display_name[0] != '\0', b);
	// TODO is short
	#endif

	//char description[MAX_DESCR_LENGTH];
	// TODO checks

	//uint8_t active;
	ASSERT(b->active == 0, b);

	//uint8_t bool_val;
	//init state
	ASSERT(b->bool_val == 0, b);

	//uint8_t int_val;
	//init state
	ASSERT(b->int_val == INT32_INIT, b);

	//float f_val;
	//init state
	ASSERT(b->f_val == FLOAT_INIT, b);

	//FFTime last_update
	//ASSERT(b->last_update == UINT32_INIT, b);
	//ASSERT(b->last_logged == UINT32_INIT, b);

	//Settings

	switch(b->type) {
	case SYS_SYSTEM:
		ASSERT(b->settings.sys.language != UINT8_INIT, b);
		ASSERT(b->settings.sys.start_delay != UINT16_INIT, b);
		ASSERT(b->settings.sys.temp_scale != UINT8_INIT, b);
		ASSERT(b->settings.sys.week_start != UINT8_INIT, b);
		break;

	case IN_ONEWIRE:
		ASSERT(b->settings.in.interface != BLOCK_ID_INIT, b);
		ASSERT(b->settings.in.interface >= BLOCK_ID_BASE, b);
		//ASSERT(b->settings.in.interface == IF_DS1820B, b);
		//ASSERT(b->settings.in.if_num < UINT8_INIT, b);
		ASSERT(b->settings.in.data_units != UNIT_ERROR, b);
		ASSERT(b->settings.in.data_units < LAST_UNIT, b);
		ASSERT(b->settings.in.log_rate < UINT16_INIT, b);
		//uint8_t data_type;		// float, int
		//Prob derived from block type - consider dropping
		ASSERT(b->settings.in.last_polled == UINT32_INIT, b);
		ASSERT(b->settings.in.last_logged == UINT32_INIT, b);

		break;

	case IN_VEDIRECT:
		ASSERT( (b->settings.in.interface == IF_VED_VOLTAGE) || \
				(b->settings.in.interface == IF_VED_CURRENT) || \
				(b->settings.in.interface == IF_VED_POWER) || \
				(b->settings.in.interface == IF_VED_SOC) ,b);
		//ASSERT(b->settings.in.if_num < UINT8_INIT, b);
		ASSERT(b->settings.in.data_units != UNIT_ERROR, b);
		ASSERT(b->settings.in.data_units < LAST_UNIT, b);
		ASSERT(b->settings.in.poll_rate < UINT16_INIT, b);
		ASSERT(b->settings.in.log_rate < UINT16_INIT, b);
		ASSERT(b->settings.in.last_polled == UINT32_INIT, b);
		ASSERT(b->settings.in.last_logged == UINT32_INIT, b);
		break;

	case IN_DIGITAL:
		ASSERT(b->settings.in.interface != BLOCK_ID_INIT, b);
		ASSERT(b->settings.in.interface >= BLOCK_ID_BASE, b);

		//ASSERT(b->settings.in.interface == IF_DIG_PIN_IN, b);
		//ASSERT(b->settings.in.if_num < UINT8_INIT, b);
		ASSERT(b->settings.in.data_units == ONOFF, b);
		//uint8_t data_type;		// float, int
		//Prob derived from block type - consider dropping
		ASSERT(b->settings.in.last_polled == UINT32_INIT, b);
		ASSERT(b->settings.in.last_logged == UINT32_INIT, b);
		break;

	case MON_CONDITION_LOW:
		ASSERT(b->settings.mon.input1 != BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input1 >= BLOCK_ID_BASE, b);
		ASSERT(b->settings.mon.input2 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input3 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input4 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.act_val > -50.0, b);
		ASSERT(b->settings.mon.act_val < 50.0, b);
		ASSERT(b->settings.mon.deact_val > -50.0, b);
		ASSERT(b->settings.mon.deact_val < 50.0, b);
		ASSERT(b->settings.mon.act_val < b->settings.mon.deact_val, b);
		break;

	case MON_CONDITION_HIGH:
		ASSERT(b->settings.mon.input1 != BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input1 >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.mon.input2 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input3 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input4 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.act_val > -50.0, b);
		ASSERT(b->settings.mon.act_val < 50.0, b);
		ASSERT(b->settings.mon.deact_val > -50.0, b);
		ASSERT(b->settings.mon.deact_val < 50.0, b);
		ASSERT(b->settings.mon.act_val > b->settings.mon.deact_val, b);
		break;

	case MON_AVERAGE_CONDITION_LOW:
		ASSERT(b->settings.mon.input1 != BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input1 >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.mon.input2 != BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input2 >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.mon.input3 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input4 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.act_val > -50.0, b);
		ASSERT(b->settings.mon.act_val < 50.0, b);
		ASSERT(b->settings.mon.deact_val > -50.0, b);
		ASSERT(b->settings.mon.deact_val < 50.0, b);
		ASSERT(b->settings.mon.act_val < b->settings.mon.deact_val, b);
		break;

	case MON_AVERAGE_CONDITION_HIGH:
		ASSERT(b->settings.mon.input1 != BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input1 >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.mon.input2 != BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input2 >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.mon.input3 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input4 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.act_val > -50.0, b);
		ASSERT(b->settings.mon.act_val < 50.0, b);
		ASSERT(b->settings.mon.deact_val > -50.0, b);
		ASSERT(b->settings.mon.deact_val < 50.0, b);
		ASSERT(b->settings.mon.act_val > b->settings.mon.deact_val, b);
		break;

	case MON_TRIGGER:
		ASSERT(b->settings.mon.input1 != BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input1 >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.mon.input2 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input3 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.input4 == BLOCK_ID_INIT, b);
		ASSERT(b->settings.mon.act_val >= 0, b);
		ASSERT(b->settings.mon.act_val <= 1, b);
		ASSERT(b->settings.mon.deact_val >= 0, b);
		ASSERT(b->settings.mon.deact_val <= 1, b);
		break;

	case SCH_START_STOP:
		ASSERT(b->settings.sch.days[0] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[1] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[2] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[3] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[4] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[5] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[6] != UINT8_INIT, b);

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
		ASSERT(b->settings.sch.days[0] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[1] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[2] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[3] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[4] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[5] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[6] != UINT8_INIT, b);
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
		ASSERT(b->settings.sch.days[0] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[1] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[2] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[3] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[4] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[5] != UINT8_INIT, b);
		ASSERT(b->settings.sch.days[6] != UINT8_INIT, b);
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
		ASSERT(b->settings.rl.param1 != UINT16_INIT, b);
		ASSERT(b->settings.rl.param1 >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.rl.param2 != UINT16_INIT, b);
		ASSERT(b->settings.rl.param2 >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.rl.param3 == UINT16_INIT, b);

		ASSERT(b->settings.rl.param_not != UINT16_INIT, b);
		ASSERT(b->settings.rl.param_not >= BLOCK_ID_BASE, b);
		break;

	case RL_LOGIC_SINGLE:
		ASSERT(b->settings.rl.param1 != UINT16_INIT, b);
		ASSERT(b->settings.rl.param1 >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.rl.param2 == UINT16_INIT, b);
		ASSERT(b->settings.rl.param3 == UINT16_INIT, b);
		ASSERT(b->settings.rl.param_not == UINT16_INIT, b);
		break;

	case RL_LOGIC_AND:
		ASSERT(b->settings.rl.param1 != UINT16_INIT, b);
		ASSERT(b->settings.rl.param1 >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.rl.param2 != UINT16_INIT, b);
		ASSERT(b->settings.rl.param2 >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.rl.param3 == UINT16_INIT, b);
		ASSERT(b->settings.rl.param_not == UINT16_INIT, b);
		break;

	case RL_LOGIC_SINGLENOT:
		ASSERT(b->settings.rl.param1 != UINT16_INIT, b);
		ASSERT(b->settings.rl.param1 >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.rl.param2 == UINT16_INIT, b);
		ASSERT(b->settings.rl.param3 == UINT16_INIT, b);

		ASSERT(b->settings.rl.param_not != UINT16_INIT, b);
		ASSERT(b->settings.rl.param_not >= BLOCK_ID_BASE, b);
		break;

	case CON_ONOFF:
		ASSERT(b->settings.con.rule != UINT16_INIT, b);
		ASSERT(b->settings.con.rule >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.con.output != UINT16_INIT, b);
		ASSERT(b->settings.con.output >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.con.act_cmd < LAST_COMMAND, b);
		ASSERT(b->settings.con.act_cmd != CMD_ERROR, b);
		ASSERT(b->settings.con.deact_cmd < LAST_COMMAND, b);
		ASSERT(b->settings.con.deact_cmd != CMD_ERROR, b);
		break;

	case CON_SYSTEM:
		ASSERT(b->settings.con.rule != UINT16_INIT, b);
		ASSERT(b->settings.con.rule >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.con.output != UINT16_INIT, b);
		ASSERT(b->settings.con.output >= BLOCK_ID_BASE, b);

		ASSERT(b->settings.con.act_cmd < LAST_COMMAND, b);
		ASSERT(b->settings.con.act_cmd != CMD_ERROR, b);
		//assert(b->settings.con.deact_cmd < LAST_COMMAND);
		//assert(b->settings.con.deact_cmd != CMD_ERROR);
		break;

	case OUT_DIGITAL:
		ASSERT(b->settings.out.interface != IF_ERROR, b);
		ASSERT(b->settings.out.interface < LAST_INTERFACE, b);
		ASSERT(b->settings.out.interface == IF_DIG_PIN_OUT, b);
		ASSERT(b->settings.out.if_num < UINT8_INIT, b);
		break;

	case OUT_SYSTEM_CALL:
		ASSERT(b->settings.out.interface != IF_ERROR, b);
		ASSERT(b->settings.out.interface < LAST_INTERFACE, b);
		//ASSERT(b->settings.out.interface == IF_SYSTEM_FUNCTION, b);
		//assert(b->settings.out.if_num < UINT8_INIT);
		break;

	}

}


