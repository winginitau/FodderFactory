/*****************************************************************
 Identifier.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#include "Identifier.h"


Identifier::Identifier() {
	IdentifierName[0] = '\0';
	Type = ID_TYPE_UNDEFINED;
}

Identifier::~Identifier() {
	// Auto-generated destructor stub
}
