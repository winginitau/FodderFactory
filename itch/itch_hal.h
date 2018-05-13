/*****************************************************************
 itch_hal.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 9 Apr. 2018

******************************************************************/

#ifndef ITCH_HAL_H_
#define ITCH_HAL_H_

#include <stdio.h>

char *strcpy_misc(char *dest, uint8_t src);
char *strcat_misc(char *dest, uint8_t src);
char *strcpy_hal(char *dest, const char *src);
char *strcat_hal(char *dest, const char *src);
void *memcpy_hal(void *dest, const void *src, size_t sz);

#ifdef ITCH_DEBUG
char *strcpy_debug(char *dest, uint8_t src);
char *strcat_debug(char *dest, uint8_t src);
#endif


#endif /* ITCH_HAL_H_ */
