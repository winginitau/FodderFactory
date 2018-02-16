/*****************************************************************
 Lexer.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#ifndef LEXER_H_
#define LEXER_H_

#include "StringList.h"
#include "LineBuffer.h"
#include "Identifier.h"
#include "Identifiers.h"
#include "AST.h"

class Lexer {
private:
    // temporary holders and processing flags
    char token_str[MAX_WORD_LENGTH];
    char tokens[MAX_WORDS_PER_LINE][MAX_WORD_LENGTH];
    char directive_str[MAX_WORD_LENGTH];
    int directive;
    int process_result;
    int last_directive;
    int error_type;
    LineBuffer line;

    // sectionally persistent directives
    // persist while result = R_UNFINISHED
    bool code_section;
    bool enum_section;
    char enum_array_type[MAX_WORD_LENGTH];
    char enum_array_instance[MAX_WORD_LENGTH];
    char enum_identifier[MAX_WORD_LENGTH];

    // globally persistent directives
    // Values in these need to survive calls to Init();
    bool grammar_section;
    StringList comments;                //initialised in constructor
    StringList sub_section_closes;      //initialised in constructor
    bool redundant_close_as_comment;
    bool ignore_case;
    StringList escape_sequences;            //initialised in constructor
    bool enum_terminating_member;
    bool enum_plus_list_array;
    char enum_start_value[MAX_WORD_LENGTH];
    char enum_array_member_label[MAX_WORD_LENGTH];
    bool enum_array_reserve_words;
    bool output_available;
    char output_string[MAX_LINE_LENGTH];
    StringList output_queue;
    int term_level;
    ASTNode* ast_node_temp;
    int previous_directive;
    bool action_since_last_term;


    // global structures and variables (that generally persist into the parser)
    Identifier ids[MAX_IDENTIFIERS];
    uint16_t id_idx;
    Identifiers idents;



public:
    Lexer();
    virtual ~Lexer();
    void Init();
    int ProcessLine(LineBuffer& line);
    bool OutputAvailable();
    char* GetOutputAsString(char* output_str);
    int MatchToken(char* token_str);
    char* GetErrorString(char* error_str);
	AST ast;

protected:
	void Process_D_ESCAPE_SEQUENCE(void);
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
	void Process_D_INCLUDE(void);

};

#endif /* LEXER_H_ */
