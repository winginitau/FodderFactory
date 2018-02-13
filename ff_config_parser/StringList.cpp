/*****************************************************************
 SetList.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 9 Feb. 2018

 ******************************************************************/

#include "StringList.h"

StringList::StringList() {
	head = NULL;
}

StringList::~StringList() {
	// TODO Auto-generated destructor stub
}

bool StringList::AddString(const char * str) {
	StringNode* walker;

	if (head == NULL) { 	// special case - empty list
		head = (StringNode *)malloc(sizeof(StringNode));
		if (head != NULL) {
			strcpy(head->str, str);
			head->next = NULL;
			return true;
		} else return false;

	}

	walker = head;

	while (walker->next != NULL) {
		walker = walker->next;
	}

	walker->next = (StringNode *)malloc(sizeof(StringNode));
	if (walker->next != NULL) {
		strcpy(walker->next->str, str);
		walker->next->next = NULL;
		return true;
	} else return false;
}

bool StringList::IsIn(const char * str) {
	StringNode* walker;

	walker = head;

	while (walker != NULL) {
		if (strcmp(head->str, str) == 0) return true;
		walker = walker->next;
	}
	return false;
}

void StringList::WalkList(bool (*CallBack)(const char *)) {
	StringNode* walker;

	walker = head;

	while (walker != NULL) {
		CallBack(walker->str);
		walker = walker->next;
	}
}

bool StringList::EnQueue(const char * str) {
	return AddString(str);
}

char* StringList::DeQueue(char* str) {
	StringNode* temp;
	strcpy(str, head->str);
	temp = head;
	head = head->next;
	free(temp);
	return str;
}

uint16_t StringList::GetSize(void) {
	StringNode* walker;
	walker = head;

	uint16_t count;

	count = 0;

	while (walker != NULL) {
		count++;
		walker = walker->next;
	}
	return count;
}

uint16_t StringList::GetLocation(const char* match_str) {
	StringNode* walker;
	walker = head;

	uint16_t count;

	count = 0;

	while (walker != NULL) {
		if(strcmp(walker->str, match_str) == 0) {
			return count;
		}
		count++;
		walker = walker->next;
	}
	return -1;
}

char* StringList::GetStringAtLocation(char* result, uint16_t location) {
	StringNode* walker;
	walker = head;

	uint16_t count;

	count = 0;

	while (walker != NULL) {
		if (count == location) {
			return strcpy(result, walker->str);
		} else {
			count++;
			walker = walker->next;
		}
	}
	result = NULL;
	return NULL;
}


