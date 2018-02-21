/*****************************************************************
 processor_errors.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 19 Feb. 2018

******************************************************************/

#ifndef PROCESSOR_ERRORS_H_
#define PROCESSOR_ERRORS_H_

enum {
	E_ERROR_ERROR = 0,
	E_NO_ERROR,
	E_UNKNOWN_DIRECTIVE,
	E_NULL_TOKEN_TO_LEXER,
	E_CODE_SECTION_ALREADY_ACTIVE,
	E_CODE_SECTION_IN_GRAMMAR_SECTION,
	E_CODE_END_WITHOUT_START,
	E_GRAMMAR_SECTION_ALREADY_ACTIVE,
	E_GRAMMAR_SECTION_IN_CODE_SECTION,
	E_GRAMMAR_END_WITHOUT_START,
	E_COMMENT_TOKEN_NULL,
	E_INTERNAL_ERROR_ADDING_TO_SET,
	E_SUBSECTION_CLOSE_TOKEN_NULL,
	E_ESCAPE_SEQUENCE_TOKEN_NULL,
	E_ENUM_START_VALUE_NULL,
	E_ENUM_ARRAY_TYPE_NULL,
	E_ENUM_ARRAY_INSTANCE_NULL,
	E_ENUM_ARRAY_MEMBER_LABEL_NULL,
	E_ENUM_IDENTFIFER_NULL,
	E_ENUM_START_BEFORE_PREREQ,
	E_TERM_WITHOUT_ARGUMENTS,
	E_TERM_TYPE_NOT_RECOGNISED,
	E_TERM_LEVEL_NOT_INCRMENTAL,
	E_SIBLING_TO_ROOT_ATTEMPTED,
	E_INCLUDE_WITHOUT_FILENAME,
	E_READING_INCLUDE_FILE,
	E_IDENTIFIER_NOT_FOUND_IN_LIST_OR_PAIR,
	E_IDENTIFER_NAME_NOT_FOUND,
	E_IDENTIFIER_ALREADY_EXISTS,
	E_ACTON_WITHOUT_IDENTIFIFER,
	E_ACTION_NOT_DEFINED,
	E_TERM_DEC_WIHTOUT_ACTION,
	E_EXPECTED_FUNCTION_FOR_ACTION,
	E_EXPECTED_ACTION_IDENTIFIER,
	E_EXPECTED_LOOKUP_IDENTIFIER,
	E_EXPECTED_FUNCTION_FOR_LOOKUP,
	E_UNKNOWN_IDENT_OR_LOOKUP,
	E_UNKNOWN_TERM_OR_MALFORMED_DIRECTIVE,
	E_AST_NODE_UNDEFINED_TYPE,
	E_BUILDING_ACTION_PROTO,
	E_INVALID_IDENTIFIER,
	E_DUPLICATE_KEYWORD_AT_LEVEL,
	LAST_ERROR,
};

static const EnumStringArray error_strings[LAST_ERROR] = {
		"Processing error type",
		"Unknown error",
		"Unknown directive encountered",
		"Line starting will a Null token passed to lexer",
		"%code-start encountered again within existing code section",
		"%code-start encountered inside grammar section",
		"%code-end encountered before %code-start",
		"%grammar-start encountered again within existing grammar section",
		"%grammar-start encountered inside code section",
		"%grammar-end encountered before %grammar-start start",
		"Expected token following %comment, got NULL",
		"Internal error adding to set. Out of memory?",
		"Expected token following %sub-section-close, got NULL",
		"Expected token following %escape-sequence, got NULL",
		"Expected token following %enum-start-value, got NULL",
		"Expected token following %enum-array-type, got NULL",
		"Expected token following %enum-array-instance, got NULL",
		"Expected token following %enum-array-member-label, got NULL",
		"Expected token following %enum-identifier, got NULL",
		"%enums-plus-list-array declared but encountered %enum-start before array pre-requisites defined",
		"%1..%9 term directive encountered with incomplete arguments",
		"Type following term directive (%1...%9) not recognised",
		"Encountered term that skipped incremental sequence",
		"Error in call to AST - first node should be a child of root but AddSibling called while root == NULL",
		"%include encountered without filename argument",
		"Error reading include file (either file open or during read)",
		"Identifier name not found as any label for lists or pairs",
		"Identifier name not found trying to set instance name",
		"Identifier already exists and can't be defined more than once",
		"Expected identifier following %action",
		"Action identifier not previously defined",
		"Grammar ambiguous - term level decrement without preceding action",
		"Expected FunctionName following action identifier",
		"Expected action identifier following %action-define",
		"Expected lookup identifier following %lookup-list",
		"Expected FunctionName following lookup identifier",
		"Identifier or lookup token not previously defined",
		"Unknown term following term-level directive or malformed directive",
		"Encountered AST parent of undefined type while building function parameter list",
		"Error building action prototype",
		"Identifier did not match ANSI C identifier regular expression",
		"Duplication of keyword at this term level detected",

};


#endif /* PROCESSOR_ERRORS_H_ */
