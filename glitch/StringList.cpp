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
	Reset();
}

bool StringList::AddString(const char * str) {
	StringNode* walker;

	if (head == NULL) { 	// empty list
		head = (StringNode *)malloc(sizeof(StringNode));
		if (head != NULL) {
			head->str = (char *)malloc((strlen(str) + 1) * sizeof(char));
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
		walker->next->str = (char *)malloc((strlen(str) + 1) * sizeof(char));
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
	if (head == NULL) {
		strcpy(str, "");
		return NULL;
	} else {
		// head exists
		strcpy(str, head->str);
		if (head->next == NULL) {
			// head was the only node
			free(head->str);
			free(head);
			head = NULL;
		} else {
			// more in list - grab next and make it new head
			StringNode* temp;
			temp = head->next;
			free(head->str);
			free(head);
			head = temp;
		}
		return str;
	}
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

	uint16_t index;

	index = 0;

	while (walker != NULL) {
		if(strcmp(walker->str, match_str) == 0) {
			return index;
		}
		index++;
		walker = walker->next;
	}
	return -1;
}

char* StringList::GetStringAtLocation(char* result, uint16_t location) {
	StringNode* walker;
	walker = head;

	uint16_t index;

	index = 0;

	while (walker != NULL) {
		if (index == location) {
			return strcpy(result, walker->str);
		} else {
			if (walker->next == NULL) {
				result = NULL;
				return NULL;
			} else {
				index++;
				walker = walker->next;
			}
		}
	}
	result = NULL;
	return NULL;
}

void StringList::Reset() {
	ResetFromHead(head);
	head = NULL;
}

void StringList::ResetFromHead(StringNode* s) {
	StringNode* child;
	if (s == NULL) {
		//do nothing
	} else {
		// Child?
		if (s->next != NULL) {
			//Reserve a pointer to the child
			child = s->next;
			//recursively delete the child (and its children)
			ResetFromHead(child);
		}
		// now that the child and its children are gone, free up the node memory
		free(s->str);
		free(s);
	}
}


