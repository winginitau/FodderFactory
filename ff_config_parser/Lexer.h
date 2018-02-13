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
    bool output_available;
    char output_string[MAX_LINE_LENGTH];
    StringList output_queue;
    int error_type;

    // sectionally persistent directives
    // persist while result = R_UNFINISHED
    bool code_section;
    bool enum_section;
    char enum_array_type[MAX_WORD_LENGTH];
    char enum_array_instance[MAX_WORD_LENGTH];
    char enum_array_reserve_identifier[MAX_WORD_LENGTH];

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
    int term_level;
    ASTNode* ast_node_temp;


    // global structures and variables (that generally persist into the parser)
    Identifier ids[MAX_IDENTIFIERS];
    uint16_t id_idx;
    AST ast;



public:
    Lexer();
    virtual ~Lexer();
    void Init();
    int ProcessLine(LineBuffer& line);
    bool OutputAvailable();
    char* GetOutputAsString(char* output_str);
    int MatchToken(char* token_str);
    char* GetErrorString(char* error_str);

};

#endif /* LEXER_H_ */
