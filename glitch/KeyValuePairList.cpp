/*****************************************************************
 KeyValuePairList.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#include "KeyValuePairList.h"

KeyValuePairList::KeyValuePairList() {
	// Auto-generated constructor stub

}

KeyValuePairList::~KeyValuePairList() {
	// Auto-generated destructor stub
}

bool KeyValuePairList::Add(char* key_str, char* value_str) {
	bool result_key, result_value;

	result_key = key_list.AddString(key_str);
	result_value = value_list.AddString(value_str);

	return (result_key && result_value);
}

void KeyValuePairList::WalkKeys(bool (*CallBack)(const char *)) {
	key_list.WalkList(CallBack);
}

void KeyValuePairList::WalkValues(bool (*CallBack)(const char *)) {
	value_list.WalkList(CallBack);
}

char* KeyValuePairList::GetValue(const char* key, char* value) {
	uint16_t index;
	index = 0;

	if(key_list.IsIn(key)) {
		index = key_list.GetLocation(key);
		value_list.GetStringAtLocation(value, index);
	}
	return value;
}

uint16_t KeyValuePairList::GetSize(void) {
	return key_list.GetSize();
}
uint16_t KeyValuePairList::GetLocation(const char* match_key) {
	return key_list.GetLocation(match_key);
}
void KeyValuePairList::GetPairAtLocation(char* key, char* value, uint16_t location) {
	key = key_list.GetStringAtLocation(key, location);
	value = value_list.GetStringAtLocation(value, location);
}

