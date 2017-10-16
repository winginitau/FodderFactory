/************************************************
 ff_events.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Event Processing
************************************************/
#ifndef SRC_FF_EVENTS_H_
#define SRC_FF_EVENTS_H_

/************************************************
 Includes
************************************************/
#include <stdint.h>
#include "ff_datetime.h"

/************************************************
 Public and Forward Data Structure Declarations
************************************************/
typedef struct EVENT_NODE {
	FFDateTime time_stamp;
	uint8_t source;
	uint8_t message_type;
	uint8_t message;
	int int_val;
	float float_val;
} EventNode;

typedef struct EVENT_BUFFER EventBuffer;

/************************************************
 Function Prototypes
************************************************/

uint8_t EventBufferEmpty (void);

EventNode* EventBufferPop(void);

void EventMsg(uint8_t source, uint8_t msg_type, uint8_t log_msg, int i_val, float f_val);

void EventBufferInit(void);




#endif /* SRC_FF_EVENTS_H_ */