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
    enum_identifier[0] = '\0';
    id_idx = 0;
    term_level = 0;		// not in a term at all
    previous_directive = D_NONE;
    action_since_last_term = false;

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



}

int Lexer::MatchToken(char* token_str) {
	int i;
	if (token_str[0] == '\0') {
		return D_NULL;
	} else {
		for (i = LAST_DIRECTIVE; i > D_UNKNOWN; i--) {
			if (strcmp(token_str, grammar_directives[i].text) == 0) {
				return i;
			}
		}
		return i;
	}
}

char* Lexer::GetErrorString(char* error_str) {
    strcpy(error_str, error_strings[error_type].text);
    return error_str;
}

int Lexer::ProcessLine(LineBuffer& line_buf) {

	line = line_buf;

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
                    line.GetTokenStr(tokens[0], 0);
                    line.GetTokenStr(tokens[1], 1);
                    idents.Add(enum_identifier, tokens[0], tokens[1]);
                    // TODO possibly add code to process lines as enum-only entries
                    // if token[1] fails
                    process_result = R_UNFINISHED;
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
            if (code_section) {  // handle blank lines in the code section
                line.GetRawBuffer(output_string);
                strcat(output_string, "\n");
                output_queue.EnQueue(output_string);
                output_available = true;
                process_result = R_UNFINISHED;
            } else {
            	// ignore NULL token
            	process_result = R_COMPLETE;
            	//error_type = E_NULL_TOKEN_TO_LEXER;
            }
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
        case D_ESCAPE_SEQUENCE: Process_D_ESCAPE_SEQUENCE(); break;
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
        case D_ENUM_START_VALUE: 	Process_D_ENUM_START_VALUE(); break;
        case D_ENUM_ARRAY_TYPE: 	Process_D_ENUM_ARRAY_TYPE(); break;
        case D_ENUM_ARRAY_INSTANCE: Process_D_ENUM_ARRAY_INSTANCE(); break;
        case D_ENUM_ARRAY_MEMBER_LABEL:	Process_D_ENUM_ARRAY_MEMBER_LABEL(); break;
        case D_ENUM_ARRAY_RESERVE: 	Process_D_ENUM_ARRAY_RESERVE(); break;
        case D_ENUM_ARRAY_NO_RESERVE: Process_D_ENUM_ARRAY_NO_RESERVE(); break;
        case D_ENUM_IDENTIFIFER:	Process_D_ENUM_IDENTIFIFER();	break;
        case D_ENUM_START: 			Process_D_ENUM_START(); 		break;
        case D_ENUM_END: 			Process_D_ENUM_END(); 			break;
        case D_TERM_1:
        case D_TERM_2:
        case D_TERM_3:
        case D_TERM_4:
        case D_TERM_5:
        case D_TERM_6:
        case D_TERM_7:
        case D_TERM_8:
        case D_TERM_9: 				Process_D_TERM(); 				break;
        case D_ACTION_DEFINE: 		Process_D_ACTION_DEFINE(); 		break;
        case D_ACTION: 				Process_D_ACTION(); 			break;
        case D_GRAMMAR_END: 		Process_D_GRAMMAR_END(); 		break;
        case D_INCLUDE: 			Process_D_INCLUDE(); 			break;
    }
    previous_directive = directive;
    return process_result;
}

void Lexer::Process_D_ESCAPE_SEQUENCE(void) {
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
}

void Lexer::Process_D_ENUM_START_VALUE(void) {
    if (line.GetTokenStr(token_str, 1) == NULL) {
        process_result = R_ERROR;
        error_type = E_ENUM_START_VALUE_NULL;
    } else {
        strcpy(enum_start_value, token_str);
        process_result = R_COMPLETE;
    }
}

void Lexer::Process_D_ENUM_ARRAY_TYPE(void) {
    if (line.GetTokenStr(token_str, 1) == NULL) {
        process_result = R_ERROR;
        error_type = E_ENUM_ARRAY_TYPE_NULL;
    } else {
        strcpy(enum_array_type, token_str);
        process_result = R_COMPLETE;
    }
}

void Lexer::Process_D_ENUM_ARRAY_INSTANCE(void) {
    if (line.GetTokenStr(token_str, 1) == NULL) {
        process_result = R_ERROR;
        error_type = E_ENUM_ARRAY_INSTANCE_NULL;
    } else {
        strcpy(enum_array_instance, token_str);
        process_result = R_COMPLETE;
    }
}

void Lexer::Process_D_ENUM_ARRAY_MEMBER_LABEL(void) {
    if (line.GetTokenStr(token_str, 1) == NULL) {
        process_result = R_ERROR;
        error_type = E_ENUM_ARRAY_MEMBER_LABEL_NULL;
    } else {
        strcpy(enum_array_member_label, token_str);
        process_result = R_COMPLETE;
    }
}

