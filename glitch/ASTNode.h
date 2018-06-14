/*****************************************************************
 ASTNode.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#ifndef ASTNODE_H_
#define ASTNODE_H_

#include "config.h"
#include <stdlib.h>
#include <stdint.h>

static const EnumStringArray ast_type_strings[LAST_AST_TYPE] = {
		"undefined",
		"keyword",
		"enum-array",
		"lookup",
		"param-date",
		"param-time",
		"param-integer",
		"param-uint8",
		"param-uint16",
		"param-uint32",
		"param-int16",
		"param-int32",
		"param-float",
		"param-string",

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
	bool mode_change;
	bool finish;
	ASTNode* parent;
	ASTNode* next_sibling;
	ASTNode* first_child;

	char action_identifier[MAX_BUFFER_WORD_LENGTH];
	char mode_change_identifier[MAX_BUFFER_WORD_LENGTH];

	ASTNode();
	virtual ~ASTNode();

};

#endif /* ASTNODE_H_ */
