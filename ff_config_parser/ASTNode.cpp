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
	finish = false;
	parent = NULL;
	next_sibling = NULL;
	first_child = NULL;
}

ASTNode::~ASTNode() {
	// TODO Auto-generated destructor stub
}

