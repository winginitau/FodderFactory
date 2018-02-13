/*****************************************************************
 LineBuffer.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 9 Feb. 2018

 ******************************************************************/

#include "LineBuffer.h"

LineBuffer::LineBuffer() {
    Init();
}

LineBuffer::~LineBuffer() {
}

void LineBuffer::Init() {
    buf[0] = '\0';
    buf_preserve[0] = '\0';
    char_idx = 0;
    word_idx = 0;
    available = false;
    empty_line = false;
    for (int i = 0; i < MAX_WORDS_PER_LINE; i++) {
        word_list[i] = '\0';
    }
    word_count = 0;
}

int LineBuffer::Tokenise() {
    strcpy(buf_preserve, buf);
    int i = 0;
    word_list[i] = strtok(buf, TOKEN_DELIM);
    while (word_list[i] != '\0') {
        i++;
        word_list[i] = strtok(0, TOKEN_DELIM);
    }
    word_idx = i;
    word_count = i;
    return i;
}

int LineBuffer::AddChar(char ch) {
    if (ch != '\r') {    // ignore \r
        if (ch == '\n') {
            if (char_idx > 0) {         // ie already chars on the line - non empty line
                buf[char_idx] = '\0';   // terminate the string
                char_idx++;
                available = true;
            } else {                // char_idx == 0 and ch == \n - empty line
                buf[1] = '\0';
                char_idx = 1;
                available = true;
                empty_line = true;
            }
        } else {                    // not \r not \n
            buf[char_idx] = ch;         // add the char
            char_idx++;
        }
    }
    return char_idx;
}

bool LineBuffer::Available(void) {
    return available;
}

bool LineBuffer::IsEmptyLine(void) {
    return empty_line;
}

void LineBuffer::PrintTokens() {
    for (int i=0; i < word_idx; i++) {
        printf("%s ", word_list[i]);
    }
    printf("\n");
}

char* LineBuffer::GetTokenStr(char* token, uint16_t idx) {
    if (idx < word_count) {
    	return strcpy(token, word_list[idx]);
    } else return NULL;
}

void LineBuffer::PrintRawBuffer() {
    for (int i=0; i < char_idx; i++) {
        printf("%c", buf_preserve[i]);
    }
}

char* LineBuffer::GetRawBuffer(char* ret_buf) {
    return strcpy(ret_buf, buf_preserve);
}

void LineBuffer::Reset() {
    Init();
}

