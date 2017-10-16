/************************************************
 ff_HAL.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Hardware Abstraction Layer (HAL)
************************************************/
#ifndef FF_HAL_H_
#define FF_HAL_H_

/************************************************
 Includes
************************************************/
#ifdef FF_SIMULATOR
#include <stdint.h>
#endif

#include "ff_registry.h"
/************************************************
 Public and Forward Data Structure Declarations
************************************************/


/************************************************
 Function Prototypes
************************************************/
uint8_t HALSaveEventBuffer(void);
void HALDigitalWrite (uint8_t if_num, uint8_t digital_val);
void HALInitDigitalOutput (uint8_t if_num);
float GetTemperature(int if_num);
void TempSensorsTakeReading(void);
void InitTempSensors(void);
void HALInitUI(void);
void HALDrawDataScreenCV(const UIDataSet* uids, FFDateTime dt);
void HALDrawDataScreen(const UIDataSet* uids, FFDateTime dt);
FFDateTime HALFFDTNow(void);
void HALInitRTC(void);

#ifdef FF_ARDUINO
void HALDebugLCD(String log_entry);
#endif

#endif /* FF_HAL_H_ */
