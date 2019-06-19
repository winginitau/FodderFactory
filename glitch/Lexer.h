/******************************************************************
 glitch - Grammar Lexer and Interactive Terminal Command sHell

 Copyright 2018, 2019, Brendan McLearie
 Distributed under MIT license - see LICENSE.txt
 See also README.txt

 File: Lexer.h
 - Defines the lexicon used to build grammars
 - Defines the main lexer class, its flags and functions
******************************************************************/

#ifndef LEXER_H_
#define LEXER_H_

#include "config.h"

#include "StringList.h"
#include "LineBuffer.h"
#include "Identifier.h"
#include "Identifiers.h"
#include "ASTNode.h"
#include "AST.h"

#include <regex.h>
#include <string.h>
#include <stdlib.h>

#define TOKEN_DELIM " "

// All lexicons start with '%'
// For each reserved lexicon the text
// and an enum identifier is defined.

enum {
// Lexicons used to describe a grammar
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
	D_INCLUDE_HEADER,
	D_INCLUDE_CODE,
	D_LOOKUP_LIST,
	D_HEADER_START,
	D_HEADER_END,
	D_CHANGE_MODE,
	LAST_DIRECTIVE,
};

static const EnumStringArray grammar_directives[LAST_DIRECTIVE] = {
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
		"%include-header",
		"%include-code",
		"%lookup-list",
		"%header-start",
		"%header-end",
		"%change-mode",
};

// Identifiers for the output queues
enum {
	Q_USER,
	Q_HEADER,
	Q_CODE,
	Q_USER_CODE,
	LAST_Q_TYPE,
};

class Lexer {
private:
    // Temporary holders and processing flags
    char token_str[MAX_BUFFER_WORD_LENGTH];
    char tokens[MAX_BUFFER_WORDS_PER_LINE][MAX_BUFFER_WORD_LENGTH];
    char directive_str[MAX_BUFFER_WORD_LENGTH];
    int directive;
    int process_result;
    int last_directive;
    int error_type;
    int ast_build_action_code_result;
    LineBuffer line;
    char temp_string[MAX_BUFFER_LENGTH];	// for general use in constructing messages

    // Sectionally persistent directives
    // persist while result = R_UNFINISHED
    bool code_section;
    bool enum_section;
    bool header_section;
    char enum_array_type[MAX_BUFFER_WORD_LENGTH];
    char enum_array_instance[MAX_BUFFER_WORD_LENGTH];
    char enum_identifier[MAX_BUFFER_WORD_LENGTH];

    // Globally persistent directives
    // Values in these need to survive calls to Init();
    bool grammar_section;
    StringList comments;                //initialised in constructor
    StringList sub_section_closes;      //initialised in constructor
    bool redundant_close_as_comment;
    bool ignore_case;
    char escape_sequence[MAX_BUFFER_WORD_LENGTH];            //initialised in constructor
    bool enum_terminating_member;
    bool enum_plus_list_array;
    char enum_start_value[MAX_BUFFER_WORD_LENGTH];
    char enum_array_member_label[MAX_BUFFER_WORD_LENGTH];

    bool enum_array_reserve_words;
    bool header_output_available;
    bool user_output_available;
    bool code_output_available;
    bool user_code_output_available;
    char output_string[MAX_BUFFER_LENGTH];
    StringList user_output_queue;			// queue of lines for stdout
    StringList header_output_queue;			// queue of lines for the header file
    StringList code_output_queue;			// queue of lines for the code file
    StringList user_code_output_queue;		// queue of lines for the user code file

    int term_level;
    ASTNode* ast_node_temp;
    int previous_directive;
    bool action_since_last_term;

    //XXX these got confused with ids Identeifers (plural) used directly
    // Global structures and variables (that generally persist into the parser)
    //Identifier ids[MAX_IDENTIFIERS];
    //uint16_t id_idx;

public:
    Lexer();
    virtual ~Lexer();

    // Max size and counters for various items to constrain memory use in the parser
    //int max_enum_string_array_string_size;
    int max_identifier_label_size;
    int max_ast_label_size;
    int max_ast_action_size;
    int max_identifier_count;
    int max_param_count;

    Identifiers idents; // made public so that counts can be accessed later

    void Init();
    int ProcessLine(LineBuffer& line);

    void RegisterSize(int* max, char* str);

    bool Header_OutputAvailable();
    bool User_OutputAvailable();
    bool Code_OutputAvailable();
    bool User_Code_OutputAvailable();

    char* GetOutputAsString(int queue, char* output_str);
    int MatchTokenToDirective(char* token_str);
    char* GetErrorString(char* error_str);

    AST ast;

protected:
	void Process_D_UNKNOWN(void);
	void Process_D_GRAMMAR_COMMENT(void);
	void Process_D_NULL(void);
	void Process_D_CODE_START(void);
	void Process_D_CODE_END(void);
	void Process_D_GRAMMAR_START(void);
	void Process_D_COMMENT(void);
	void Process_D_SUB_SECTION_CLOSE(void);
	void Process_D_REDUNDANT_CLOSE_AS_COMMENT(void);
	void Process_D_IGNORE_CASE(void);
	void Process_D_ESCAPE_SEQUENCE(void);
	void Process_D_ENUM_TERMINATING_MEMBER(void);
	void Process_D_ENUM_PLUS_LIST_ARRAY(void);
	void Process_D_ENUM_NO_TERMINATING_MEMBER(void);
	void Process_D_ENUM_NO_LIST_ARRAY(void);
	void Process_D_ENUM_START_VALUE(void);
	void Process_D_ENUM_ARRAY_TYPE(void);
	void Process_D_ENUM_ARRAY_INSTANCE(void);
	void Process_D_ENUM_ARRAY_MEMBER_LABEL(void);
	void Process_D_ENUM_ARRAY_RESERVE(void);
	void Process_D_ENUM_ARRAY_NO_RESERVE(void);
	void Process_D_ENUM_IDENTIFIFER(void);
	void Process_D_ENUM_START(void);
	void Process_D_ENUM_END(void);
	void Process_D_TERM(void);
    void Process_D_ACTION_DEFINE(void);
    void Process_D_ACTION(void);
    void Process_D_GRAMMAR_END(void);
	void Process_D_INCLUDE_HEADER(void);
	void Process_D_INCLUDE_CODE(void);
	void Process_D_LOOKUP_LIST(void);
	void Process_D_HEADER_START(void);
	void Process_D_HEADER_END(void);
	void Process_D_CHANGE_MODE(void);
};

#endif /* LEXER_H_ */
