#include "out.h"

#include "itch.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern ITCH itch;

void ShowBlocks(void) {
	RegShowBlocks(WriteLineCallback);
}

void ShowSystem(void) {
	RegShowSystem(WriteLineCallback);
}

void ShowBlockByLabel(char* BLOCK_LABEL) {
	RegShowBlockByLabel(BLOCK_LABEL, WriteLineCallback);
}

void ShowBlockByID(int16_t param1_int) {
	RegShowBlockByID(param1_int, WriteLineCallback);
}

void SendCommandToBlockLabel(char* BLOCK_LABEL, uint16_t COMMAND_STRING) {
	RegSendCommandToBlockLabel(BLOCK_LABEL, COMMAND_STRING, WriteLineCallback);
}

void SendCommandToBlockID(int16_t param1_int, uint16_t COMMAND_STRING) {
	RegSendCommandToBlockID(param1_int, COMMAND_STRING, WriteLineCallback);
}

void ShowTime(void) {
	RegShowTime(WriteLineCallback);
}

void SetTime(char* param1_time) {
	RegSetTime(param1_time, WriteLineCallback);
}

void ShowDate(void) {
	RegShowDate(WriteLineCallback);
}

void SetDate(char* param1_date) {
	RegSetDate(param1_date, WriteLineCallback);
}

void ConfigReset(void) {
	RegConfigReset(WriteLineCallback);
}

void ConfigLoad(void) {
	RegConfigLoad(WriteLineCallback);
}

void ConfigLoadBinary(void) {
	RegConfigLoadBinary(WriteLineCallback);
}

void ConfigLoadINI(void) {
	RegConfigLoadINI(WriteLineCallback);
}

void ConfigSave(void) {
	RegConfigSave(WriteLineCallback);
}

void ConfigSaveBinary(void) {
	RegConfigSaveBinary(WriteLineCallback);
}

void ConfigBlockSystem(char* param1_string, uint16_t SYS_CONFIG, char* param2_string) {
	RegConfigBlockSystem(param1_string, SYS_CONFIG, param2_string, WriteLineCallback);
}

void ConfigBlockInput(char* param1_string, uint16_t IN_CONFIG, char* param2_string) {
	RegConfigBlockInput(param1_string, IN_CONFIG, param2_string, WriteLineCallback);
}

void ConfigBlockMonitor(char* param1_string, uint16_t MON_CONFIG, char* param2_string) {
	RegConfigBlockMonitor(param1_string, MON_CONFIG, param2_string, WriteLineCallback);
}

void ConfigBlockSchedule(char* param1_string, uint16_t SCH_CONFIG, char* param2_string) {
	RegConfigBlockSchedule(param1_string, SCH_CONFIG, param2_string, WriteLineCallback);
}

void ConfigBlockRule(char* param1_string, uint16_t RL_CONFIG, char* param2_string) {
	RegConfigBlockRule(param1_string, RL_CONFIG, param2_string, WriteLineCallback);
}

void ConfigBlockController(char* param1_string, uint16_t CON_CONFIG, char* param2_string) {
	RegConfigBlockController(param1_string, CON_CONFIG, param2_string, WriteLineCallback);
}

void ConfigBlockOutput(char* param1_string, uint16_t OUT_CONFIG, char* param2_string) {
	RegConfigBlockOutput(param1_string, OUT_CONFIG, param2_string, WriteLineCallback);
}

void InitSetupAll(void) {
	RegInitSetupAll(WriteLineCallback);
}

void InitValidateAll(void) {
	RegInitValidateAll(WriteLineCallback);
}

void InitDisableAll(void) {
	RegInitDisableAll(WriteLineCallback);
}

void SystemReboot(void) {
	RegSystemReboot(WriteLineCallback);
}

void BlockIDCmdOn(int16_t param1_int) {
	RegBlockIDCmdOn(param1_int, WriteLineCallback);
}

void BlockIDCmdOff(int16_t param1_int) {
	RegBlockIDCmdOff(param1_int, WriteLineCallback);
}

uint8_t LookupBlockLabel(char* lookup_string) {
	// Modify to suit. Default: call through to a function with USER_CODE_EXTERNAL_CALL
	// prefixed to to the same name as this lookup function.

	if (RegLookupBlockLabel(lookup_string) != 0) return 1;
	return 0;
}

