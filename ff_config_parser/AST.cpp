/*****************************************************************
 AST.cpp

 Copyright (C) 2018 Brendan McLearie 

 Created on: 11 Feb. 2018

 ******************************************************************/

#include "AST.h"

AST::AST() {
	root = NULL;
	current = NULL;
	current_level = 0;
	next_id = 1;

    output_string[0] = '\0';

	grammar_def_count = 0;
}

AST::~AST() {
	// TODO Auto-generated destructor stub
}

int AST::AddSiblingToCurrent(ASTNode * node) {
	ASTNode* walker;
	if(root == NULL) {		//first node should always be a sibling of root
		return E_SIBLING_TO_ROOT_ATTEMPTED;
	}

	if(current->next_sibling == NULL) {
		current->next_sibling = node;
	} else {
		walker = current->next_sibling;
		while(walker->next_sibling != NULL) {
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

int AST::AddNode(int term_level, const char* term_type) {
	return AddNode(term_level, term_type, NULL);
}

int AST::AddNode(int term_level, const char* term_type, const char* term) {
	ASTNode* n;
	n = new(ASTNode);
	n->id = next_id;
	next_id++;
	if(term != NULL) strcpy(n->label, term);
	n->term_level = term_level;
	int i;
	for (i = LAST_AST_TYPE; i != 0; i--) {
		if (strcmp(ast_type_strings[i].text, term_type) == 0) break;
	}
	if (i == 0) {
		return E_TERM_TYPE_NOT_RECOGNISED;
	} else {
		n->type = i;
	}
	if (term_level > current_level + 1) {
		return E_TERM_LEVEL_NOT_INCRMENTAL;
	}
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

void AST::DumpTree() {

	grammar_def_count = 0;

	DT(root, false);  // to count the output lines

	sprintf(output_string, "#ifdef USE_PROGMEM\n");
	output.EnQueue(output_string);
	sprintf(output_string, "static const GrammarDef grammar_def [%d] PROGMEM = {\n", grammar_def_count);
	output.EnQueue(output_string);
	sprintf(output_string, "#else\n");
	output.EnQueue(output_string);
	sprintf(output_string, "static const GrammarDef grammar_def [%d] = {\n", grammar_def_count);
	output.EnQueue(output_string);
	sprintf(output_string, "#endif\n");
	output.EnQueue(output_string);

	DT(root, true);

	sprintf(output_string, "};\n");
	output.EnQueue(output_string);

	output.SetOutputAvailable();
}

void AST::DT(ASTNode* w, bool print) {
	char temp[MAX_LINE_LENGTH];

	if(w != NULL) {
		sprintf(output_string, "\t%u, \"%s\", \"%s\", \"%s\", %u, %u, %d, %d, ", w->id, w->label, w->unique, w->help, w->term_level, w->type, w->action, w->finish);
		if(w->parent != NULL) {
			sprintf(temp, "%u, ", w->parent->id);
		} else {
			sprintf(temp, "0, ");
		}
		strcat(output_string, temp);

		if(w->first_child != NULL) {
			sprintf(temp, "%u, ", w->first_child->id);
		} else {
			sprintf(temp, "0, ");
		}
		strcat(output_string, temp);

		if(w->next_sibling != NULL) {
			sprintf(temp, "%u, ", w->next_sibling->id);
		} else {
			sprintf(temp, "0, ");
		}
		strcat(output_string, temp);

		sprintf(temp, "\"%s\",\n", w->action_identifier);
		strcat(output_string, temp);

		grammar_def_count++;

		if(print) output.EnQueue(output_string);

		DT(w->first_child, print);
		DT(w->next_sibling, print);
	}
}


void AST::AttachActionToCurrent(char* action_identifier) {
	current->action = true;
	strcpy(current->action_identifier, action_identifier);
}
