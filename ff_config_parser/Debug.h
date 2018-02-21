/*****************************************************************
 Debug.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 16 Feb. 2018

 ******************************************************************/

#ifndef DEBUG_H_
#define DEBUG_H_

#include "common_config.h"

class Debug {
public:
	Debug();
	virtual ~Debug();

	char* GetFromStringArray(char* dest, const EnumStringArray* arr, int n);

};

#endif /* DEBUG_H_ */
