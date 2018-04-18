#include "out.h"
#include "ff_registry.h"

#include "itch.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern ITCH itch;

void ShowBlocks(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ShowBlocks(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "ShowBlocks(...) with param list (void)\n\n\r");
	#endif

	RegShowBlocks(itch.WriteLnImmediate);

	itch.WriteLine(temp);
}

void ShowSystem(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ShowSystem(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "ShowSystem(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void ShowBlockByCategory(uint16_t block_category) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ShowBlockByCategory(...) with param list (uint16_t:block_category:%d)\n\r"), block_category);
	#else
		sprintf(temp, "ShowBlockByCategory(...) with param list (uint16_t:block_category:%d)\n\r", block_category);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void ShowBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ShowBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r"), block_category, param1_int);
	#else
		sprintf(temp, "ShowBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r", block_category, param1_int);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void ShowBlockByLabel(char* block_label) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ShowBlockByLabel(...) with param list (char*:block_label:%s)\n\r"), block_label);
	#else
		sprintf(temp, "ShowBlockByLabel(...) with param list (char*:block_label:%s)\n\r", block_label);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void ShowBlockByID(int16_t param1_int) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ShowBlockByID(...) with param list (int16_t:param1_int:%d)\n\r"), param1_int);
	#else
		sprintf(temp, "ShowBlockByID(...) with param list (int16_t:param1_int:%d)\n\r", param1_int);
	#endif

	RegShowBlockByID(param1_int, itch.WriteLnImmediate);

	itch.WriteLine(temp);
}

