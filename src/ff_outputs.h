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
//#include "ff_sys_config.h"

#ifdef FF_ARDUINO
//#include "Arduino.h"
#endif
#ifdef FF_SIMULATOR
//#include <stdint.h>
#endif

#include "ff_registry.h"

/************************************************
 Public and Forward Data Structure Declarations
************************************************/



/************************************************
 Function Prototypes
************************************************/

void OutputSetup(BlockNode *b);
void OutputOperate(BlockNode *b);

void SetOutputCommand(char* label, uint8_t command);

void SetupOutputs(void);

void PollOutputs(void);




#endif /* SRC_FF_OUTPUTS_H_ */
