/************************************************
 ff_ini_config.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Config INI Functions
 ************************************************/
#ifndef FF_INI_CONFIG_H_
#define FF_INI_CONFIG_H_

/************************************************
 Includes
************************************************/

//#include "ff_registry.h"

//#ifdef FF_ARDUINO
//#include "Arduino.h"
//#endif

#include <stdint.h>

/************************************************
  Function prototypes
************************************************/


void ReadAndParseConfig(void);
void WriteRunningConfig(void);
uint8_t ConfigureBlock(uint8_t block_cat, const char *block_label, const char *key_str, const char *value_str);




#endif /* FF_INI_CONFIG_H_ */
