/*****************************************************************
 AST.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#ifndef AST_H_
#define AST_H_

#include "config.h"
#include "ASTNode.h"
#include "StringList.h"
#include "OutputBuffer.h"

class AST {
private:
	ASTNode* root;
	ASTNode* current;
	uint8_t current_level;
	uint16_t next_id;
	void DT(ASTNode* tree, bool print);

    char output_string[MAX_LINE_LENGTH];

    int grammar_def_count;


public:
	AST();
	virtual ~AST();

	OutputBuffer output;

	int AddSiblingToCurrent(ASTNode* node);
	int AddChildToCurrent(ASTNode* node);
	int AddNode(int term_level, const char* term_type);
	int AddNode(int term_level, const char* term_type, const char* term);
	void DumpTree();

	void AttachActionToCurrent(char* action_identifier);
};

#endif /* AST_H_ */
