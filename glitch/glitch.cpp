/*
 * ff_parser.cpp
 *
 *  Created on: 5 Feb. 2018
 *      Author: brendan
 */


#include "glitch_config.h"

#ifdef DEBUG
#include "Debug.h"
#endif

//#define PROC_DEBUG		// dubug output in the processor executable

#include "LineBuffer.h"
#include "StringList.h"
#include "Lexer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>



#define GRAMMAR_FILE_DEFAULT "grammar.gf"
#define OUTPUT_FILE_BASE_DEFAULT "out"


/*************************************
 * GLOBALS
************************************/

FILE* hf;	// output header file - global, so can be seen within other modules.
FILE* cf;	// output code file - global, so can be seen within other modules.
FILE* uf;	// user code file - templates written if not exist
FILE* gf;	// grammar file - doesn't need to be global but put here with the other two anyway

char gf_name[MAX_BUFFER_LENGTH];
char hf_name[MAX_BUFFER_LENGTH];
char cf_name[MAX_BUFFER_LENGTH];
char uf_name[MAX_BUFFER_LENGTH];

int verbose = 0;
bool overwrite_user_file = false;
Lexer lex;

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
		case Q_USER_CODE:
			if (overwrite_user_file) {
				fprintf(uf, "%s", out);
				#ifdef PROC_DEBUG
				printf("DEBUG USER_CODE \t%s", out);
				#endif
			} else {
				#ifdef PROC_DEBUG
				printf("DEBUG USER_CODE (not written) \t%s", out);
				#endif
			}
			break;
		default:
			printf("FATAL Error, Nonexistent output destination referenced in Processor::SendToOutput()\n");
			exit(-1);
	}
}

void ProcessGrammarFile() {
	// Main processing loop - reads grammar file char by char
	// When whole lines are available, it tokenises
	// 	and calls the lexer to process the line.
	// Handles return from the lexer and directs output

	int line_count = 0;		// For reporting count of processed and location of errors
	LineBuffer line;		// A custom class that allows char by char build up,
							//	tokenisation, indexable tokens, and preservation of original,
							//	smart handling of \r \n, EOL flagging etc
							// 	Strings enclosed in double quotes ("...") are treated as a single token

	char ch;				// used to read the grammar file char by char

	char output_line[MAX_BUFFER_LENGTH];	// generic buffer for any string going "out"

	int result = R_NONE;	// line-to-line processing state machine

	ch = fgetc(gf);			// get the first char from the grammar file
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
				case R_IGNORE:		// fall through to unfinished - used for comments
				case R_UNFINISHED:
					while (lex.User_OutputAvailable()) {
						lex.GetOutputAsString(Q_USER, output_line);
						SendToOutput(Q_USER, output_line);
					}
					break; //continue
				case R_COMPLETE: {
					while (lex.User_OutputAvailable()) {
						lex.GetOutputAsString(Q_USER, output_line);
						SendToOutput(Q_USER, output_line);
					}
					while (lex.Header_OutputAvailable()) {
						lex.GetOutputAsString(Q_HEADER, output_line);
						SendToOutput(Q_HEADER, output_line);
					}
					while (lex.Code_OutputAvailable()) {
						lex.GetOutputAsString(Q_CODE, output_line);
						SendToOutput(Q_CODE, output_line);
					}
					while (lex.User_Code_OutputAvailable()) {
						lex.GetOutputAsString(Q_USER_CODE, output_line);
						if (overwrite_user_file) {
							SendToOutput(Q_USER_CODE, output_line);
						}
					}
					// Only if R_COMPLETE is the lexer reset
					// 	which clears sectional flags
					lex.Init();
					break;
				}
			}
			line.Reset();
		}
		ch = fgetc(gf);
	}
	printf("// Lines Processed: %d\n\n", line_count);
}

