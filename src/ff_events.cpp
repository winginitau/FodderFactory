/************************************************
 ff_events.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Event Processing
************************************************/


/************************************************
 Includes
************************************************/
#include "ff_events.h"
#include "ff_sys_config.h"
#include "ff_string_consts.h"

//#include "ff_datetime.h"
#include "ff_debug.h"
#include "ff_registry.h"
//#include "ff_utils.h"
#include "ff_filesystem.h"

#include <time.h>


/************************************************
 Data Structures
************************************************/


typedef struct EVENT_BUFFER {
	int head;
	int tail;
	int size;
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
		}
		return e_ptr;
	} else {
#ifdef FF_ARDUINO
		DebugLog("STOP EventBufferPop before init");
#endif
		while (1);
	}
}

void EventBufferPush(EventNode event) {
	if (event_buffer.init == 0) {

		event_buffer.event_list[event_buffer.head] = event;

		event_buffer.head = (event_buffer.head + 1) % event_buffer.size;
		if (event_buffer.head == event_buffer.tail) {
			event_buffer.tail = (event_buffer.tail + 1) % event_buffer.size;
		}
	} else {
#ifdef FF_ARDUINO
		DebugLog("STOP EventBufferPush before INIT");
#endif
		while (1);
	}
}

void EventMsg(uint16_t source, uint16_t destination, uint8_t msg_type, uint8_t msg_str) {
//XXX
}

void EventMsg(uint16_t source, uint8_t msg_type) {
	//source and type only
	EventMsg(source, msg_type, M_NULL, UINT16_INIT, 0);
}

void EventMsg(uint16_t source, uint8_t msg_type, uint8_t msg_str) {
	//source, type, message string
	EventMsg(source, msg_type, msg_str, UINT16_INIT, 0);
}

void EventMsg(uint16_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val) {

	if(event_buffer.init != 0) {
		DebugLog("STOP EventMsg Before Init");
		while (1);
	}
	EventNode event;

	event.time_stamp = time(NULL);
	event.source = source;
	event.message_type = msg_type;
	event.message = msg_str;
	event.int_val = i_val;
	event.float_val = f_val;

	EventBufferPush(event);

	if(EventBufferFull()) {
		if (SaveEventBuffer()) {     			//write to file
			//DebugLog("Events Saved to File");
			if (!EventBufferEmpty()) {
				DebugLog("ERROR Save did not fully flush Event Buffer");
			}
		}
	}

	//TODO - Possibly call this from the Blocks rather than processing for every event
	//Or possible not - who should know if a reg update is required? Block or Event?

	UpdateStateRegister(source, msg_type, msg_str, i_val, f_val);

#ifdef DEBUG

	//String debug_log_message;
	//debug_log_message = (String)GetBlockLabelString(source) + ", " + (String)GetMessageTypeString(msg_type) + ", " + (String)GetMessageString(msg_str) + ", " + (String)i_val + ", " + (String)f_val;
	//DebugLog(debug_log_message);
	DebugLog(source, msg_type, msg_str, i_val, f_val);

#endif
}



void EventBufferInit(void) {
	// Initalise the event buffer
	DebugLog("[INIT] Setting up Message Queue (AKA Event Ring Buffer)");
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


