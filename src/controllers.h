/************************************************
 ff_controllers.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Controller Processing
 ************************************************/
#ifndef SRC_FF_CONTROLLERS_H_
#define SRC_FF_CONTROLLERS_H_

#include <registry.h>

/************************************************
 Data Structures
 ************************************************/

//typedef struct CONTROL_BLOCK ControlBlock;

/************************************************
 Function prototypes
 ************************************************/

void ControllerSetup(BlockNode *b);
void ControllerOperate(BlockNode *b);
void ControllerShow(BlockNode *b, void(Callback(const char *)));


#endif /* SRC_FF_CONTROLLERS_H_ */
