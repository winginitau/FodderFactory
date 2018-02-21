/*****************************************************************
 TokenList.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 20 Feb. 2018

 ******************************************************************/

#ifndef TOKENLIST_H_
#define TOKENLIST_H_

#include "common_config.h"
#include "processor_out.h"
#include <stdint.h>

typedef struct TOKEN_NODE {
	char label[MAX_BUFFER_WORD_LENGTH];
	uint8_t type;
	uint16_t id;
	struct TOKEN_NODE* next;
} TokenNode;


class TokenList {
private:
	TokenNode* head;
	TokenNode* walker;
	TokenNode* tail;

	uint16_t size;

	void ResetFromHead(TokenNode* n);

public:
	TokenList();
	virtual ~TokenList();

	TokenNode* AddASTAToTokenList(ASTA node);
	TokenNode* Add(TokenNode* n);
	TokenNode* NewTokenNode(void);
	void Reset();
	char* GetHeadLabel(char* result);
	char* GetCurrentLabel(char* result);
	uint8_t GetCurrentType();
	uint16_t GetCurrentID();
	uint8_t AtEnd();
	void ToTop();
	TokenNode* Next();
	uint16_t GetSize();
	uint8_t IsEmpty();

protected:

};

#endif /* TOKENLIST_H_ */
