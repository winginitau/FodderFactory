/************************************************
 ff_schedules.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Schedule Processing
 ************************************************/
#ifndef SCHEDULES_H_
#define SCHEDULES_H_

/************************************************
 Includes
************************************************/
#include <registry.h>

/************************************************
  Function prototypes
************************************************/

void ScheduleOperate(BlockNode *b);
void ScheduleSetup(BlockNode *b);


#endif /* SCHEDULES_H_ */
