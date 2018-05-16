/*****************************************************************
 Identifiers.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 15 Feb. 2018

 ******************************************************************/

#ifndef IDENTIFIERS_H_
#define IDENTIFIERS_H_


#include "config.h"
//#include "string_consts.h"
#include "Identifier.h"
#include "OutputBuffer.h"

#include <regex.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#include "Debug.h"
#endif

enum {
	WRITE_HEADER = 0,
	WRITE_CODE,
	WRITE_USER_CODE,
};



class Identifiers {
private:
	 Identifier ids[MAX_IDENTIFIERS];
	 int idents_count;
	 int idents_func_idx;

	 int GetIdxByLabel(char* label);
	 int GetIdxByInstance(char* instance_name);


public:
	Identifiers();
	virtual ~Identifiers();

	OutputBuffer output;

	int DEFINE_ident_map_count;
	int DEFINE_lookup_map_count;
	int DEFINE_func_map_count;
	int DEFINE_ident_member_count_temp;

	int NewIdent(char* identifier_name, int id_type);

	int AddMember(char* identifier_name, char* key, char* value);
	int AddMember(char* identifier_name, char* item);

	int GetSizeByIdentifierName(char* identifier_name);
	int GetSizeByInstanceName(char* instance_name);

	int SetInstanceName(char* identifier_name, char* instance_name);
	int GetInstanceName(char* identifier_name, char* instance_name);

	int GetXlatID(char* identifier_name);

	int GetEntryAtLocation(char* identifier_name, char* key_result, char* value_result, int location);
	int GetEntryAtLocation(char* identifier_name, char* string_result, int location);

	bool Exists(char* identifier_name);

	void WriteXLATMapArrays(void);
	void WriteXLATArrayOfTypeAndInstance(int type, char* instance_name);

	void WriteXLATMapLookupFunctions(int header_or_code);
	void WriteXLATMapLookupFuncOfTypeAndInstance(char* func_name, char* map_instance, int header_or_code, char* member_count_define);

	void WriteIdentMemberLookupFunction(int header_or_code);
	void WriteIdentMemberLookupCase(int case_num, char* string_array_instance, char* terminating_member_name);

	void WriteLookupMemberLookupFunction(int header_or_code);
	void WriteLookupMemberLookupCase(int case_num, char* function_name);

	void WriteUserLookupFunctions(int header_or_code);

	//void DEBUGDumpIdentifiers(void);

	bool IdentifierValid(char * ident);


};

#endif /* IDENTIFIERS_H_ */
