/*****************************************************************
 Identifiers.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 15 Feb. 2018

 ******************************************************************/

#include "Identifiers.h"
#include <regex.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//#ifdef DEBUG
#include "Debug.h"
#include "glitch_errors.h"
Debug debug;
//#endif

Identifiers::Identifiers() {
	idents_count = 0;
	idents_func_idx = 0;

}

Identifiers::~Identifiers() {
	// Auto-generated destructor stub
}

int Identifiers::NewIdent(char* reserved_name, int id_type) {
	int idx;

	if (IdentifierValid(reserved_name)) {

		idx = idents_count;
		strcpy(ids[idx].IdentifierName, reserved_name);
		ids[idx].Type = id_type;
		idents_count++;
		if (id_type == ID_ACTION_PAIR) {
			ids[idx].func_xlat = idents_func_idx;
			idents_func_idx++;
		}
		return E_NO_ERROR;
	} else
		return E_INVALID_IDENTIFIER;
}

int Identifiers::Add(char* identifier_name, char* key, char* value) {
	int result = E_NO_ERROR;
	int idx = GetIdxByLabel(identifier_name);
	if (idx < idents_count) {
		switch (ids[idx].Type) {
			case ID_ENUM_ARRAY_PAIR:
			case ID_ACTION_PAIR:
				result = ids[idx].KeyValueList.Add(key, value);
				break;
			case ID_ENUM_LIST:
			case ID_LOOKUP_LIST:
				result = ids[idx].SimpleList.AddString(key);
				break;
		}

	} else {
		result = E_IDENTIFIER_NOT_FOUND_IN_LIST_OR_PAIR;
	}
	return result;
}

int Identifiers::Add(char* identifier_name, char* item) {
	return Add(identifier_name, item, NULL);
}

int Identifiers::GetSize(char* identifier_name) {
	int idx = GetIdxByLabel(identifier_name);
	switch (ids[idx].Type) {
		case ID_ENUM_ARRAY_PAIR:
		case ID_ACTION_PAIR:
			return ids[idx].KeyValueList.GetSize();
			break;
		case ID_ENUM_LIST:
		case ID_LOOKUP_LIST:
			return ids[idx].SimpleList.GetSize();
			break;
	}
	return -1;
}

int Identifiers::GetIdxByLabel(char* label) {
	int idx = 0;

	while (idx < idents_count) {
		if (strcmp(ids[idx].IdentifierName, label) == 0) { //found
			return idx;
		}
		idx++;
	}
	return idx;
}

int Identifiers::SetInstanceName(char* identifier_name, char* instance_name) {
	int idx = 0;
		while (idx < idents_count) {
			if (strcmp(ids[idx].IdentifierName, identifier_name) == 0) { //found
				strcpy(ids[idx].InstanceName, instance_name);
				return E_NO_ERROR;
			}
			idx++;
		}
		return E_IDENTIFER_NAME_NOT_FOUND;
}

int Identifiers::GetInstanceName(char* identifier_name, char* instance_name) {
	int idx = 0;
		while (idx < idents_count) {
			if (strcmp(ids[idx].IdentifierName, identifier_name) == 0) { //found
				strcpy(instance_name, ids[idx].InstanceName);
				return E_NO_ERROR;
			}
			idx++;
		}
		return E_IDENTIFER_NAME_NOT_FOUND;
}

int Identifiers::GetXlatID(char* identifier_name) {
int idx = 0;
	while (idx < idents_count) {
		if (strcmp(ids[idx].IdentifierName, identifier_name) == 0) { //found
			return ids[idx].func_xlat;
		}
		idx++;
	}
	return E_IDENTIFER_NAME_NOT_FOUND;
}


int Identifiers::GetEntryAtLocation(char* identifier_name, char* key_result, char* value_result, int location) {
	int idx = 0;
		while (idx < idents_count) {
			if (strcmp(ids[idx].IdentifierName, identifier_name) == 0) { //found
				ids[idx].KeyValueList.GetPairAtLocation(key_result, value_result, location);
				return E_NO_ERROR;
			}
			idx++;
		}
		return E_IDENTIFER_NAME_NOT_FOUND;
}

int Identifiers::GetEntryAtLocation(char* identifier_name, char* string_result, int location) {
	int idx = 0;
		while (idx < idents_count) {
			if (strcmp(ids[idx].IdentifierName, identifier_name) == 0) { //found
				ids[idx].SimpleList.GetStringAtLocation(string_result, location);
				return E_NO_ERROR;
			}
			idx++;
		}
		return E_IDENTIFER_NAME_NOT_FOUND;
}

bool Identifiers::Exists(char* identifier_name) {
	int idx = 0;
		while (idx < idents_count) {
			if (strcmp(ids[idx].IdentifierName, identifier_name) == 0) {
				return true; //found
			}
			idx++;
		}
		return false;
}


