/*****************************************************************
 AST.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#ifndef AST_H_
#define AST_H_

#include "config.h"
#include "ASTNode.h"

class AST {
private:
	ASTNode* root;
	ASTNode* current;
	uint8_t current_level;
	uint16_t next_id;

public:
	AST();
	virtual ~AST();

	int AddSiblingToCurrent(ASTNode* node);
	int AddChildToCurrent(ASTNode* node);
	int AddNode(int term_level, const char* term_type);
	int AddNode(int term_level, const char* term_type, const char* term);
};

#endif /* AST_H_ */
