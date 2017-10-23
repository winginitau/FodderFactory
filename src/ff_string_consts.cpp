/*
 * ff_string_consts.cpp
 *
 *  Created on: 22 Oct. 2017
 *      Author: brendan
 */

#include "ff_sys_config.h"
#include "ff_string_consts.h"

#ifdef FF_SIMULATOR
#include <string.h>
#include <stdint.h>
#endif




uint8_t SimpleStringArrayIndex(const char* array[], const char* key) {
	//
	for (int i = 0; i < sizeof(array); i++) {
		if (strcmp(key, array[i]) == 0) {
			return i;
		}
	}
	return 255;
}

/*
uint8_t SimpleStringArrayIndex(const SimpleStringArray array[], const char* key) {
	//
	for (int i = 0; i < sizeof(array); i++) {
		if (strcmp(key, array[i].text) == 0) {
			return i;
		}
	}
	return 255;
}
*/
