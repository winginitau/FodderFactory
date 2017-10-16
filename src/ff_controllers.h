/************************************************
 ff_controllers.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Controller Processing
 ************************************************/
#ifndef SRC_FF_CONTROLLERS_H_
#define SRC_FF_CONTROLLERS_H_



#ifdef FF_ARDUINO
//#include <Arduino.h>
#endif

//#include "ff_sys_config.h"
//#include "ff_datetime.h"


/************************************************
 Data Structures
 ************************************************/

//typedef struct CONTROL_BLOCK ControlBlock;

/************************************************
 Function prototypes
 ************************************************/
void SetupControllers(void);
void RunControllers(void);



#endif /* SRC_FF_CONTROLLERS_H_ */