void Lexer::Process_D_ENUM_ARRAY_RESERVE(void) {
    enum_array_reserve_words = true;
    process_result = R_COMPLETE;
}

void Lexer::Process_D_ENUM_ARRAY_NO_RESERVE(void) {
    enum_array_reserve_words = false;
    process_result = R_COMPLETE;
}

void Lexer::Process_D_ENUM_IDENTIFIFER(void) {
    if (line.GetTokenStr(token_str, 1) == NULL) {
        process_result = R_ERROR;
        error_type = E_ENUM_IDENTFIFER_NULL;
    } else {
        strcpy(enum_identifier, token_str);
        process_result = R_COMPLETE;
    }
}

void Lexer::Process_D_ENUM_START(void) {
    // do logic checks that pre reqs met
    if (enum_identifier == '\0') {
        process_result = R_ERROR;
        error_type = E_ENUM_START_BEFORE_PREREQ;
    }
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
        (enum_identifier == '\0')) {
            process_result = R_ERROR;
            error_type = E_ENUM_START_BEFORE_PREREQ;
        }
    }
    if (process_result != R_ERROR) {
    	if (enum_plus_list_array) {
    		idents.NewIdent(enum_identifier, ID_ENUM_ARRAY_PAIR);
    		idents.SetInstanceName(enum_identifier, enum_array_instance);
    	} else {
    		idents.NewIdent(enum_identifier, ID_ENUM_LIST);
    	}
        enum_section = true;
        process_result = R_UNFINISHED;
    }
}

