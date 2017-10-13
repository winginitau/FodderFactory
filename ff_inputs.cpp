/************************************************
 ff_inputs.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Input Processing
 ************************************************/


/************************************************
  Includes
************************************************/
#include "ff_inputs.h"
#include "ff_sys_config.h"
#include "ff_HAL.h"
#include "ff_string_consts.h"
#include "ff_events.h"

//#include "ff_utils.h"
//#include "RTClib.h"
#ifdef FF_SIMULATOR
#include <string.h>
#endif

/************************************************
  Data Structures
************************************************/
typedef struct INPUT_BLOCK {
	uint8_t input_no;
	char label[MAX_LABEL_LENGTH];
	uint8_t device_num;
	//char descr[MAX_DESCR_LENGTH];
	uint8_t log_rate_day;
	uint8_t log_rate_hour;
	uint8_t log_rate_minute;
	uint8_t log_rate_second;
	//uint8_t data_units;
	int	current_i_val;
	float current_f_val;
	FFDateTime last_read;
} InputBlock;


/************************************************
  Globals
************************************************/
InputBlock inputs[INPUT_COUNT];

/************************************************
  Input Processing Functions
************************************************/

float InputCurrentFVal(char* label) {
	int i = 0;
	while (!strcmp(inputs[i].label, label)) {
		i++;
	}
	if (i < INPUT_COUNT) {
		return inputs[i].current_f_val;
	} else {
		EventMsg(SSS, ERROR, M_INPUT_LABEL_NOT_FOUND, 0, 0);
		return -0.00;
	}
}

void ProcessInput(int i) {
	TempSensorsTakeReading();
	inputs[i].last_read = FFDTNow();
	inputs[i].current_f_val = GetTemperature(inputs[i].device_num);
	EventMsg(i + GetBlockTypeOffset(FF_INPUT), DATA, M_F_READ, 0, inputs[i].current_f_val);
}

void PollInputs(void) {

	FFDateTime now, last, next, rate;

	for (int i= 0; i < INPUT_COUNT; i++) {
		rate.year = 0;
		rate.month = 0;
		rate.day = inputs[i].log_rate_day;
		rate.hour = inputs[i].log_rate_hour;
		rate.minute = inputs[i].log_rate_minute;
		rate.second = inputs[i].log_rate_second;

		last = inputs[i].last_read;
		next = FFDTAdd(last, rate);
		now = FFDTNow();

		if (FFDTGrEq(now, next)) {
			ProcessInput(i);
		}
	}
}


void SetupInputs(void) {
	//String tmpstr;

	InitTempSensors();

	EventMsg(SSS, INFO, M_TEMPS_INIT, 0, 0);

	int i = 0;
	//TODO temp until reading loop done
	//TODO - read a config file once we have SD memory
	inputs[i].input_no = I1_INPUT_NO;
	strcpy(inputs[i].label, I1_DEVICE_LABEL);
	inputs[i].device_num = I1_DEVICE_NUM;
	//strcpy(inputs[i].descr, I1_DESCR);
	inputs[i].log_rate_day = I1_LOG_RATE_DAY;
	inputs[i].log_rate_hour = I1_LOG_RATE_HOUR;
	inputs[i].log_rate_minute = I1_LOG_RATE_MINUTE;
	inputs[i].log_rate_second = I1_LOG_RATE_SECOND;
	//inputs[i].data_units = I1_DATA_UNITS;
	inputs[i].current_i_val = 0;
	inputs[i].current_f_val = 0.00;

	SetBlockLabelString(FF_INPUT, i, inputs[i].label);

	i = 1; 	//TODO temp until reading loop done

	inputs[i].input_no = I2_INPUT_NO;
	strcpy(inputs[i].label, I2_DEVICE_LABEL);
	inputs[i].device_num = I2_DEVICE_NUM;
	//strcpy(inputs[i].descr, I2_DESCR);
	inputs[i].log_rate_day = I2_LOG_RATE_DAY;
	inputs[i].log_rate_hour = I2_LOG_RATE_HOUR;
	inputs[i].log_rate_minute = I2_LOG_RATE_MINUTE;
	inputs[i].log_rate_second = I2_LOG_RATE_SECOND;
	//inputs[i].data_units = I2_DATA_UNITS;
	inputs[i].current_i_val = 0;
	inputs[i].current_f_val = 0.00;

	SetBlockLabelString(FF_INPUT, i, inputs[i].label);

	i = 2; 	//TODO temp until reading loop done

	inputs[i].input_no = I3_INPUT_NO;
	strcpy(inputs[i].label, I3_DEVICE_LABEL);
	inputs[i].device_num = I3_DEVICE_NUM;
	//strcpy(inputs[i].descr, I3_DESCR);
	inputs[i].log_rate_day = I3_LOG_RATE_DAY;
	inputs[i].log_rate_hour = I3_LOG_RATE_HOUR;
	inputs[i].log_rate_minute = I3_LOG_RATE_MINUTE;
	inputs[i].log_rate_second = I3_LOG_RATE_SECOND;
	//inputs[i].data_units = I3_DATA_UNITS;
	inputs[i].current_i_val = 0;
	inputs[i].current_f_val = 0.00;

	SetBlockLabelString(FF_INPUT, i, inputs[i].label);

	for (i = 0; i < INPUT_COUNT; i++) {
		ProcessInput(i);
	}

}












