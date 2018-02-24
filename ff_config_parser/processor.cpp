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

#define PROC_DEBUG		// dubug output in the processor executable

#include "LineBuffer.h"
#include "StringList.h"
#include "Lexer.h"
#include <stdlib.h>


#define GRAMMAR_FILE_DEFAULT "grammar.txt"
#define OUTPUT_FILE_BASE_DEFAULT "out"


FILE* hf;	// output header file - global, so can be seen within other modules.
FILE* cf;	// output code file - global, so can be seen within other modules.
FILE* gf;	// grammar file - doesn't need to be global but put here with the other two anyway


void SendToOutput(int out_q, char* out) {
	// output directing
	switch (out_q) {
		case Q_USER:
			printf("%s", out);
			break;
		case Q_HEADER:
			fprintf(hf, "%s", out);
			#ifdef PROC_DEBUG
			printf("DEBUG HEADER \t%s", out);
			#endif
			break;
		case Q_CODE:
			fprintf(cf, "%s", out);
			#ifdef PROC_DEBUG
			printf("DEBUG CODE \t%s", out);
			#endif
			break;
		default:
			printf("FATAL Error, Nonexistent output detination referenced in Processor::SendToOutput()\n");
			exit(-1);
	}
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
					SendToOutput(Q_USER, output_line);
					lex.GetErrorString(output_line);
					strcat(output_line, "\n>> ");
					SendToOutput(Q_USER, output_line);
					line.GetRawBuffer(output_line);
					strcat(output_line, "\n");
					SendToOutput(Q_USER, output_line);
					//TODO: send to stdout and errorout
					//TODO: Lookup error code
					exit(-1);
					break;
				case R_NONE:
					sprintf(output_line, ">> Error on Line: %d\n>> ", line_count);
					SendToOutput(Q_USER, output_line);
					sprintf(output_line, "Lexer did not return a result at all. Error in Lexer?\n>> ");
					SendToOutput(Q_USER, output_line);
					line.GetRawBuffer(output_line);
					strcat(output_line, "\n");
					SendToOutput(Q_USER, output_line);
					//TODO: send to stdout and errorout
					//TODO: Lookup error code
					exit(-1);
					break; //continue
				case R_UNFINISHED:
					while (lex.Header_OutputAvailable()) {
						lex.GetOutputAsString(Q_USER, output_line);
						SendToOutput(Q_USER, output_line);
					}
					break; //continue
				case R_COMPLETE: {
					while (lex.Header_OutputAvailable()) {
						lex.GetOutputAsString(Q_USER, output_line);
						SendToOutput(Q_USER, output_line);
					}
					while (lex.Header_OutputAvailable()) {
						lex.GetOutputAsString(Q_HEADER, output_line);
						SendToOutput(Q_HEADER, output_line);
					}
					while (lex.Header_OutputAvailable()) {
						lex.GetOutputAsString(Q_CODE, output_line);
						SendToOutput(Q_CODE, output_line);
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

int ArgsAndFiles(int argc, char* argv[]) {
	char gf_name[MAX_BUFFER_LENGTH];
	char hf_name[MAX_BUFFER_LENGTH];
	char cf_name[MAX_BUFFER_LENGTH];

	bool use_cpp = true;
	char* clo_error = NULL;

	strcpy(gf_name, GRAMMAR_FILE_DEFAULT);
	strcpy(hf_name, OUTPUT_FILE_BASE_DEFAULT);
	strcpy(cf_name, OUTPUT_FILE_BASE_DEFAULT);

	//handle command line arguments
	// - g <grammar_file>
	// - o <header_and C/CPP files_out>
	// - c Produce ANSI C file (otherwise its CPP)

	if (argc != 1) { // some arguments
		//for (int i = 0; i < argc; i++) {
		//	printf("argv[%d] \t%s\n", i, argv[i]);
		//}

		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-c") == 0) {
				use_cpp = false;
			}
			if (strcmp(argv[i], "-o") == 0) {
				if (argv[(i + 1)][0] != '-') {
					clo_error = (strcpy(hf_name, argv[(i + 1)]));
					if (clo_error != NULL) {
						clo_error = strcpy(cf_name, hf_name);
					}
				}
				if (clo_error == NULL) {
					printf(">> Command error - got -o but valid outputs filename base did not follow.\n");
					return 1;
				}
			}
			if (strcmp(argv[i], "-g") == 0) {
				if (argv[(i + 1)][0] != '-') {
					clo_error = (strcpy(gf_name, argv[(i + 1)]));
				}
				if (clo_error == NULL) {
					printf(">> Command error - got -g but valid grammar filename did not follow.\n");
					return 1;
				}
			}
		}
	}


	if (clo_error != NULL) {
		strcat(hf_name, ".h");

		if (use_cpp) {
			strcat(cf_name, ".cpp");
		} else {
			strcat(cf_name, ".c");
		}

		//open grammar file
		gf = fopen(gf_name, "r");
		if (gf == 0) {
			printf(">> Error opening grammar file.\n");
			return 1;
		}

		// open output header file
		hf = fopen(hf_name, "w");
		if (hf == 0) {
			printf(">> Error opening header file for writing.\n");
			return 1;
		}

		// open output code file
		cf = fopen(cf_name, "w");
		if (hf == 0) {
			printf(">> Error opening code file for writing.\n");
			return 1;
		}
	}
	return 0;
}

/****************************************************************************
   Main Calling Routine
****************************************************************************/
int main(int argc, char* argv[]) {

	if(ArgsAndFiles(argc, argv)) {
		printf(">> Can't Continue. Exit.\n");
		exit(-1);
	}

	Lexer lex;

	ProcessGrammarFile(lex, gf);

	fclose(gf);
	fclose(hf);
	fclose(cf);


	return 0;
}


