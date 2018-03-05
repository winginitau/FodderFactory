#include "out.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern void ITCHWriteLine(char *str);

void ShowBlocks(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ShowBlocks(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ShowSystem(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ShowSystem(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ShowBlockByCategory(uint16_t block_category) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ShowBlockByCategory(...) with param list (uint16_t:block_category:%d)\n\n\r", block_category);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ShowBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ShowBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\n\r", block_category, param1_int);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ShowBlockByLabel(char* block_label) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ShowBlockByLabel(...) with param list (char*:block_label:%s)\n\n\r", block_label);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ShowBlockByID(int16_t param1_int) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ShowBlockByID(...) with param list (int16_t:param1_int:%d)\n\n\r", param1_int);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void MessagesMute(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "MessagesMute(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void MessagesUnmute(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "MessagesUnmute(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void MonitorStart(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "MonitorStart(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void MonitorStop(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "MonitorStop(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void MonitorRemoveByLabel(char* block_label) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "MonitorRemoveByLabel(...) with param list (char*:block_label:%s)\n\n\r", block_label);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void MonitorRemoveByBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "MonitorRemoveByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\n\r", block_category, param1_int);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void MonitorByLabel(char* block_label) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "MonitorByLabel(...) with param list (char*:block_label:%s)\n\n\r", block_label);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void MonitorByBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "MonitorByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\n\r", block_category, param1_int);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ClearBlockByLabel(char* block_label) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ClearBlockByLabel(...) with param list (char*:block_label:%s)\n\n\r", block_label);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ClearBlockByBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ClearBlockByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\n\r", block_category, param1_int);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void SendCommandToBlockCatN(uint16_t block_category, int16_t param1_int, uint16_t command) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "SendCommandToBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d, uint16_t:command:%d)\n\n\r", block_category, param1_int, command);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void SendCommandToBlockLabel(char* block_label, uint16_t command) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "SendCommandToBlockLabel(...) with param list (char*:block_label:%s, uint16_t:command:%d)\n\n\r", block_label, command);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void SimIntDataMessageFromBCatN(uint16_t block_category, int16_t param1_int, int16_t param2_int) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "SimIntDataMessageFromBCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d, int16_t:param2_int:%d)\n\n\r", block_category, param1_int, param2_int);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void SimFloatDataMessageFromBCatN(uint16_t block_category, int16_t param1_int, float param2_float) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "SimFloatDataMessageFromBCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d, float:param2_float:%f)\n\n\r", block_category, param1_int, param2_float);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void SimIntDataMessageFromBlockLabel(char* block_label, int16_t param2_int) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "SimIntDataMessageFromBlockLabel(...) with param list (char*:block_label:%s, int16_t:param2_int:%d)\n\n\r", block_label, param2_int);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void SimFloatDataMessageFromBlockLabel(char* block_label, float param2_float) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "SimFloatDataMessageFromBlockLabel(...) with param list (char*:block_label:%s, float:param2_float:%f)\n\n\r", block_label, param2_float);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void DebugOn(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "DebugOn(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void DebugOff(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "DebugOff(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void DebugTermOn(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "DebugTermOn(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void DebugTermOff(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "DebugTermOff(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void DebugSetLevel(int16_t param1_int) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "DebugSetLevel(...) with param list (int16_t:param1_int:%d)\n\n\r", param1_int);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ShowTime(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ShowTime(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void SetTime(char* param1_time) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "SetTime(...) with param list (char*:param1_time:%s)\n\n\r", param1_time);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ShowDate(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ShowDate(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void SetDate(char* param1_date) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "SetDate(...) with param list (char*:param1_date:%s)\n\n\r", param1_date);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ConfClearBlocks(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ConfClearBlocks(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ConfClearAll(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ConfClearAll(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ConfReadDefaultFile(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ConfReadDefaultFile(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void ConfReadFilename(char* param1_string) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "ConfReadFilename(...) with param list (char*:param1_string:%s)\n\n\r", param1_string);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void WriteToDefaultConfigFile(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "WriteToDefaultConfigFile(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void WriteEmptyDefaultConfigFile(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "WriteEmptyDefaultConfigFile(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void WriteToFilenameConfigFile(char* param1_string) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "WriteToFilenameConfigFile(...) with param list (char*:param1_string:%s)\n\n\r", param1_string);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void CopyConfigFileToFile(char* param1_string, char* param2_string) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "CopyConfigFileToFile(...) with param list (char*:param1_string:%s, char*:param2_string:%s)\n\n\r", param1_string, param2_string);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void BlockDisableByBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "BlockDisableByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\n\r", block_category, param1_int);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void BlockDisableByLabel(char* param1_string) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "BlockDisableByLabel(...) with param list (char*:param1_string:%s)\n\n\r", param1_string);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void BlockEnableByBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "BlockEnableByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\n\r", block_category, param1_int);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void BlockEnableByLabel(char* block_label) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "BlockEnableByLabel(...) with param list (char*:block_label:%s)\n\n\r", block_label);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void BlockDeleteByBlockCatN(uint16_t block_category, int16_t param1_int) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "BlockDeleteByBlockCatN(...) with param list (uint16_t:block_category:%d, int16_t:param1_int:%d)\n\n\r", block_category, param1_int);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void BlockDeleteByLabel(char* param1_string) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "BlockDeleteByLabel(...) with param list (char*:param1_string:%s)\n\n\r", param1_string);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void BlockRename(char* param1_string, char* param2_string) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "BlockRename(...) with param list (char*:param1_string:%s, char*:param2_string:%s)\n\n\r", param1_string, param2_string);
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void TerminalExit(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "TerminalExit(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void TerminalLogout(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "TerminalLogout(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

void SystemReboot(void) {
	// >>>
	// >>> INSERT CODE HERE TO CARRY OUT THE DESIRED ACTION
	// >>>
	char temp[MAX_BUFFER_LENGTH];
	sprintf(temp, "SystemReboot(...) with param list (void)\n\n\r");
	// >>>
	// >>> AND SEND THE RESULTS OUT VIA CALLS TO ICLIWriteLine
	// >>>
	ITCHWriteLine(temp);
}

