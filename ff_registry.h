/************************************************
 ff_registry.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Component Registry
************************************************/
#ifndef SRC_FF_REGISTRY_H_
#define SRC_FF_REGISTRY_H_

/************************************************
 Includes
************************************************/
#include "ff_datetime.h"

/************************************************
 Public and Forward Data Structure Declarations
************************************************/

typedef struct UI_DATA_SET {
	float inside_current;
	float inside_min;
	FFDateTime inside_min_dt;
	float inside_max;
	FFDateTime inside_max_dt;

	float outside_current;
	float outside_min;
	FFDateTime outside_min_dt;
	float outside_max;
	FFDateTime outside_max_dt;

	float water_current;
	float water_min;
	FFDateTime water_min_dt;
	float water_max;
	FFDateTime water_max_dt;

	uint8_t light_flag;				//current state of lights
	uint8_t water_heater_flag;		//current state of water heater
} UIDataSet;



/************************************************
 Function Prototypes
************************************************/

char const* GetBlockLabelString(int idx);

void SetBlockLabelString(uint8_t block_type, int idx, const char* label);

//uint8_t GetBlockIndexByLabel (const char * label);

//void SaveEventBuffer(void);

void UpdateStateRegister(uint8_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val);

uint8_t GetBlockTypeOffset (uint8_t block_type);

UIDataSet* GetUIDataSet(void);

void InitStateRegister(void);


#endif /* SRC_FF_REGISTRY_H_ */