void Lexer::Process_D_ENUM_END(void) {
	strcpy(output_string, "enum {\n");
	output_queue.EnQueue(output_string);
	for (uint16_t i = 0; i < idents.GetSize(enum_identifier); i++) {
		if (enum_plus_list_array) {
			idents.GetEntryAtLocation(enum_identifier,tokens[0], tokens[1], i);
		} else {
			idents.GetEntryAtLocation(enum_identifier,tokens[0], i);
		}
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
		uint16_t size = idents.GetSize(enum_identifier);		// # entries in the enum
		if (enum_terminating_member) {
			// array size cardinal will be the enum value of the last member
			// less the value of the first entry (if not 0)
			// XXX: enum_terminating_member only works if enum_start_value == 0
			//  add checks to enum start pre-req checks

			// get the last enum @ size-1
			idents.GetEntryAtLocation(enum_identifier,tokens[0], tokens[1], size-1);
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

    	for (uint16_t i = 0; i < size - 1; i++) {
			idents.GetEntryAtLocation(enum_identifier,tokens[0], tokens[1], i);
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
    */
    output_available = true;
    enum_section = false;
    process_result = R_COMPLETE;

    // Housekeeping - These need to be explicitly defined for each
    // enum list, so null them out so they get caught in the pre-req
    // checks if they have not been explicitly defined for the next enum.
    enum_array_type[0] = '\0';
    enum_array_instance[0] = '\0';
    enum_identifier[0] = '\0';
}

void Lexer::Process_D_TERM(void) {
	if (line.GetTokenStr(tokens[1], 1) == NULL) {
		process_result = R_ERROR;
		error_type = E_TERM_WITHOUT_ARGUMENTS;
	} else {
		// get rid of the leading %
		int i;
		for (i = 1; i < MAX_WORD_LENGTH; i++) {
			tokens[0][i - 1] = tokens[0][i];
		}
		tokens[0][i - 1] = '\0';

		// Work out term level and if we are decrementing
		// If decrementing then there should have been an %action directive
		// 	between this and the previous term directive.
		int new_term_level;
		new_term_level = atoi(tokens[0]);
		if ((new_term_level <= term_level) && (!action_since_last_term)) {
			error_type = E_TERM_DEC_WIHTOUT_ACTION;
		} else {
			term_level = new_term_level;
			// Add the term to the AST:
			// Form: <level> <term-type> (<term>)
			// Present forms / types:
			//
			// <n> keyword <keyword>		// AST_KEYWORD
			// <n> identifier <identifier>	// AST_IDENTIFIER
			// <n> lookup <lookup-list>		// AST_LOOKUP
			//
			// <n> param-string				// AST_PARAM_STRING,
			// <n> param-integer			// AST_PARAM_INTEGER,
			// <n> param-float				// AST_PARAM_FLOAT,
			// <n> param-time				// AST_PARAM_TIME,
			// <n> param-date				// AST_PARAM_DATE,
			if (line.GetTokenStr(tokens[2], 2) == NULL) {
				error_type = ast.AddNode(term_level, tokens[1]);
			} else {
				//
				// XXX
				//
				// add existence checking for identifiers and lookups
				//
				error_type = ast.AddNode(term_level, tokens[1], tokens[2]);
			}
		}
	}
	if (error_type == E_NO_ERROR) {
		process_result = R_COMPLETE;
		action_since_last_term = false;
	} else {
		process_result = R_ERROR;
	}
}

void Lexer::Process_D_ACTION_DEFINE(void) {
	// Set up an identifier list of key val pairs
	//	ACTION_IDENTIFIER as IdentifierName
	//	FunctionName as InstanceName
	//
	//	Then later populate through grammar parsing for
	//		parameter types and names in order.
	line.GetTokenStr(tokens[0], 0);

	if (line.GetTokenStr(tokens[1], 1) == NULL) {
		error_type = E_EXPECTED_ACTION_IDENTIFIER;
		process_result = R_ERROR;

	} else {
		if (line.GetTokenStr(tokens[2], 2) == NULL) {
			error_type = E_EXPECTED_FUNCTION_FOR_ACTION;
			process_result = R_ERROR;
		} else {
			if (idents.Exists(tokens[1])) {
				error_type = E_IDENTIFIER_ALREADY_EXISTS;
				process_result = R_ERROR;
			} else {
				idents.NewIdent(tokens[1], ID_ACTION_PAIR);
				idents.SetInstanceName(tokens[1], tokens[2]);
				process_result = R_COMPLETE;
			}
		}
	}
}

void Lexer::Process_D_ACTION(void) {
	// Attach the action to the last added AST node
	// check first that the action identifier has been defined
    if (line.GetTokenStr(tokens[1], 1) == NULL) {
        process_result = R_ERROR;
        error_type = E_ACTON_WITHOUT_IDENTIFIFER;
    } else {
    	if(!idents.Exists(tokens[1])) {
    		error_type = E_ACTION_NOT_DEFINED;
    		process_result = R_ERROR;
    	} else {
    		ast.AttachActionToCurrent(tokens[1]);
    		process_result = R_COMPLETE;
    		action_since_last_term = true;
    	}
    }
}

void Lexer::Process_D_GRAMMAR_END(void) {
	if (grammar_section) {
		//
		// XXX
		//
		strcpy(output_string, "//TODO: Lexer Lookup Hooks\n");
		output_queue.EnQueue(output_string);
		strcpy(output_string, "//TODO: Command Line Options Processing\n");
		output_queue.EnQueue(output_string);
		strcpy(output_string, "//TODO: AST Validation Walk - \n");
		output_queue.EnQueue(output_string);
		strcpy(output_string, "//TODO: AST Flag end points\n");
		output_queue.EnQueue(output_string);
		strcpy(output_string, "//TODO: AST Order Ambiguity Report\n");
		output_queue.EnQueue(output_string);
		strcpy(output_string, "//TODO: AST Determine Partial Keyword Uniqueness\n");
		output_queue.EnQueue(output_string);
		strcpy(output_string, "//TODO: AST Warn unused IDs, lookups, params\n");
		output_queue.EnQueue(output_string);
		strcpy(output_string, "//TODO: Parser/AST output Action prototypes\n");
		output_queue.EnQueue(output_string);
		strcpy(output_string, "//TODO: Configuration Grammar with %section directive\n");
		output_queue.EnQueue(output_string);
		strcpy(output_string, "//TODO: Context change on <identifier> value\n\n");
		output_queue.EnQueue(output_string);

		ast.DumpTree();
		idents.DumpIdentifiers();

		while (ast.output.OutputAvailable()) {
			ast.output.GetOutputAsString(output_string);
			output_queue.EnQueue(output_string);
		}

		while (idents.output.OutputAvailable()) {
			idents.output.GetOutputAsString(output_string);
			output_queue.EnQueue(output_string);
		}

		output_available = true;
		grammar_section = false;
		process_result = R_COMPLETE;
	} else {
		process_result = R_ERROR;
		error_type = E_GRAMMAR_END_WITHOUT_START;
	}
}

void Lexer::Process_D_INCLUDE(void) {
	if (line.GetTokenStr(tokens[1], 1) == NULL) {
		process_result = R_ERROR;
		error_type = E_INCLUDE_WITHOUT_FILENAME;
	} else {
		FILE* f;
		char* res;
		char in_buf[MAX_LINE_LENGTH];
		f = fopen(tokens[1], "r");
		if (f != NULL) {
			while (!feof(f)) {
				res = fgets(in_buf, MAX_LINE_LENGTH, f);
				if (res != NULL) {
					output_queue.EnQueue(in_buf);
					output_available = true;
				} else {
					process_result = R_ERROR;
					error_type = E_READING_INCLUDE_FILE;
				}
			}
			process_result = R_COMPLETE;
		} else {
			process_result = R_ERROR;
			error_type = E_READING_INCLUDE_FILE;
		}
	}
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


