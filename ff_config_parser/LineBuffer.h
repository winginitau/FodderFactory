/*****************************************************************
 LineBuffer.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 9 Feb. 2018

 ******************************************************************/

#ifndef LINEBUFFER_H_
#define LINEBUFFER_H_

//#include "config.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef MAX_BUFFER_LENGTH
#define MAX_BUFFER_LENGTH 200
#endif

#ifndef MAX_BUFFER_WORD_LENGTH
#define MAX_BUFFER_WORD_LENGTH 200
#endif

#ifndef MAX_BUFFER_WORDS_PER_LINE
#define MAX_BUFFER_WORDS_PER_LINE 50
#endif

//#define MAX_IDENTIFIERS 50
#ifndef TOKEN_DELIM
#define TOKEN_DELIM " "
#endif

class LineBuffer {
private:
    char buf[MAX_BUFFER_LENGTH];
    char buf_preserve[MAX_BUFFER_LENGTH];
    uint16_t char_idx;
    uint16_t word_idx;
    bool available;
    bool empty_line;
    char* word_list[MAX_BUFFER_WORDS_PER_LINE];
    uint16_t word_count;
public:
    LineBuffer();
    virtual ~LineBuffer();
    void Init();
    int AddChar(char ch);
    int GetLine(char* line);
    int Tokenise();
    //void PrintTokens();
    char* GetTokenStr(char* token, uint16_t idx);
    //void PrintRawBuffer();
    char* GetRawBuffer(char* ret_buf);
    bool Available();
    bool IsEmptyLine();
    void Reset();
    char* GetToEOL(char* result, int after_token);

};


#endif /* LINEBUFFER_H_ */
