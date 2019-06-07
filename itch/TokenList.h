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

//typedef struct AST_ARRAY ASTA;

typedef struct TOKEN_NODE {
	//char label[MAX_IDENTIFIER_LABEL_SIZE];
	char* label;
	uint8_t type;
	uint16_t id;
	struct TOKEN_NODE* next;
} TokenNode;

typedef struct TOKEN_LIST {
	TokenNode* head;
	TokenNode* walker;
	TokenNode* tail;
	uint16_t size;
} TokenList;


TokenList* TLNewTokenList(void);
void TLDeleteTokenList(TokenList* tl);

TokenNode* TLAddASTAToTokenList(TokenList* tl, ASTA_Node node);
TokenNode* TLAdd(TokenList* tl, TokenNode* n);
TokenNode* TLNewTokenNode(void);
void TLReset(TokenList* tl);
void TLResetFromHead(TokenNode* n);
char* TLCopyHeadLabel(TokenList* tl, char* result);
char* TLCopyCurrentLabel(TokenList* tl, char* result);
char* TLGetCurrentLabelPtr(TokenList* tl);
uint8_t TLGetCurrentType(TokenList* tl);
uint16_t TLGetCurrentID(TokenList* tl);
uint8_t TLAtEnd(TokenList* tl);
void TLToTop(TokenList* tl);
TokenNode* TLNext(TokenList* tl);
uint16_t TLGetSize(TokenList* tl);
uint8_t TLIsEmpty(TokenList* tl);
void TLDeleteByType(TokenList* tl, int8_t dtype);
uint16_t TLCountByType(TokenList* tl, uint8_t type);

void TLDelFromList(TokenNode** head, int8_t dtype);


#endif /* TOKENLIST_H_ */
