#include "out.h"

#include "itch.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern ITCH itch;

void ShowBlocks(void) {
	RegShowBlocks(itch.WriteLine);
}

void ShowSystem(void) {
	RegShowSystem(itch.WriteLine);
}

void ShowBlockByLabel(char* BLOCK_LABEL) {
	RegShowBlockByLabel(BLOCK_LABEL, itch.WriteLine);
}

void ShowBlockByID(int16_t param1_int) {
	RegShowBlockByID(param1_int, itch.WriteLine);
}

void SendCommandToBlockLabel(char* BLOCK_LABEL, uint16_t COMMAND_STRING) {
	RegSendCommandToBlockLabel(BLOCK_LABEL, COMMAND_STRING, itch.WriteLine);
}

void SendCommandToBlockID(int16_t param1_int, uint16_t COMMAND_STRING) {
	RegSendCommandToBlockID(param1_int, COMMAND_STRING, itch.WriteLine);
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

void ConfigReset(void) {
	RegConfigReset(itch.WriteLine);
}

void ConfigLoad(void) {
	RegConfigLoad(itch.WriteLine);
}

void ConfigLoadBinary(void) {
	RegConfigLoadBinary(itch.WriteLine);
}

void ConfigLoadINI(void) {
	RegConfigLoadINI(itch.WriteLine);
}

void ConfigSave(void) {
	RegConfigSave(itch.WriteLine);
}

void ConfigSaveBinary(void) {
	RegConfigSaveBinary(itch.WriteLine);
}

void ConfigBlockSystem(char* param1_string, char* param2_string) {
	RegConfigBlockSystem(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSystem(char* param1_string, char* param2_string) {
	RegConfigBlockSystem(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSystem(char* param1_string, char* param2_string) {
	RegConfigBlockSystem(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSystem(char* param1_string, char* param2_string) {
	RegConfigBlockSystem(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSystem(char* param1_string, char* param2_string) {
	RegConfigBlockSystem(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSystem(char* param1_string, char* param2_string) {
	RegConfigBlockSystem(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockInput(char* param1_string, char* param2_string) {
	RegConfigBlockInput(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockInput(char* param1_string, char* param2_string) {
	RegConfigBlockInput(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockInput(char* param1_string, char* param2_string) {
	RegConfigBlockInput(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockInput(char* param1_string, char* param2_string) {
	RegConfigBlockInput(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockInput(char* param1_string, char* param2_string) {
	RegConfigBlockInput(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockInput(char* param1_string, char* param2_string) {
	RegConfigBlockInput(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockInput(char* param1_string, char* param2_string) {
	RegConfigBlockInput(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockMonitor(char* param1_string, char* param2_string) {
	RegConfigBlockMonitor(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockMonitor(char* param1_string, char* param2_string) {
	RegConfigBlockMonitor(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockMonitor(char* param1_string, char* param2_string) {
	RegConfigBlockMonitor(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockMonitor(char* param1_string, char* param2_string) {
	RegConfigBlockMonitor(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockMonitor(char* param1_string, char* param2_string) {
	RegConfigBlockMonitor(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockMonitor(char* param1_string, char* param2_string) {
	RegConfigBlockMonitor(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockMonitor(char* param1_string, char* param2_string) {
	RegConfigBlockMonitor(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockMonitor(char* param1_string, char* param2_string) {
	RegConfigBlockMonitor(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockMonitor(char* param1_string, char* param2_string) {
	RegConfigBlockMonitor(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSchedule(char* param1_string, char* param2_string) {
	RegConfigBlockSchedule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSchedule(char* param1_string, char* param2_string) {
	RegConfigBlockSchedule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSchedule(char* param1_string, char* param2_string) {
	RegConfigBlockSchedule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSchedule(char* param1_string, char* param2_string) {
	RegConfigBlockSchedule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSchedule(char* param1_string, char* param2_string) {
	RegConfigBlockSchedule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSchedule(char* param1_string, char* param2_string) {
	RegConfigBlockSchedule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSchedule(char* param1_string, char* param2_string) {
	RegConfigBlockSchedule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockSchedule(char* param1_string, char* param2_string) {
	RegConfigBlockSchedule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockRule(char* param1_string, char* param2_string) {
	RegConfigBlockRule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockRule(char* param1_string, char* param2_string) {
	RegConfigBlockRule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockRule(char* param1_string, char* param2_string) {
	RegConfigBlockRule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockRule(char* param1_string, char* param2_string) {
	RegConfigBlockRule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockRule(char* param1_string, char* param2_string) {
	RegConfigBlockRule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockRule(char* param1_string, char* param2_string) {
	RegConfigBlockRule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockRule(char* param1_string, char* param2_string) {
	RegConfigBlockRule(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockController(char* param1_string, char* param2_string) {
	RegConfigBlockController(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockController(char* param1_string, char* param2_string) {
	RegConfigBlockController(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockController(char* param1_string, char* param2_string) {
	RegConfigBlockController(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockController(char* param1_string, char* param2_string) {
	RegConfigBlockController(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockController(char* param1_string, char* param2_string) {
	RegConfigBlockController(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockController(char* param1_string, char* param2_string) {
	RegConfigBlockController(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockController(char* param1_string, char* param2_string) {
	RegConfigBlockController(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockOutput(char* param1_string, char* param2_string) {
	RegConfigBlockOutput(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockOutput(char* param1_string, char* param2_string) {
	RegConfigBlockOutput(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockOutput(char* param1_string, char* param2_string) {
	RegConfigBlockOutput(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockOutput(char* param1_string, char* param2_string) {
	RegConfigBlockOutput(param1_string, param2_string, itch.WriteLine);
}

void ConfigBlockOutput(char* param1_string, char* param2_string) {
	RegConfigBlockOutput(param1_string, param2_string, itch.WriteLine);
}

void SystemReboot(void) {
	RegSystemReboot(itch.WriteLine);
}

void BlockIDCmdOn(int16_t param1_int) {
	RegBlockIDCmdOn(param1_int, itch.WriteLine);
}

void BlockIDCmdOff(int16_t param1_int) {
	RegBlockIDCmdOff(param1_int, itch.WriteLine);
}

uint8_t LookupBlockLabel(char* lookup_string) {
	// Modify to suit. Default: call through to a function with USER_CODE_EXTERNAL_CALL
	// prefixed to to the same name as this lookup function.

	if (RegLookupBlockLabel(lookup_string) != 0) return 1;
	return 0;
}

