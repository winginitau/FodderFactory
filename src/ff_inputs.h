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

#include "ff_registry.h"

//#ifdef FF_ARDUINO
//#include "Arduino.h"
//#endif

/************************************************
  Function prototypes
************************************************/
void InputOperate(BlockNode *b);
void InputSetup(BlockNode *b);


#endif /* FF_INPUTS_H_ */
