/*****************************************************************
 Identifier.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#ifndef IDENTIFIER_H_
#define IDENTIFIER_H_

#include "config.h"
#include "KeyValuePairList.h"

class Identifier {
public:

	Identifier();
	virtual ~Identifier();

	char IdentifierName[MAX_LINE_LENGTH];
	char InstanceName[MAX_LINE_LENGTH];

	int Type;

	KeyValuePairList KeyValueList;
	StringList SimpleList;
};

#endif /* IDENTIFIER_H_ */
