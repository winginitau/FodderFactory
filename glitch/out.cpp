//
// DO NOT MODIFY THIS FILE
//
// THIS FILE IS AUTOMATICALLY GENERATED IN ITS ENTIRETY
// ANY CHANGES MADE IN THIS FILE WILL BE OVERWRITTEN WITHOUT WARNING
// WHENEVER THE GLITCH IS INVOKED.
//

#include "itch_strings.h"

#include "out.h"

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
			SendCommandToBlockLabel(params[0].param_char_star, params[1].param_uint16_t);
			break;
		case 5:
			SendCommandToBlockID(params[0].param_int16_t, params[1].param_uint16_t);
			break;
		case 6:
			ShowTime();
			break;
		case 7:
			SetTime(params[0].param_char_star);
			break;
		case 8:
			ShowDate();
			break;
		case 9:
			SetDate(params[0].param_char_star);
			break;
		case 10:
			SystemReboot();
			break;
		case 11:
			BlockIDOn(params[0].param_int16_t);
			break;
		case 12:
			BlockIDOff(params[0].param_int16_t);
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
		#ifdef ARDUINO
			memcpy_P(&temp, &ident_map[idx], sizeof(XLATMap));
		#else
			memcpy(&temp, &ident_map[idx], sizeof(XLATMap));
		#endif
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
		#ifdef ARDUINO
			memcpy_P(&temp, &lookup_map[idx], sizeof(XLATMap));
		#else
			memcpy(&temp, &lookup_map[idx], sizeof(XLATMap));
		#endif
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
		#ifdef ARDUINO
			memcpy_P(&temp, &func_map[idx], sizeof(XLATMap));
		#else
			memcpy(&temp, &func_map[idx], sizeof(XLATMap));
		#endif
		if(strcasecmp(temp.label, key) == 0) {
			return temp.xlat_id;
		}
		idx++;
	}
	return -1;
}

uint16_t LookupIdentifierMembers(uint16_t ident_xlat, char* lookup_string) {
	SimpleStringArray temp;
	uint16_t idx = 0;
	switch(ident_xlat) {
		case 0: {
			while (idx < LAST_BLOCK_CAT) {
				#ifdef ARDUINO
					memcpy_P(&temp, &block_cat_names[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &block_cat_names[idx], sizeof(SimpleStringArray));
				#endif
				if(strcasecmp(lookup_string, temp.text) == 0) {
					return idx;
				}
				idx++;
			}
		}
			break;
		case 1: {
			while (idx < LAST_COMMAND) {
				#ifdef ARDUINO
					memcpy_P(&temp, &command_strings[idx], sizeof(SimpleStringArray));
				#else
					memcpy(&temp, &command_strings[idx], sizeof(SimpleStringArray));
				#endif
				if(strcasecmp(lookup_string, temp.text) == 0) {
					return idx;
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

uint8_t LookupLookupMembers(uint16_t ident_xlat, char* lookup_string) {
	switch(ident_xlat) {
		case 0:
			return LookupBlockLabel(lookup_string);
			break;
		default:
		return 0;
	}
	return 0;
}

