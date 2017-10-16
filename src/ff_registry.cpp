/************************************************
 ff_registry.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Component Registry
************************************************/

/************************************************
 Includes
************************************************/
#include "ff_registry.h"
#include "ff_sys_config.h"
#include "ff_string_consts.h"

#ifdef FF_SIMULATOR
#include <string.h>
#endif


/************************************************
 Data Structures
************************************************/

typedef struct BLOCK_TYPE {
	char label[MAX_LABEL_LENGTH];
	uint8_t active;
} Block;

typedef struct FF_STATE_REGISTER {
	//system config ad flags
	uint8_t language;
	uint8_t temperature_scale;
	uint8_t time_real_status = 0;
	uint8_t FS_present = 0;
	uint8_t config_valid;
	uint8_t save_event_buffer = 0;

	//blocks

	uint8_t block_cat_offset[LAST_BLOCK_CAT];

	uint8_t block_list_size;


	uint8_t system_start;
	uint8_t system_count;

	uint8_t input_start;
	uint8_t input_count;

	uint8_t monitor_start;
	uint8_t monitor_count;

	uint8_t schedule_start;
	uint8_t schedule_count;

	uint8_t rule_start;
	uint8_t rule_count;

	uint8_t controller_start;
	uint8_t controller_count;

	uint8_t output_start;
	uint8_t output_count;

	UIDataSet ui_data;

	Block block_list[BLOCK_COUNT];
	//Block* dev_block_list;

} FFStateRegister;


/************************************************
 Globals
************************************************/

static FFStateRegister sr;

/************************************************
 Functions
************************************************/

char const* GetBlockLabelString(int idx) {
	return sr.block_list[idx].label;
}

void SetBlockLabelString(uint8_t block_cat, int idx, const char* label) {
	switch (block_cat) {
	case FF_SYSTEM:
		idx = idx + sr.system_start;
		break;
	case FF_INPUT:
		idx = idx + sr.input_start;
		break;
	case FF_MONITOR:
		idx = idx + sr.monitor_start;
		break;
	case FF_SCHEDULE:
		idx = idx + sr.schedule_start;
		break;
	case FF_RULE:
		idx = idx + sr.rule_start;
		break;
	case FF_CONTROLLER:
		idx = idx + sr.controller_start;
		break;
	case FF_OUTPUT:
		idx = idx + sr.output_start;
		break;
	}
	strcpy(sr.block_list[idx].label, label);
	sr.block_list[idx].active = 0;
}

uint8_t GetBlockIndexByLabel (const char * label) {
	int i = 0;
	while (i < sr.block_list_size && !strcmp(sr.block_list[i].label, label)) {
		i++;
	}
	return i;
}

uint8_t GetBlockTypeOffset (uint8_t block_type) {
	uint8_t offset;
	switch (block_type) {
	case FF_SYSTEM:
		offset=sr.system_start;
		break;
	case FF_INPUT:
		offset=sr.input_start;
		break;
	case FF_MONITOR:
		offset=sr.monitor_start;
		break;
	case FF_SCHEDULE:
		offset=sr.schedule_start;
		break;
	case FF_RULE:
		offset=sr.rule_start;
		break;
	case FF_CONTROLLER:
		offset=sr.controller_start;
		break;
	case FF_OUTPUT:
		offset=sr.output_start;
		break;
	}
	return offset;
}

UIDataSet* GetUIDataSet(void) {
	return &sr.ui_data;
}

