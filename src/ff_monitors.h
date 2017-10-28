/************************************************
 ff_monitors.h

 FF System Core (Stand-alone Embedded)

 (c) Brendan McLearie - bmclearie@mclearie.com

 Monitor Block Routines

 ************************************************/

#ifndef FF_MONITORS_H_
#define FF_MONITORS_H_

/************************************************
 Includes
************************************************/
#include "ff_registry.h"

/************************************************
 Public and Forward Data Structure Declarations
************************************************/



/************************************************
 Function Prototypes
************************************************/

void MonitorSetup(BlockNode *b);
void MonitorOperate(BlockNode *b);


#endif /* FF_MONITORS_H_ */
