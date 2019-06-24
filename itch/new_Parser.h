/*****************************************************************
 Parser.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#ifndef PARSER_H_
#define PARSER_H_

#include "config.h"
#include <new_NodeMap.h>
//#include "OutputBuffer.h"
#include <new_TokenList.h>


/******************************************************************************
 * Forward declarations for globals defined elsewhere
 ******************************************************************************/

#ifdef ITCH_DEBUG
extern void M(char * strn);
#endif

extern char g_itch_replay_buff[MAX_INPUT_LINE_SIZE];

//extern OutputBuffer g_itch_output_buff;		// General output buffer
//extern char g_out_str[MAX_OUTPUT_LINE_SIZE];	// Strings being assembled for output
//extern char g_temp_str[MAX_OUTPUT_LINE_SIZE];	// Generl string temp
//extern ASTA g_temp_asta;						// Temp asta node (and for the Progmem working copy)

/******************************************************************************
 * Structs and typdefs
 ******************************************************************************/

#define PF_ERRORONLINE 	0b00000001
#define PF_DELIMREACHED 0b00000010
#define PF_STRINGLIT	0b00000100
#define PF_EOLPROCESSED	0b00001000
#define PF_ESCAPE		0b00010000
#define PF_ESCLB		0b00100000
#define PF_HELPACTIVE	0b01000000

typedef struct PARSER_FLAGS {
	//uint8_t error_on_line;
	//uint8_t delim_reached;
	//uint8_t string_litteral;
	//uint8_t eol_processed;
	//uint8_t escape;
	//uint8_t help_active;

	uint8_t parse_result;
	uint8_t match_result;
	uint8_t last_error;

	//uint8_t reenter;
} P_FLAGS;

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

	void ParserInit();
	uint8_t ParseProcess(char ch);
	uint8_t ParserMatch(void);
	uint8_t ParserMatchEvaluate(void);
	void ParserSaveTokenAsParameter(void);

	void ResetPossibleList(void);
	void ParserWriteLineErrorString(void);
	void ParserSetError(uint16_t err);
	void ParserResetLine(void);
	void ParserBufferInject(char* inject_str);
	uint8_t ParserActionDispatcher(uint8_t asta_id);

	void P_ESCAPE(char ch);
	uint8_t P_EOL();
	uint8_t P_DOUBLE_QUOTE();
	uint8_t P_SPACE_TAB();
	void P_ADD_TO_PARSE_BUFFER(char ch);

#endif /* PARSER_H_ */
