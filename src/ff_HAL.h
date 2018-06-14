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
#include <SdFat.h>
#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h
//
// Pin numbers in templates must be constants.
const uint8_t SOFT_MISO_PIN = 12;
const uint8_t SOFT_MOSI_PIN = 11;
const uint8_t SOFT_SCK_PIN  = 13;
//
// Chip select may be constant or RAM variable.
const uint8_t SD_CHIP_SELECT_PIN = 10;

// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;

// Test file.
//SdFile file;
#else  // ENABLE_SOFTWARE_SPI_CLASS
#error ENABLE_SOFTWARE_SPI_CLASS must be set non-zero in SdFat/SdFatConfig.h
#endif  //ENABLE_SOFTWARE_SPI_CLASS


/************************************************
 Function Prototypes
************************************************/
uint8_t HALVEDirectInit(void);
int32_t HALReadVEData(uint16_t data_type);

uint8_t HALSaveEventBuffer(void);
uint8_t HALInitSerial(uint8_t port, uint16_t baudrate);
uint8_t HALEventSerialSend(EventNode* e, uint8_t port);

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
#ifdef FF_ARDUINO
uint8_t HALSetRTCTime(char *time_str);
uint8_t HALSetRTCDate(char *date_str);
#endif
void HALInitRTC(void);
void HALReboot(void);

#ifdef FF_ARDUINO
void HALDebugLCD(String log_entry);
#endif

#endif /* FF_HAL_H_ */