void MessagesMute(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("MessagesMute(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "MessagesMute(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void MessagesUnmute(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("MessagesUnmute(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "MessagesUnmute(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void MonitorStart(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("MonitorStart(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "MonitorStart(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void MonitorStop(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("MonitorStop(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "MonitorStop(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void MonitorRemoveByLabel(char* block_label) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("MonitorRemoveByLabel(...) with param list (char*:block_label:%s)\n\r"), block_label);
	#else
		sprintf(temp, "MonitorRemoveByLabel(...) with param list (char*:block_label:%s)\n\r", block_label);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void MonitorRemoveByBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("MonitorRemoveByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r"), block_category, param1_int);
	#else
		sprintf(temp, "MonitorRemoveByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r", block_category, param1_int);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void MonitorByLabel(char* block_label) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("MonitorByLabel(...) with param list (char*:block_label:%s)\n\r"), block_label);
	#else
		sprintf(temp, "MonitorByLabel(...) with param list (char*:block_label:%s)\n\r", block_label);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void MonitorByBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("MonitorByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r"), block_category, param1_int);
	#else
		sprintf(temp, "MonitorByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r", block_category, param1_int);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void ClearBlockByLabel(char* block_label) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ClearBlockByLabel(...) with param list (char*:block_label:%s)\n\r"), block_label);
	#else
		sprintf(temp, "ClearBlockByLabel(...) with param list (char*:block_label:%s)\n\r", block_label);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void ClearBlockByBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ClearBlockByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r"), block_category, param1_int);
	#else
		sprintf(temp, "ClearBlockByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r", block_category, param1_int);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void SendCommandToBlockCatN(uint16_t block_category, int16_t param1_int, uint16_t command) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("SendCommandToBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d, uint16_t:command:%d)\n\r"), block_category, param1_int, command);
	#else
		sprintf(temp, "SendCommandToBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d, uint16_t:command:%d)\n\r", block_category, param1_int, command);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void SendCommandToBlockLabel(char* block_label, uint16_t command) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("SendCommandToBlockLabel(...) with param list (char*:block_label:%s, uint16_t:command:%d)\n\r"), block_label, command);
	#else
		sprintf(temp, "SendCommandToBlockLabel(...) with param list (char*:block_label:%s, uint16_t:command:%d)\n\r", block_label, command);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void SimIntDataMessageFromBCatN(uint16_t block_category, int16_t param1_int, int16_t param2_int) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("SimIntDataMessageFromBCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d, int16_t:param2_int:%d)\n\r"), block_category, param1_int, param2_int);
	#else
		sprintf(temp, "SimIntDataMessageFromBCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d, int16_t:param2_int:%d)\n\r", block_category, param1_int, param2_int);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void SimFloatDataMessageFromBCatN(uint16_t block_category, int16_t param1_int, float param2_float) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("SimFloatDataMessageFromBCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d, float:param2_float:%f)\n\r"), block_category, param1_int, param2_float);
	#else
		sprintf(temp, "SimFloatDataMessageFromBCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d, float:param2_float:%f)\n\r", block_category, param1_int, param2_float);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void SimIntDataMessageFromBlockLabel(char* block_label, int16_t param2_int) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("SimIntDataMessageFromBlockLabel(...) with param list (char*:block_label:%s, int16_t:param2_int:%d)\n\r"), block_label, param2_int);
	#else
		sprintf(temp, "SimIntDataMessageFromBlockLabel(...) with param list (char*:block_label:%s, int16_t:param2_int:%d)\n\r", block_label, param2_int);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void SimFloatDataMessageFromBlockLabel(char* block_label, float param2_float) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("SimFloatDataMessageFromBlockLabel(...) with param list (char*:block_label:%s, float:param2_float:%f)\n\r"), block_label, param2_float);
	#else
		sprintf(temp, "SimFloatDataMessageFromBlockLabel(...) with param list (char*:block_label:%s, float:param2_float:%f)\n\r", block_label, param2_float);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void DebugOn(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("DebugOn(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "DebugOn(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void DebugOff(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("DebugOff(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "DebugOff(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void DebugTermOn(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("DebugTermOn(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "DebugTermOn(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void DebugTermOff(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("DebugTermOff(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "DebugTermOff(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void DebugSetLevel(int16_t param1_int) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("DebugSetLevel(...) with param list (int16_t:param1_int:%d)\n\r"), param1_int);
	#else
		sprintf(temp, "DebugSetLevel(...) with param list (int16_t:param1_int:%d)\n\r", param1_int);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void ShowTime(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ShowTime(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "ShowTime(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void SetTime(char* param1_time) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("SetTime(...) with param list (char*:param1_time:%s)\n\r"), param1_time);
	#else
		sprintf(temp, "SetTime(...) with param list (char*:param1_time:%s)\n\r", param1_time);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void ShowDate(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ShowDate(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "ShowDate(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void SetDate(char* param1_date) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("SetDate(...) with param list (char*:param1_date:%s)\n\r"), param1_date);
	#else
		sprintf(temp, "SetDate(...) with param list (char*:param1_date:%s)\n\r", param1_date);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void ConfClearBlocks(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ConfClearBlocks(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "ConfClearBlocks(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void ConfClearAll(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ConfClearAll(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "ConfClearAll(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void ConfReadDefaultFile(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ConfReadDefaultFile(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "ConfReadDefaultFile(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void ConfReadFilename(char* param1_string) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("ConfReadFilename(...) with param list (char*:param1_string:%s)\n\r"), param1_string);
	#else
		sprintf(temp, "ConfReadFilename(...) with param list (char*:param1_string:%s)\n\r", param1_string);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void WriteToDefaultConfigFile(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("WriteToDefaultConfigFile(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "WriteToDefaultConfigFile(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void WriteEmptyDefaultConfigFile(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("WriteEmptyDefaultConfigFile(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "WriteEmptyDefaultConfigFile(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void WriteToFilenameConfigFile(char* param1_string) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("WriteToFilenameConfigFile(...) with param list (char*:param1_string:%s)\n\r"), param1_string);
	#else
		sprintf(temp, "WriteToFilenameConfigFile(...) with param list (char*:param1_string:%s)\n\r", param1_string);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void CopyConfigFileToFile(char* param1_string, char* param2_string) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("CopyConfigFileToFile(...) with param list (char*:param1_string:%s, char*:param2_string:%s)\n\r"), param1_string, param2_string);
	#else
		sprintf(temp, "CopyConfigFileToFile(...) with param list (char*:param1_string:%s, char*:param2_string:%s)\n\r", param1_string, param2_string);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void BlockDisableByBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("BlockDisableByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r"), block_category, param1_int);
	#else
		sprintf(temp, "BlockDisableByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r", block_category, param1_int);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void BlockDisableByLabel(char* param1_string) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("BlockDisableByLabel(...) with param list (char*:param1_string:%s)\n\r"), param1_string);
	#else
		sprintf(temp, "BlockDisableByLabel(...) with param list (char*:param1_string:%s)\n\r", param1_string);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void BlockEnableByBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("BlockEnableByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r"), block_category, param1_int);
	#else
		sprintf(temp, "BlockEnableByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r", block_category, param1_int);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void BlockEnableByLabel(char* block_label) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("BlockEnableByLabel(...) with param list (char*:block_label:%s)\n\r"), block_label);
	#else
		sprintf(temp, "BlockEnableByLabel(...) with param list (char*:block_label:%s)\n\r", block_label);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void BlockDeleteByBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("BlockDeleteByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r"), block_category, param1_int);
	#else
		sprintf(temp, "BlockDeleteByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\r", block_category, param1_int);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void BlockDeleteByLabel(char* param1_string) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("BlockDeleteByLabel(...) with param list (char*:param1_string:%s)\n\r"), param1_string);
	#else
		sprintf(temp, "BlockDeleteByLabel(...) with param list (char*:param1_string:%s)\n\r", param1_string);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void BlockRename(char* param1_string, char* param2_string) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("BlockRename(...) with param list (char*:param1_string:%s, char*:param2_string:%s)\n\r"), param1_string, param2_string);
	#else
		sprintf(temp, "BlockRename(...) with param list (char*:param1_string:%s, char*:param2_string:%s)\n\r", param1_string, param2_string);
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void TerminalExit(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("TerminalExit(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "TerminalExit(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void TerminalLogout(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("TerminalLogout(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "TerminalLogout(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

void SystemReboot(void) {
	// >>>
	// >>> INSERT ACTION CODE HERE
	// >>>
	char temp[MAX_OUTPUT_LINE_SIZE];
	#ifdef ARDUINO
		sprintf_P(temp, PSTR("SystemReboot(...) with param list (void)\n\n\r"));
	#else
		sprintf(temp, "SystemReboot(...) with param list (void)\n\n\r");
	#endif
	// >>>
	// >>> Results Callback via ITCHriteLine
	// >>>
	itch.WriteLine(temp);
}

