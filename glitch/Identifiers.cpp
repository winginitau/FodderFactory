/*****************************************************************
 Identifiers.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 15 Feb. 2018

 ******************************************************************/

#include "Identifiers.h"
#include "glitch_errors.h"

#ifdef DEBUG
Debug debug;
#endif

Identifiers::Identifiers() {
	idents_count = 0;
	idents_func_idx = 0;

	DEFINE_ident_map_count = 0;
	DEFINE_lookup_map_count = 0;
	DEFINE_func_map_count = 0;
	DEFINE_ident_member_count_temp = 0;
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
		switch (id_type) {
			case ID_ENUM_ARRAY_PAIR:
				DEFINE_ident_map_count++;
				break;
			case ID_ENUM_LIST:

				break;
			case ID_LOOKUP_LIST:
				DEFINE_lookup_map_count++;
				break;
			case ID_ACTION_PAIR:
				ids[idx].func_xlat = idents_func_idx;
				idents_func_idx++;
				DEFINE_func_map_count++;
				break;
		}
		return E_NO_ERROR;
	} else
		return E_INVALID_IDENTIFIER;
}

int Identifiers::AddMember(char* identifier_name, char* key, char* value) {
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

int Identifiers::AddMember(char* identifier_name, char* item) {
	return AddMember(identifier_name, item, NULL);
}

int Identifiers::GetSizeByIdentifierName(char* identifier_name) {
	int idx = GetIdxByLabel(identifier_name);
	switch (ids[idx].Type) {
		case ID_ENUM_ARRAY_PAIR:
			return ids[idx].KeyValueList.GetSize();
		case ID_ACTION_PAIR:
			return ids[idx].KeyValueList.GetSize();
			break;
		case ID_ENUM_LIST:
			// XXX Plain enum lists - implemented? drop?
		case ID_LOOKUP_LIST:
			return ids[idx].SimpleList.GetSize();
			break;
	}
	return -1;
}

int Identifiers::GetSizeByInstanceName(char* instance_name) {
	int idx = GetIdxByInstance(instance_name);
	switch (ids[idx].Type) {
		case ID_ENUM_ARRAY_PAIR:
			return ids[idx].KeyValueList.GetSize();
		case ID_ACTION_PAIR:
			return ids[idx].KeyValueList.GetSize();
			break;
		case ID_ENUM_LIST:
			// XXX Plain enum lists - implemented? drop?
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

int Identifiers::GetIdxByInstance(char* instance_name) {
	int idx = 0;

	while (idx < idents_count) {
		if (strcmp(ids[idx].InstanceName, instance_name) == 0) { //found
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

/***********************************************************************************************
 * Code Writing Functions - to create header, code and user code sections
 **********************************************************************************************/

// XXX Term "Identifier" used generically for any reserved word in the lexer stored in memory
// as an Identifier class which is bound by CRUD functions in the Identifiers class.
// However in the parser (and therefore this code generator) the ident map refers only to
// those type of identifiers which are ENUM ARRAY PAIRS - which are an array instance which
// has member strings that relate to the associated enum.

void Identifiers::WriteXLATMapArrays(void) {
	// Write each of the XLAT map arrays into the output header

	char temp[MAX_BUFFER_LENGTH];

	strcpy(temp, "ident_map");
	WriteXLATArrayOfTypeAndInstance(ID_ENUM_ARRAY_PAIR, temp);
	strcpy(temp, "lookup_map");
	WriteXLATArrayOfTypeAndInstance(ID_LOOKUP_LIST, temp);
	strcpy(temp, "func_map");
	WriteXLATArrayOfTypeAndInstance(ID_ACTION_PAIR, temp);

	output.SetOutputAvailable();
}

void Identifiers::WriteXLATArrayOfTypeAndInstance(int type, char* array_instance_name) {
	// Write the XLAT array
	int idx;
	int xlat_idx;
	int count;
	char out[MAX_BUFFER_LENGTH];

	// count number of idents of the type being written out
	// TODO this is redundant to the DEFINE_ public members used to
	// write the DEFINES into the header file. But this function
	// was genericised before then so leave for now.... but the
	// count should be the same as the DEFINE!

	idx = 0;
	count = 0;
	while (idx < idents_count) {
		if (ids[idx].Type == type) {
			count++;
		}
		idx++;
	}

	// Write out the xlat array header
	sprintf(out, "#ifdef USE_PROGMEM\n");
	output.EnQueue(out);
	sprintf(out, "static const XLATMap %s [%d] PROGMEM = {\n", array_instance_name, count);
	output.EnQueue(out);
	sprintf(out, "#else\n");
	output.EnQueue(out);
	sprintf(out, "static const XLATMap %s [%d] = {\n", array_instance_name, count);
	output.EnQueue(out);
	sprintf(out, "#endif\n");
	output.EnQueue(out);
	// entries
	xlat_idx = 0;
	idx = 0;

	// Iterate over all the idents
	while (idx < idents_count) {

		// Select only those that are of specified type for this array
		if (ids[idx].Type == type) {

			// For the func map the link to the ASTA is the action string which is
			// IdentifierName rather than InstanceName
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

void Identifiers::WriteXLATMapLookupFunctions(int header_or_code) {
	// Write each of the XLAT Lookup functions into the code file

	// This function genericised further after the DEFINE members
	// were added - see TODO note in WriteIdentMap()

	char func_name[MAX_BUFFER_LENGTH];
	char array_instance_name[MAX_BUFFER_LENGTH];
	char member_count_define_name[MAX_BUFFER_LENGTH];

	strcpy(func_name, "LookupIdentMap");
	strcpy(array_instance_name, "ident_map");
	strcpy(member_count_define_name, "XLAT_IDENT_MAP_COUNT");
	WriteXLATMapLookupFuncOfTypeAndInstance(func_name, array_instance_name, header_or_code, member_count_define_name);

	strcpy(func_name, "LookupLookupMap");
	strcpy(array_instance_name, "lookup_map");
	strcpy(member_count_define_name, "XLAT_LOOKUP_MAP_COUNT");
	WriteXLATMapLookupFuncOfTypeAndInstance(func_name, array_instance_name, header_or_code, member_count_define_name);

	strcpy(func_name, "LookupFuncMap");
	strcpy(array_instance_name, "func_map");
	strcpy(member_count_define_name, "XLAT_FUNC_MAP_COUNT");
	WriteXLATMapLookupFuncOfTypeAndInstance(func_name, array_instance_name, header_or_code, member_count_define_name);

	output.SetOutputAvailable();
}


void Identifiers::WriteXLATMapLookupFuncOfTypeAndInstance(char* func_name, char* map_instance, int header_or_code, char* member_count_define) {
	// Write the specified XLAT Lookup function into the header
	// or code file (as specified by header_or_code)

	char out[MAX_BUFFER_LENGTH];

	// If writing the header, then do it an return
	if (header_or_code == WRITE_HEADER) {
		sprintf(out, "uint16_t %s (char* key);\n", func_name);
		output.EnQueue(out);
		return;
	}

	// Else writing the code file function
	sprintf(out, "uint16_t %s (char* key) {\n", func_name);
	output.EnQueue(out);
	sprintf(out, "\tXLATMap temp;\n");
	output.EnQueue(out);
	sprintf(out, "\tuint16_t idx = 0;\n");
	output.EnQueue(out);
	//sprintf(out, "\tcount = sizeof(%s) / sizeof(XLATMap);\n", map_instance);
	//output.EnQueue(out);
	sprintf(out, "\twhile (idx < %s) {\n", member_count_define);
	output.EnQueue(out);

	sprintf(out, "\t\t#ifdef ARDUINO\n");
	output.EnQueue(out);
	sprintf(out, "\t\t\tmemcpy_P(&temp, &%s[idx], sizeof(XLATMap));\n", map_instance);
	output.EnQueue(out);
	sprintf(out, "\t\t#else\n");
	output.EnQueue(out);
	sprintf(out, "\t\t\tmemcpy(&temp, &%s[idx], sizeof(XLATMap));\n", map_instance);
	output.EnQueue(out);
	sprintf(out, "\t\t#endif\n");
	output.EnQueue(out);

	sprintf(out, "\t\tif(strcasecmp(temp.label, key) == 0) {\n");
	output.EnQueue(out);
	sprintf(out, "\t\t\treturn temp.xlat_id;\n");
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

void Identifiers::WriteIdentMemberLookupFunction(int header_or_code) {
	// Write the "LookupIdetifierMembers" function to header / code file
	// "cases" are the per ident_map lookup xlat case

	char out[MAX_BUFFER_LENGTH];
	int case_idx = 0;
	int idx = 0;
	int member_count;
	char terminating_member_name[MAX_BUFFER_LENGTH];
	char ignore[MAX_BUFFER_LENGTH];

	// If header then write it and return
	if (header_or_code == WRITE_HEADER) {
		sprintf(out, "uint16_t LookupIdentifierMembers(uint16_t ident_xlat, char* lookup_string);\n");
		output.EnQueue(out);
		output.SetOutputAvailable();
		return;
	}

	// Else write the code file function
	// write definition and preamble
	sprintf(out, "uint16_t LookupIdentifierMembers(uint16_t ident_xlat, char* lookup_string) {\n");
	output.EnQueue(out);
	sprintf(out, "	SimpleStringArray temp;\n");
	output.EnQueue(out);
	sprintf(out, "	uint16_t idx = 0;\n");
	output.EnQueue(out);
	sprintf(out, "	switch(ident_xlat) {\n");
	output.EnQueue(out);

	// Write each case stanza referring only to the ENUM ARRAY PAIR idents
	while (idx < idents_count) {
		if (ids[idx].Type == ID_ENUM_ARRAY_PAIR) {
			// get the count of entries in this enum list
			member_count = GetSizeByIdentifierName(ids[idx].IdentifierName);
			// get the last entry - the terminating member name
			GetEntryAtLocation(ids[idx].IdentifierName, terminating_member_name, ignore, (member_count -1));

			WriteIdentMemberLookupCase(case_idx, ids[idx].InstanceName, terminating_member_name);
			case_idx++;
		}
		idx++;
	}

	// Write the default case and outro
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

	output.SetOutputAvailable();
}


void Identifiers::WriteIdentMemberLookupCase(int case_num, char* string_array_instance, char* terminating_member_name) {
	char out[MAX_BUFFER_LENGTH];


	sprintf(out, "		case %d: {\n", case_num);
	output.EnQueue(out);
	//sprintf(out, "			count = sizeof(%s) / sizeof(SimpleStringArray);\n", string_array_instance);
	//output.EnQueue(out);
	sprintf(out, "			while (idx < %s) {\n", terminating_member_name);
	output.EnQueue(out);

	sprintf(out, "				#ifdef ARDUINO\n");
	output.EnQueue(out);
	sprintf(out, "					memcpy_P(&temp, &%s[idx], sizeof(SimpleStringArray));\n", string_array_instance);
	output.EnQueue(out);
	sprintf(out, "				#else\n");
	output.EnQueue(out);
	sprintf(out, "					memcpy(&temp, &%s[idx], sizeof(SimpleStringArray));\n", string_array_instance);
	output.EnQueue(out);
	sprintf(out, "				#endif\n");
	output.EnQueue(out);
	sprintf(out, "				if(strcasecmp(lookup_string, temp.text) == 0) {\n");
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

void Identifiers::WriteLookupMemberLookupFunction(int header_or_code) {
	// Write the "LookupLookupMembers" function to header / code file
	// "cases" are the per ident_map lookup xlat case

	char out[MAX_BUFFER_LENGTH];
	int case_idx = 0;
	int idx = 0;

	// If header then write it and return
	if (header_or_code == WRITE_HEADER) {
		sprintf(out, "uint8_t LookupLookupMembers(uint16_t ident_xlat, char* lookup_string);\n");
		output.EnQueue(out);
		output.SetOutputAvailable();
		return;
	}

	// Else write the code file function
	// write definition and preamble
	sprintf(out, "uint8_t LookupLookupMembers(uint16_t ident_xlat, char* lookup_string) {\n");
	output.EnQueue(out);
	sprintf(out, "	switch(ident_xlat) {\n");
	output.EnQueue(out);

	// Write each case stanza referring only to the ID_LOOKUP_LIST idents
	while (idx < idents_count) {
		if (ids[idx].Type == ID_LOOKUP_LIST) {
			WriteLookupMemberLookupCase(case_idx, ids[idx].InstanceName);
			case_idx++;
		}
		idx++;
	}

	// Write the default case and outro
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

	output.SetOutputAvailable();
}

void Identifiers::WriteLookupMemberLookupCase(int case_num, char* function_name) {
	char out[MAX_BUFFER_LENGTH];

	sprintf(out, "		case %d:\n", case_num);
	output.EnQueue(out);
	sprintf(out, "			return %s(lookup_string);\n", function_name);
	output.EnQueue(out);
	sprintf(out, "			break;\n");
	output.EnQueue(out);
}


void Identifiers::WriteUserLookupFunctions(int header_or_code) {
	// Write each of the lookup list function outlines to header / user_code file

	char out[MAX_BUFFER_LENGTH];
	int idx = 0;

	idx = 0;

	// Iterate over all the idents
	while (idx < idents_count) {

		// Select only those that are ID_LOOKUP_LIST
		if (ids[idx].Type == ID_LOOKUP_LIST) {
			// If header then write it and return
			if (header_or_code == WRITE_HEADER) {
				sprintf(out, "uint8_t %s(char* lookup_string);\n", ids[idx].InstanceName);
				output.EnQueue(out);
				output.SetOutputAvailable();
			} else {
				// Else write the code file function
				// write definition and preamble
				sprintf(out, "uint8_t %s(char* lookup_string) {\n", ids[idx].InstanceName);
				output.EnQueue(out);
				sprintf(out, "\t//\n");
				output.EnQueue(out);
				sprintf(out, "\t// Insert lookup call here that returns 0 or 1 if lookup string found\n");
				output.EnQueue(out);
				sprintf(out, "\t//\n");
				output.EnQueue(out);
				sprintf(out, "\treturn 0;\n");
				output.EnQueue(out);
				sprintf(out, "}\n\n");
				output.EnQueue(out);
			}
		}
		idx++;
	}
	output.SetOutputAvailable();
}




/*
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
*/

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
