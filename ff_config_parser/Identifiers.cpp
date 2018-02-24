/*****************************************************************
 Identifiers.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 15 Feb. 2018

 ******************************************************************/

#include "Identifiers.h"
#include "processor_errors.h"
#include <regex.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//#ifdef DEBUG
#include "Debug.h"
Debug debug;
//#endif

Identifiers::Identifiers() {
	count = 0;

}

Identifiers::~Identifiers() {
	// Auto-generated destructor stub
}

int Identifiers::NewIdent(char* reserved_name, int id_type) {
	int idx;

	if (IdentifierValid(reserved_name)) {

		idx = count;
		count++;
		strcpy(ids[idx].IdentifierName, reserved_name);
		ids[idx].Type = id_type;
		return E_NO_ERROR;
	} else
		return E_INVALID_IDENTIFIER;
}

int Identifiers::Add(char* identifier_name, char* key, char* value) {
	int result = E_NO_ERROR;
	int idx = GetIdxByLabel(identifier_name);
	if (idx < count) {
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

	while (idx < count) {
		if (strcmp(ids[idx].IdentifierName, label) == 0) { //found
			return idx;
		}
		idx++;
	}
	return idx;
}

int Identifiers::SetInstanceName(char* identifier_name, char* instance_name) {
	int idx = 0;
		while (idx < count) {
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
		while (idx < count) {
			if (strcmp(ids[idx].IdentifierName, identifier_name) == 0) { //found
				strcpy(instance_name, ids[idx].InstanceName);
				return E_NO_ERROR;
			}
			idx++;
		}
		return E_IDENTIFER_NAME_NOT_FOUND;
}

int Identifiers::GetEntryAtLocation(char* identifier_name, char* key_result, char* value_result, int location) {
	int idx = 0;
		while (idx < count) {
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
		while (idx < count) {
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
		while (idx < count) {
			if (strcmp(ids[idx].IdentifierName, identifier_name) == 0) {
				return true; //found
			}
			idx++;
		}
		return false;
}

void Identifiers::DumpIdentifiers(void) {
	int idx = 0;
	char out[MAX_BUFFER_LENGTH];
	char temp[MAX_BUFFER_LENGTH];
	char temp2[MAX_BUFFER_LENGTH];

		while (idx < count) {
			sprintf(out, "IdentifierName: %s\n", ids[idx].IdentifierName);
			output.EnQueue(out);
			debug.GetFromStringArray(temp, identifier_types, ids[idx].Type);
			sprintf(out, "Type \t\t%s\n", temp);
			output.EnQueue(out);
			sprintf(out, "InstanceName: \t%s\n", ids[idx].InstanceName);
			output.EnQueue(out);
			switch (ids[idx].Type) {
				case ID_ENUM_ARRAY_PAIR:
				case ID_ACTION_PAIR:
					for (int i = 0; i < ids[idx].KeyValueList.GetSize(); i++) {
						ids[idx].KeyValueList.GetPairAtLocation(temp, temp2, i);
						sprintf(out, "K: %s\t\tV: %s\n", temp, temp2);
						output.EnQueue(out);
					}
					break;
				case ID_ENUM_LIST:
				case ID_LOOKUP_LIST:
					for (int i = 0; ids[idx].SimpleList.GetSize(); i++) {
						ids[idx].SimpleList.GetStringAtLocation(temp, i);
						sprintf(out, "I: %s\n", temp);
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
	    exit(1);
	}

	// execute
	result = regexec(&regex, ident, 0, NULL, 0);
	if (!result) {
	    return true;
	}
	else if (result == REG_NOMATCH) {
	    return false;
	}
	else {
	    regerror(result, &regex, error_buf, sizeof(error_buf));
	    fprintf(stdout, "Regex matching failed: %s\n", error_buf);
	    exit(-1);
	}
	regfree(&regex);

}
