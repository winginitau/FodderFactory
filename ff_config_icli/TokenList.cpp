/*****************************************************************
 TokenList.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 20 Feb. 2018

 ******************************************************************/

#include "TokenList.h"

#include "TokenNode.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>


TokenList::TokenList() {
	head = NULL;
	walker = NULL;
	tail = NULL;
	size = 0;
}

TokenList::~TokenList() {
	Reset();
}



uint16_t TokenList::GetSize() {
	return size;
}

char* TokenList::GetHeadLabel(char *result) {
	if (head != NULL) {
		return strcpy(result, head->label);
	} else {
		return NULL;
	}
}

char* TokenList::GetCurrentLabel(char* result) {
	if (walker != NULL) {
		return strcpy(result, walker->label);
	} else {
		return NULL;
	}
}

uint8_t TokenList::GetCurrentType() {
	if (walker != NULL) {
		return walker->type;
	} else {
		return 0;
	}
}

uint16_t TokenList::GetCurrentID() {
	if (walker != NULL) {
		return walker->id;
	} else {
		return 0;
	}
}

void TokenList::ToTop() {
	walker = head;
}

TokenNode* TokenList::Next() {
	// catch empty list condition
	if (walker == NULL) {
		// return NULL to indicate no more nodes
		return NULL;
	} else {
		// at tail, walker should point to last member, not NULL
		if (walker->next != NULL) {
			// there's at least one more in the list
			// point walker at it
			walker = walker->next;
			return walker;
		} else { // walker not NULL but ->next is!
			// return NULL to signal end of list
			// but leave walker pointing to tail
			return NULL;
		}
	}
}

void TokenList::Reset() {
	ResetFromHead(head);
	head = NULL;
	walker = NULL;
	tail = NULL;
	size = 0;
}

void TokenList::ResetFromHead(TokenNode* n) {
	TokenNode* child;
	if (n == NULL) {
		//do nothing
	} else {
		// Child?
		if (n->next != NULL) {
			//Reserve a pointer to the child
			child = n->next;
			//recursively delete the child (and its children)
			ResetFromHead(child);
		}
		// now that the child and its children are gone, free up the node memory
		free(n);
	}
}

uint8_t TokenList::IsEmpty() {
	if (size == 0) {
		return 1;
	} else return 0;
}

TokenNode* TokenList::AddASTAToTokenList(ASTA node) {
	// malloc new node
	TokenNode* tn;
	tn = NewTokenNode();
	// fill in the data
	strcpy(tn->label, node.label);
	tn->id = node.id;
	tn->type = node.type;
	return Add(tn);
}

TokenNode* TokenList::Add(TokenNode* tn) {
	// add already malloc'd node to the end of the list
	TokenNode* temp_walker;

	// make sure the new node has next set to NULL;
	tn->next = NULL;

	if (head == NULL) { // empty list
		// incoming node becomes head
		head = tn;
		size = 1;
		tail = tn;
		walker = tn;
		return head;
	} else {
		temp_walker = head;
		// walk list until ->next is NULL, ie at end
		while (temp_walker->next != NULL) {
			temp_walker = temp_walker->next;
		}
		// now on last node
		temp_walker->next = tn;
		size++;
		tail = tn;
		return tn;
	}
}

uint8_t TokenList::AtEnd() {
	if (walker == tail) {
		return 1;
	} else return 0;
}

TokenNode* TokenList::NewTokenNode(void) {
	// just malloc a new node - don't add it to the list yet
	TokenNode* tn;
	tn = (TokenNode* )malloc(sizeof(TokenNode));
	if (tn) {
		tn->label[0] = '\0';
		tn->type = AST_UNDEFINED;
		tn->id = 0;
		tn->next = NULL;
		return tn;
	} else {
		// malloc failed
		return NULL;
	}
}

