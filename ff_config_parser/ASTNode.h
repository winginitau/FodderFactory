/*****************************************************************
 ASTNode.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#ifndef ASTNODE_H_
#define ASTNODE_H_

#include "common_config.h"
#include <stdint.h>

static const EnumStringArray ast_type_strings[LAST_AST_TYPE] = {
		"undefined",
		"keyword",
		"identifier",
		"lookup",
		"param-string",
		"param-integer",
		"param-float",
		"param-time",
		"param-date",

};

enum {
	AST_END_YES,
	AST_END_NO,
	AST_END_MAYBE,
};


class ASTNode {
private:

public:
	uint16_t id;
	char label[MAX_BUFFER_WORD_LENGTH];
	char unique[MAX_BUFFER_WORD_LENGTH];
	char help[MAX_BUFFER_LENGTH];
	int term_level;
	int type;
	bool action;
	bool finish;
	ASTNode* parent;
	ASTNode* next_sibling;
	ASTNode* first_child;

	char action_identifier[MAX_BUFFER_WORD_LENGTH];

	ASTNode();
	virtual ~ASTNode();

};

#endif /* ASTNODE_H_ */
