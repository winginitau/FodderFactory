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

/************************************************
  Function prototypes
************************************************/
void InputOperate(BlockNode *b);
void InputSetup(BlockNode *b);
void InputShow(BlockNode *b, void(Callback(char *)));

#endif /* INPUTS_H_ */
