/*****************************************************************
 Lexer.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#include "Lexer.h"
#include "glitch_errors.h"


Lexer::Lexer() {
    // Initialise globally persistent directives
    // Values in these need to survive calls to Init();
    grammar_section = false;
    redundant_close_as_comment = false;
    ignore_case = false;
    enum_terminating_member = false;
    enum_plus_list_array = false;
    strcpy(enum_start_value, "0");
    enum_array_member_label[0] = '\0';
    enum_array_reserve_words = false;
    enum_array_type[0] = '\0';
    enum_array_instance[0] = '\0';
    enum_identifier[0] = '\0';
    id_idx = 0;
    term_level = 0;		// not in a term at all
    previous_directive = D_NONE;
    action_since_last_term = false;

    max_enum_string_array_string_size = 0;
    max_identifier_label_size = 0;
    max_ast_label_size = 0;
    max_ast_action_size = 0;
    max_identifier_count = 0;
    max_param_count = 0;


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
    header_output_available = false;
    output_string[0] = '\0';
    error_type = E_NO_ERROR;
    // sectionally persistent directives
    // persist only while line result = R_UNFINISHED or R_IGNORE
    code_section = false;
    enum_section = false;
    header_section = false;

    temp_string[0] = '\0';



}

int Lexer::MatchTokenToDirective(char* token_str) {
	// Match the token string to directives and
	// 	return the related enum value
	// Assumes that the string is a valid token (ie no leading space etc)
	int i;

	// Special case - NULL string
	if (token_str[0] == '\0') {
		return D_NULL;
	}

	// Special case - grammar comment - first two chars are "%#"
	if ((strlen(token_str) >= 2) && (token_str[0] == '%') && (token_str[1] == '#')) {
		return D_GRAMMAR_COMMENT;
	}

	// Match it or return D_UNKNOWN
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

void Lexer::RegisterSize(int* max, char* str) {
	// Update the passed max_ size counter (that will be used as #defines in the parser)
	int sz = strlen(str);
	if (sz > *max) *max = sz;
}

int Lexer::ProcessLine(LineBuffer& line_buf) {

	line = line_buf;					// can't remember why there's a second reference

    line.GetTokenStr(token_str, 0);     // get the first token on the line
    line.GetTokenStr(tokens[0], 0);		// can't remember why we need both
    directive = MatchTokenToDirective(token_str);	// The first token is a directive unless it is data
    									// 	inside sectional start and stop directives
    switch (directive) {
        case D_UNKNOWN: 					Process_D_UNKNOWN(); 						break;
        case D_GRAMMAR_COMMENT: 			Process_D_GRAMMAR_COMMENT(); 				break;
        case D_NULL:						Process_D_NULL();							break;
		case D_CODE_START: 					Process_D_CODE_START();	 					break;
        case D_CODE_END: 					Process_D_CODE_END(); 						break;
        case D_GRAMMAR_START: 				Process_D_GRAMMAR_START(); 					break;
        case D_COMMENT: 					Process_D_COMMENT(); 						break;
        case D_SUB_SECTION_CLOSE: 			Process_D_SUB_SECTION_CLOSE(); 				break;
        case D_REDUNDANT_CLOSE_AS_COMMENT: 	Process_D_REDUNDANT_CLOSE_AS_COMMENT();	 	break;
        case D_IGNORE_CASE: 				Process_D_IGNORE_CASE(); 					break;
        case D_ESCAPE_SEQUENCE: 			Process_D_ESCAPE_SEQUENCE(); 				break;
        case D_ENUM_TERMINATING_MEMBER: 	Process_D_ENUM_TERMINATING_MEMBER(); 		break;
        case D_ENUM_PLUS_LIST_ARRAY: 		Process_D_ENUM_PLUS_LIST_ARRAY(); 			break;
        case D_ENUM_NO_TERMINATING_MEMBER: 	Process_D_ENUM_NO_TERMINATING_MEMBER(); 	break;
        case D_ENUM_NO_LIST_ARRAY: 			Process_D_ENUM_NO_LIST_ARRAY(); 			break;
        case D_ENUM_START_VALUE: 			Process_D_ENUM_START_VALUE(); 				break;
        case D_ENUM_ARRAY_TYPE: 			Process_D_ENUM_ARRAY_TYPE(); 				break;
        case D_ENUM_ARRAY_INSTANCE: 		Process_D_ENUM_ARRAY_INSTANCE(); 			break;
        case D_ENUM_ARRAY_MEMBER_LABEL:		Process_D_ENUM_ARRAY_MEMBER_LABEL(); 		break;
        case D_ENUM_ARRAY_RESERVE: 			Process_D_ENUM_ARRAY_RESERVE(); 			break;
        case D_ENUM_ARRAY_NO_RESERVE: 		Process_D_ENUM_ARRAY_NO_RESERVE(); 			break;
        case D_ENUM_IDENTIFIFER:			Process_D_ENUM_IDENTIFIFER();				break;
        case D_ENUM_START: 					Process_D_ENUM_START(); 					break;
        case D_ENUM_END: 					Process_D_ENUM_END(); 						break;
        case D_TERM_1:
        case D_TERM_2:
        case D_TERM_3:
        case D_TERM_4:
        case D_TERM_5:
        case D_TERM_6:
        case D_TERM_7:
        case D_TERM_8:
        case D_TERM_9: 						Process_D_TERM(); 							break;
        case D_ACTION_DEFINE: 				Process_D_ACTION_DEFINE(); 					break;
        case D_ACTION: 						Process_D_ACTION(); 						break;
        case D_GRAMMAR_END: 				Process_D_GRAMMAR_END(); 					break;
        case D_INCLUDE_HEADER: 				Process_D_INCLUDE_HEADER();					break;
        case D_INCLUDE_CODE: 				Process_D_INCLUDE_CODE();					break;
        case D_LOOKUP_LIST:					Process_D_LOOKUP_LIST();					break;
        case D_HEADER_START:				Process_D_HEADER_START();					break;
        case D_HEADER_END:					Process_D_HEADER_END();						break;
    }
    previous_directive = directive;
    return process_result;
}

void Lexer::Process_D_UNKNOWN(void) {
	// The first token did not match any directive
	// Therefore it is either a data line inside start / top sectional directives
	//	or it is an error
	if (header_section) {
		line.GetRawBuffer(output_string);
		strcat(output_string, "\n");
		header_output_queue.EnQueue(output_string);
		header_output_available = true;
		process_result = R_UNFINISHED;
	} else {
		if(code_section) {
			line.GetRawBuffer(output_string);
			strcat(output_string, "\n");
			code_output_queue.EnQueue(output_string);
			code_output_available = true;
			process_result = R_UNFINISHED;

		} else {
		if (enum_section) {
			line.GetTokenStr(tokens[0], 0);
			line.GetTokenStr(tokens[1], 1);
			idents.AddMember(enum_identifier, tokens[0], tokens[1]);
			RegisterSize(&max_enum_string_array_string_size, tokens[1]);
			// TODO possibly add code to process lines as enum-only entries
			// if token[1] fails
			process_result = R_UNFINISHED;
		} else {
			process_result = R_ERROR;
			error_type = E_UNKNOWN_DIRECTIVE;
		}
		}
	}
}

void Lexer::Process_D_GRAMMAR_COMMENT(void) {
	//ignore
	process_result = R_IGNORE;
}

void Lexer::Process_D_NULL(void) {
	if (header_section) {  // handle blank lines in the code and header section
		line.GetRawBuffer(output_string);
		strcat(output_string, "\n");
		header_output_queue.EnQueue(output_string);
		header_output_available = true;
		process_result = R_UNFINISHED;
	} else {
		if (code_section) {  // handle blank lines in the code and header section
			line.GetRawBuffer(output_string);
			strcat(output_string, "\n");
			code_output_queue.EnQueue(output_string);
			code_output_available = true;
			process_result = R_UNFINISHED;
		} else {
		// ignore NULL token
		process_result = R_COMPLETE;
		//error_type = E_NULL_TOKEN_TO_LEXER;
		}
	}
}

void Lexer::Process_D_CODE_START(void) {
	if (code_section || header_section || grammar_section) {
		process_result = R_ERROR;
		error_type = E_CODE_WHILE_OTHER_ACTIVE;
	} else {
		code_section = true;
		process_result = R_UNFINISHED;
		user_output_queue.EnQueue("Found code-start\n");
		user_output_available = true;
	}
}

void Lexer::Process_D_CODE_END(void) {
	if (code_section) {
		code_section = false;
		process_result = R_COMPLETE;
		user_output_queue.EnQueue("Code Specified by code-start and code-end inserted into the code file\n");
		user_output_available = true;
	} else {
		process_result = R_ERROR;
		error_type = E_CODE_END_WITHOUT_START;
	}
}

void Lexer::Process_D_GRAMMAR_START(void) {
	if (code_section || header_section || grammar_section) {
		process_result = R_ERROR;
		error_type = E_GRAMMAR_WHILE_OTHER_ACTIVE;
	} else {
		grammar_section = true;
		process_result = R_COMPLETE;
		user_output_queue.EnQueue("Found grammar-start\n");
		user_output_available = true;
	}
}


void Lexer::Process_D_COMMENT(void) {
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
}

void Lexer::Process_D_SUB_SECTION_CLOSE(void) {
	if (line.GetTokenStr(token_str, 1) == NULL) {
		process_result = R_ERROR;
		error_type = E_SUBSECTION_CLOSE_TOKEN_NULL;
	} else {
		if (sub_section_closes.AddString(token_str)) {
			process_result = R_COMPLETE;
			sprintf(temp_string, "Registered \"%s\" as a config sub-section closure\n", token_str);
			user_output_queue.EnQueue(temp_string);
			user_output_available = true;
		} else {
			process_result = R_ERROR;
			error_type = E_INTERNAL_ERROR_ADDING_TO_SET;
		}
	}
}

void Lexer::Process_D_REDUNDANT_CLOSE_AS_COMMENT(void) {
	redundant_close_as_comment = true;
	process_result = R_COMPLETE;
	user_output_queue.EnQueue("Redundant-close-as-comment set ON\n");
	user_output_available = true;
}

void Lexer::Process_D_IGNORE_CASE(void) {
	ignore_case = true;
	process_result = R_COMPLETE;
	user_output_queue.EnQueue("Ignore-case set ON\n");
	user_output_available = true;
}

void Lexer::Process_D_ESCAPE_SEQUENCE(void) {
    if (line.GetTokenStr(token_str, 1) == NULL) {
        process_result = R_ERROR;
        error_type = E_ESCAPE_SEQUENCE_TOKEN_NULL;
    } else {
        if (escape_sequences.AddString(token_str)) {
            process_result = R_COMPLETE;
			sprintf(temp_string, "Registered \"%s\" as a terminal escape sequence\n", token_str);
			user_output_queue.EnQueue(temp_string);
			user_output_available = true;
        } else {
            process_result = R_ERROR;
            error_type = E_INTERNAL_ERROR_ADDING_TO_SET;
        }
    }
}

void Lexer::Process_D_ENUM_TERMINATING_MEMBER(void) {
	if(strcmp(enum_start_value, "0") == 0) {
		enum_terminating_member = true;
		process_result = R_COMPLETE;
		user_output_queue.EnQueue("Enums with terminating members set to ON\n");
		user_output_available = true;
	} else {
        process_result = R_ERROR;
        error_type = E_ENUM_START_NOT_ZERO_AND_TERMINATING_MEMBER_TRUE;
	}
}

void Lexer::Process_D_ENUM_PLUS_LIST_ARRAY(void) {
	enum_plus_list_array = true;
	process_result = R_COMPLETE;
	user_output_queue.EnQueue("Enums with associated string lists set to ON\n");
	user_output_available = true;
}

void Lexer::Process_D_ENUM_NO_TERMINATING_MEMBER(void) {
	enum_terminating_member = false;
	process_result = R_COMPLETE;
	user_output_queue.EnQueue("Enums with terminating members set to OFF\n");
	user_output_available = true;
}

void Lexer::Process_D_ENUM_NO_LIST_ARRAY(void) {
	enum_plus_list_array = false;
	process_result = R_COMPLETE;
	user_output_queue.EnQueue("Enums with associated string lists set to OFF\n");
	user_output_available = true;
}

void Lexer::Process_D_ENUM_START_VALUE(void) {
    if (line.GetTokenStr(token_str, 1) == NULL) {
        process_result = R_ERROR;
        error_type = E_ENUM_START_VALUE_NULL;
    } else {
    	if ((strcmp(token_str, "0") != 0) && enum_terminating_member) {
            process_result = R_ERROR;
            error_type = E_ENUM_START_NOT_ZERO_AND_TERMINATING_MEMBER_TRUE;
    	} else {
    		strcpy(enum_start_value, token_str);
    		process_result = R_COMPLETE;
    		sprintf(temp_string, "Enum start value set to %s\n", token_str);
    		user_output_queue.EnQueue(temp_string);
    		user_output_available = true;
    	}
    }
}

void Lexer::Process_D_ENUM_ARRAY_TYPE(void) {
    if (line.GetTokenStr(token_str, 1) == NULL) {
        process_result = R_ERROR;
        error_type = E_ENUM_ARRAY_TYPE_NULL;
    } else {
    	RegisterSize(&max_identifier_label_size, token_str);
        strcpy(enum_array_type, token_str);
        process_result = R_COMPLETE;
		sprintf(temp_string, "Enum string array type set to: %s\n", token_str);
		user_output_queue.EnQueue(temp_string);
		user_output_available = true;
    }
}

void Lexer::Process_D_ENUM_ARRAY_INSTANCE(void) {
    if (line.GetTokenStr(token_str, 1) == NULL) {
        process_result = R_ERROR;
        error_type = E_ENUM_ARRAY_INSTANCE_NULL;
    } else {
    	RegisterSize(&max_identifier_label_size, token_str);
    	RegisterSize(&max_ast_label_size, token_str);
        strcpy(enum_array_instance, token_str);
        process_result = R_COMPLETE;
        // indicate user output at end of processing the list
    }
}

void Lexer::Process_D_ENUM_ARRAY_MEMBER_LABEL(void) {
    if (line.GetTokenStr(token_str, 1) == NULL) {
        process_result = R_ERROR;
        error_type = E_ENUM_ARRAY_MEMBER_LABEL_NULL;
    } else {
    	RegisterSize(&max_identifier_label_size, token_str);
        strcpy(enum_array_member_label, token_str);
        process_result = R_COMPLETE;
		sprintf(temp_string, "Enum array member field label set to: %s\n", token_str);
		user_output_queue.EnQueue(temp_string);
		user_output_available = true;
    }
}

void Lexer::Process_D_ENUM_ARRAY_RESERVE(void) {
    enum_array_reserve_words = true;
    process_result = R_COMPLETE;
    // XXX not sure if this is used or is defualt anyway
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
    	RegisterSize(&max_identifier_label_size, token_str);
        strcpy(enum_identifier, token_str);
        process_result = R_COMPLETE;
        // indicate user output at end of processing the list
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
        if ((enum_array_type[0] == '\0') ||
        (enum_array_instance[0] == '\0') ||
        (enum_array_member_label[0] == '\0')) {
            process_result = R_ERROR;
            error_type = E_ENUM_START_BEFORE_PREREQ;
        }
    }
    if (enum_array_reserve_words) {
        if ((!enum_plus_list_array) ||
        (enum_identifier[0] == '\0')) {
            process_result = R_ERROR;
            error_type = E_ENUM_START_BEFORE_PREREQ;
        }
    }
    // Check there were no errors - now set up the identifier
    if (process_result != R_ERROR) {
    	if (enum_plus_list_array) {
    		error_type = idents.NewIdent(enum_identifier, ID_ENUM_ARRAY_PAIR);
    		idents.SetInstanceName(enum_identifier, enum_array_instance);
            // indicate user output at end of processing the list
    	} else {
    		error_type = idents.NewIdent(enum_identifier, ID_ENUM_LIST);
    	}
    	if (error_type == E_NO_ERROR) {
    		enum_section = true;
    		process_result = R_UNFINISHED;
    	} else {
            process_result = R_ERROR;

    	}
    }
}

void Lexer::Process_D_ENUM_END(void) {
	// Write the enum definition and the associated string array (if defined)
	// to the header file.

	// write the start of the enum definition
	strcpy(output_string, "enum {\n");
	header_output_queue.EnQueue(output_string);

	// Get the size of the ident list holding the enum symbols
	uint16_t enum_list_size = idents.GetSizeByIdentifierName(enum_identifier);
	uint16_t string_list_size = 0;	// work this out later depending on terminating member

	// and iterate through the enums writing the symbols to the header file
	for (uint16_t i = 0; i < enum_list_size; i++) {
		if (enum_plus_list_array) {
			// get the enum symbol and the associated string
			idents.GetEntryAtLocation(enum_identifier,tokens[0], tokens[1], i);
		} else {
			// or only the enum symbol if there's no strings
			idents.GetEntryAtLocation(enum_identifier,tokens[0], i);
		}
		// write the symbol
		strcpy(output_string, "\t");
		strcat(output_string, tokens[0]);
		// if its the first symbol, append " = <enum_start_value>"
		if (i == 0) {
            strcat(output_string, " = ");
            strcat(output_string, enum_start_value);
		}
		// and then a ','
		strcat(output_string, ",\n");
		header_output_queue.EnQueue(output_string);
	}
	// write the enum close
	strcpy(output_string, "};\n\n");
	header_output_queue.EnQueue(output_string);

    // tell the user
	sprintf(temp_string, "Processed enum list with enum identifier: %s\n", enum_identifier);
	user_output_queue.EnQueue(temp_string);
	user_output_available = true;

	// set up for the associated string array (if defined)
	if (enum_plus_list_array) {
		if (enum_terminating_member) {
			// See discussion elsewhere on looping arrays with enums that have terminating members
			// A prerequisite is that the value of the first enum symbol is 0 - checked elsewhere

			// If there's a terminating enum member then
			// - its symbol is the size of the string array
			// - its located at enum_list_size -1.
			// Put it into token[0] (token[1] will be null but working with a tupple get both)
			idents.GetEntryAtLocation(enum_identifier,tokens[0], tokens[1], enum_list_size-1);
			// and the string list size is one less than the enum list size
			string_list_size = enum_list_size - 1;
		} else {
			// no terminating member - string array size cardinal
			// will be the number of entries in the enum
			sprintf(token_str, "%u", enum_list_size);
			strcpy(tokens[0], token_str);
			string_list_size = enum_list_size;
		}

		// write the string array definition - token[0] containing the size
		strcpy(output_string, "#ifdef USE_PROGMEM\n");
    	header_output_queue.EnQueue(output_string);

    	strcpy(output_string, "static const ");
		strcat(output_string, enum_array_type);
		strcat(output_string, " ");
		strcat(output_string, enum_array_instance);
		strcat(output_string, " [");
		strcat(output_string, tokens[0]);
		strcat(output_string, "] PROGMEM = {\n");
    	header_output_queue.EnQueue(output_string);

		strcpy(output_string, "#else\n");
    	header_output_queue.EnQueue(output_string);

    	strcpy(output_string, "static const ");
		strcat(output_string, enum_array_type);
		strcat(output_string, " ");
		strcat(output_string, enum_array_instance);
		strcat(output_string, " [");
		strcat(output_string, tokens[0]);
		strcat(output_string, "] = {\n");
    	header_output_queue.EnQueue(output_string);

		strcpy(output_string, "#endif\n");
    	header_output_queue.EnQueue(output_string);

    	// write the instantiation strings
    	for (uint16_t i = 0; i < string_list_size; i++) {
			idents.GetEntryAtLocation(enum_identifier,tokens[0], tokens[1], i);
    		strcpy(output_string, "\t\"");
    		strcat(output_string, tokens[1]);
    		strcat(output_string, "\",\n");
        	header_output_queue.EnQueue(output_string);
    	}

    	// write the array close
    	strcpy(output_string, "};\n\n");
    	header_output_queue.EnQueue(output_string);
    }

    header_output_available = true;
    enum_section = false;
    process_result = R_COMPLETE;

    // tell the user
	sprintf(temp_string, "Processed associated string list of type: %s Array name: %s\n", enum_array_type, enum_array_instance);
	user_output_queue.EnQueue(temp_string);
	user_output_available = true;


    // Housekeeping - These need to be explicitly defined for each
    // enum list, so null them out so they get caught in the pre-req
    // checks if they have not been explicitly defined for the next enum.

    // enum_array_type[0] = '\0';
    enum_array_instance[0] = '\0';
    enum_identifier[0] = '\0';
}

void Lexer::Process_D_TERM(void) {
	// Called to process a line where the first directive
	// has the form %<n> starting with %1

	// get the term that follows
	if (line.GetTokenStr(tokens[1], 1) == NULL) {
		process_result = R_ERROR;
		error_type = E_TERM_WITHOUT_ARGUMENTS;
	} else {
		// There is a second token, proceed
		// get rid of the leading % by shifting that token[0] left
		int i;
		for (i = 1; i < MAX_BUFFER_WORD_LENGTH; i++) {
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

			// see if this is a 2 or 3 token form
			if (line.GetTokenStr(tokens[2], 2) == NULL) {
				// 2 - Must be a param type - AST will check for valid types when added
				error_type = ast.NewNode(term_level, tokens[1]);
		    	RegisterSize(&max_ast_label_size, tokens[1]);
			} else {
				// it is a 3 token form (identifier or lookup)
				if (   (strcmp(tokens[1], "identifier") == 0)
					|| (strcmp(tokens[1], "lookup") == 0) ) {
					// check that it has already been defined
					if (idents.Exists(tokens[2])) {
						// add it to the term to the AST
						error_type = ast.NewNode(term_level, tokens[1], tokens[2]);
				    	RegisterSize(&max_identifier_label_size, tokens[2]);
				    	RegisterSize(&max_ast_label_size, tokens[2]);
					} else {
						// tried to use
						error_type = E_UNKNOWN_IDENT_OR_LOOKUP;
					}
				} else {
					// it should be a keyword - check
					if (strcmp(tokens[1], "keyword") == 0) {
						// XXX duplicate check needs to be done in AST - its the only place the keywords persist
						error_type = ast.NewNode(term_level, tokens[1], tokens[2]);
				    	RegisterSize(&max_ast_label_size, tokens[1]);
						RegisterSize(&max_identifier_label_size, tokens[2]);
				    	RegisterSize(&max_ast_label_size, tokens[2]);
					} else {
						error_type = E_UNKNOWN_TERM_OR_MALFORMED_DIRECTIVE;
					}

				}
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
				if ((error_type = idents.NewIdent(tokens[1], ID_ACTION_PAIR)) == E_NO_ERROR) {
					idents.SetInstanceName(tokens[1], tokens[2]);
			    	RegisterSize(&max_identifier_label_size, tokens[1]);
			    	RegisterSize(&max_identifier_label_size, tokens[2]);
			    	RegisterSize(&max_ast_action_size, tokens[2]);
					process_result = R_COMPLETE;
				} else {
					process_result = R_ERROR;
				}
			}
		}
	}
}

void Lexer::Process_D_ACTION(void) {
	// Attach the action to the last added AST node
	// check first that the action identifier has been defined
	// Build the action calling code

    if (line.GetTokenStr(tokens[1], 1) == NULL) {
        process_result = R_ERROR;
        error_type = E_ACTON_WITHOUT_IDENTIFIFER;
    } else {
    	if(!idents.Exists(tokens[1])) {
    		error_type = E_ACTION_NOT_DEFINED;
    		process_result = R_ERROR;
    	} else {
    		ast.AttachActionToCurrent(tokens[1]);
    		ast.BuildActionCode(idents);
    		while (ast.header_output_queue.OutputAvailable()) {
    			ast.header_output_queue.GetOutputAsString(output_string);
    			header_output_queue.EnQueue(output_string);
        		header_output_available = true;
    		}
    		while (ast.user_code_output_queue.OutputAvailable()) {
    			ast.user_code_output_queue.GetOutputAsString(output_string);
    			user_code_output_queue.EnQueue(output_string);
    			user_code_output_available = true;
    		}
    		while (ast.code_output_queue.OutputAvailable()) {
    			ast.code_output_queue.GetOutputAsString(output_string);
    			code_output_queue.EnQueue(output_string);
    			code_output_available = true;
    		}

    		process_result = R_COMPLETE;
    		action_since_last_term = true;
    	}
    }
}

void Lexer::Process_D_GRAMMAR_END(void) {
	// Grammar file definitions now complete
	// - close the action caller
	//	(As each action is declared in the grammar, the user_code templates
	//		are built - because building the parameter lists for the function templates
	//		requires an AST walk "back up" the tree from the current action node
	//		to enumerate all the keywords, idents, lookups, and params that are the
	//		"up tree" nodes on the path to that action and need to be included
	//		to make up the calls.)
	// - Write out the XLAT arrays and lookup functions to the code and header files
	// - Search the identifier set for LOOKUP nodes and build the prototypes
	//	 and code templates

	if (grammar_section) {

		// write the default case and closures for the action caller
		// Before writing anything else to the code file
		sprintf(output_string, "\t\tdefault:\n");
		code_output_queue.EnQueue(output_string);
		sprintf(output_string, "\t\t\treturn PE_FUNC_XLAT_NOT_MATCHED_IN_CALLFUNCTION;\n");
		code_output_queue.EnQueue(output_string);
		sprintf(output_string, "\t\t\tbreak;\n");
		code_output_queue.EnQueue(output_string);
		sprintf(output_string, "\t}\n");
		code_output_queue.EnQueue(output_string);
		sprintf(output_string, "\treturn PEME_NO_ERROR;\n");
		code_output_queue.EnQueue(output_string);
		sprintf(output_string, "}\n\n");
		code_output_queue.EnQueue(output_string);

		//
		// To Do List
		//
		//strcpy(output_string, "//TODO: AST Validation Walk - \n");
		//code_output_queue.EnQueue(output_string);
		//strcpy(output_string, "//TODO: AST Order Ambiguity Report\n");
		//code_output_queue.EnQueue(output_string);
		//strcpy(output_string, "//TODO: Parser Match Partial Identifiers \n");
		//code_output_queue.EnQueue(output_string);
		//strcpy(output_string, "//TODO: AST Warn unused IDs, lookups, params\n");
		//code_output_queue.EnQueue(output_string);
		//strcpy(output_string, "//TODO: Configuration Grammar with %section directive\n");
		//code_output_queue.EnQueue(output_string);
		//strcpy(output_string, "//TODO: Context change on <identifier> value\n\n");
		//code_output_queue.EnQueue(output_string);

		// Write out the main ASTA Array
		ast.WriteASTArray(&idents);
		// and send it to the header file
		while (ast.header_output_queue.OutputAvailable()) {
			ast.header_output_queue.GetOutputAsString(output_string);
			header_output_queue.EnQueue(output_string);
		}

		// write out the identifiers "XLAT map arrays" (idents, lookups, actions)
		idents.WriteXLATMapArrays();
		// and send it to the header file
		while (idents.output.OutputAvailable()) {
			idents.output.GetOutputAsString(output_string);
			header_output_queue.EnQueue(output_string);
		}

		// write out the "XLAT map lookup function prototypes" (idents, lookups, actions)
		idents.WriteXLATMapLookupFunctions(WRITE_HEADER);
		// and send it to the header file
		while (idents.output.OutputAvailable()) {
			idents.output.GetOutputAsString(output_string);
			header_output_queue.EnQueue(output_string);
		}

		// write out the "XLAT map lookup function bodies" (idents, lookups, actions)
		idents.WriteXLATMapLookupFunctions(WRITE_CODE);
		// and send it to the code file
		while (idents.output.OutputAvailable()) {
			idents.output.GetOutputAsString(output_string);
			code_output_queue.EnQueue(output_string);
		}

		// write out the XLAT map "ident member" lookup functions prototypes
		idents.WriteIdentMemberLookupFunction(WRITE_HEADER);
		// and send it to the header file
		while (idents.output.OutputAvailable()) {
			idents.output.GetOutputAsString(output_string);
			header_output_queue.EnQueue(output_string);
		}

		// write out the XLAT map "ident member" lookup function bodies
		idents.WriteIdentMemberLookupFunction(WRITE_CODE);
		// and send it to the code file
		while (idents.output.OutputAvailable()) {
			idents.output.GetOutputAsString(output_string);
			code_output_queue.EnQueue(output_string);
		}

		// write out the XLAT map "lookup member" lookup functions prototypes
		idents.WriteLookupMemberLookupFunction(WRITE_HEADER);
		// and send it to the header file
		while (idents.output.OutputAvailable()) {
			idents.output.GetOutputAsString(output_string);
			header_output_queue.EnQueue(output_string);
		}

		// write out the XLAT map "lookup member" lookup function bodies
		idents.WriteLookupMemberLookupFunction(WRITE_CODE);
		// and send it to the code file
		while (idents.output.OutputAvailable()) {
			idents.output.GetOutputAsString(output_string);
			code_output_queue.EnQueue(output_string);
		}

		// write out user code lookup function outlines - prototype first
		idents.WriteUserLookupFunctions(WRITE_HEADER);
		// and send it to the header file
		while (idents.output.OutputAvailable()) {
			idents.output.GetOutputAsString(output_string);
			header_output_queue.EnQueue(output_string);
		}

		// write out user code lookup function outlines - user_code
		idents.WriteUserLookupFunctions(WRITE_USER_CODE);
		// and send it to the user_code file
		while (idents.output.OutputAvailable()) {
			idents.output.GetOutputAsString(output_string);
			user_code_output_queue.EnQueue(output_string);
		}



		// Add the User Code Action Function Caller function prototype to the header
		sprintf(output_string, "uint16_t CallFunction(uint8_t func_xlat, ParamUnion params[]);\n");
		header_output_queue.EnQueue(output_string);

		header_output_available = true;
		code_output_available = true;
		user_code_output_available = true;
		grammar_section = false;
		process_result = R_COMPLETE;
	} else {
		process_result = R_ERROR;
		error_type = E_GRAMMAR_END_WITHOUT_START;
	}
}

void Lexer::Process_D_INCLUDE_HEADER(void) {
	if (line.GetTokenStr(tokens[1], 1) == NULL) {
		process_result = R_ERROR;
		error_type = E_INCLUDE_WITHOUT_FILENAME;
	} else {
		FILE* f;
		char* res;
		char in_buf[MAX_BUFFER_LENGTH];
		f = fopen(tokens[1], "r");
		if (f != NULL) {
			while (!feof(f)) {
				res = fgets(in_buf, MAX_BUFFER_LENGTH, f);
				if (res != NULL) {
					header_output_queue.EnQueue(in_buf);
					header_output_available = true;
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

void Lexer::Process_D_INCLUDE_CODE(void) {
	if (line.GetTokenStr(tokens[1], 1) == NULL) {
		process_result = R_ERROR;
		error_type = E_INCLUDE_WITHOUT_FILENAME;
	} else {
		FILE* f;
		char* res;
		char in_buf[MAX_BUFFER_LENGTH];
		f = fopen(tokens[1], "r");
		if (f != NULL) {
			while (!feof(f)) {
				res = fgets(in_buf, MAX_BUFFER_LENGTH, f);
				if (res != NULL) {
					code_output_queue.EnQueue(in_buf);
					code_output_available = true;
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

void Lexer::Process_D_LOOKUP_LIST(void) {
	// Set up an identifier list of key val pairs
	//	<lookup-identifier> as IdentifierName
	// Set FunctionName as InstanceName
	//
	//	//TODO: Then later populate through grammar parsing for
	//		parameter types and names in order.
	//	//TODO: for now the lookup function just returns 0 or 1
	//		to indicate if the string is in the list
	//		- lookup code to be provided by user in out_user_code.cpp
	line.GetTokenStr(tokens[0], 0);

	if (line.GetTokenStr(tokens[1], 1) == NULL) {
		error_type = E_EXPECTED_LOOKUP_IDENTIFIER;
		process_result = R_ERROR;

	} else {
		if (line.GetTokenStr(tokens[2], 2) == NULL) {
			error_type = E_EXPECTED_FUNCTION_FOR_LOOKUP;
			process_result = R_ERROR;
		} else {
			if (idents.Exists(tokens[1])) {
				error_type = E_IDENTIFIER_ALREADY_EXISTS;
				process_result = R_ERROR;
			} else {
				error_type = idents.NewIdent(tokens[1], ID_LOOKUP_LIST);
				if (error_type == E_NO_ERROR) {
					idents.SetInstanceName(tokens[1], tokens[2]);
			    	RegisterSize(&max_identifier_label_size, tokens[2]);
					process_result = R_COMPLETE;
				} else {
					process_result = R_ERROR;
				}
			}
		}
	    // tell the user
		sprintf(temp_string, "Registered lookup list \"%s\" calling function instance \"%s\"\n", tokens[1], tokens[2]);
		user_output_queue.EnQueue(temp_string);
		user_output_available = true;
	}
}

void Lexer::Process_D_HEADER_START(void) {
	if (code_section || header_section || grammar_section) {
		process_result = R_ERROR;
		error_type = E_HEADER_WHILE_OTHER_ACTIVE;
	} else {
		header_section = true;
		process_result = R_UNFINISHED;
	}
}

void Lexer::Process_D_HEADER_END(void) {
	if (header_section) {
		header_section = false;
		process_result = R_COMPLETE;
	} else {
		process_result = R_ERROR;
		error_type = E_HEADER_END_WITHOUT_START;
	}
}

bool Lexer::Header_OutputAvailable() {
    return header_output_available;
}

bool Lexer::User_OutputAvailable() {
    return user_output_available;
}

bool Lexer::Code_OutputAvailable() {
    return code_output_available;
}

bool Lexer::User_Code_OutputAvailable() {
    return user_code_output_available;
}


char* Lexer::GetOutputAsString(int queue, char* output_str) {

	switch (queue) {
		case Q_USER:
			user_output_queue.DeQueue(output_string);
			if (user_output_queue.GetSize() == 0) {
				user_output_available = false;
			}
			return strcpy(output_str, output_string);
			break;
		case Q_HEADER:
			header_output_queue.DeQueue(output_string);
			if (header_output_queue.GetSize() == 0) {
				header_output_available = false;
			}
			return strcpy(output_str, output_string);
			break;
		case Q_CODE:
			code_output_queue.DeQueue(output_string);
			if (code_output_queue.GetSize() == 0) {
				code_output_available = false;
			}
			return strcpy(output_str, output_string);
			break;
		case Q_USER_CODE:
			user_code_output_queue.DeQueue(output_string);
			if (user_code_output_queue.GetSize() == 0) {
				user_code_output_available = false;
			}
			return strcpy(output_str, output_string);
			break;
		default:
			printf("FATAL Error, Nonexistent output queue referenced in Lexer::GetOutputAsString()\n");
			exit(-1);
	}
}


