/*****************************************************************
 KeyValuePairList.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#ifndef KEYVALUEPAIRLIST_H_
#define KEYVALUEPAIRLIST_H_

#include "StringList.h"


class KeyValuePairList {

private:
	StringList key_list;
	StringList value_list;

public:
	KeyValuePairList();
	virtual ~KeyValuePairList();
	bool Add(char* key_str, char* value_str);
	void WalkKeys(bool (*CallBack)(const char *));
	void WalkValues(bool (*CallBack)(const char *));
	char* GetValue(const char* key, char* value);
	uint16_t GetSize(void);
	uint16_t GetLocation(const char* match_key);
	void GetPairAtLocation(char* key, char* value, uint16_t location);


};

#endif /* KEYVALUEPAIRLIST_H_ */
