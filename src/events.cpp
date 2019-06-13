/************************************************
 ff_events.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Event Processing
************************************************/


/************************************************
 Includes
************************************************/
#include <build_config.h>
#include <debug_ff.h>
#include <events.h>
#include <filesystem.h>
#include <HAL.h>
#include <registry.h>
#include <time.h>
#include <stdio.h>
#include <string_consts.h>
#include <utils.h>

/************************************************
 Data Structures
************************************************/


typedef struct EVENT_BUFFER {
	uint8_t head;
	uint8_t tail;
	uint8_t size;
	uint8_t init;
	EventNode event_list[EVENT_BUFFER_SIZE];
} EventBuffer;


/************************************************
 Globals
************************************************/

static EventBuffer event_buffer;


/************************************************
 Functions
************************************************/

const char* FormatEventMessage (EventNode* e, char* e_str) {
	char ymd_str[14];
	char hms_str[12];
	const char* source_str;
	const char* destination_str;
	char msg_type_str[MAX_MESSAGE_STRING_LENGTH];
	char msg_str[MAX_MESSAGE_STRING_LENGTH];
	char float_str[20];
	char fmt_str[25];

	strcpy_hal(fmt_str, F("%Y-%m-%d"));
	strftime(ymd_str, 14, fmt_str, localtime(&(e->time_stamp)));
	strcpy_hal(fmt_str, F("%H:%M:%S"));
	strftime(hms_str, 12, fmt_str, localtime(&(e->time_stamp)));
	source_str = GetBlockLabelString(e->source);
	destination_str = GetBlockLabelString(e->destination);
	GetMessageTypeString(msg_type_str, e->message_type);
	GetMessageString(msg_str, e->message);
	FFFloatToCString(float_str, e->float_val);
	strcpy_hal(fmt_str, F("%s,%s,%s,%s,%s,%s,%ld,%s"));
	sprintf(e_str, fmt_str, ymd_str, hms_str, source_str, destination_str, msg_type_str, msg_str, e->int_val, float_str);

	return e_str;
}


uint8_t EventSendSubscribers(EventNode* e) {
	// XXX add logic to iterate a subscriber list and send to each
	//Manual inclusions to start with - need to implement subscriber publisher registry functions

	#ifdef SUPRESS_EVENT_MESSAGES
	(void)e;
	#endif

	#ifndef SUPRESS_EVENT_MESSAGES
	#ifdef EVENT_SERIAL
		HALEventSerialSend(e, EVENT_SERIAL_PORT);
	#endif

	#ifdef EVENT_CONSOLE
		char e_str[MAX_LOG_LINE_LENGTH];
		FormatEventMessage(e, e_str);
		printf("%s\n", e_str);
	#endif //DEBUG_CONSOLE
	#endif //ndef SUPRESS_EVENT_MESSAGES

	//TODO - OLD Comment:
	//Possibly call this from the Blocks rather than processing for every event
	//Or possible not - who should know if a reg update is required? Block or Event?

	// Partial move in this direction

	/*
	if     ((e->source == GetBlockIDByLabel(DISPLAY_INSIDE_SOURCE_BLOCK)) ||
			(e->source == GetBlockIDByLabel(DISPLAY_OUTSIDE_SOURCE_BLOCK)) ||
			(e->source == GetBlockIDByLabel(DISPLAY_WATER_SOURCE_BLOCK)) ) {
		UpdateStateRegister(e->source, e->message_type, e->message, e->int_val, e->float_val);
	}

	if ( (e->message_type == E_COMMAND) && (e->message == CMD_RESET_MINMAX) )
		UpdateStateRegister(e->source, e->message_type, e->message, e->int_val, e->float_val);
	*/

	return 1;
}

