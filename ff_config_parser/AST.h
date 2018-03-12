/*****************************************************************
 AST.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#ifndef AST_H_
#define AST_H_

#include "ASTNode.h"
//#include "common_config.h"
#include "StringList.h"
#include "OutputBuffer.h"
#include "Identifiers.h"

class AST {
private:
	ASTNode* root;
	ASTNode* current;
	uint8_t current_level;
	uint16_t next_id;
	void DT(ASTNode* tree, Identifiers* idents, bool print);

    char output_string[MAX_BUFFER_LENGTH];



    bool caller_func_preamble_done;



public:
	AST();
	virtual ~AST();

	int grammar_def_count;
	int max_param_count;

	OutputBuffer header_output_queue;
	OutputBuffer code_output_queue;
	OutputBuffer user_output_queue;
	OutputBuffer user_code_output_queue;


	int AddSiblingToCurrent(ASTNode* node);
	int AddChildToCurrent(ASTNode* node);
	int ValidateTermTypeStr(const char * type_str);
	bool CheckForExistingSiblingKeywords(ASTNode* start_node, const char* keyword);
	int AddNode(int term_level, const char* term_type);
	int AddNode(int term_level, const char* term_type, const char* term);
	void WriteASTArray(Identifiers* idents); // needs idents as well as AST to write the AST Array

	int GetASTTypeString(char* return_string, int type);

	void AttachActionToCurrent(char* action_identifier);
	int BuildActionCode(Identifiers& idents);
	int DetermineUnique(void);
};

#endif /* AST_H_ */
