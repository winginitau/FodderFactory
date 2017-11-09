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
#include "ff_HAL.h"
#include "ff_string_consts.h"
#include "ff_events.h"

/************************************************
 Data Structures
************************************************/

/************************************************
 Globals
************************************************/

/************************************************
 Functions
************************************************/


void InitUI(void) {
	HALInitUI();
#ifdef FF_ARDUINO
	EventMsg(SSS, E_INFO, M_LCD_INIT);
#endif
#ifdef FF_SIMULATOR
	EventMsg(SSS, E_INFO, M_SIM_CONSOLE);
#endif
}

void UpdateUI(void) {

	const UIDataSet* ui_data_ptr = GetUIDataSet();

	time_t dt_now = time(NULL);

	HALDrawDataScreenCV(ui_data_ptr, dt_now);

}

