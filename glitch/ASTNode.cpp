/*****************************************************************
 ASTNode.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#include "ASTNode.h"

ASTNode::ASTNode() {
	id = 0;
	term_level = 0;
	label[0] = '\0';
	unique[0] = '\0';
	help[0] = '\0';
	type = AST_UNDEFINED;
	action = false;
	mode_change = false;
	finish = false;
	parent = NULL;
	first_child = NULL;
	next_sibling = NULL;

	action_identifier[0] = '\0';
	mode_change_identifier[0] = '\0';
}

ASTNode::~ASTNode() {
	// Auto-generated destructor stub
}

