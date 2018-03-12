/*****************************************************************
 AST.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#include "common_config.h"
#include "AST.h"
#include "KeyValuePairList.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "glitch_errors.h"


AST::AST() {
	root = NULL;
	current = NULL;
	current_level = 0;
	next_id = 1;

    output_string[0] = '\0';

	grammar_def_count = 0;
	max_param_count = 0;

	caller_func_preamble_done = false;

}

AST::~AST() {
	// Auto-generated destructor stub
}

int AST::AddSiblingToCurrent(ASTNode * node) {
	ASTNode* walker;
	if(root == NULL) {		//first node should always be a child of root
		return E_SIBLING_TO_ROOT_ATTEMPTED;
	}

	if(current->next_sibling == NULL) {
		// check for duplicate keyword entries as we walk
		if ((current->type == AST_KEYWORD) && (node->type == AST_KEYWORD )) {
			if (strcmp(current->label, node->label) == 0) {
				return E_DUPLICATE_KEYWORD_AT_LEVEL;
			}
		}
		current->next_sibling = node;
	} else {
		walker = current->next_sibling;
		while(walker->next_sibling != NULL) {
			// check for duplicate keyword entries as we walk
			if ((walker->type == AST_KEYWORD) && (node->type == AST_KEYWORD )) {
				if (strcmp(walker->label, node->label) == 0) {
					return E_DUPLICATE_KEYWORD_AT_LEVEL;
				}
			}
			walker = walker->next_sibling;
		}
		walker->next_sibling = node;
	}
	node->next_sibling = NULL;
	node->first_child = NULL;
	node->parent = current->parent;
	current = node;
	return E_NO_ERROR;
}

int AST::AddChildToCurrent(ASTNode* node) {
	ASTNode* walker;

	if (root == NULL) {		//special case - first node from level 0
		root = node;
		node->parent = NULL;
	} else {
		if(current->first_child == NULL) {
			current->first_child = node;
		} else {
			walker = current->first_child;
			while(walker->next_sibling != NULL) {
				// check for duplicate keyword entries as we walk
				if ((walker->type == AST_KEYWORD) && (node->type == AST_KEYWORD )) {
					if (strcmp(walker->label, node->label) == 0) {
						return E_DUPLICATE_KEYWORD_AT_LEVEL;
					}
				}
				walker = walker->next_sibling;
			}
			walker->next_sibling = node;
		}
		node->parent = current;
	}

	node->first_child = NULL;
	node->next_sibling = NULL;
	current = node;
	return E_NO_ERROR;
}


int AST::ValidateTermTypeStr(const char* type_str) {
	// check for valid term type and return it
	int i;
	for (i = LAST_AST_TYPE; i != 0; i--) {
		if (strcmp(ast_type_strings[i].text, type_str) == 0) break;
	}
	return i;
}

bool AST::CheckForExistingSiblingKeywords(ASTNode* start_node, const char* keyword) {
	ASTNode* temp;

	if (start_node == NULL) {
		// empty tree?
		return false;
	}
	// get to the top of the sibling list at this level
	if (start_node->parent == NULL) {
		// must be a level 1 node
		temp = root;
	} else {
		temp = start_node->parent->first_child;
	}
	// now at first member of a sibling group
	if (temp->type == AST_KEYWORD) {
		if (strcmp(temp->label, keyword) == 0) {
			// duplicate found
			return true;
		}
	}
	while (temp->next_sibling != NULL) {
		temp = temp->next_sibling;
		if (temp->type == AST_KEYWORD) {
			if (strcmp(temp->label, keyword) == 0) {
				// duplicate found
				return true;
			}
		}
	}
	return false;
}

int AST::AddNode(int term_level, const char* term_type) {
	return AddNode(term_level, term_type, NULL);
}

int AST::AddNode(int term_level, const char* term_type, const char* term) {

	// create a new AST node
	ASTNode* n;
	n = new(ASTNode);

	n->id = next_id; 	// next_id - just an incrementing int already set to the next available.
	next_id++;
	if(term != NULL) strcpy(n->label, term);
	n->term_level = term_level;

	// check for valid term type and set it
	n->type = ValidateTermTypeStr(term_type);
	if (n->type == 0) {
		return E_TERM_TYPE_NOT_RECOGNISED;
	}

	// check that term level is the same or incrementing (ie adding a leaf node to the tree)
	if (term_level > current_level + 1) {
		return E_TERM_LEVEL_NOT_INCRMENTAL;
	}

	// Perform some naming rule checks
	// - dont need to here - regex checked when adding to identifier lists

	// Add as a sibling or child as appropriate to level
	if (term_level == current_level +1) {
		current_level = term_level;
		return AddChildToCurrent(n);
	}
	if (term_level == current_level) {
		return AddSiblingToCurrent(n);
	}
	while (term_level < current_level) {
		current_level--;
		current = current->parent;
	}
	return AddSiblingToCurrent(n);
}

void AST::WriteASTArray(Identifiers* idents) {

	grammar_def_count = 0;

	DT(root, idents, false);  // to count the output lines

	sprintf(output_string, "\n#ifdef USE_PROGMEM\n");
	header_output_queue.EnQueue(output_string);
	sprintf(output_string, "static const ASTA asta [%d] PROGMEM = {\n", grammar_def_count);
	header_output_queue.EnQueue(output_string);
	sprintf(output_string, "#else\n");
	header_output_queue.EnQueue(output_string);
	sprintf(output_string, "static const ASTA asta [%d] = {\n", grammar_def_count);
	header_output_queue.EnQueue(output_string);
	sprintf(output_string, "#endif\n");
	header_output_queue.EnQueue(output_string);

	DT(root, idents, true);

	sprintf(output_string, "};\n\n");
	header_output_queue.EnQueue(output_string);

	header_output_queue.SetOutputAvailable();
}

void AST::DT(ASTNode* w, Identifiers* idents, bool print) {
	char temp[MAX_BUFFER_LENGTH];
	char instance_name[MAX_BUFFER_LENGTH];
	char type_string[MAX_BUFFER_LENGTH];

	if (w != NULL) {
//		sprintf(output_string, "\t%u, \"%s\", \"%s\", \"%s\", %u, %u, %d, %d, ", w->id, w->label, w->unique, w->help, w->term_level, w->type, w->action, w->finish);
		sprintf(output_string, "\t%u, %u, ", w->id, w->type);
		switch (w->type) {
			case AST_KEYWORD:
			case AST_LOOKUP:
				sprintf(temp, "\"%s\", ", w->label);
				break;
			case AST_IDENTIFIER:

				// output instance name rather than label
				idents->GetInstanceName(w->label, instance_name);
				sprintf(temp, "\"%s\", ", instance_name);
				break;
			case AST_PARAM_DATE:
			case AST_PARAM_TIME:
			case AST_PARAM_INTEGER:
			case AST_PARAM_FLOAT:
			case AST_PARAM_STRING:
				GetASTTypeString(type_string, w->type);
				sprintf(temp, "\"%s\", ", type_string);
				break;
			default:
				break;
		}
		strcat(output_string, temp);
		sprintf(temp, "%u, ", w->action);
		strcat(output_string, temp);
		if (w->parent != NULL) {
			sprintf(temp, "%u, ", w->parent->id);
		} else {
			sprintf(temp, "0, ");
		}
		strcat(output_string, temp);

		if (w->first_child != NULL) {
			sprintf(temp, "%u, ", w->first_child->id);
		} else {
			sprintf(temp, "0, ");
		}
		strcat(output_string, temp);

		if (w->next_sibling != NULL) {
			sprintf(temp, "%u, ", w->next_sibling->id);
		} else {
			sprintf(temp, "0, ");
		}
		strcat(output_string, temp);

		sprintf(temp, "\"%s\",\n", w->action_identifier);
		strcat(output_string, temp);

		grammar_def_count++;

		if (print) header_output_queue.EnQueue(output_string);

		DT(w->first_child, idents, print);
		DT(w->next_sibling, idents, print);
	}
}


int AST::GetASTTypeString(char* return_string, int type) {
	// check for valid term type and return it
	int i;
	for (i = LAST_AST_TYPE; i != 0; i--) {
		if(i == type) {
			strcpy(return_string, ast_type_strings[i].text);
			return i;
		}
	}
	return i;
}

void AST::AttachActionToCurrent(char* action_identifier) {
	current->action = true;
	strcpy(current->action_identifier, action_identifier);
}


int AST::BuildActionCode(Identifiers& idents) {
	// While on the AST node that is action-able - "current", walk via
	// parent pointers up the tree, building the function parameter
	// list that will be included in the function prototype for this action.

	KeyValuePairList params;
	ASTNode* walker;
	int param_count = 0;
	int param_index = 0;
	char param_name[MAX_BUFFER_LENGTH];
	char param_type[MAX_BUFFER_LENGTH];
	char func_name[MAX_BUFFER_LENGTH];
	int func_xlat_id;
	char temp[MAX_BUFFER_LENGTH];
	char temp2[MAX_BUFFER_LENGTH];


	// ***************************** Count the Params for the Function Definition and Prototype
	walker = current;
	while (walker != NULL ) {
		// count the num of params that need to be included in the function
		if (walker->type > AST_KEYWORD) {
			param_count++;
		}

		// count the ones that needs to be numerically labelled
		if (walker->type >= AST_LOOKUP) {
			param_index++;
		}
		walker = walker->parent;
	}

	// remember the max count for define config in the parser
	if (param_count > max_param_count) max_param_count = param_count;

	// ***************************** Assemble param names and types walking up tree

	walker = current;
	while (walker != NULL) {
		switch (walker->type) {
			case AST_UNDEFINED:
				return E_AST_NODE_UNDEFINED_TYPE;
				break;
			case AST_KEYWORD:
				// ignore - it got us to here anyway
				break;
			case AST_IDENTIFIER:
				if (idents.Exists(walker->label)) {
					sprintf(param_type, "uint16_t");
					params.Add(walker->label, param_type);
				} else
					return E_BUILDING_ACTION_PROTO;
				break;
			case AST_LOOKUP:
				if (idents.Exists(walker->label)) {
					sprintf(param_type, "char*");
					params.Add(walker->label, param_type);
				} else
					return E_BUILDING_ACTION_PROTO;
				break;
			case AST_PARAM_STRING:
				sprintf(param_type, "char*");
				sprintf(param_name, "param%u_string", param_index);
				params.Add(param_name, param_type);
				param_index--;
				break;
			case AST_PARAM_INTEGER:
				sprintf(param_type, "int16_t");
				sprintf(param_name, "param%u_int", param_index);
				params.Add(param_name, param_type);
				param_index--;
				break;
			case AST_PARAM_FLOAT:
				sprintf(param_type, "float");
				sprintf(param_name, "param%u_float", param_index);
				params.Add(param_name, param_type);
				param_index--;
				break;
			case AST_PARAM_TIME:
				sprintf(param_type, "char*");
				sprintf(param_name, "param%u_time", param_index);
				params.Add(param_name, param_type);
				param_index--;
				break;
			case AST_PARAM_DATE:
				sprintf(param_type, "char*");
				sprintf(param_name, "param%u_date", param_index);
				params.Add(param_name, param_type);
				param_index--;
				break;
		}
		walker = walker->parent;
	}

	// ****************************** Get the function name
	// stored in instance_name of the idents headers
	if (idents.GetInstanceName(current->action_identifier, func_name) != E_NO_ERROR) {
		return E_BUILDING_ACTION_PROTO;
	}

	// ****************************** Get the function's xlat ID
	func_xlat_id = idents.GetXlatID(current->action_identifier);

	// *******************************   Function Caller Preamble - ONCE!
	// write the preamble for the function caller into the code file - Once!
	if(caller_func_preamble_done == false) {
		sprintf(output_string, "uint16_t CallFunction(uint8_t func_xlat, ParamUnion params[]) {\n");
		code_output_queue.EnQueue(output_string);
		sprintf(output_string, "\tswitch (func_xlat) {\n");
		code_output_queue.EnQueue(output_string);
		caller_func_preamble_done = true;
	}

	// ********************************* Header and User_Code function 1st line
	strcpy(output_string, "void ");
	sprintf(temp, "%s(", func_name);
	strcat(output_string, temp);

	// ***************************no params - (void)
	if (param_count == 0) {
		// preserve the line so far
		strcpy(temp2, output_string);

		// Header Proto
		strcat(output_string, "void);\n");
		header_output_queue.EnQueue(output_string);

		// Restore the line preserved earlier
		strcpy(output_string, temp2);

		// User_Code function 1st line
		sprintf(temp, "void) {\n");
		strcat(output_string, temp);
		user_code_output_queue.EnQueue(output_string);

	} else {
		// *************************** param list
		for (int i = param_count - 1; i > 0; i--) {
			params.GetPairAtLocation(param_name, param_type, i);
			sprintf(temp, "%s %s, ", param_type, param_name);
			strcat(output_string, temp);
		}

		// Preserve to far
		strcpy(temp2, output_string);

		// *************************************** last param and close - Prototype
		params.GetPairAtLocation(param_name, param_type, 0);
		sprintf(temp, "%s %s);\n", param_type, param_name);
		strcat(output_string, temp);
		header_output_queue.EnQueue(output_string);

		// Restore the line preserved earlier
		strcpy(output_string, temp2);

		// *************************************** last param and func open - User_Code
		sprintf(temp, "%s %s) {\n", param_type, param_name);
		strcat(output_string, temp);
		user_code_output_queue.EnQueue(output_string);
	}

	// ******************************************* Function caller Case:
	// No params - () call and close case
	if (param_count == 0) {
		sprintf(output_string, "\t\tcase %d:\n", func_xlat_id);
		code_output_queue.EnQueue(output_string);
		sprintf(output_string, "\t\t\t%s();\n", func_name);
		code_output_queue.EnQueue(output_string);
		sprintf(output_string, "\t\t\tbreak;\n");
		code_output_queue.EnQueue(output_string);
	} else {
		// Params to enumerate
		sprintf(output_string, "\t\tcase %d:\n", func_xlat_id);
		code_output_queue.EnQueue(output_string);
		sprintf(output_string, "\t\t\t%s(", func_name);

		// Iterate and enumerate the params
		int e = 0;
		for (int i = param_count - 1; i > 0; i--) {
			params.GetPairAtLocation(param_name, param_type, i);

			//remove the *
			char* star;
			star = strrchr(param_type, '*');
			if (star) {
				*star = '\0';
				strcat(param_type, "_star");
			}

			sprintf(temp, "params[%d].param_%s, ", e, param_type);
			strcat(output_string, temp);
			e++;
		}

		// Last param and close the call and case
		params.GetPairAtLocation(param_name, param_type, 0);

		//remove the *
		char* star;
		star = strrchr(param_type, '*');
		if (star) {
			*star = '\0';
			strcat(param_type, "_star");
		}

		// form the last param and write the close and case break;
		sprintf(temp, "params[%d].param_%s);\n", param_count -1 , param_type);
		strcat(output_string, temp);
		code_output_queue.EnQueue(output_string);
		sprintf(output_string, "\t\t\tbreak;\n");
		code_output_queue.EnQueue(output_string);
	}


	//ShowBlockByCategory(params[0].param_int16);
	//char** ShowBlockCatN(int block_category, int param1_int);
	//ShowBlockCatN(params[0].param_int16, params[1].param_int16);

	// *************************************** User_Code function body
	sprintf(output_string, "\t// >>>\n");
	user_code_output_queue.EnQueue(output_string);
	sprintf(output_string, "\t// >>> INSERT ACTION CODE HERE\n");
	user_code_output_queue.EnQueue(output_string);
	sprintf(output_string, "\t// >>>\n");
	user_code_output_queue.EnQueue(output_string);
	sprintf(output_string, "\tchar temp[MAX_BUFFER_LENGTH];\n");
	user_code_output_queue.EnQueue(output_string);

	// have the example code print debug info (func and param types / value)
	sprintf(output_string, "\tsprintf(temp, \"%s(...) with param list (", func_name);

	if (param_count == 0) {
		sprintf(temp, "void)\\n\\n\\r\");\n");
		strcat(output_string, temp);
		user_code_output_queue.EnQueue(output_string);
	} else {
		for (int i = param_count - 1; i > 0; i--) {
			params.GetPairAtLocation(param_name, param_type, i);
			if ( (strcmp(param_type, "uint16_t") == 0) || (strcmp(param_type, "int16_t") == 0) ) {
				sprintf(temp, "%s:%s:%%d, ", param_type, param_name);
			} else {
				if (strcmp(param_type, "char*") == 0) {
					sprintf(temp, "%s:%s:%%s, ", param_type, param_name);
				} else {
					if (strcmp(param_type, "float") == 0) {
						sprintf(temp, "%s:%s:%%f, ", param_type, param_name);
					} else {
						printf("ERROR Param Type not recognised building User Function code example\n\n");
						exit(-1);
					}
				}
			}
			strcat(output_string, temp);
		}

		params.GetPairAtLocation(param_name, param_type, 0);
		if ( (strcmp(param_type, "uint16_t") == 0) || (strcmp(param_type, "int16_t") == 0) ) {
			sprintf(temp, "%s:%s:%%d)\\n\\n\\r\", ", param_type, param_name);
		} else {
			if (strcmp(param_type, "char*") == 0) {
				sprintf(temp, "%s:%s:%%s)\\n\\n\\r\", ", param_type, param_name);
			} else {
				if (strcmp(param_type, "float") == 0) {
					sprintf(temp, "%s:%s:%%f)\\n\\n\\r\", ", param_type, param_name);
				} else {
					printf("ERROR Param Type not recognised building User Function code example\n\n");
					exit(-1);
				}
			}
		}
		strcat(output_string, temp);

		for (int i = param_count - 1; i > 0; i--) {
			params.GetPairAtLocation(param_name, param_type, i);
			sprintf(temp, "%s, ", param_name);
			strcat(output_string, temp);
		}
		params.GetPairAtLocation(param_name, param_type, 0);
		sprintf(temp, "%s);\n", param_name);
		strcat(output_string, temp);
		user_code_output_queue.EnQueue(output_string);
	}

	// rest of the user_code function body and closures
	sprintf(output_string, "\t// >>>\n");
	user_code_output_queue.EnQueue(output_string);
	sprintf(output_string, "\t// >>> Results Callback via ITCHriteLine\n");
	user_code_output_queue.EnQueue(output_string);
	sprintf(output_string, "\t// >>>\n");
	user_code_output_queue.EnQueue(output_string);
	sprintf(output_string, "\tITCHWriteLine(temp);\n");
	user_code_output_queue.EnQueue(output_string);
	sprintf(output_string, "}\n\n");
	user_code_output_queue.EnQueue(output_string);

	header_output_queue.SetOutputAvailable();
	user_code_output_queue.SetOutputAvailable();
	code_output_queue.SetOutputAvailable();

	return E_NO_ERROR;
}

int AST::DetermineUnique(void) {
	return 0;
}