void Identifiers::WriteIdentMap(int type, char* instance_name) {
	int idx;
	int xlat_idx;
	int count;
	char out[MAX_BUFFER_LENGTH];

	// count number of enum idents - ID_ENUM_ARRAY_PAIR
	idx = 0;
	count = 0;
	while (idx < idents_count) {
		if (ids[idx].Type == type) {
			count++;
		}
		idx++;
	}

	// write out the enum indentifers xlat array
	// header
	sprintf(out, "#ifdef USE_PROGMEM\n");
	output.EnQueue(out);
	sprintf(out, "static const XLATMap %s [%d] PROGMEM = {\n", instance_name, count);
	output.EnQueue(out);
	sprintf(out, "#else\n");
	output.EnQueue(out);
	sprintf(out, "static const XLATMap %s [%d] = {\n", instance_name, count);
	output.EnQueue(out);
	sprintf(out, "#endif\n");
	output.EnQueue(out);
	// entries
	xlat_idx = 0;
	idx = 0;
	while (idx < idents_count) {
		if (ids[idx].Type == type) {
			if (type == ID_ACTION_PAIR) {
				sprintf(out, "\t\"%s\", %d,\n", ids[idx].IdentifierName, ids[idx].func_xlat);
			} else {
				sprintf(out, "\t\"%s\", %d,\n", ids[idx].InstanceName, xlat_idx);
			}

			output.EnQueue(out);
			xlat_idx++;
		}
		idx++;
	}
	sprintf(out, "};\n\n");
	output.EnQueue(out);
}

void Identifiers::WriteIdentifierMaps(void) {

	char temp[MAX_BUFFER_LENGTH];

	strcpy(temp, "ident_map");
	WriteIdentMap(ID_ENUM_ARRAY_PAIR, temp);
	strcpy(temp, "lookup_map");
	WriteIdentMap(ID_LOOKUP_LIST, temp);
	strcpy(temp, "func_map");
	WriteIdentMap(ID_ACTION_PAIR, temp);

	output.SetOutputAvailable();
}

void Identifiers::WriteIdentMapFunc(char* func_name, char* map_instance, int header_or_code) {
	char out[MAX_BUFFER_LENGTH];

	if (header_or_code == WRITE_HEADER) {
		sprintf(out, "uint16_t %s (char* key);\n", func_name);
		output.EnQueue(out);
		return;
	}

	sprintf(out, "uint16_t %s (char* key) {\n", func_name);
	output.EnQueue(out);
	sprintf(out, "\tuint16_t count;\n");
	output.EnQueue(out);
	sprintf(out, "\tuint16_t idx = 0;\n");
	output.EnQueue(out);
	sprintf(out, "\tcount = sizeof(%s) / sizeof(XLATMap);\n", map_instance);
	output.EnQueue(out);
	sprintf(out, "\twhile (idx < count) {\n");
	output.EnQueue(out);
	sprintf(out, "\t\tif(strcasecmp(%s[idx].label, key) == 0) {\n", map_instance);
	output.EnQueue(out);
	sprintf(out, "\t\t\treturn %s[idx].xlat_id;\n", map_instance);
	output.EnQueue(out);
	sprintf(out, "\t\t}\n");
	output.EnQueue(out);
	sprintf(out, "\t\tidx++;\n");
	output.EnQueue(out);
	sprintf(out, "\t}\n");
	output.EnQueue(out);
	sprintf(out, "\treturn -1;\n");
	output.EnQueue(out);
	sprintf(out, "}\n\n");
	output.EnQueue(out);
}

void Identifiers::WriteIdentMapFunctions(int header_or_code) {

	char func_name[MAX_BUFFER_LENGTH];
	char instance_name[MAX_BUFFER_LENGTH];

	strcpy(func_name, "LookupIdentMap");
	strcpy(instance_name, "ident_map");
	WriteIdentMapFunc(func_name, instance_name, header_or_code);

	strcpy(func_name, "LookupLookupMap");
	strcpy(instance_name, "lookup_map");
	WriteIdentMapFunc(func_name, instance_name, header_or_code);

	strcpy(func_name, "LookupFuncMap");
	strcpy(instance_name, "func_map");
	WriteIdentMapFunc(func_name, instance_name, header_or_code);

	output.SetOutputAvailable();
}

void Identifiers::WriteIdentMemberLookupCase(int case_num, char* string_array_instance) {
	char out[MAX_BUFFER_LENGTH];

	sprintf(out, "		case %d: {\n", case_num);
	output.EnQueue(out);
	sprintf(out, "			count = sizeof(%s) / sizeof(SimpleStringArray);\n", string_array_instance);
	output.EnQueue(out);
	sprintf(out, "			while (idx < count) {\n");
	output.EnQueue(out);
	sprintf(out, "				if(strcasecmp(%s[idx].text, lookup_string) == 0) {\n", string_array_instance);
	output.EnQueue(out);
	sprintf(out, "					return idx;\n");
	output.EnQueue(out);
	sprintf(out, "				}\n");
	output.EnQueue(out);
	sprintf(out, "				idx++;\n");
	output.EnQueue(out);
	sprintf(out, "			}\n");
	output.EnQueue(out);
	sprintf(out, "		}\n");
	output.EnQueue(out);
	sprintf(out, "			break;\n");
	output.EnQueue(out);
}

