#include "out.h"

#include "itch.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <ff_registry.h>
extern ITCH itch;

void ShowBlocks(void) {
	RegShowBlocks(itch.WriteLine);
}

void ShowSystem(void) {
	RegShowSystem(itch.WriteLine);
}

void ShowBlockByLabel(char* block_label) {
	RegShowBlockByLabel(block_label, itch.WriteLine);
}

void ShowBlockByID(int16_t param1_int) {
	RegShowBlockByID(param1_int, itch.WriteLine);
}

void SendCommandToBlockLabel(char* block_label, uint16_t command) {
	RegSendCommandToBlockLabel(block_label, command, itch.WriteLine);
}

void SendCommandToBlockID(int16_t param1_int, uint16_t command) {
	RegSendCommandToBlockID(param1_int, command, itch.WriteLine);
}

void ShowTime(void) {
	RegShowTime(itch.WriteLine);
}

void SetTime(char* param1_time) {
	RegSetTime(param1_time, itch.WriteLine);
}

void ShowDate(void) {
	RegShowDate(itch.WriteLine);
}

void SetDate(char* param1_date) {
	RegSetDate(param1_date, itch.WriteLine);
}

void SystemReboot(void) {
	RegSystemReboot(itch.WriteLine);
}

uint8_t LookupBlockLabel(char* lookup_string) {
	// Modify to suit. Default: call through to a function with USER_CODE_EXTERNAL_CALL
	// prefixed to to the same name as this lookup function.

	if (RegLookupBlockLabel(lookup_string) != 0) return 1;
	return 0;
}

uint8_t LookupOtherList(char* lookup_string) {
	// Modify to suit. Default: call through to a function with USER_CODE_EXTERNAL_CALL
	// prefixed to to the same name as this lookup function.

	//if (RegLookupOtherList(lookup_string) != 0) return 1;
	return 0;
}

