/************************************************
 ff_ini_config.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Config INI Functions
 ************************************************/
#ifndef INIT_CONFIG_H_
#define INIT_CONFIG_H_

/************************************************
 Includes
************************************************/

#include <stdint.h>


/************************************************
  Function prototypes
************************************************/

void InitSystem(void);

void ConfigParse(char* buf);
void InitConfigLoad(uint8_t);

void InitConfigSave(void);
#ifdef RESURRECT_DEPRECIATED
void InitConfigSaveBinary(void);
void InitConfigLoadINI(void);
void InitConfigLoadBinary(void);
#endif
uint8_t ConfigureBlock(uint8_t block_cat, const char *block_label, const char *key_str, const char *value_str);



#endif /* INIT_CONFIG_H_ */
