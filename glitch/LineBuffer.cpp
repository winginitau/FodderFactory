/******************************************************************

 glitch - Grammar Lexer and Interactive Terminal Command sHell

 Copyright 2018, 2019, Brendan McLearie
 Distributed under MIT license - see LICENSE.txt
 See also README.txt

 File: LineBuffer.cpp
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
    for (int i = 0; i < MAX_BUFFER_WORDS_PER_LINE; i++) {
        word_list[i] = NULL;
    }
    word_count = 0;
}

int LineBuffer::Tokenise() {
	// Put the tokens present in the line into word_list[]
	// Treat anything in double quotes ("...") as a single token
	// Assumes: there is at most one single string designated by double quotes
	// Assumes: that any quoted string is at the end of the line
	// Any number of tokens may precede the quoted string.

	//char quoted_string_buf[MAX_BUFFER_LENGTH];

	// Preserve the raw buffer for future use
    strcpy(buf_preserve, buf);

    // Check if the buffer contains a double quote ("...") delimited string
    char *pre_quotes;
    char *in_quotes;
    bool quotes = false;

    pre_quotes = strtok(buf, "\"");
    if (pre_quotes != NULL) {
    	// its not an empty string
    	in_quotes = strtok(NULL, "\"");
    	if(in_quotes != NULL) {
    		// quoted string now pointed to by in_quotes
    		// with either eol or the next \" being null
    		quotes = true;
    		//strcpy(quoted_string_buf, in_quotes);
    	}
    }

    // buf still points to the start of string.
    // buf now with the first \" turned to null - ie end of string
    // or buf is unmodified if \" wasn't found.
    int i = 0;
    word_list[i] = strtok(buf, TOKEN_DELIM);
    while (word_list[i] != NULL) {
        i++;
        word_list[i] = strtok(0, TOKEN_DELIM);
    }
    if(quotes) {
    	//strcpy(word_list[i], quoted_string_buf);
    	word_list[i] = in_quotes;
    	i++;
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

//void LineBuffer::PrintTokens() {
//    for (uint16_t i=0; i < word_idx; i++) {
//        printf("%s ", word_list[i]);
//    }
//    printf("\n");
//}

char* LineBuffer::GetTokenStr(char* token, uint16_t idx) {
    // Copy the token at position idx of the word list into
	// the char * token provided by the caller. Return that pointer.
	if (empty_line) {
    	token[0] = '\0';
    	return token;
    }
	if (idx < word_count) {
    	return strcpy(token, word_list[idx]);
    } else return NULL;
}

//void LineBuffer::PrintRawBuffer() {
//    for (uint16_t i=0; i < char_idx; i++) {
//        printf("%c", buf_preserve[i]);
//    }
//}

char* LineBuffer::GetRawBuffer(char* ret_buf) {
    return strcpy(ret_buf, buf_preserve);
}

void LineBuffer::Reset() {
    Init();
}

char* LineBuffer::GetToEOL(char* result, int after_token) {

	result = strstr(buf_preserve, word_list[after_token+1]);

    return result;
}
