/************************************************
 ff_schedules.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Schedule Processing
 ************************************************/
#ifndef FF_SCHEDULES_H_
#define FF_SCHEDULES_H_

/************************************************
 Includes
************************************************/
#include "ff_registry.h"

/************************************************
  Function prototypes
************************************************/

void ScheduleOperate(BlockNode *b);
void ScheduleSetup(BlockNode *b);


#endif /* FF_SCHEDULES_H_ */
