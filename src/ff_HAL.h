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
#include "ff_datetime.h"
#include "ff_events.h"
/************************************************
 Public and Forward Data Structure Declarations
************************************************/


/************************************************
 Function Prototypes
************************************************/
uint8_t HALSaveEventBuffer(void);

uint8_t HALInitSerial(uint8_t port, uint16_t baudrate);

uint8_t HALEventSerialSend(EventNode* e, uint8_t port, uint16_t baudrate);

#ifdef USE_ITCH
void HALInitItch(void);
void HALPollItch(void);
void HALItchWriteLnImmediate(char *str);
#endif

uint8_t HALDigitalRead(uint8_t if_num);

void HALInitDigitalInput(uint8_t if_num);
void HALDigitalWrite (uint8_t if_num, uint8_t digital_val);
void HALInitDigitalOutput (uint8_t if_num);
float GetTemperature(int if_num);
void TempSensorsTakeReading(void);
void InitTempSensors(void);

#ifdef UI_ATTACHED
void HALInitUI(void);
void HALDrawDataScreenCV(const UIDataSet* uids, time_t dt);
//void HALDrawDataScreen(const UIDataSet* uids, time_t dt);
#endif

//FFDateTime HALFFDTNow(void);
time_t TimeNow(void);
void HALInitRTC(void);

#ifdef FF_ARDUINO
void HALDebugLCD(String log_entry);
#endif

#endif /* FF_HAL_H_ */
