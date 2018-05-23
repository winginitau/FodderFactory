/*****************************************************************
 itch_hal.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 9 Apr. 2018

******************************************************************/

#ifndef ITCH_HAL_H_
#define ITCH_HAL_H_

#include <stdio.h>

char *strcpy_itch_misc(char *dest, uint8_t src);
char *strcat_itch_misc(char *dest, uint8_t src);
char *strcpy_itch_hal(char *dest, const char *src);
char *strcat_itch_hal(char *dest, const char *src);
void *memcpy_itch_hal(void *dest, const void *src, size_t sz);

#ifdef ITCH_DEBUG
char *strcpy_itch_debug(char *dest, uint8_t src);
char *strcat_itch_debug(char *dest, uint8_t src);
#endif


#endif /* ITCH_HAL_H_ */
