/*****************************************************************
 TokenList.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 20 Feb. 2018

 ******************************************************************/

#ifdef ARDUINO
#include <Arduino.h>
#endif


#include "TokenList.h"

#ifdef LINUX
#include <malloc.h>
#endif

#include <string.h>
#include <stdlib.h>


TokenList* TLNewTokenList(void) {
	TokenList* tl;
	tl = (TokenList*)malloc(sizeof(TokenList));
	tl->head = NULL;
	tl->walker = NULL;
	tl->tail = NULL;
	tl->size = 0;
	return tl;
}

void TLDeleteTokenList(TokenList* tl) {
	TLReset(tl);
	free(tl);
}

uint16_t TLGetSize(TokenList* tl) {
	return tl->size;
}

char* TLCopyHeadLabel(TokenList* tl, char *result) {
	if (tl->head != NULL) {
		return strcpy(result, tl->head->label);
	} else {
		return NULL;
	}
}

char* TLCopyCurrentLabel(TokenList* tl, char* result) {
	if (tl->walker != NULL) {
		return strcpy(result, tl->walker->label);
	} else {
		return NULL;
	}
}

char* TLGetCurrentLabelPtr(TokenList* tl) {
	if (tl->walker != NULL) {
		return tl->walker->label;
	} else {
		return NULL;
	}
}

uint8_t TLGetCurrentType(TokenList* tl) {
	if (tl->walker != NULL) {
		return tl->walker->type;
	} else {
		return 0;
	}
}

uint16_t TLGetCurrentID(TokenList* tl) {
	if (tl->walker != NULL) {
		return tl->walker->id;
	} else {
		return 0;
	}
}

void TLToTop(TokenList* tl) {
	tl->walker = tl->head;
}

TokenNode* TLNext(TokenList* tl) {
	// catch empty list condition
	if (tl->walker == NULL) {
		// return NULL to indicate no more nodes
		return NULL;
	} else {
		// at tail, walker should point to last member, not NULL
		if (tl->walker->next != NULL) {
			// there's at least one more in the list
			// point walker at it
			tl->walker = tl->walker->next;
			return tl->walker;
		} else { // walker not NULL but ->next is!
			// return NULL to signal end of list
			// but leave walker pointing to tail
			return NULL;
		}
	}
}

void TLReset(TokenList* tl) {
	TLResetFromHead(tl->head);
	tl->head = NULL;
	tl->walker = NULL;
	tl->tail = NULL;
	tl->size = 0;
}

void TLResetFromHead(TokenNode* n) {
	TokenNode* child;
	if (n == NULL) {
		//do nothing
	} else {
		// Child?
		if (n->next != NULL) {
			//Reserve a pointer to the child
			child = n->next;
			//recursively delete the child (and its children)
			TLResetFromHead(child);
		}
		// now that the child and its children are gone, free up the node memory
		free(n->label);
		free(n);
	}
}

uint8_t TLIsEmpty(TokenList* tl) {
	if (tl->size == 0) {
		return 1;
	} else return 0;
}

TokenNode* TLAddASTAToTokenList(TokenList* tl, ASTA node) {
	// malloc new node
	TokenNode* tn;
	tn = TLNewTokenNode();
	// malloc enough for the label
	tn->label = (char *)malloc((strlen(node.label) + 1) * sizeof(char));
	// fill in the data
	strcpy(tn->label, node.label);
	tn->id = node.id;
	tn->type = node.type;
	return TLAdd(tl, tn);
}

TokenNode* TLAdd(TokenList* tl, TokenNode* tn) {
	// add already malloc'd node to the end of the list
	TokenNode* temp_walker;

	// make sure the new node has next set to NULL;
	tn->next = NULL;

	if (tl->head == NULL) { // empty list
		// incoming node becomes head
		tl->head = tn;
		tl->size = 1;
		tl->tail = tn;
		tl->walker = tn;
		return tl->head;
	} else {
		temp_walker = tl->head;
		// walk list until ->next is NULL, ie at end
		while (temp_walker->next != NULL) {
			temp_walker = temp_walker->next;
		}
		// now on last node
		temp_walker->next = tn;
		tl->size++;
		tl->tail = tn;
		return tn;
	}
}

uint8_t TLAtEnd(TokenList* tl) {
	if (tl->walker == tl->tail) {
		return 1;
	} else return 0;
}

TokenNode* TLNewTokenNode(void) {
	// just malloc a new node - don't add it to the list yet
	TokenNode* tn;
	tn = (TokenNode* )malloc(sizeof(TokenNode));
	if (tn) {
		//tn->label[0] = '\0';
		tn->label = NULL;
		tn->type = AST_UNDEFINED;
		tn->id = 0;
		tn->next = NULL;
		return tn;
	} else {
		// malloc failed
		return NULL;
	}
}

void TLDeleteByType(TokenList* tl, int8_t dtype) {
	TLDelFromList(&(tl->head), dtype);
	tl->walker = tl->head;

	// re-locate the tail (in case it was deleted)
	if (tl->head ==NULL) {
		tl->tail = NULL;
		tl->size = 0;
	} else {
		tl->size = 1;
		TokenNode* temp;
		temp = tl->head;
		while(temp->next != NULL) {
			temp = temp->next;
			tl->size++;
		}
		tl->tail = temp;
	}
}

void TLDelFromList(TokenNode** head, int8_t type) {

	TokenNode* temp = *head;
	TokenNode* prev = NULL;
	TokenNode* next = NULL;

	while (temp != NULL) {
		next = temp->next;
		if (temp->type == type) {
			free(temp->label);
			free(temp);
			if (prev != NULL) {
				prev->next = next;
			} else {
				*head = next;
			}
		} else {
			prev = temp;
		}
		temp = next;
	}
}

uint16_t TLCountByType(TokenList* tl, uint8_t type) {
	uint16_t count = 0;
	TokenNode* temp = tl->head;

	while (temp != NULL) {
		if (temp->type == type) { count ++; }
		temp = temp->next;
	}
	return count;
}
