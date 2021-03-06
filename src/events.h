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
#include <datetime_ff.h>
#include <stdint.h>
#include <time.h>

/************************************************
 Public and Forward Data Structure Declarations
************************************************/
typedef struct EVENT_NODE {
	time_t time_stamp;
	uint16_t source;
	uint16_t destination;
	uint8_t message_type;
	uint8_t message;
	int32_t int_val;
	float float_val;
} EventNode;

typedef struct EVENT_BUFFER EventBuffer;

/************************************************
 Function Prototypes
************************************************/

const char* FormatEventMessage (EventNode* e, char* e_str);
uint8_t EventBufferEmpty (void);
EventNode* EventBufferPop(void);

void EventMsg(uint16_t source, uint16_t destination, uint8_t msg_type, uint8_t msg_str, int32_t i_val, float f_val);
void EventMsg(uint16_t source, uint16_t destination, uint8_t msg_type, uint8_t msg_str);
void EventMsg(uint16_t source, uint8_t msg_type, uint8_t msg_str, int32_t i_val, float f_val);
void EventMsg(uint16_t source, uint8_t msg_type, uint8_t msg_str);
void EventMsg(uint16_t source, uint8_t msg_type);
//void EventMsg(uint16_t source, uint8_t msg_type, float f_val);

void EventBufferInit(void);




#endif /* SRC_FF_EVENTS_H_ */
