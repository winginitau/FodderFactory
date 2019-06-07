/*****************************************************************
 Identifier.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#ifndef IDENTIFIER_H_
#define IDENTIFIER_H_

#include "config.h"
#include "KeyValuePairList.h"

enum {
	ID_TYPE_UNDEFINED = 0,
	ID_ENUM_ARRAY_PAIR,
	ID_ENUM_LIST,
	ID_LOOKUP_LIST,
	ID_ACTION_PAIR,
	LAST_IDENTIFIER_TYPE,
};

static const EnumStringArray identifier_types[LAST_IDENTIFIER_TYPE] = {
		"ID_TYPE_UNDEFINED",
		"ID_ENUM_ARRAY_PAIR",
		"ID_ENUM_LIST",
		"ID_LOOKUP_LIST",
		"ID_ACTION_PAIR",
};


class Identifier {
public:

	Identifier();
	virtual ~Identifier();

	char IdentifierName[MAX_BUFFER_LENGTH];
	char InstanceName[MAX_BUFFER_LENGTH];

	int Type;
	int func_xlat;
	bool action_built;

	KeyValuePairList KeyValueList;
	StringList SimpleList;
};

#endif /* IDENTIFIER_H_ */
