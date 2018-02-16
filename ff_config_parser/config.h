/*****************************************************************
 defines.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 9 Feb. 2018

******************************************************************/

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 150
#define MAX_WORD_LENGTH 150
#define MAX_WORDS_PER_LINE 30
#define MAX_IDENTIFIERS 50

#define TOKEN_DELIM " "

typedef struct SIMPLE_STRING_ARRAY_TYPE {
	// Array of strings (usually labels and messages) relating to enums
	// Inside this struct, extended intitaliser lists are ok
	char text[MAX_LINE_LENGTH];
} SimpleStringArray;


// Grammar definition directives (all start with %)

enum {
// directives used to describe a grammar
	D_UNKNOWN = 0,
	D_NONE,
	D_NULL,
    D_GRAMMAR_COMMENT,
	D_CODE_START,
	D_CODE_END,
	D_GRAMMAR_START,
	D_GRAMMAR_END,
	D_COMMENT,
	D_SUB_SECTION_CLOSE,
	D_REDUNDANT_CLOSE_AS_COMMENT,
	D_IGNORE_CASE,
	D_ESCAPE_SEQUENCE,
	D_ENUM_TERMINATING_MEMBER,
	D_ENUM_PLUS_LIST_ARRAY,
	D_ENUM_NO_ERROR_MEMBER,
	D_ENUM_NO_TERMINATING_MEMBER,
	D_ENUM_NO_LIST_ARRAY,
	D_ENUM_START_VALUE,
	D_ENUM_ARRAY_TYPE,
	D_ENUM_ARRAY_INSTANCE,
	D_ENUM_ARRAY_MEMBER_LABEL,
	D_ENUM_ARRAY_RESERVE,
	D_ENUM_ARRAY_NO_RESERVE,
	D_ENUM_IDENTIFIFER,
	D_ENUM_START,
	D_ENUM_END,
	D_TERM_1,
	D_TERM_2,
	D_TERM_3,
	D_TERM_4,
	D_TERM_5,
	D_TERM_6,
	D_TERM_7,
	D_TERM_8,
	D_TERM_9,
	D_ACTION_DEFINE,
	D_ACTION,
	D_INCLUDE,
	LAST_DIRECTIVE,
};



static const SimpleStringArray grammar_directives[LAST_DIRECTIVE] = {
		"DIRECTVE_UNKNOWN",
		"DIRECTIVE_NONE",
		"\0",
		"%#",
		"%code-start",
		"%code-end",
		"%grammar-start",
		"%grammar-end",
		"%comment",
		"%sub-section-close",
		"%redundant-close-as-comment",
		"%ignore-case",
		"%escape-sequence",
		"%enum-terminating-member",
		"%enum-plus-list-array",
		"%enum-no-error-member",
		"%enum-no-terminating-member",
		"%enum-no-list-array",
		"%enum-start-value",
		"%enum-array-type",
		"%enum-array-instance",
		"%enum-array-member-label",
		"%enum-array-reserve-words",
		"%enum-array-no-reserve-words",
		"%enum-identifier",
		"%enum-start",
		"%enum-end",
		"%1",
		"%2",
		"%3",
		"%4",
		"%5",
		"%6",
		"%7",
		"%8",
		"%9",
		"%action-define",
		"%action",
		"%include",


};


enum {
	R_ERROR = 0,
	R_NONE,
	R_UNFINISHED,
	R_COMPLETE,
};


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
	LAST_ERROR,
};


static const SimpleStringArray error_strings[LAST_ERROR] = {
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

};

enum {
	ID_TYPE_UNDEFINED = 0,
	ID_ENUM_ARRAY_PAIR,
	ID_ENUM_LIST,
	ID_LOOKUP_LIST,
	ID_ACTION_PAIR,
	LAST_IDENTIFIER_TYPE,
};


enum {
	AST_UNDEFINED = 0,
	AST_KEYWORD,
	AST_IDENTIFIER,
	AST_LOOKUP,
	AST_PARAM_STRING,
	AST_PARAM_INTEGER,
	AST_PARAM_FLOAT,
	AST_PARAM_TIME,
	AST_PARAM_DATE,
	LAST_AST_TYPE,
};

static const SimpleStringArray ast_type_strings[LAST_AST_TYPE] = {
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



#endif /* CONFIG_H_ */
