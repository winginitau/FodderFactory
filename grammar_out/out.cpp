//
// DO NOT MODIFY THIS FILE
//
// THIS FILE IS AUTOMATICALLY GENERATED IN ITS ENTIRETY
// ANY CHANGES MADE IN THIS FILE WILL BE OVERWRITTEN WITHOUT WARNING
// WHENEVER GLITCH IS INVOKED.
//

#include <itch_config.h>
#include <itch_strings.h>

#include <out.h>

#if defined(PLATFORM_LINUX)
#elif defined(PLATFORM_ARDUINO)
#include <Arduino.h>
#else
#error "A PLATFORM_ Directive must be provided"
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint16_t CallFunction(uint8_t func_xlat, ParamUnion params[]) {
	switch (func_xlat) {
		case 0:
			ShowBlocks();
			break;
		case 1:
			ShowSystem();
			break;
		case 2:
			ShowBlockByLabel(params[0].param_char_star);
			break;
		case 3:
			ShowBlockByID(params[0].param_int16_t);
			break;
		case 4:
			ShowMemory();
			break;
		case 5:
			SetCommandOnBlockLabel(params[0].param_char_star, params[1].param_uint16_t);
			break;
		case 6:
			SetCommandOnBlockID(params[0].param_int16_t, params[1].param_uint16_t);
			break;
		case 7:
			ShowTime();
			break;
		case 8:
			SetTime(params[0].param_char_star);
			break;
		case 9:
			ShowDate();
			break;
		case 10:
			SetDate(params[0].param_char_star);
			break;
		case 11:
			ConfigClear();
			break;
		case 12:
			ConfigLoad();
			break;
		case 13:
			ConfigSave();
			break;
		case 14:
			ConfigShow();
			break;
		case 15:
			ConfigDeleteBID(params[0].param_int16_t);
			break;
		case 16:
			ConfigDeleteBlockLabel(params[0].param_char_star);
			break;
		case 17:
			ConfigInterface(params[0].param_char_star, params[1].param_uint16_t, params[2].param_char_star);
			break;
		case 18:
			ConfigBlockSystem(params[0].param_char_star, params[1].param_uint16_t, params[2].param_char_star);
			break;
		case 19:
			ConfigBlockInput(params[0].param_char_star, params[1].param_uint16_t, params[2].param_char_star);
			break;
		case 20:
			ConfigBlockMonitor(params[0].param_char_star, params[1].param_uint16_t, params[2].param_char_star);
			break;
		case 21:
			ConfigBlockSchedule(params[0].param_char_star, params[1].param_uint16_t, params[2].param_char_star);
			break;
		case 22:
			ConfigBlockRule(params[0].param_char_star, params[1].param_uint16_t, params[2].param_char_star);
			break;
		case 23:
			ConfigBlockController(params[0].param_char_star, params[1].param_uint16_t, params[2].param_char_star);
			break;
		case 24:
			ConfigBlockOutput(params[0].param_char_star, params[1].param_uint16_t, params[2].param_char_star);
			break;
		case 25:
			InitSetupAll();
			break;
		case 28:
			InitSetupBID(params[0].param_int16_t);
			break;
		case 26:
			InitValidateAll();
			break;
		case 29:
			InitValidateBID(params[0].param_int16_t);
			break;
		case 27:
			InitDisableAll();
			break;
		case 30:
			InitDisableBID(params[0].param_int16_t);
			break;
		case 31:
			AdminDisableBID(params[0].param_int16_t);
			break;
		case 32:
			AdminEnableBID(params[0].param_int16_t);
			break;
		case 33:
			AdminCmdOnBID(params[0].param_int16_t);
			break;
		case 34:
			AdminCmdOffBID(params[0].param_int16_t);
			break;
		case 35:
			SystemReboot();
			break;
		case 36:
			IFOneWireScanBID(params[0].param_int16_t);
			break;
		case 37:
			IFOneWireScanLabel(params[0].param_char_star);
			break;
		case 38:
			IFOneWireAssignBID(params[0].param_int16_t, params[1].param_int16_t);
			break;
		case 39:
			IFDS1820BRead(params[0].param_int16_t);
			break;
		case 40:
			IFDS1820BTest(params[0].param_int16_t);
			break;
		default:
			return PE_FUNC_XLAT_NOT_MATCHED_IN_CALLFUNCTION;
			break;
	}
	return PEME_NO_ERROR;
}

