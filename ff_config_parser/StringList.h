/*****************************************************************
 SetList.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 9 Feb. 2018

 ******************************************************************/

#ifndef STRINGLIST_H_
#define STRINGLIST_H_

#include "config.h"

typedef struct STRING_NODE {
	char str[MAX_LINE_LENGTH];
	struct STRING_NODE* next;
} StringNode;

class StringList {
private:
	StringNode* head;

public:
	StringList();
	virtual ~StringList();
	bool AddString(const char * str);
	bool IsIn(const char * str);
	void WalkList(bool (*CallBack)(const char *));
	bool EnQueue(const char * str);
	char* DeQueue(char* str);
	uint16_t GetSize(void);
	uint16_t GetLocation(const char* match_str);
	char * GetStringAtLocation(char* result, uint16_t location);
};

#endif /* STRINGLIST_H_ */
