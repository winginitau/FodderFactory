/*****************************************************************
 Lexer.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#include "Lexer.h"
#include "AST.h"
#include "ASTNode.h"

Lexer::Lexer() {
    // Initialise globally persistent directives
    // Values in these need to survive calls to Init();
    grammar_section = false;
    redundant_close_as_comment = false;
    ignore_case = false;
    enum_terminating_member = false;
    enum_plus_list_array = false;
    enum_start_value[0] = '\0';
    enum_array_member_label[0] = '\0';
    enum_array_reserve_words = false;
    enum_array_type[0] = '\0';
    enum_array_instance[0] = '\0';
    enum_array_reserve_identifier[0] = '\0';
    id_idx = 0;
    term_level = 0;		// not in a term at all

    // Then call Init for the line-by-line transients
    Init();
}

Lexer:: ~Lexer() {
}

void Lexer::Init() {
    // Init holders
    token_str[0] = '\0';
    directive_str[0] = '\0';
    directive = D_NONE;
    process_result = R_NONE;
    last_directive = 0;
    output_available = false;
    output_string[0] = '\0';
    error_type = E_NO_ERROR;
    // sectionally persistent directives
    // persist only while line result = R_UNFINISHED
    code_section = false;
    enum_section = false;


    // discovered grammar


}

int Lexer::MatchToken(char* token_str) {
    int i;
    for (i = LAST_DIRECTIVE; i > D_UNKNOWN; i--) {
        if (strcmp(token_str, grammar_directives[i].text) == 0) {
            return i;
        }
    }
    return i;
}

char* Lexer::GetErrorString(char* error_str) {
    strcpy(error_str, error_strings[error_type].text);
    return error_str;
}

int Lexer::ProcessLine(LineBuffer& line) {

    line.GetTokenStr(token_str, 0);     //get the first token on the line
    line.GetTokenStr(tokens[0], 0);
    directive = MatchToken(token_str);
    switch (directive) {
        case D_UNKNOWN:
            if (code_section) {
                line.GetRawBuffer(output_string);
                strcat(output_string, "\n");
                output_queue.EnQueue(output_string);
                output_available = true;
                process_result = R_UNFINISHED;
            } else {
                if (enum_section) {
                    if(enum_plus_list_array) {
                    	line.GetTokenStr(tokens[0], 0);
                    	line.GetTokenStr(tokens[1], 1);
                    	ids[id_idx].KeyValueList.Add(tokens[0], tokens[1]);
                    }

                    // add code to process lines as enum entries
                    //line.GetRawBuffer(output_string);
                    //strcat(output_string, "\n");
                    //output_queue.EnQueue(output_string);
                    //output_available = true;
                    process_result = R_UNFINISHED;
                    //
                } else {
                    process_result = R_ERROR;
                    error_type = E_UNKNOWN_DIRECTIVE;
                }
            }
            break;
        case D_GRAMMAR_COMMENT:
            //ignore
            process_result = R_COMPLETE;
            break;
        case D_NULL:
            process_result = R_ERROR;
            error_type = E_NULL_TOKEN_TO_LEXER;
            break;
        case D_CODE_START:
            if (code_section) {
                process_result = R_ERROR;
                error_type = E_CODE_SECTION_ALREADY_ACTIVE;
            } else {
                if (grammar_section) {
                    process_result = R_ERROR;
                    error_type = E_CODE_SECTION_IN_GRAMMAR_SECTION;
                } else {
                    code_section = true;
                    process_result = R_UNFINISHED;
                }
            }
            break;
        case D_CODE_END:
            if (code_section) {
                code_section = false;
                process_result = R_COMPLETE;
            } else {
                process_result = R_ERROR;
                error_type = E_CODE_END_WITHOUT_START;
            }
            break;
        case D_GRAMMAR_START:
            if (grammar_section) {
                process_result = R_ERROR;
                error_type = E_GRAMMAR_SECTION_ALREADY_ACTIVE;
            } else {
                if (code_section) {
                    process_result = R_ERROR;
                    error_type = E_GRAMMAR_SECTION_IN_CODE_SECTION;
                } else {
                    grammar_section = true;
                    process_result = R_COMPLETE;
                }
            }
            break;
        case D_GRAMMAR_END:
            if (grammar_section) {
                grammar_section = false;
                process_result = R_COMPLETE;
            } else {
                process_result = R_ERROR;
                error_type = E_GRAMMAR_END_WITHOUT_START;
            }
            break;
        case D_COMMENT:
            if (line.GetTokenStr(token_str, 1) == NULL) {
                process_result = R_ERROR;
                error_type = E_COMMENT_TOKEN_NULL;
            } else {
                if (comments.AddString(token_str)) {
                    process_result = R_COMPLETE;
                } else {
                    process_result = R_ERROR;
                    error_type = E_INTERNAL_ERROR_ADDING_TO_SET;
                }
            }
            break;
        case D_SUB_SECTION_CLOSE:
            if (line.GetTokenStr(token_str, 1) == NULL) {
                process_result = R_ERROR;
                error_type = E_SUBSECTION_CLOSE_TOKEN_NULL;
            } else {
                if (sub_section_closes.AddString(token_str)) {
                    process_result = R_COMPLETE;
                } else {
                    process_result = R_ERROR;
                    error_type = E_INTERNAL_ERROR_ADDING_TO_SET;
                }
            }
            break;
        case D_REDUNDANT_CLOSE_AS_COMMENT:
            redundant_close_as_comment = true;
            process_result = R_COMPLETE;
            break;
        case D_IGNORE_CASE:
            ignore_case = true;
            process_result = R_COMPLETE;
            break;
        case D_ESCAPE_SEQUENCE:
            if (line.GetTokenStr(token_str, 1) == NULL) {
                process_result = R_ERROR;
                error_type = E_ESCAPE_SEQUENCE_TOKEN_NULL;
            } else {
                if (escape_sequences.AddString(token_str)) {
                    process_result = R_COMPLETE;
                } else {
                    process_result = R_ERROR;
                    error_type = E_INTERNAL_ERROR_ADDING_TO_SET;
                }
            }
            break;
        case D_ENUM_TERMINATING_MEMBER:
            enum_terminating_member = true;
            process_result = R_COMPLETE;
            break;
        case D_ENUM_PLUS_LIST_ARRAY:
            enum_plus_list_array = true;
            process_result = R_COMPLETE;
            break;
        case D_ENUM_NO_TERMINATING_MEMBER:
            enum_terminating_member = false;
            process_result = R_COMPLETE;
            break;
        case D_ENUM_NO_LIST_ARRAY:
            enum_plus_list_array = false;
            process_result = R_COMPLETE;
            break;
        case D_ENUM_START_VALUE:
            if (line.GetTokenStr(token_str, 1) == NULL) {
                process_result = R_ERROR;
                error_type = E_ENUM_START_VALUE_NULL;
            } else {
                strcpy(enum_start_value, token_str);
                process_result = R_COMPLETE;
            }
            break;
        case D_ENUM_ARRAY_TYPE:
            if (line.GetTokenStr(token_str, 1) == NULL) {
                process_result = R_ERROR;
                error_type = E_ENUM_ARRAY_TYPE_NULL;
            } else {
                strcpy(enum_array_type, token_str);
                process_result = R_COMPLETE;
            }
            break;
        case D_ENUM_ARRAY_INSTANCE:
            if (line.GetTokenStr(token_str, 1) == NULL) {
                process_result = R_ERROR;
                error_type = E_ENUM_ARRAY_INSTANCE_NULL;
            } else {
                strcpy(enum_array_instance, token_str);
                process_result = R_COMPLETE;
            }
            break;
        case D_ENUM_ARRAY_MEMBER_LABEL:
            if (line.GetTokenStr(token_str, 1) == NULL) {
                process_result = R_ERROR;
                error_type = E_ENUM_ARRAY_MEMBER_LABEL_NULL;
            } else {
                strcpy(enum_array_member_label, token_str);
                process_result = R_COMPLETE;
            }
            break;
        case D_ENUM_ARRAY_RESERVE:
            enum_array_reserve_words = true;
            process_result = R_COMPLETE;
            break;
        case D_ENUM_ARRAY_NO_RESERVE:
            enum_array_reserve_words = false;
            process_result = R_COMPLETE;
            break;
        case D_ENUM_ARRAY_RESERVE_IDENTIFIFER:
            if (line.GetTokenStr(token_str, 1) == NULL) {
                process_result = R_ERROR;
                error_type = E_ENUM_ARRAY_RESERVE_IDENTFIFER_NULL;
            } else {
                strcpy(enum_array_reserve_identifier, token_str);
                process_result = R_COMPLETE;
            }
            break;
        case D_ENUM_START:
            // do logic checks that pre reqs met
            if(enum_start_value[0] == '\0') {
                process_result = R_ERROR;
                error_type = E_ENUM_START_BEFORE_PREREQ;
            }
            if (enum_plus_list_array) {
                if ((enum_array_type == '\0') ||
                (enum_array_instance == '\0') ||
                (enum_array_member_label == '\0')) {
                    process_result = R_ERROR;
                    error_type = E_ENUM_START_BEFORE_PREREQ;
                }
            }
            if (enum_array_reserve_words) {
                if ((!enum_plus_list_array) ||
                (enum_array_reserve_identifier == '\0')) {
                    process_result = R_ERROR;
                    error_type = E_ENUM_START_BEFORE_PREREQ;
                }
            }
            if (process_result != R_ERROR) {
                strcpy(ids[id_idx].ReservedName, enum_array_instance);
                if(enum_plus_list_array) {
                	ids[id_idx].Type = ID_ENUM_LIST_ARRAY;
                }
                enum_section = true;
                process_result = R_UNFINISHED;
            }
            break;
        case D_ENUM_END:
    		strcpy(output_string, "enum {\n");
    		output_queue.EnQueue(output_string);
    		for (uint16_t i = 0; i < ids[id_idx].KeyValueList.GetSize(); i++) {
    			ids[id_idx].KeyValueList.GetPairAtLocation(tokens[0], tokens[1], i);
        		strcpy(output_string, "\t");
        		strcat(output_string, tokens[0]);
        		if (i == 0) {
                    strcat(output_string, " = ");
                    strcat(output_string, enum_start_value);
        		}
        		strcat(output_string, ",\n");
        		output_queue.EnQueue(output_string);
    		}
        	strcpy(output_string, "};\n\n");
        	output_queue.EnQueue(output_string);

        	if (enum_plus_list_array) {
        		uint16_t size = ids[id_idx].KeyValueList.GetSize();		// # entries in the enum
        		if (enum_terminating_member) {
        			// array size cardinal will be the enum value of the last member
        			// less the value of the first entry (if not 0)
        			// XXX: enum_terminating_member only works if enum_start_value == 0
        			//  add checks to enum start pre-req checks

        			// get the last enum @ size-1
            		ids[id_idx].KeyValueList.GetPairAtLocation(tokens[0], tokens[1], size-1);
        		} else {
        			// arrary size cardinal will be the number of entries in the enum
        			sprintf(token_str, "%u", size);
        			strcpy(tokens[0], token_str);
        		}

        		strcpy(output_string, "#ifdef USE_PROGMEM\n");
            	output_queue.EnQueue(output_string);

            	strcpy(output_string, "static const ");
        		strcat(output_string, enum_array_type);
        		strcat(output_string, " ");
        		strcat(output_string, enum_array_instance);
        		strcat(output_string, " [");
        		strcat(output_string, tokens[0]);
        		strcat(output_string, "] PROGMEM = {\n");
            	output_queue.EnQueue(output_string);

        		strcpy(output_string, "#else\n");
            	output_queue.EnQueue(output_string);

            	strcpy(output_string, "static const ");
        		strcat(output_string, enum_array_type);
        		strcat(output_string, " ");
        		strcat(output_string, enum_array_instance);
        		strcat(output_string, " [");
        		strcat(output_string, tokens[0]);
        		strcat(output_string, "] = {\n");
            	output_queue.EnQueue(output_string);

        		strcpy(output_string, "#endif\n");
            	output_queue.EnQueue(output_string);

            	for (uint16_t i = 0; i < ids[id_idx].KeyValueList.GetSize(); i++) {
            		ids[id_idx].KeyValueList.GetPairAtLocation(tokens[0], tokens[1], i);
            		strcpy(output_string, "\t\"");
            		strcat(output_string, tokens[1]);
            		strcat(output_string, "\",\n");
                	output_queue.EnQueue(output_string);
            	}

            	strcpy(output_string, "};\n\n");
            	output_queue.EnQueue(output_string);
            }

        	/*
            %enum-array-member-label text
            %enum-array-reserve-words
            %enum-array-reserve-identifier <block-category>
            */
            output_available = true;
            enum_section = false;
            process_result = R_COMPLETE;

            // Housekeeping - These need to be explicitly defined for each
            // enum list, so null them out so they get caught in the pre-req
            // checks if they have not been explicitly defined for the next enum.
            enum_array_type[0] = '\0';
            enum_array_instance[0] = '\0';
            enum_array_reserve_identifier[0] = '\0';
            break;
        case D_TERM_1:
        case D_TERM_2:
        case D_TERM_3:
        case D_TERM_4:
        case D_TERM_5:
        case D_TERM_6:
        case D_TERM_7:
        case D_TERM_8:
        case D_TERM_9:
            if (line.GetTokenStr(tokens[1], 1) == NULL) {
                process_result = R_ERROR;
                error_type = E_TERM_WITHOUT_ARGUMENTS;
            } else {
                // get rid of the leading %
            	int i;
            	for (i = 1; i < MAX_WORD_LENGTH; i++) {
                	tokens[0][i-1] = tokens[0][i];
                }
                tokens[0][i-1] = '\0';
            	term_level = atoi(tokens[0]);
            	if(line.GetTokenStr(tokens[2], 2) == NULL) {
                	error_type = ast.AddNode(term_level, tokens[1]);
            	} else {
            		error_type = ast.AddNode(term_level, tokens[1], tokens[2]);
            	}
            	if (error_type == E_NO_ERROR) {
            		process_result = R_COMPLETE;
            	} else {
            		process_result = R_ERROR;
            	}
            }
            break;
        case D_ACTION_DEFINE:
        	// XXX code to write the code of the action !!!
        	process_result = R_COMPLETE;
        	break;
        case D_ACTION:
        	// XXX code to call the code of the action !!!
        	process_result = R_COMPLETE;
        	break;

    }

    return process_result;
}

bool Lexer::OutputAvailable() {
    return output_available;
}

char* Lexer::GetOutputAsString(char* output_str) {

    output_queue.DeQueue(output_string);
    if (output_queue.GetSize() == 0) {
        output_available = false;
    }
    return strcpy(output_str, output_string);
}