uint16_t LookupIdentMap (char* key) {
	XLATMap temp;
	uint16_t idx = 0;
	while (idx < XLAT_IDENT_MAP_COUNT) {
		#ifdef PLATFORM_ARDUINO
			memcpy_P(&temp, &ident_map[idx], sizeof(XLATMap));
		#else
			memcpy(&temp, &ident_map[idx], sizeof(XLATMap));
		#endif //PLATFORM_ARDUINO
		if(strcasecmp(temp.label, key) == 0) {
			return temp.xlat_id;
		}
		idx++;
	}
	return -1;
}

uint16_t LookupLookupMap (char* key) {
	XLATMap temp;
	uint16_t idx = 0;
	while (idx < XLAT_LOOKUP_MAP_COUNT) {
		#ifdef PLATFORM_ARDUINO
			memcpy_P(&temp, &lookup_map[idx], sizeof(XLATMap));
		#else
			memcpy(&temp, &lookup_map[idx], sizeof(XLATMap));
		#endif //PLATFORM_ARDUINO
		if(strcasecmp(temp.label, key) == 0) {
			return temp.xlat_id;
		}
		idx++;
	}
	return -1;
}

uint16_t LookupFuncMap (char* key) {
	XLATMap temp;
	uint16_t idx = 0;
	while (idx < XLAT_FUNC_MAP_COUNT) {
		#ifdef PLATFORM_ARDUINO
			memcpy_P(&temp, &func_map[idx], sizeof(XLATMap));
		#else
			memcpy(&temp, &func_map[idx], sizeof(XLATMap));
		#endif //PLATFORM_ARDUINO
		if(strcasecmp(temp.label, key) == 0) {
			return temp.xlat_id;
		}
		idx++;
	}
	return -1;
}

uint16_t LookupIdentifierMembers(uint16_t ident_xlat, char* lookup_string, uint8_t str_len) {
	SimpleStringArray temp;
	uint16_t idx = 0;
	switch(ident_xlat) {
		case 0: {
			while (idx < LAST_BLOCK_CAT) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &block_cat_strings[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &block_cat_strings[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 1: {
			while (idx < LAST_BLOCK_TYPE) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &block_type_strings[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &block_type_strings[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 2: {
			while (idx < LAST_COMMAND) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &command_strings[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &command_strings[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 3: {
			while (idx < LAST_UNIT) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &unit_strings[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &unit_strings[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 4: {
			while (idx < LAST_UNIT_ABBR) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &unit_abbr_strings[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &unit_abbr_strings[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 5: {
			while (idx < LAST_DAY) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &day_strings[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &day_strings[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 6: {
			while (idx < LAST_INTERFACE) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &interface_type_strings[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &interface_type_strings[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 7: {
			while (idx < LAST_STATUS) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &status_strings[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &status_strings[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 8: {
			while (idx < LAST_IF_CONFIG) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &if_config_keys[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &if_config_keys[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 9: {
			while (idx < LAST_SYS_CONFIG) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &sys_config_keys[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &sys_config_keys[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 10: {
			while (idx < LAST_IN_CONFIG) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &in_config_keys[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &in_config_keys[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 11: {
			while (idx < LAST_MON_CONFIG) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &mon_config_keys[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &mon_config_keys[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 12: {
			while (idx < LAST_SCH_CONFIG) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &sch_config_keys[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &sch_config_keys[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 13: {
			while (idx < LAST_RL_CONFIG) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &rl_config_keys[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &rl_config_keys[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 14: {
			while (idx < LAST_CON_CONFIG) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &con_config_keys[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &con_config_keys[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		case 15: {
			while (idx < LAST_OUT_CONFIG) {
				#ifdef PLATFORM_ARDUINO
					memcpy_P(&temp, &out_config_keys[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &out_config_keys[idx], sizeof(SimpleStringArray));
				#endif //PLATFORM_ARDUINO
				if(strnlen(temp.text, MAX_AST_IDENTIFIER_SIZE) == str_len) {
					if(strncasecmp(lookup_string, temp.text, str_len) == 0) {
						return idx;
					}
				}
				idx++;
			}
		}
			break;
		default:
		return 0;
	}
	return 0;
}

uint8_t LookupLookupMembers(uint16_t ident_xlat, char* lookup_string, uint8_t str_len) {
	char str_terminated[MAX_AST_IDENTIFIER_SIZE];
	strncpy(str_terminated, lookup_string, MAX_AST_IDENTIFIER_SIZE);
	str_terminated[((str_len < MAX_AST_IDENTIFIER_SIZE) ? str_len : MAX_AST_IDENTIFIER_SIZE)] = '\0';
	switch(ident_xlat) {
		case 0:
			return LookupBlockLabel(str_terminated);
			break;
		default:
		return 0;
	}
	return 0;
}
