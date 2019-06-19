/*****************************************************************
 system.h

 Copyright (C) 2019 Brendan McLearie 

 Created on: 17 Jun 2019

******************************************************************/

#include <build_config.h>
#include <registry.h>

#ifndef SYSTEM_H_
#define SYSTEM_H_


void SystemSetup(BlockNode* b);
void SystemOperate(BlockNode* b);
void SystemShow(BlockNode *b, void(Callback(char *)));


#endif /* SYSTEM_H_ */
