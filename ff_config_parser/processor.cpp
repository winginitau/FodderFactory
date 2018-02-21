/*
 * ff_parser.cpp
 *
 *  Created on: 5 Feb. 2018
 *      Author: brendan
 */


#include "common_config.h"
#include "processor_errors.h"

#ifdef DEBUG
#include "Debug.h"
#endif

#include "LineBuffer.h"
#include "StringList.h"
#include "Lexer.h"
#include <stdlib.h>

FILE* hf;

//XXX: Placeholder
void SendToOutput(char* out) {
	// place holder for output directing
	fprintf(hf, "%s", out);
	printf("%s", out);

}


void ProcessGrammarFile(Lexer lex, FILE* gf) {

	int line_count = 0;
	LineBuffer line;
	//Lexer lex;
	char ch;

	char output_line[MAX_BUFFER_LENGTH];

	int result = R_NONE;

	ch = fgetc(gf);
	while (ch != EOF) {
		line.AddChar(ch);

		if (line.Available()) {
			line_count++;
			line.Tokenise();
			result = lex.ProcessLine(line);
			switch (result) {
				case R_ERROR:
					sprintf(output_line, ">> Error on Line: %d\n>> ", line_count);
					SendToOutput(output_line);
					lex.GetErrorString(output_line);
					strcat(output_line, "\n>> ");
					SendToOutput(output_line);
					line.GetRawBuffer(output_line);
					strcat(output_line, "\n");
					SendToOutput(output_line);
					//TODO: send to stdout and errorout
					//TODO: Lookup error code
					exit(-1);
					break;
				case R_NONE:
					sprintf(output_line, ">> Error on Line: %d\n>> ", line_count);
					SendToOutput(output_line);
					sprintf(output_line, "Lexer did not return a result at all. Error in Lexer?\n>> ");
					SendToOutput(output_line);
					line.GetRawBuffer(output_line);
					strcat(output_line, "\n");
					SendToOutput(output_line);
					//TODO: send to stdout and errorout
					//TODO: Lookup error code
					exit(-1);
					break; //continue
				case R_UNFINISHED:
					while (lex.OutputAvailable()) {
						lex.GetOutputAsString(output_line);
						SendToOutput(output_line);
					}
					break; //continue
				case R_COMPLETE: {
					while (lex.OutputAvailable()) {
						lex.GetOutputAsString(output_line);
						SendToOutput(output_line);
					}
					lex.Init();
					break;
				}
			}
			//}
			line.Reset();
		}
		ch = fgetc(gf);
	}

	printf("// Lines Processed: %d\n\n", line_count);
}


/****************************************************************************
   Main Calling Routine
****************************************************************************/
int main(void) {
	//handle arguments
	// - g <grammar_file> grammar file in
	// - o <header_file_out>

	//open grammar file
	FILE* gf;
	gf = fopen("config_grammar.txt", "r");


	hf = fopen("processor_out.h", "w");

	Lexer lex;

	ProcessGrammarFile(lex, gf);

	fclose(gf);
	fclose(hf);

	//lex.ast.DumpTree();

	//ICLI();

	return 0;
}


