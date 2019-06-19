/*****************************************************************
 block_common.h

 Copyright (C) 2019 Brendan McLearie 

 Created on: 15 Jun 2019

 Functions to operate on the common section of all
 block categories and types.

******************************************************************/

#ifndef BLOCK_COMMON_H_
#define BLOCK_COMMON_H_

/************************************************
 Includes
************************************************/

#include <registry.h>

/************************************************
  Function prototypes
************************************************/
void InputOperate(BlockNode *b);
void InputSetup(BlockNode *b);

void CommonShow(BlockNode *b, void(Callback(char *)));


#endif /* BLOCK_COMMON_H_ */
