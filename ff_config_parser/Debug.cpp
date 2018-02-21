/*****************************************************************
 Debug.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 16 Feb. 2018

 ******************************************************************/

#include "Debug.h"
#include <string.h>

Debug::Debug() {
	// Auto-generated constructor stub

}

Debug::~Debug() {
	// Auto-generated destructor stub
}

char* Debug::GetFromStringArray(char* dest, const EnumStringArray* arr, int n) {
	return strcpy(dest, &(*arr[n].text));
}
