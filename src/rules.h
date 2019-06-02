/************************************************
 ff_schedules.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Rule Processing
 ************************************************/
#ifndef RULES_H_
#define RULES_H_

/************************************************
 Includes
************************************************/
#include <registry.h>

/************************************************
  Function prototypes
************************************************/

void RuleOperate(BlockNode *b);
void RuleSetup(BlockNode *b);


#endif /* RULES_H_ */
