/*****************************************************************
 ASTNode.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#ifndef ASTNODE_H_
#define ASTNODE_H_

#include "config.h"

class ASTNode {
private:

public:
	uint16_t id;
	char label[MAX_WORD_LENGTH];
	char unique[MAX_WORD_LENGTH];
	char help[MAX_LINE_LENGTH];
	int term_level;
	int type;
	bool action;
	bool finish;
	ASTNode* parent;
	ASTNode* next_sibling;
	ASTNode* first_child;

	char action_identifier[MAX_WORD_LENGTH];

	ASTNode();
	virtual ~ASTNode();

};

#endif /* ASTNODE_H_ */
