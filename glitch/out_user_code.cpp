#include "out.h"

#include "itch.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern ITCH itch;

void ShowBlocks(void) {
	RegShowBlocks(itch.WriteLineCallback);
}

void ShowSystem(void) {
	RegShowSystem(itch.WriteLineCallback);
}

void ShowBlockByLabel(char* BLOCK_LABEL) {
	RegShowBlockByLabel(BLOCK_LABEL, itch.WriteLineCallback);
}

void ShowBlockByID(int16_t param1_int) {
	RegShowBlockByID(param1_int, itch.WriteLineCallback);
}

void SendCommandToBlockLabel(char* BLOCK_LABEL, uint16_t COMMAND_STRING) {
	RegSendCommandToBlockLabel(BLOCK_LABEL, COMMAND_STRING, itch.WriteLineCallback);
}

void SendCommandToBlockID(int16_t param1_int, uint16_t COMMAND_STRING) {
	RegSendCommandToBlockID(param1_int, COMMAND_STRING, itch.WriteLineCallback);
}

void ShowTime(void) {
	RegShowTime(itch.WriteLineCallback);
}

void SetTime(char* param1_time) {
	RegSetTime(param1_time, itch.WriteLineCallback);
}

void ShowDate(void) {
	RegShowDate(itch.WriteLineCallback);
}

void SetDate(char* param1_date) {
	RegSetDate(param1_date, itch.WriteLineCallback);
}

void ConfigReset(void) {
	RegConfigReset(itch.WriteLineCallback);
}

void ConfigLoad(void) {
	RegConfigLoad(itch.WriteLineCallback);
}

void ConfigLoadBinary(void) {
	RegConfigLoadBinary(itch.WriteLineCallback);
}

void ConfigLoadINI(void) {
	RegConfigLoadINI(itch.WriteLineCallback);
}

void ConfigSave(void) {
	RegConfigSave(itch.WriteLineCallback);
}

void ConfigSaveBinary(void) {
	RegConfigSaveBinary(itch.WriteLineCallback);
}

void ConfigBlockSystem(char* param1_string, uint16_t SYS_CONFIG, char* param2_string) {
	RegConfigBlockSystem(param1_string, SYS_CONFIG, param2_string, itch.WriteLineCallback);
}

void ConfigBlockInput(char* param1_string, uint16_t IN_CONFIG, char* param2_string) {
	RegConfigBlockInput(param1_string, IN_CONFIG, param2_string, itch.WriteLineCallback);
}

void ConfigBlockMonitor(char* param1_string, uint16_t MON_CONFIG, char* param2_string) {
	RegConfigBlockMonitor(param1_string, MON_CONFIG, param2_string, itch.WriteLineCallback);
}

void ConfigBlockSchedule(char* param1_string, uint16_t SCH_CONFIG, char* param2_string) {
	RegConfigBlockSchedule(param1_string, SCH_CONFIG, param2_string, itch.WriteLineCallback);
}

void ConfigBlockRule(char* param1_string, uint16_t RL_CONFIG, char* param2_string) {
	RegConfigBlockRule(param1_string, RL_CONFIG, param2_string, itch.WriteLineCallback);
}

void ConfigBlockController(char* param1_string, uint16_t CON_CONFIG, char* param2_string) {
	RegConfigBlockController(param1_string, CON_CONFIG, param2_string, itch.WriteLineCallback);
}

void ConfigBlockOutput(char* param1_string, uint16_t OUT_CONFIG, char* param2_string) {
	RegConfigBlockOutput(param1_string, OUT_CONFIG, param2_string, itch.WriteLineCallback);
}

void InitSetupAll(void) {
	RegInitSetupAll(itch.WriteLineCallback);
}

void InitValidateAll(void) {
	RegInitValidateAll(itch.WriteLineCallback);
}

void InitDisableAll(void) {
	RegInitDisableAll(itch.WriteLineCallback);
}

void SystemReboot(void) {
	RegSystemReboot(itch.WriteLineCallback);
}

void BlockIDCmdOn(int16_t param1_int) {
	RegBlockIDCmdOn(param1_int, itch.WriteLineCallback);
}

void BlockIDCmdOff(int16_t param1_int) {
	RegBlockIDCmdOff(param1_int, itch.WriteLineCallback);
}

uint8_t LookupBlockLabel(char* lookup_string) {
	// Modify to suit. Default: call through to a function with USER_CODE_EXTERNAL_CALL
	// prefixed to to the same name as this lookup function.

	if (RegLookupBlockLabel(lookup_string) != 0) return 1;
	return 0;
}

