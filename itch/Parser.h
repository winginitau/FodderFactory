/*****************************************************************
 Parser.h

 Copyright (C) 2018 Brendan McLearie 

 Created on: 17 Feb. 2018

 ******************************************************************/

#ifndef PARSER_H_
#define PARSER_H_

#include "config.h"
#include "NodeMap.h"
#include "OutputBuffer.h"
#include "TokenList.h"


/******************************************************************************
 * Forward declarations for globals defined elsewhere
 ******************************************************************************/

#ifdef ITCH_DEBUG
extern void M(char * strn);
#endif

extern char g_itch_replay_buff[MAX_INPUT_LINE_SIZE];

extern OutputBuffer g_itch_output_buff;		// General output buffer
//extern char g_out_str[MAX_OUTPUT_LINE_SIZE];	// Strings being assembled for output
//extern char g_temp_str[MAX_OUTPUT_LINE_SIZE];	// Generl string temp
//extern ASTA g_temp_asta;						// Temp asta node (and for the Progmem working copy)

/******************************************************************************
 * Structs and typdefs
 ******************************************************************************/

typedef struct PARSER_FLAGS {
	uint8_t error_on_line;
	uint8_t parse_result;
	uint8_t delim_reached;
	uint8_t string_litteral;
	uint8_t match_result;
	uint8_t last_error;
	uint8_t help_active;
	uint8_t escape;
	uint8_t eol_processed;
} P_FLAGS;

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

	void ParserInit();
	void ParserFinish();

	uint8_t Parse(char ch);

	uint8_t ParserMatch(void);
	uint8_t ParserMatchEvaluate(void);
	void ParserSaveTokenAsParameter(void);

	void ResetPossibleList(void);
	void ParserGetErrorString(char* error);
	void ParserSetError(uint16_t err);
	void ParserResetLine(void);
	void ParserBufferInject(char* inject_str);
	uint16_t ParserActionDispatcher(uint16_t asta_id);

	void P_ESCAPE(char ch);
	uint8_t P_EOL();
	uint8_t P_DOUBLE_QUOTE();
	uint8_t P_SPACE_TAB();
	void P_ADD_TO_BUFFER(char ch);


#endif /* PARSER_H_ */