int ArgsAndFiles(int argc, char* argv[]) {
	bool use_cpp = true;
	char* clo_result = NULL;

	strcpy(gf_name, GRAMMAR_FILE_DEFAULT);
	strcpy(hf_name, OUTPUT_FILE_BASE_DEFAULT);
	strcpy(cf_name, OUTPUT_FILE_BASE_DEFAULT);
	strcpy(uf_name, OUTPUT_FILE_BASE_DEFAULT);
	strcat(uf_name, "_user_code");

	// Handle command line arguments
	// -g <grammar_file>
	// -o <header_and C/CPP files_out>
	// -c Produce ANSI C file (otherwise its CPP)
	// -f Force overwriting of the user_code file

	if (argc != 1) { // some arguments

		// Iterate over all options
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-c") == 0) {
				use_cpp = false;
			}
			if (strcmp(argv[i], "-o") == 0) {
				// check that the next option is not another option specifier
				// Option i + 1 should be a filename
				if (argv[(i + 1)][0] != '-') {
					// copy the filename to the header filename variable
					clo_result = (strcpy(hf_name, argv[(i + 1)]));
					// if the first copy worked, set up the other filenames
					if (clo_result != NULL) {
						clo_result = strcpy(cf_name, hf_name);
						clo_result = strcpy(uf_name, hf_name);
						// add the "_user_code" bit for the uf
						strcat(uf_name, "_user_code");
					}
				}
				if (clo_result == NULL) {
					printf(">> Command error - got -o but valid outputs filename base did not follow.\n");
					return 1;
				}
			}
			if (strcmp(argv[i], "-g") == 0) {
				if (argv[(i + 1)][0] != '-') {
					clo_result = (strcpy(gf_name, argv[(i + 1)]));
				}
				if (clo_result == NULL) {
					printf(">> Command error - got -g but valid grammar filename did not follow.\n");
					return 1;
				}
			}
			if (strcmp(argv[i], "-v") == 0) {
				verbose = 1;
			}
			if (strcmp(argv[i], "-vv") == 0) {
				verbose = 2;
			}
			if (strcmp(argv[i], "-d") == 0) {
				verbose = 2;
			}
			if (strcmp(argv[i], "-f") == 0) {
				overwrite_user_file = true;
			}
		}
	}

	if (verbose) {
		printf("%s, Version %s\n", PROG_NAME, VERSION);
		if (verbose == 1) {
			printf("Verbose set to ON\n");
		}
		if (verbose == 2) {
			printf("Verbose set to DEBUG\n");
		}
	}

	// add filename extensions
	strcat(hf_name, ".h");
	if (use_cpp) {
		strcat(cf_name, ".cpp");
		strcat(uf_name, ".cpp");
	} else {
		strcat(cf_name, ".c");
		strcat(uf_name, ".c");
	}
	if (verbose) {
		printf("Grammar file: \t%s\n", gf_name);
		printf("Header file: \t%s\n", hf_name);
		printf("Code file: \t%s\n", cf_name);
		printf("User Code file: %s\n", uf_name);
	}

	//open grammar file
	gf = fopen(gf_name, "r");
	if (gf == 0) {
		printf(">> Error opening grammar file: %s\n", gf_name);
		return 1;
	}

	// open output header file
	hf = fopen(hf_name, "w");
	if (hf == 0) {
		printf(">> Error opening header file for writing: %s\n", hf_name);
		return 1;
	}

	// open output code file
	cf = fopen(cf_name, "w");
	if (hf == 0) {
		printf(">> Error opening code file for writing: %s\n", cf_name);
		return 1;
	}

	// open the user code file
	if (overwrite_user_file) {
		printf("-f option given - overwriting user_code file\n");
		uf = fopen(uf_name, "w");
		if (uf == 0) {
			printf(">> Error opening user code file for writing: %s\n", uf_name);
			return 1;
		}

	} else {
		uf = fopen(uf_name, "r");
		if (uf) {
			char ans;
			printf("\nWARNING: %s already exists and may contain user generated code!\n", uf_name);
			printf("Do you want to overwrite user file %s ? (All changes will be lost)\n\n", uf_name);
			printf("Confirm: (Y/y)es (N/n)o ? ");
			ans = getchar();
			if (toupper(ans) == 'Y') {
				fclose(uf);
				uf = fopen(uf_name, "w");
				overwrite_user_file = true;
				if (uf == 0) {
					printf(">> Error opening user code file for writing: %s\n", uf_name);
					return 1;
				}
			} else { // no
				fclose(uf);
			}
		} else { // uf did not exist
			uf = fopen(uf_name, "w");
			overwrite_user_file = true;
			if (uf == 0) {
				printf(">> Error opening user code file for writing: %s\n", uf_name);
				return 1;
			}
		}
	}
	return 0;
}

