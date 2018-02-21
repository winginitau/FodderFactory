/*****************************************************************
 Identifiers.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 15 Feb. 2018

 ******************************************************************/

#ifndef IDENTIFIERS_H_
#define IDENTIFIERS_H_

#include "Identifier.h"
#include "OutputBuffer.h"

#define IDENTIFIER_REGEX "^[a-zA-Z0-9_]{1,31}$"

class Identifiers {
private:
	 Identifier ids[MAX_IDENTIFIERS];
	 int count;

	 int GetIdxByLabel(char* label);



public:
	Identifiers();
	virtual ~Identifiers();

	OutputBuffer output;

	int NewIdent(char* identifier_name, int id_type);
	int Add(char* identifier_name, char* key, char* value);
	int Add(char* identifier_name, char* item);
	int GetSize(char* identifier_name);
	int SetInstanceName(char* identifier_name, char* instance_name);
	int GetInstanceName(char* identifier_name, char* instance_name);
	int GetEntryAtLocation(char* identifier_name, char* key_result, char* value_result, int location);
	int GetEntryAtLocation(char* identifier_name, char* string_result, int location);
	bool Exists(char* identifier_name);
	void DumpIdentifiers(void);
	bool IdentifierValid(char * ident);


};

#endif /* IDENTIFIERS_H_ */