void UpdateStateRegister(uint8_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val) {

	//TODO include further registry block logic update here

	//TODO optimise
	if (strcmp(sr.block_list[source].label, "INSIDE_TOP_TEMP") == 0) {
		sr.ui_data.inside_current = f_val;
		if (f_val < sr.ui_data.inside_min) {
			sr.ui_data.inside_min = f_val;
			sr.ui_data.inside_min_dt = FFDTNow();
		}
		if (f_val > sr.ui_data.inside_max) {
			sr.ui_data.inside_max = f_val;
			sr.ui_data.inside_max_dt = FFDTNow();
		}
	}

	if (strcmp(sr.block_list[source].label, "OUTSIDE_TEMP") == 0) {
		sr.ui_data.outside_current = f_val;
		if (f_val < sr.ui_data.outside_min) {
			sr.ui_data.outside_min = f_val;
			sr.ui_data.outside_min_dt = FFDTNow();
		}
		if (f_val > sr.ui_data.outside_max) {
			sr.ui_data.outside_max = f_val;
			sr.ui_data.outside_max_dt = FFDTNow();
		}
	}

	if (strcmp(sr.block_list[source].label, "WATER_TEMP") == 0) {
			sr.ui_data.water_current = f_val;
			if (f_val < sr.ui_data.water_min) {
				sr.ui_data.water_min = f_val;
				sr.ui_data.outside_min_dt = FFDTNow();
			}
			if (f_val > sr.ui_data.water_max) {
				sr.ui_data.water_max = f_val;
				sr.ui_data.water_max_dt = FFDTNow();
			}
	}

	if (strcmp(sr.block_list[source].label, "RESET_MIN_MAX") == 0) {
				sr.ui_data.inside_min = sr.ui_data.inside_current;
				sr.ui_data.inside_min_dt = FFDTNow();
				sr.ui_data.inside_max = sr.ui_data.inside_current;
				sr.ui_data.inside_max_dt = FFDTNow();

				sr.ui_data.outside_min = sr.ui_data.outside_current;
				sr.ui_data.outside_min_dt = FFDTNow();
				sr.ui_data.outside_max = sr.ui_data.outside_current;
				sr.ui_data.outside_max_dt = FFDTNow();

				sr.ui_data.water_min = sr.ui_data.water_current;
				sr.ui_data.water_min_dt = FFDTNow();
				sr.ui_data.water_max = sr.ui_data.water_current;
				sr.ui_data.water_max_dt = FFDTNow();
	}

	sr.ui_data.light_flag = 0;
	sr.ui_data.water_heater_flag = 0;
}

void InitStateRegister(void) {
	sr.language = ENGLISH;
	sr.temperature_scale = DEG_C;
	sr.time_real_status = 0; 	//false at this stage
	sr.FS_present = 0; 			//false at this stage
	sr.config_valid = 0;		//false at this stage
	sr.save_event_buffer = 0;

//TODO mod code due to config parsing
	sr.block_list_size = BLOCK_COUNT;
	sr.system_start = 0;
	sr.system_count = 1; 		//manual for now - ref sys_config define

	strcpy(sr.block_list[sr.system_start].label, "SYSTEM");
	strcpy(sr.block_list[sr.block_list_size-1].label, "LAST_BLOCK"); //size declared +1

	sr.input_start = sr.system_start + sr.system_count;
	sr.input_count = INPUT_COUNT;
	sr.monitor_start = sr.input_start + sr.input_count;
	sr.monitor_count = 0; ///for now
	sr.schedule_start = sr.monitor_start + sr.monitor_count;
	sr.schedule_count = 0; //for now
	sr.rule_start = sr.schedule_start + sr.schedule_count;
	sr.rule_count = 0; //for now
	sr.controller_start = sr.rule_start + sr.rule_count;
	sr.controller_count = CONTROL_COUNT;
	sr.output_start = sr.controller_start + sr.controller_count;
	sr.output_count = OUTPUT_COUNT;

//initialise the min and max counters used for UI display
	sr.ui_data.inside_current = 0;
	sr.ui_data.inside_min = 100;
	sr.ui_data.inside_min_dt = FFDTNow();
	sr.ui_data.inside_max = -50;
	sr.ui_data.inside_max_dt = FFDTNow();

	sr.ui_data.outside_current = 0;
	sr.ui_data.outside_min = 100;
	sr.ui_data.outside_min_dt = FFDTNow();
	sr.ui_data.outside_max = -50;
	sr.ui_data.outside_max_dt = FFDTNow();

	sr.ui_data.water_current = 0;
	sr.ui_data.water_min = 100;
	sr.ui_data.water_min_dt = FFDTNow();
	sr.ui_data.water_max = -50;
	sr.ui_data.water_max_dt = FFDTNow();

	//TODO these will go
	sr.ui_data.light_flag = 0;
	sr.ui_data.water_heater_flag = 0;
}