/****************************************************************************
   Main Calling Routine
****************************************************************************/
int main(int argc, char* argv[]) {
	char out[MAX_BUFFER_LENGTH];
	char include_guard[MAX_BUFFER_LENGTH];
	char* temp;

	// Process command line arguments, open files etc
	if(ArgsAndFiles(argc, argv)) {
		printf(">> Can't Continue. Exit.\n");
		exit(-1);
	}

	// Write output file header preamble and filename dependent pre-proc directives
	for (int i = Q_HEADER; i <= Q_CODE; i++) {
		sprintf(out, "//\n");
		SendToOutput(i, out);
		sprintf(out, "// DO NOT MODIFY THIS FILE\n");
		SendToOutput(i, out);
		sprintf(out, "//\n");
		SendToOutput(i, out);
		sprintf(out, "// THIS FILE IS AUTOMATICALLY GENERATED IN ITS ENTIRETY\n");
		SendToOutput(i, out);
		sprintf(out, "// ANY CHANGES MADE IN THIS FILE WILL BE OVERWRITTEN WITHOUT WARNING\n");
		SendToOutput(i, out);
		sprintf(out, "// WHENEVER THE GLITCH IS INVOKED.\n");
		SendToOutput(i, out);
		sprintf(out, "//\n\n");
		SendToOutput(i, out);
	}

	// Write include guards to the output header file
	temp = strrchr(hf_name, '.');
	int dot_idx = (int)(temp - hf_name);
	memcpy(include_guard, hf_name, dot_idx);
	include_guard[dot_idx] = '\0';
	temp = include_guard;
	while (*temp) {
		*temp = toupper(*temp);
		temp++;
	}

	strcat(include_guard, "_H_");

	sprintf(out, "#ifndef %s\n", include_guard);
	SendToOutput(Q_HEADER, out);
	sprintf(out, "#define %s\n\n", include_guard);
	SendToOutput(Q_HEADER, out);

	// Preprocessor defines placeholder
	sprintf(out, "DEFINES_PLACEHOLDER\n");
	SendToOutput(Q_HEADER, out);

	// Write header includes into the code and user_code files
	sprintf(out, "#include \"itch_strings.h\"\n\n");
	SendToOutput(Q_CODE, out);
	sprintf(out, "#include \"%s\"\n\n", hf_name);
	SendToOutput(Q_CODE, out);
	SendToOutput(Q_USER_CODE, out);

	// itch include for the user_code so it can find the call back
	sprintf(out, "#include \"itch.h\"\n");
	SendToOutput(Q_USER_CODE, out);

	// includes for the code and user_code files
	sprintf(out, "#include <stdint.h>\n");
	SendToOutput(Q_CODE, out);
	SendToOutput(Q_USER_CODE, out);
	sprintf(out, "#include <stdio.h>\n");
	SendToOutput(Q_CODE, out);
	SendToOutput(Q_USER_CODE, out);
	sprintf(out, "#include <stdlib.h>\n\n");
	SendToOutput(Q_CODE, out);
	SendToOutput(Q_USER_CODE, out);

	// Callback function forward declaration
	//sprintf(out, "extern void ITCHWriteLine(char *str);\n\n");
	sprintf(out, "extern ITCH itch;\n\n");
	SendToOutput(Q_USER_CODE, out);


	// Do all the hard word - Process the Grammar File
	//	writing the main contents of the header, code and user code files
	ProcessGrammarFile();

	// Write include guard ends to header file
	sprintf(out, "\n#endif // %s\n\n", include_guard);
	SendToOutput(Q_HEADER, out);

	// Close the header file, reopen for reading. Make a temporary copy,
	// line by line substituting the now finalised values for various
	// preprocessor #defines.
	fclose(hf);

	FILE* tf;
	char temp_buf[MAX_BUFFER_LENGTH];

	tf = fopen("hf_temp", "w");
	hf = fopen(hf_name, "r");

	while (fgets(temp_buf, MAX_BUFFER_LENGTH, hf) != NULL) {
		if (strcmp(temp_buf, "DEFINES_PLACEHOLDER\n") == 0) {
			sprintf(out, "#define MAX_ENUM_STRING_ARRAY_STRING_SIZE %d\n", (lex.max_enum_string_array_string_size + 1));
			fputs(out, tf);
			sprintf(out, "#define MAX_IDENTIFIER_LABEL_SIZE %d\n", (lex.max_identifier_label_size + 1));
			fputs(out, tf);
			sprintf(out, "#define MAX_AST_LABEL_SIZE %d\n", (lex.max_ast_label_size + 1));
			fputs(out, tf);
			sprintf(out, "#define MAX_AST_ACTION_SIZE %d\n", (lex.max_ast_action_size + 1));
			fputs(out, tf);
			sprintf(out, "#define AST_NODE_COUNT %d\n", lex.ast.ast_node_count);
			fputs(out, tf);
			sprintf(out, "#define MAX_PARAM_COUNT %d\n\n", lex.ast.max_param_count);
			fputs(out, tf);
			sprintf(out, "#define XLAT_IDENT_MAP_COUNT %d\n", (lex.idents.DEFINE_ident_map_count));
			fputs(out, tf);
			sprintf(out, "#define XLAT_LOOKUP_MAP_COUNT %d\n", (lex.idents.DEFINE_lookup_map_count));
			fputs(out, tf);
			sprintf(out, "#define XLAT_FUNC_MAP_COUNT %d\n", (lex.idents.DEFINE_func_map_count));
			fputs(out, tf);

		} else {
			fputs(temp_buf, tf);
		}
	}
	fclose(tf);
	fclose(hf);

	// Copy the now complete temp file back
	tf = fopen("hf_temp", "r");
	hf = fopen(hf_name, "w");

	while (fgets(temp_buf, MAX_BUFFER_LENGTH, tf) != NULL) {
		fputs(temp_buf, hf);
	}

	// Close the temp file and header file
	fclose(tf);
	fclose(hf);

	// Close all the other files
	fclose(gf);
	fclose(cf);
	fclose(uf);


	return 0;
}