uint8_t EventBufferEmpty (void) {
	if (event_buffer.head == event_buffer.tail) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t EventBufferFull (void) {
	if ((event_buffer.head +1) % event_buffer.size == event_buffer.tail) {
		return 1;
	} else {
		return 0;
	}
}

EventNode* EventBufferPop(void) {
	if (event_buffer.init == 0) {

		EventNode* e_ptr;

		if (event_buffer.head != event_buffer.tail) {  						//ie not empty
			e_ptr = &event_buffer.event_list[event_buffer.tail];
			event_buffer.tail = (event_buffer.tail + 1) % event_buffer.size;
			return e_ptr;
		}

	} else {
#ifdef FF_ARDUINO
		DebugLog(SSS, E_STOP, M_EVENTMSG_BEFORE_INIT);
#endif
		while (1);
	}
	return 0;
}

void EventBufferPush(EventNode event) {
	if (event_buffer.init == 0) {

		// event is a simple struct of basic types, so assignment here works to copy
		event_buffer.event_list[event_buffer.head] = event;


		event_buffer.head = (event_buffer.head + 1) % event_buffer.size;
		if (event_buffer.head == event_buffer.tail) {
			event_buffer.tail = (event_buffer.tail + 1) % event_buffer.size;
		}
	} else {
#ifdef FF_ARDUINO
		DebugLog(SSS, E_STOP, M_EVENTMSG_BEFORE_INIT);
#endif
		while (1);
	}
}

//***** source and destination functions

void EventMsg(uint16_t source, uint16_t destination, uint8_t msg_type, uint8_t msg_str, int32_t i_val, float f_val) {
	// most developed message function - all fields
	// Accepts 32 bit in value
	if(event_buffer.init != 0) {
		DebugLog(SSS, E_STOP, M_EVENTMSG_BEFORE_INIT);
		while (1);
	}
	EventNode event;

	event.time_stamp = TimeNow();
	event.source = source;
	event.destination = destination;
	event.message_type = msg_type;
	event.message = msg_str;
	event.int_val = i_val;
	event.float_val = f_val;

	//EventBufferPush(event);
	EventSendSubscribers(&event);

	/*
	if(EventBufferFull()) {
		if (HALSaveEventBuffer()) {     			//write to file
			//DebugLog("Events Saved to File");
			if (!EventBufferEmpty()) {
				DebugLog(SSS, E_ERROR, M_BUF_NOT_EMPTY);
			}
		}
	}
	 */

	#ifdef DEBUG
		DebugLog(source, destination, msg_type, msg_str, i_val, f_val);
	#endif
}

void EventMsg(uint16_t source, uint16_t destination, uint8_t msg_type, uint8_t msg_str) {
	// simplified messages - no numerical data
	EventMsg(source, destination, msg_type, msg_str, INT32_INIT, FLOAT_INIT);
}

//***** source only event functions

// 32 bit int, Standard "complete" source only message
void EventMsg(uint16_t source, uint8_t msg_type, uint8_t msg_str, int32_t i_val, float f_val) {
	EventMsg(source, BLOCK_ID_NA, msg_type, msg_str, i_val, f_val);
}

void EventMsg(uint16_t source, uint8_t msg_type) {
	//source and type enum only
	EventMsg(source, msg_type, M_NULL, INT32_INIT, FLOAT_INIT);
}

void EventMsg(uint16_t source, uint8_t msg_type, uint8_t msg_str) {
	//source, type enum, message enum
	EventMsg(source, msg_type, msg_str, INT32_INIT, FLOAT_INIT);
}

void EventBufferInit(void) {
	// Initalise the event buffer
	DebugLog(SSS, E_VERBOSE, M_INIT_EVENT_BUF);
	event_buffer.head = 0;
	event_buffer.tail = 0;
	event_buffer.size = EVENT_BUFFER_SIZE;
	for (int i = 0; i < event_buffer.size; i++) {
		event_buffer.event_list[i].time_stamp = 0;
		event_buffer.event_list[i].source = 0;
		event_buffer.event_list[i].message_type = 0;
		event_buffer.event_list[i].message = 0;
		event_buffer.event_list[i].int_val = 0;
		event_buffer.event_list[i].float_val = 0.00;
	}
	event_buffer.init = 0;

}


