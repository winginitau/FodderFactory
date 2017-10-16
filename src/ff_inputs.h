/************************************************
 ff_inputs.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Input Processing
 ************************************************/
#ifndef FF_INPUTS_H_
#define FF_INPUTS_H_

/************************************************
 Includes
************************************************/
//#include "ff_sys_config.h"

//#ifdef FF_ARDUINO
//#include "Arduino.h"
//#endif

/************************************************
  Function prototypes
************************************************/

float InputCurrentFVal(char* label);
void SetupInputs(void);
void PollInputs(void);


#endif /* FF_INPUTS_H_ */
