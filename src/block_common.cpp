/*****************************************************************
 block_common.cpp

 Copyright (C) 2019 Brendan McLearie 

 Created on: 15 Jun 2019

******************************************************************/


/************************************************
  Includes
************************************************/
#include <block_common.h>
//#include <build_config.h>
//#include <debug_ff.h>
//#include <events.h>
#include <out.h>
#include <utils.h>
#include <registry.h>
#include <string_consts.h>

#ifdef PLATFORM_LINUX
#endif

/************************************************
  Data Structures
************************************************/


/************************************************
  Globals
************************************************/


/************************************************
  Functions
************************************************/

void CommonShow(BlockNode *b, void(Callback(const char *))) {
	char out_str[MAX_MESSAGE_STRING_LENGTH];
	char fmt_str[MAX_LABEL_LENGTH];
	char label_str[MAX_LABEL_LENGTH];

	strcpy_hal(out_str, F("Base Data"));
	Callback(out_str);

	strcpy_hal(fmt_str, F(" id:           %d"));
	sprintf(out_str, fmt_str, b->id);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" cat:          %s (%d)"));
	strcpy_hal(label_str, block_cat_strings[b->cat].text);
	sprintf(out_str, fmt_str, label_str, b->cat);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" type:         %s (%d)"));
	strcpy_hal(label_str, block_type_strings[b->type].text);
	sprintf(out_str, fmt_str, label_str, b->type);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" label:        %s"));
	sprintf(out_str, fmt_str, b->label);
	Callback(out_str);

#ifndef EXCLUDE_DISPLAYNAME
	strcpy_hal(fmt_str, F(" display_name: %s"));
	sprintf(out_str, fmt_str, b->display_name);
	Callback(out_str);
#endif
#ifndef EXCLUDE_DESCRIPTION
	strcpy_hal(fmt_str, F(" description:  %s"));
	sprintf(out_str, fmt_str, b->description);
	Callback(out_str);
#endif

	strcpy_hal(out_str, F("Operational Data"));
	Callback(out_str);

	strcpy_hal(fmt_str, F(" active:       %s (%d)"));
	if (b->active == 1) {
		strcpy_hal(label_str, F("ON"));
	} else {
		strcpy_hal(label_str, F("OFF"));
	}
	sprintf(out_str, fmt_str, label_str, b->active);
	Callback(out_str);

	strcpy_hal(fmt_str, F(" bool_val:     %s (%d)"));
	if (b->bool_val == 1) {
		strcpy_hal(label_str, F("True"));
	} else {
		strcpy_hal(label_str, F("False"));
	}
	sprintf(out_str, fmt_str, label_str, b->bool_val);
	Callback(out_str);

	if (b->int_val == INT32_INIT) {
		strcpy_hal(out_str, F(" int_val:      INT32_INIT"));
	} else {
		strcpy_hal(fmt_str, F(" int_val:      %ld"));
		sprintf(out_str, fmt_str, b->int_val);
	}
	Callback(out_str);

	if (b->f_val == FLOAT_INIT) {
		strcpy_hal(out_str, F(" f_val:        FLOAT_INIT"));
	} else {
		FFFloatToCString(label_str, b->f_val);
		strcpy_hal(fmt_str, F(" f_val:        %s"));
		sprintf(out_str, fmt_str, label_str);
	}
	Callback(out_str);

	strcpy_hal(fmt_str, F(" status:       %s (%d)"));
	strcpy_hal(label_str, status_strings[b->status].text);
	sprintf(out_str, fmt_str, label_str, b->status);
	Callback(out_str);
}
