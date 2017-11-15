/************************************************
 ff_schedules.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Rule Processing
 ************************************************/
#ifndef FF_RULES_H_
#define FF_RULES_H_

/************************************************
 Includes
************************************************/
#include "ff_registry.h"

/************************************************
  Function prototypes
************************************************/

void RuleOperate(BlockNode *b);
void RuleSetup(BlockNode *b);


#endif /* FF_RULES_H_ */
