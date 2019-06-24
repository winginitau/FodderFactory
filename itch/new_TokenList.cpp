/*****************************************************************
 TokenList.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 20 Feb. 2018

 ******************************************************************/

#ifdef ARDUINO
#include <Arduino.h>
#endif


#include <new_TokenList.h>

#ifdef LINUX
#include <malloc.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <itch_hal.h>
#include <itch.h>
#include <itch_strings.h>


uint8_t TLGetSize(TokenList* tl) {
	return tl->size;
}


char* TLCopyCurrentLabel(TokenList* tl, char* result) {
	if (tl->size > 0) {
		return strcpy(result, tl->list[tl->current].label);
	} else {
		return NULL;
	}
}

char* TLGetCurrentLabelPtr(TokenList* tl) {
	if ( (tl->size > 0) && tl->list[tl->current].label != NULL) {
		return tl->list[tl->current].label;
	} else {
		return NULL;
	}
}

uint8_t TLGetCurrentType(TokenList* tl) {
	if (tl->size > 0) {
		return tl->list[tl->current].type;
	} else {
		return 0;
	}
}

uint8_t TLGetCurrentID(TokenList* tl) {
	if (tl->size > 0) {
		return tl->list[tl->current].id;
	} else {
		return 0;
	}
}

void TLToTop(TokenList* tl) {
	tl->current = 0;
}

TokenNode* TLNext(TokenList* tl) {
	// Move the current index forward and return a pointer to that element.
	// Typical use: allows the next get call to return details from next element.
	// If on last element already, return NULL to indicate that there's no
	// more but leave "current" pointing to the last element.

	// Catch empty list condition
	if (tl->size == 0) {
		// return NULL to indicate no next element
		return NULL;
	} else {
		if (tl->current == (tl->size - 1)) {
			// current indexes last element - leave it there
			return NULL;
		} else {
			tl->current++;
			return &tl->list[tl->current];
		}
		//TODO: Is there any way that current++ can exceed bounds?
	}
}

void TLInitialise(TokenList* tl) {
// same as TLReset without freeing the label. Label set to NULL
	tl->size = 0;
	tl->current = 0;
	for (uint8_t i = 0; i < MAX_GRAMMAR_DEPTH; i++) {
		tl->list[i].id = 0;
		tl->list[i].type = 0;
		tl->list[i].label = NULL;
	}
}


void TLReset(TokenList* tl) {
// Same as TLinitalise except it frees the label if allocated
	tl->size = 0;
	tl->current = 0;
	for (uint8_t i = 0; i < MAX_GRAMMAR_DEPTH; i++) {
		tl->list[i].id = 0;
		tl->list[i].type = 0;
		if(tl->list[i].label != NULL) {
			free(tl->list[i].label);
			tl->list[i].label = NULL;
		}
	}
}

uint8_t TLIsEmpty(TokenList* tl) {
	if (tl->size == 0) {
		return 1;
	} else return 0;
}

TokenNode* TLAddASTAToTokenList(TokenList* tl, ASTA_Node node) {
	if (tl->size == MAX_GRAMMAR_DEPTH) {
		char temp_str[MAX_OUTPUT_LINE_SIZE];
		strcpy_itch_hal(temp_str, itch_error_strings[TL_EXCEED_GRAMMAR_DEPTH].text);
		ITCHWriteLine(temp_str);
		return NULL;
	}
	// list indexed from zero (0), therefore size is the index of next free element
	// malloc enough for the label
	tl->list[tl->size].label = (char *)malloc((strlen(node.label) + 1) * sizeof(char));
	// fill in the data
	strcpy(tl->list[tl->size].label, node.label);
	tl->list[tl->size].id = node.id;
	tl->list[tl->size].type = node.type;
	// point the current index to element
	tl->current = tl->size;
	// increment size to show number of elements and to be ready for next add
	tl->size++;
	return &tl->list[tl->size];
}

uint8_t _FindByType(TokenList* tl, int8_t ftype) {
	// return the index of the matched type element
	// if not found, return size (which is greater than largest index)
	// zero could be a valid find
	uint8_t idx = 0;
	while(idx < tl->size) {
		if (tl->list[idx].type == ftype) {
			return idx;
		}
		idx++;
	}
	return idx;
}

void TLDeleteByType(TokenList* tl, int8_t dtype) {
	// delete all elements matching type
	// preserve oder
	// adjust size
	// TODO reverse delete operation - more efficient

	uint8_t didx;

	didx = _FindByType(tl, dtype);
	//keep deleting while matching elements
	while (didx < tl->size) {
		// shift left if not last element
		while ( (didx + 1) < tl->size) {
			tl->list[didx].id = tl->list[didx+1].id;
			tl->list[didx].type = tl->list[didx+1].type;
			// if deleted label was allocated, free it first
			if(tl->list[didx].label != NULL) {
				free(tl->list[didx].label);
			}
			// then reassign the one to the right in its place
			tl->list[didx].label = tl->list[didx+1].label;
		}
		// reduce size
		tl->size--;
		// check for next
		didx = _FindByType(tl, dtype);
	}
}

uint8_t TLCountByType(TokenList* tl, uint8_t type) {
	uint8_t count = 0;
	uint8_t idx = 0;

	while (idx < tl->size) {
		if (tl->list[idx].type == type) { count ++; }
		idx++;
	}
	return count;
}
