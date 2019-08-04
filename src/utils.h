/************************************************
 ff_utils.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Utility and System Functions
 ************************************************/
#ifndef UTILS_H_
#define UTILS_H_

/************************************************
 Includes
 ************************************************/

#include <build_config.h>

#ifdef PLATFORM_ARDUINO
#include <Arduino.h>
#endif //PLATFORM_ARDUINO


#ifdef PLATFORM_LINUX
#include <stdint.h>
#endif

/************************************************
 Data Structures
 ************************************************/



/************************************************
 Function prototypes
 ************************************************/

//cool macro for easier string handling
//#define WriteMessageType(X) WriteMessageType(GetMessageTypeString((X)))

uint8_t VarianceExceedsPercent(int32_t old_val, int32_t new_val, uint8_t thres_pc);
uint8_t VarianceExceedsAbsolute(int32_t old_val, int32_t new_val, uint16_t thres_abs);

uint8_t DayStrToFlag(uint8_t day_flag[7], const char* day_str);
char* FFFloatToCString(char* buf, float f);
char* FlagToDayStr(char* day_str, uint8_t day_flag[7]);
char* CTimeToISODateTimeString(char *iso_str, time_t t);
char* CTimeToLocalTimeString(char *t_str, time_t t);

uint32_t StringToTimeValue(const char* time_str);
char* TimeValueToTimeString(char *HMSStr, const time_t tv);
uint32_t NextTime(uint32_t sch_start, uint32_t sch_repeat);
uint32_t LastTime(uint32_t sch_start, uint32_t sch_repeat);

uint8_t DallasAddressStringToArray(const char *addr_str, uint8_t addr[8]);
char* DallasAddressArrayToString(char *addr_str, uint8_t addr[8]);
void DallasCopy(uint8_t s[8], uint8_t d[8]);
uint8_t DallasCompare(uint8_t a1[8], uint8_t a2[8]);

char *strpad(char *s, uint8_t w);
char *strcatpad(char *d, const char *s, uint8_t w);
char *strcatpad_hal(char *d, const char *s, uint8_t w);
#ifdef PLATFORM_ARDUINO
char *strcatpad_hal(char *d, const __FlashStringHelper *s, uint8_t w);
#endif //PLATFORM_ARDUINO

#endif /* UTILS_H_ */
