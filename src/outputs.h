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

#include <registry.h>

/************************************************
 Public and Forward Data Structure Declarations
************************************************/


/************************************************
 Function Prototypes
************************************************/

void OutputSetup(BlockNode *b);
void OutputOperate(BlockNode *b);
void OutputShow(BlockNode *b, void(Callback(const char *)));

#endif /* SRC_FF_OUTPUTS_H_ */
