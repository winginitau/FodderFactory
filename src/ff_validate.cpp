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
#include <assert.h>
//#include <stdint.h>
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

void DumpBlock(BlockNode* block) {


}

void Validate(BlockNode* b) {
	char debug_msg[MAX_DEBUG_LENGTH];


	sprintf(debug_msg, "VALIDATE: CAT:%d,%d[%s][%s], TYPE:%d[%s], ID:%d, LABEL:[%s]", b->block_cat, block_cat_defs[b->block_cat].cat_id, block_cat_defs[b->block_cat].conf_section_label, block_cat_defs[b->block_cat].conf_section_key_base, b->block_type, block_type_strings[b->block_type], b->block_id, b->block_label);
	DebugLog(debug_msg);

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
		//type belongs to category

	//uint16_t block_id;
		//not null
	assert(b->block_id != 0xFFFF);	//is unique
		//matches the block_label

	//char block_label[MAX_LABEL_LENGTH];
		//not null
	assert(b->block_label[0] != '\0');
	//is unique
		//matches block_id

	//char display_name[MAX_LABEL_LENGTH];
		//is not null
	assert(b->display_name[0] != '\0');
		//is short

	//char description[MAX_DESCR_LENGTH];
		//optional

	//uint8_t active;
		//is 0
	assert(b->active == 0);

			//BlockSettings settings;

	switch(b->block_type) {
	case IN_ONEWIRE:
		break;

	case IN_DIGITAL:
		break;

	case MON_CONDITION_LOW:
		assert(b->settings.mon.input1 != 0xFFFF);
		assert(b->settings.mon.input2 == 0xFFFF);
		assert(b->settings.mon.input3 == 0xFFFF);
		assert(b->settings.mon.input4 == 0xFFFF);
		assert(b->settings.mon.act_val > -50);
		assert(b->settings.mon.act_val < 50);
		assert(b->settings.mon.deact_val > -50);
		assert(b->settings.mon.deact_val < 50);
		assert(b->settings.mon.act_val < b->settings.mon.deact_val);
		break;

	case MON_CONDITION_HIGH:
		assert(b->settings.mon.input1 != 0xFFFF);
		assert(b->settings.mon.input2 == 0xFFFF);
		assert(b->settings.mon.input3 == 0xFFFF);
		assert(b->settings.mon.input4 == 0xFFFF);
		assert(b->settings.mon.act_val > -50);
		assert(b->settings.mon.act_val < 50);
		assert(b->settings.mon.deact_val > -50);
		assert(b->settings.mon.deact_val < 50);
		assert(b->settings.mon.act_val > b->settings.mon.deact_val);
		break;

	case MON_AVERAGE_CONDITION_LOW:
		assert(b->settings.mon.input1 != 0xFFFF);
		assert(b->settings.mon.input2 != 0xFFFF);
		assert(b->settings.mon.input3 == 0xFFFF);
		assert(b->settings.mon.input4 == 0xFFFF);
		assert(b->settings.mon.act_val > -50);
		assert(b->settings.mon.act_val < 50);
		assert(b->settings.mon.deact_val > -50);
		assert(b->settings.mon.deact_val < 50);
		assert(b->settings.mon.act_val < b->settings.mon.deact_val);
		break;

	case MON_AVERAGE_CONDITION_HIGH:
		assert(b->settings.mon.input1 != 0xFFFF);
		assert(b->settings.mon.input2 != 0xFFFF);
		assert(b->settings.mon.input3 == 0xFFFF);
		assert(b->settings.mon.input4 == 0xFFFF);
		assert(b->settings.mon.act_val > -50);
		assert(b->settings.mon.act_val < 50);
		assert(b->settings.mon.deact_val > -50);
		assert(b->settings.mon.deact_val < 50);
		assert(b->settings.mon.act_val > b->settings.mon.deact_val);
		break;

	case MON_TRIGGER:
		assert(b->settings.mon.input1 != 0xFFFF);
		assert(b->settings.mon.input2 == 0xFFFF);
		assert(b->settings.mon.input3 == 0xFFFF);
		assert(b->settings.mon.input4 == 0xFFFF);
		assert(b->settings.mon.act_val >= 0);
		assert(b->settings.mon.act_val <= 1);
		assert(b->settings.mon.deact_val >= 0);
		assert(b->settings.mon.deact_val <= 1);
		break;

	case SCH_START_STOP:
		break;

	case SCH_ONE_SHOT:
		break;

	case SCH_START_DURATION_REPEAT:
		break;

	case RL_LOGIC_ANDNOT:
		assert(b->settings.rl.param1 != 0xFFFF);
		assert(b->settings.rl.param2 != 0xFFFF);
		assert(b->settings.rl.param3 == 0xFFFF);
		assert(b->settings.rl.param_not != 0xFFFF);
		break;

	case RL_LOGIC_SINGLE:
		assert(b->settings.rl.param1 != 0xFFFF);
		assert(b->settings.rl.param2 == 0xFFFF);
		assert(b->settings.rl.param3 == 0xFFFF);
		assert(b->settings.rl.param_not == 0xFFFF);
		break;

	case RL_LOGIC_AND:
		assert(b->settings.rl.param1 != 0xFFFF);
		assert(b->settings.rl.param2 != 0xFFFF);
		assert(b->settings.rl.param3 == 0xFFFF);
		assert(b->settings.rl.param_not == 0xFFFF);
		break;

	case RL_LOGIC_SINGLENOT:
		assert(b->settings.rl.param1 != 0xFFFF);
		assert(b->settings.rl.param2 == 0xFFFF);
		assert(b->settings.rl.param3 == 0xFFFF);
		assert(b->settings.rl.param_not != 0xFFFF);
		break;

	case CON_ONOFF:
		break;

	case CON_SYSTEM:
		break;

	case OUT_DIGITAL:
		break;




	}




}


