/************************************************
 ff_outputs.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Output Processing
 ************************************************/

#ifndef SRC_FF_OUTPUTS_H_
#define SRC_FF_OUTPUTS_H_

/************************************************
 Includes
************************************************/
#include "ff_sys_config.h"

#ifdef FF_ARDUINO
#include "Arduino.h"
#endif
#ifdef FF_SIMULATOR
#include <stdint.h>
#endif

/************************************************
 Public and Forward Data Structure Declarations
************************************************/



/************************************************
 Function Prototypes
************************************************/

void SetOutputCommand(char* label, uint8_t command);

void SetupOutputs(void);

void PollOutputs(void);




#endif /* SRC_FF_OUTPUTS_H_ */
