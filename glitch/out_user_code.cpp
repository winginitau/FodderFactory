#include "out.h"

#include "itch.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern ITCH itch;

void ShowBlocks(void) {
	//RegShowBlocks(itch.WriteLnImmediate);
}

void ShowSystem(void) {
	//RegShowSystem(itch.WriteLnImmediate);
}

void ShowBlockByLabel(char* block_label) {
	//RegShowBlockByLabel(block_label, itch.WriteLnImmediate);
}

void ShowBlockByID(int16_t param1_int) {
	//RegShowBlockByID(param1_int, itch.WriteLnImmediate);
}

void SendCommandToBlockLabel(char* block_label, uint16_t command) {
	//RegSendCommandToBlockLabel(block_label, command, itch.WriteLnImmediate);
}

void SendCommandToBlockID(int16_t param1_int, uint16_t command) {
	//RegSendCommandToBlockID(param1_int, command, itch.WriteLnImmediate);
}

void SimIntDataMessageFromBlockLabel(char* block_label, int16_t param2_int) {
	//RegSimIntDataMessageFromBlockLabel(block_label, param2_int, itch.WriteLnImmediate);
}

void SimFloatDataMessageFromBlockLabel(char* block_label, float param2_float) {
	//RegSimFloatDataMessageFromBlockLabel(block_label, param2_float, itch.WriteLnImmediate);
}

void SimIntDataMessageFromBlockID(int16_t param1_int, int16_t param2_int) {
	//RegSimIntDataMessageFromBlockID(param1_int, param2_int, itch.WriteLnImmediate);
}

void SimFloatDataMessageFromBlockID(int16_t param1_int, float param2_float) {
	//RegSimFloatDataMessageFromBlockID(param1_int, param2_float, itch.WriteLnImmediate);
}

void ShowTime(void) {
	//RegShowTime(itch.WriteLnImmediate);
}

void SetTime(char* param1_time) {
	//RegSetTime(param1_time, itch.WriteLnImmediate);
}

void ShowDate(void) {
	//RegShowDate(itch.WriteLnImmediate);
}

void SetDate(char* param1_date) {
	//RegSetDate(param1_date, itch.WriteLnImmediate);
}

void SystemReboot(void) {
	//RegSystemReboot(itch.WriteLnImmediate);
}

uint8_t LookupBlockLabel(char* lookup_string) {
	//
	// Insert lookup call here that returns 0 or 1 if lookup string found
	//
	return 0;
}

uint8_t LookupOtherList(char* lookup_string) {
	//
	// Insert lookup call here that returns 0 or 1 if lookup string found
	//
	return 0;
}

