/************************************************
 interfaces.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Interface Processing
 ************************************************/

#ifndef INTERFACES_H_
#define INTERFACES_H_

/************************************************
 Includes
************************************************/

#include <registry.h>

/************************************************
  Function prototypes
************************************************/
void ScanAndMatchDallasSensors(InterfaceNode* i, uint8_t report);
void AssignDallasSensor(InterfaceNode *s, InterfaceNode *t, uint8_t report);
void InterfaceONEWIRE_BUS_Setup(InterfaceNode* b);
void InterfaceDS1820B_Setup(InterfaceNode *b);
uint8_t InterfaceDS1820B_Read(float* tempC, uint16_t id, uint8_t wait);
void InterfaceShow(InterfaceNode *i, void(Callback(const char *)));




#endif /* INTERFACES_H_ */
