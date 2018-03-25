
uint16_t LookupIdentMap (char* key) {
	uint16_t count;
	uint16_t idx = 0;
	count = sizeof(ident_map) / sizeof(XLATMap);
	while (idx < count) {
		if(strcasecmp(ident_map[idx].label, key) == 0) {
			return ident_map[idx].xlat_id;
		}
		idx++;
	}
	return -1;
}

uint16_t LookupLookupMap (char* key) {
	uint16_t count;
	uint16_t idx = 0;
	count = sizeof(lookup_map) / sizeof(XLATMap);
	while (idx < count) {
		if(strcasecmp(lookup_map[idx].label, key) == 0) {
			return lookup_map[idx].xlat_id;
		}
		idx++;
	}
	return -1;
}

uint16_t LookupFuncMap (char* key) {
	uint16_t count;
	uint16_t idx = 0;
	count = sizeof(func_map) / sizeof(XLATMap);
	while (idx < count) {
		if(strcasecmp(func_map[idx].label, key) == 0) {
			return func_map[idx].xlat_id;
		}
		idx++;
	}
	return -1;
}

uint16_t LookupIdentifierMembers(uint16_t ident_xlat, char* lookup_string) {
	uint16_t count;
	uint16_t idx = 0;
	switch(ident_xlat) {
		case 0: {
			count = sizeof(block_cat_defs) / sizeof(SimpleStringArray);
			while (idx < count) {
				if(strcasecmp(block_cat_defs[idx].text, lookup_string) == 0) {
					return idx;
				}
				idx++;
			}
		}
			break;
		case 1: {
			count = sizeof(command_strings) / sizeof(SimpleStringArray);
			while (idx < count) {
				if(strcasecmp(command_strings[idx].text, lookup_string) == 0) {
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

//*********************************************************************************************************



uint16_t LookupIdentMap (char* key) {
	XLATMap temp;
	//uint16_t count;
	uint16_t idx = 0;
	//count = sizeof(ident_map) / sizeof(XLATMap);
	while (idx < 2) {
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
	//uint16_t count;
	uint16_t idx = 0;
	//count = sizeof(ident_map) / sizeof(XLATMap);
	while (idx < 1) {
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
	//uint16_t count;
	uint16_t idx = 0;
	//count = sizeof(ident_map) / sizeof(XLATMap);
	while (idx < 49) {
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
	//uint16_t count;
	uint16_t idx = 0;
	char temp_str[MAX_IDENTIFIER_LABEL_SIZE];
	switch(ident_xlat) {
		case 0: {
			//count = sizeof(block_cat_defs) / sizeof(SimpleStringArray);
			//while (idx < count) {

			while (idx < LAST_BLOCK_CAT) {
				#ifdef ARDUINO
					strcpy_P(temp_str, block_cat_defs[idx].text);

					if(strcasecmp(lookup_string, temp_str) == 0) {
						return idx;
					}
				#else
					if(strcasecmp(block_cat_defs[idx].text, lookup_string) == 0) {
						return idx;
					}
				#endif
				idx++;
			}
		}
			break;
		case 1: {
			//count = sizeof(command_strings) / sizeof(SimpleStringArray);
			while (idx < LAST_COMMAND) {
				#ifdef ARDUINO
					if(strcasecmp_P(lookup_string, command_strings[idx].text) == 0) {
						return idx;
					}
				#else
					if(strcasecmp(command_strings[idx].text, lookup_string) == 0) {
						return idx;
					}
				#endif
				idx++;
			}
		}
			break;
		default:
		return 0;
	}
	return 0;
}

