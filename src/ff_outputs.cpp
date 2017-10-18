/************************************************
 ff_outputs.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

  Output Processing
 ************************************************/

/************************************************
  Includes
************************************************/
#include "ff_outputs.h"
#include "ff_sys_config.h"
#include "ff_events.h"
#include "ff_registry.h"
#include "ff_string_consts.h"
#include "ff_HAL.h"

#ifdef FF_SIMULATOR
#include <string.h>
#include <stdio.h>
#endif
/************************************************
  Data Structures
************************************************/

typedef struct OUTPUT_BLOCK {
	uint8_t output_no;
	char label[MAX_LABEL_LENGTH];
	uint8_t device_num;
	//char descr[MAX_DESCR_LENGTH];
	uint8_t digital_pin;
	uint8_t current_state;
	FFDateTime last_change;
} OutputBlock;

/************************************************
  Globals
************************************************/

static OutputBlock outputs[OUTPUT_COUNT];

static int outputs_command[OUTPUT_COUNT];

/************************************************
  Functions
************************************************/

void SetOutputCommand(char* label, uint8_t command) {
	uint8_t i = 0;
	//printf("%s\n", label);
	//printf("%s\n", outputs[i].label);
	//printf("%d\n", strcmp(outputs[i].label, label));
	while (strcmp(outputs[i].label, label)) {
		i++;
		if (i >= OUTPUT_COUNT) {
			break;
		}
	}
	if (i < OUTPUT_COUNT) {
		//printf("%s\n", label);
		//printf("%s\n", outputs[i].label);
		//printf("i:%d  c:%d\n", i, command);
		outputs_command[i] = command;
	} else {
		EventMsg(SSS, ERROR, M_OUTPUT_LABEL_NOT_FOUND, 0, 0);
	}
}

void ProcessOutput(uint8_t i) {
	if (outputs_command[i] != outputs[i].current_state) {
		if (outputs_command[i] == OUTPUT_ON) {
			HALDigitalWrite (outputs[i].digital_pin, DIG_HIGH);
			outputs[i].current_state = 1;
			EventMsg(i + GetBlockTypeOffset(FF_OUTPUT), SET, M_ON, 0, 0);
		} else {
			HALDigitalWrite (outputs[i].digital_pin, DIG_LOW);
			EventMsg(i + GetBlockTypeOffset(FF_OUTPUT), SET, M_OFF, 0, 0);
			outputs[i].current_state = 0;
		}
		outputs[i].last_change = FFDTNow();
	}
}

void SetupOutputs(void) {
	uint8_t i = 0;
	//TODO temp until reading loop done
	//TODO - read a config file once we have SD memory

	outputs[i].output_no = OUT1_OUTPUT_NO;
	strcpy(outputs[i].label, OUT1_DEVICE_LABEL);
	outputs[i].device_num = OUT1_DEVICE_NUM;
	//strcpy(outputs[i].descr, OUT1_DESCR);
	outputs[i].digital_pin = OUT1_DIGITAL_PIN;

	outputs[i].current_state = 1; //set on to trigger to off as a detected change via command TODO Revisit //Move to BLOCK
	outputs[i].last_change = FFDTNow();

	HALInitDigitalOutput(outputs[i].digital_pin);
	outputs_command[i] = 0; //TODO move to BLOCK

	SetBlockLabelString(FF_OUTPUT, i, outputs[i].label);

	i = 1; 	//TODO temp until reading loop done

	outputs[i].output_no = OUT2_OUTPUT_NO;
	strcpy(outputs[i].label, OUT2_DEVICE_LABEL);
	outputs[i].device_num = OUT2_DEVICE_NUM;
	//strcpy(outputs[i].descr, OUT2_DESCR);
	outputs[i].digital_pin = OUT2_DIGITAL_PIN;

	outputs[i].current_state = 1; //set on to trigger to off as a detected change via command
	outputs[i].last_change = FFDTNow();

	HALInitDigitalOutput(outputs[i].digital_pin);
	outputs_command[i] = 0;

	SetBlockLabelString(FF_OUTPUT, i, outputs[i].label);

	EventMsg(SSS, INFO, M_SETTING_OUTPUTS, 0, 0);
	for (i = 0; i < OUTPUT_COUNT; i++) {
		ProcessOutput(i);
	}
	EventMsg(SSS, INFO, M_OUTPUTS_INIT, 0, 0);
}

void PollOutputs (void) {
	for (uint8_t i = 0; i < OUTPUT_COUNT; i++) {
		ProcessOutput(i);
	}
}
