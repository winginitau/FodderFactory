/*****************************************************************
 TokenList.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 20 Feb. 2018

 ******************************************************************/

#ifndef TOKENLIST_H_
#define TOKENLIST_H_


#include "config.h"
#include <out.h>
#include <stdint.h>


typedef struct TOKEN_NODE {
	char* label;
	uint8_t type;
	uint8_t id;
} TokenNode;

typedef struct TOKEN_LIST {
	TokenNode list[MAX_GRAMMAR_DEPTH];
	uint8_t current;
	uint8_t size;
} TokenList;

void TLInitialise(TokenList* tl);
TokenNode* TLAddASTAToTokenList(TokenList* tl, ASTA_Node node); //MapSelectMatchingNodes()
void TLReset(TokenList* tl);									// quite a few

char* TLCopyCurrentLabel(TokenList* tl, char* result); 	//ParserMatchEvaluate
char* TLGetCurrentLabelPtr(TokenList* tl);				//ParserActionDispatcher
uint8_t TLGetCurrentType(TokenList* tl);				//ParserActionDispatcher
uint8_t TLGetCurrentID(TokenList* tl); 	//MapEvaluateMatchedList x2, ParserActionDispatcher

void TLToTop(TokenList* tl);			//ParserActionDispatcher, ParserMatchEvaluate
TokenNode* TLNext(TokenList* tl);		//ParserActionDispatcher, ParserMatchEvaluate
uint8_t TLGetSize(TokenList* tl);		// Lots
uint8_t TLIsEmpty(TokenList* tl);		//MapEvaluateMatchedList

void TLDeleteByType(TokenList* tl, int8_t dtype);		//MapMatchReduce
uint8_t TLCountByType(TokenList* tl, uint8_t type);	//MapMatchReduce



#endif /* TOKENLIST_H_ */
