/************************************************
 ff_HAL.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Hardware Abstraction Layer (HAL)
************************************************/
#ifndef HAL_H_
#define HAL_H_

/************************************************
 Includes
************************************************/
#ifdef PLATFORM_LINUX
#include <stdint.h>
#endif

#include <registry.h>
#include <datetime_ff.h>
#include <events.h>
/************************************************
 Public and Forward Data Structure Declarations
************************************************/



/************************************************
 Function Prototypes
************************************************/

uint8_t HALSaveEventBuffer(void);
uint8_t HALInitSerial(uint8_t port, uint16_t baudrate);
uint8_t HALEventSerialSend(EventNode* e, uint8_t port);

#ifdef USE_ITCH
void HALInitItch(void);
void HALPollItch(void);
void HALItchWriteLnImmediate(const char *str);
void HALItchWriteLnImmediate(char *str);
void HALItchSetBufferStuffMode(void);
void HALItchSetTextDataMode(void);
void HALItchStuffAndProcess(char *str);
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
#ifdef PLATFORM_ARDUINO
uint8_t HALSetRTCTime(char *time_str);
uint8_t HALSetRTCDate(char *date_str);
#endif //PLATFORM_ARDUINO
void HALInitRTC(void);
void HALReboot(void);

#ifdef PLATFORM_ARDUINO
#ifdef DEBUG_LCD
void HALDebugLCD(String log_entry);
#endif //DEBUG_LCD
#endif //PLATFORM_ARDUINO

#endif /* HAL_H_ */
