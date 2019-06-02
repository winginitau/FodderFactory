/************************************************
 ff_inputs.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Input Processing
 ************************************************/
#ifndef INPUTS_H_
#define INPUTS_H_

/************************************************
 Includes
************************************************/

#include <registry.h>

//#ifdef FF_ARDUINO
//#include "Arduino.h"
//#endif

/************************************************
  Function prototypes
************************************************/
void InputOperate(BlockNode *b);
void InputSetup(BlockNode *b);


#endif /* INPUTS_H_ */
