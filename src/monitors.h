/************************************************
 ff_monitors.h

 FF System Core (Stand-alone Embedded)

 (c) Brendan McLearie - bmclearie@mclearie.com

 Monitor Block Routines

 ************************************************/

#ifndef MONITORS_H_
#define MONITORS_H_

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

void MonitorSetup(BlockNode *b);
void MonitorOperate(BlockNode *b);


#endif /* MONITORS_H_ */
