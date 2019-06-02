/************************************************
 ff_display.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Display Functions
************************************************/
#ifndef SRC_FF_DISPLAY_H_
#define SRC_FF_DISPLAY_H_



/************************************************
 Includes
************************************************/


/************************************************
 Public and Forward Data Structure Declarations
************************************************/


/************************************************
 Function Prototypes
************************************************/

#ifdef DEBUG_LCD
void HALDebugLCD(String log_entry);
#endif

#ifdef UI_ATTACHED
void InitUI(void);
void UpdateUI(void);
#endif //UI_ATTACHED

#endif /* SRC_FF_DISPLAY_H_ */
