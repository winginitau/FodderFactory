/*
 * ff_parser.cpp
 *
 *  Created on: 5 Feb. 2018
 *      Author: brendan
 */


#include "config.h"
#include "LineBuffer.h"
#include "StringList.h"
#include "Lexer.h"

//XXX: Placeholder
void SendToOutput(char* out) {
	// place holder for output directing
	printf("%s", out);
}


class Grammar {
private:
public:
	Grammar();
	virtual ~Grammar();
	int ReadDefinition(const char * filename);
};

int Grammar::ReadDefinition(const char * filename) {
		// open the def file and read it in
		// Populating a grammar dictionary structure
	return 0;
};





/****************************************************************************
   Main Calling Routine
****************************************************************************/
int main(void) {

	//handle arguments
	// - g <grammar_file> grammar file in
	// - o <header_file_out>

	//open grammar file
	FILE *gf;
	gf = fopen("config_grammar.txt", "r");

	int line_count=0;
	LineBuffer line;
	Lexer lex;
	char ch;

	char output_line[MAX_LINE_LENGTH];

	int result;

    ch=fgetc(gf);
    while (ch != EOF) {
    	line.AddChar(ch);

    	if (line.Available()) {
    		line_count++;
    		line.Tokenise();
    		if (line.IsEmptyLine()) {
    			// could be line spacing in a code fragment so print it
    			line.GetRawBuffer(output_line);
    			strcat(output_line, "\n");
    			SendToOutput(output_line);
    		} else {
    			result = lex.ProcessLine(line);
    			switch (result) {
    				case R_ERROR:
    	    			sprintf(output_line, ">> Error on Line: %d\n>> ", line_count);
    	    			SendToOutput(output_line);
    					lex.GetErrorString(output_line);
    					strcat(output_line, "\n>> " );
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
    		}
    		line.Reset();
    	}
    	ch=fgetc(gf);
    }

    printf("Lines Processed: %d", line_count);

	return 0;
}