void Identifiers::WriteIdentMemberLookupFunction(int header_or_code) {
	char out[MAX_BUFFER_LENGTH];
	int case_idx = 0;
	int idx = 0;

	if (header_or_code == WRITE_HEADER) {
		sprintf(out, "uint16_t LookupIdentifierMembers(uint16_t ident_xlat, char* lookup_string);\n");
		output.EnQueue(out);
		output.SetOutputAvailable();
		return;
	}

	// write definition and preamble
	sprintf(out, "uint16_t LookupIdentifierMembers(uint16_t ident_xlat, char* lookup_string) {\n");
	output.EnQueue(out);
	sprintf(out, "	uint16_t count;\n");
	output.EnQueue(out);
	sprintf(out, "	uint16_t idx = 0;\n");
	output.EnQueue(out);
	sprintf(out, "	switch(ident_xlat) {\n");
	output.EnQueue(out);

	while (idx < idents_count) {
		if (ids[idx].Type == ID_ENUM_ARRAY_PAIR) {
			WriteIdentMemberLookupCase(case_idx, ids[idx].InstanceName);
			case_idx++;
		}
		idx++;
	}

	sprintf(out, "		default:\n");
	output.EnQueue(out);
	sprintf(out, "		return 0;\n");
	output.EnQueue(out);
	sprintf(out, "	}\n");
	output.EnQueue(out);
	sprintf(out, "	return 0;\n");
	output.EnQueue(out);
	sprintf(out, "}\n\n");
	output.EnQueue(out);

	//sprintf(out, "uint16_t LookupLookupMembers(uint16_t ident_xlat, char* lookup_string) {\n");
	//sprintf(out, "uint16_t LookupFunctionParams? (uint16_t ident_xlat, char* lookup_string) {\n");

	output.SetOutputAvailable();
}


void Identifiers::DEBUGDumpIdentifiers(void) {
	int idx;
	char out[MAX_BUFFER_LENGTH];
	char temp[MAX_BUFFER_LENGTH];
	char temp2[MAX_BUFFER_LENGTH];

	// dump all for debug
	idx = 0;
	while (idx < idents_count) {
		sprintf(out, "//IdentifierName: %s\n", ids[idx].IdentifierName);
		output.EnQueue(out);
		debug.GetFromStringArray(temp, identifier_types, ids[idx].Type);
		sprintf(out, "//Type \t\t%d\t%s\n", ids[idx].Type, temp);
		output.EnQueue(out);
		sprintf(out, "//InstanceName: \t%s\n", ids[idx].InstanceName);
		output.EnQueue(out);
		switch (ids[idx].Type) {
			case ID_ENUM_ARRAY_PAIR:
			case ID_ACTION_PAIR:
				// these have key-value-pairs
				for (int i = 0; i < ids[idx].KeyValueList.GetSize(); i++) {
					ids[idx].KeyValueList.GetPairAtLocation(temp, temp2, i);
					sprintf(out, "//K: %s\t\tV: %s\n", temp, temp2);
					output.EnQueue(out);
				}
				break;
			case ID_ENUM_LIST:
			case ID_LOOKUP_LIST:
				// these have simple lists
				for (int i = 0; ids[idx].SimpleList.GetSize(); i++) {
					ids[idx].SimpleList.GetStringAtLocation(temp, i);
					sprintf(out, "//I: %s\n", temp);
					output.EnQueue(out);
				}
				break;
		}
		output.EnQueue("\n");
		idx++;
	}
	output.SetOutputAvailable();
}

bool Identifiers::IdentifierValid(char* ident) {
	//
	// Thanks to Laurence Gonsalves edited by Dmitry Egorov in answer to
	// a question on StackOverflow:
	//   https://stackoverflow.com/questions/1085083/regular-expressions-in-c-examples
	// for the structure of the code in this function.
	//
	regex_t regex;
	int result;
	char error_buf[150];

	// compile
	result = regcomp(&regex, IDENTIFIER_REGEX, REG_EXTENDED);
	if (result) {
	    fprintf(stdout, "Could not compile IDENTIFIER_REGEX\n");
	    regfree(&regex);
	    exit(1);
	}

	// execute
	result = regexec(&regex, ident, 0, NULL, 0);
	if (!result) {
		regfree(&regex);
	    return true;
	}
	else if (result == REG_NOMATCH) {
		regfree(&regex);
	    return false;
	}
	else {
	    regerror(result, &regex, error_buf, sizeof(error_buf));
	    fprintf(stdout, "Regex matching failed: %s\n", error_buf);
	    regfree(&regex);
	    exit(-1);
	}

}
