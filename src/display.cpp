/************************************************
 ff_events.cpp
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Event Processing
************************************************/


/************************************************
 Includes
************************************************/

//#include "ff_sys_config.h"
//#include "ff_utils.h"
#include <events.h>
#include <HAL.h>
#include <HAL.h>
#include <string_consts.h>
#include <time.h>

/************************************************
 Data Structures
************************************************/

/************************************************
 Globals
************************************************/

/************************************************
 Functions
************************************************/

#ifdef UI_ATTACHED
void InitUI(void) {
	HALInitUI();
#ifdef PLATFORM_ARDUINO
#ifdef LCD_DISPLAY
	EventMsg(SSS, E_INFO, M_LCD_INIT);
#endif //PLATFORM_ARDUINO
#endif
#ifdef PLATFORM_LINUX
	EventMsg(SSS, E_INFO, M_SIM_CONSOLE);
#endif
}
#endif //UI_ATTACHED

#ifdef UI_ATTACHED
void UpdateUI(void) {
	UIDataSet* ui_data_ptr = GetUIDataSet();
	time_t dt_now = TimeNow();
	HALDrawDataScreenCV(ui_data_ptr, dt_now);
}
#endif
