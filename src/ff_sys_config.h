/************************************************
 ff_sys_config.h
 FF System Core (Stand-alone Embedded)

 (c) Brendan McLearie - bmclearie@mclearie.com

 Preprocessor and compile time directives
 that configure the core system.

 See also: "ff_string_consts.h" for extended
 configuration enums, labels, etc

 ************************************************/
#ifndef FF_SYS_CONFIG_H_
#define FF_SYS_CONFIG_H_

#define VERSION v0.3dev

/************************************************
 PLATFORM Directives
 ************************************************/


//#define FF_ARDUINO
//#define FF_TEMPERATURE_SIM  //optionally
//OR
#define FF_SIMULATOR

#ifdef FF_SIMULATOR
#define FF_TEMPERATURE_SIM
#endif

#ifdef FF_TEMPERATURE_SIM
#define FF_DEFINED_TEMP_SIM
// OR
//#define FF_RANDOM_TEMP_SIM
#endif


#ifdef FF_DEFINED_TEMP_SIM
#define SIM_TEMP_0 18
#define SIM_TEMP_1 21
#define SIM_TEMP_2 26
#define SIM_TEMP_3 29
#endif

/************************************************
 DEBUG Directives
 ************************************************/
#define DEBUG                         	// Call debug output at all or not?

#ifdef DEBUG

#ifdef FF_ARDUINO
//#define DEBUG_SERIAL                  	// For use when USB connected
#define DEBUG_SERIAL_BAUDRATE 9600
#define DEBUG_LCD                     	// Debug to a connected LCD screen
#define DEBUG_LCD_DELAY 3000
#endif

//#define DEBUG_FILE						//write a debug file on SD or locally
#define DEBUG_FILENAME "FFDEBUG.TXT"

#ifdef FF_SIMULATOR
#define DEBUG_CONSOLE					//write to stdout
#endif

#define MAX_DEBUG_LENGTH 255			//line buffer length for debug messages "stack smashing if too short"

#endif //DEBUG

/************************************************
 System, Hardware, PINS and Initialisation
 ************************************************/
//#define SET_RTC YES						//WARNING - This hard codes the time on restart to the compile time
										//as the set time on device restart - use once to set then exclude
#define DIG_HIGH 0x1
#define DIG_LOW 0x0

#define ONE_WIRE_BUS 6                	//for Dallas temp signals

#define EVENT_BUFFER_SIZE 50

#define UINT8_INIT 0xFF
#define UINT16_INIT 0xFFFF
#define UINT32_INIT 0xFFFFFFFF
#define FLOAT_INIT 255.000000
#define BLOCK_ID_BASE 1000
#define SSS 500   //System Reserved Source Block ID for Messages

//Time Value type for rates and periods (in seconds)
// uint8_t = 255 seconds = 4.25 minutes
// unit16_t = 65535 seconds = 1092.25 minutes = 18.21 hours
// uint32_t = About 137 years....
#define TV_TYPE	uint32_t

// Maximum length for filename, excluding NULL char 26 chars allows an
// 8.3 filename instead and 8.3 directory with a leading slash
#define INI_FILE_MAX_FILENAME_LEN 26

#define EVENT_FILENAME "FFEVENTS.TXT"
#define CONFIG_FILENAME "FFCONFIG.TXT"

#define INI_FILE_MAX_LINE_LENGTH 150
#define INI_ERROR_TYPES 9

//#define BLOCK_COUNT 15 //(INPUT + CONTROLLERS + OUTPUTS + 1 for SYSTEM +1 for UNDEF)

#define MAX_LABEL_LENGTH 64
#define MAX_DESCR_LENGTH 80
#define MAX_BLOCKS 255
#define MAX_BLOCKS_PER_CATEGORY 255
#define MAX_CONF_KEYS_PER_BLOCK 12
/************************************************
 Inputs Definitions
 ************************************************/
#define INPUT_COUNT 3

#define I1_INPUT_NO			0
#define I1_DEVICE_LABEL		"INSIDE_TOP_TEMP"
#define I1_DEVICE_NUM		1
//#define I1_DESCR			"Dallas sensor inside the fodder factory"
#define I1_LOG_RATE_DAY		0
#define I1_LOG_RATE_HOUR	0
#define I1_LOG_RATE_MINUTE	1
#define I1_LOG_RATE_SECOND	0
//#define I1_DATA_UNITS		DEG_C

#define I2_INPUT_NO			1
#define I2_DEVICE_LABEL		"OUTSIDE_TEMP"
#define I2_DEVICE_NUM		0
//#define I2_DESCR			"Dallas sensor for outside air temperature"
#define I2_LOG_RATE_DAY		0
#define I2_LOG_RATE_HOUR	0
#define I2_LOG_RATE_MINUTE	1
#define I2_LOG_RATE_SECOND	0
//#define I2_DATA_UNITS		DEG_C

#define I3_INPUT_NO			2
#define I3_DEVICE_LABEL		"WATER_TEMP"
#define I3_DEVICE_NUM		2
//#define I3_DESCR			"Dallas sensor in the header tank"
#define I3_LOG_RATE_DAY		0
#define I3_LOG_RATE_HOUR	0
#define I3_LOG_RATE_MINUTE	1
#define I3_LOG_RATE_SECOND	0
//#define I3_DATA_UNITS		DEG_C

/************************************************
 Controllers
 ************************************************/

//  ****NOTE: Scheduled controls must be in time order to ensure correct start up sequence
#define CONTROL_COUNT 		8

/*
#define C1_CONTROL_NO			0
#define C1_LABEL			"INSIDE_TOO_COLD"
#define	C1_TYPE				CONDITION_LOW
//#define C1_DESCR			"Turn on the water heater"
#define C1_INPUT			"INSIDE_TEMP"
#define C1_OUTPUT			"WATER_HEATER"
#define C1_ACT_COMMAND		OUTPUT_ON
#define C1_DEACT_COMMAND	OUTPUT_OFF
#define C1_LOW_VAL			19.00
#define C1_HIGH_VAL			0
#define C1_SCHED_DAY		0
#define C1_SCHED_HOUR		0
#define C1_SCHED_MINUTE		0
#define C1_SCHED_SECOND		0

#define C2_CONTROL_NO		1
#define C2_LABEL			"INSIDE_TOO_HOT"
#define	C2_TYPE				CONDITION_HIGH
//#define C2_DESCR			"Turn on the fan"
#define C2_INPUT			"INSIDE_TEMP"
#define C2_OUTPUT			"COOLING_FAN"
#define C2_ACT_COMMAND		OUTPUT_ON
#define C2_DEACT_COMMAND	OUTPUT_OFF
#define C2_LOW_VAL			0
#define C2_HIGH_VAL			25.00
#define C2_SCHED_DAY		0
#define C2_SCHED_HOUR		0
#define C2_SCHED_MINUTE		0
#define C2_SCHED_SECOND		0

*/

#define C1_CONTROL_NO		0
#define C1_LABEL			"TURN_ON_WATER_HEAT_18:00"
#define	C1_TYPE				SCHED_DAILY_ON
//#define C1_DESCR			"Turn on the water heater"
#define C1_INPUT			""
#define C1_OUTPUT			"WATER_HEATER"
#define C1_ACT_COMMAND		CMD_OUTPUT_ON
#define C1_DEACT_COMMAND	CMD_OUTPUT_OFF
#define C1_LOW_VAL			0
#define C1_HIGH_VAL			0
#define C1_SCHED_DAY		0
#define C1_SCHED_HOUR		18
#define C1_SCHED_MINUTE		0
#define C1_SCHED_SECOND		0

#define C2_CONTROL_NO		1
#define C2_LABEL			"TURN_ON_LIGHTS_22:00"
#define	C2_TYPE				SCHED_DAILY_ON
//#define C2_DESCR			"Turn on the grow lights"
#define C2_INPUT			""
#define C2_OUTPUT			"LIGHTS"
#define C2_ACT_COMMAND		CMD_OUTPUT_ON
#define C2_DEACT_COMMAND	CMD_OUTPUT_OFF
#define C2_LOW_VAL			0
#define C2_HIGH_VAL			0
#define C2_SCHED_DAY		0
#define C2_SCHED_HOUR		22
#define C2_SCHED_MINUTE		0
#define C2_SCHED_SECOND		0

#define C3_CONTROL_NO		2
#define C3_LABEL			"TURN_ON_WATER_HEAT_0:02"
#define	C3_TYPE				SCHED_DAILY_ON
//#define C3_DESCR			"Turn on the water heater"
#define C3_INPUT			""
#define C3_OUTPUT			"WATER_HEATER"
#define C3_ACT_COMMAND		CMD_OUTPUT_ON
#define C3_DEACT_COMMAND	CMD_OUTPUT_OFF
#define C3_LOW_VAL			0
#define C3_HIGH_VAL			0
#define C3_SCHED_DAY		0
#define C3_SCHED_HOUR		0
#define C3_SCHED_MINUTE		2
#define C3_SCHED_SECOND		0

#define C4_CONTROL_NO		3
#define C4_LABEL			"TURN_ON_LIGHTS_0:02"
#define	C4_TYPE				SCHED_DAILY_ON
//#define C4_DESCR			"Turn on the grow lights"
#define C4_INPUT			""
#define C4_OUTPUT			"LIGHTS"
#define C4_ACT_COMMAND		CMD_OUTPUT_ON
#define C4_DEACT_COMMAND	CMD_OUTPUT_OFF
#define C4_LOW_VAL			0
#define C4_HIGH_VAL			0
#define C4_SCHED_DAY		0
#define C4_SCHED_HOUR		0
#define C4_SCHED_MINUTE		2
#define C4_SCHED_SECOND		0

#define C5_CONTROL_NO		4
#define C5_LABEL			"TURN_OFF_WATER_HEAT_9:00"
#define	C5_TYPE				SCHED_DAILY_OFF
//#define C5_DESCR			"Turn off the water heater"
#define C5_INPUT			""
#define C5_OUTPUT			"WATER_HEATER"
#define C5_ACT_COMMAND		CMD_OUTPUT_ON
#define C5_DEACT_COMMAND	CMD_OUTPUT_OFF
#define C5_LOW_VAL			0
#define C5_HIGH_VAL			0
#define C5_SCHED_DAY		0
#define C5_SCHED_HOUR		9
#define C5_SCHED_MINUTE		0
#define C5_SCHED_SECOND		0

#define C6_CONTROL_NO		5
#define C6_LABEL			"TURN_OFF_LIGHTS_9:00"
#define	C6_TYPE				SCHED_DAILY_OFF
//#define C6_DESCR			"Turn off the grow lights"
#define C6_INPUT			""
#define C6_OUTPUT			"LIGHTS"
#define C6_ACT_COMMAND		CMD_OUTPUT_ON
#define C6_DEACT_COMMAND	CMD_OUTPUT_OFF
#define C6_LOW_VAL			0
#define C6_HIGH_VAL			0
#define C6_SCHED_DAY		0
#define C6_SCHED_HOUR		9
#define C6_SCHED_MINUTE		0
#define C6_SCHED_SECOND		0

#define C7_CONTROL_NO		6
#define C7_LABEL			"RESET_MIN_MAX"
#define	C7_TYPE				SYSTEM_MSG
//#define C7_DESCR			"Reset Temperature Min and Max Daily - until we have a datalogger"
#define C7_INPUT			""
#define C7_OUTPUT			""
#define C7_ACT_COMMAND		0
#define C7_DEACT_COMMAND	0
#define C7_LOW_VAL			0
#define C7_HIGH_VAL			0
#define C7_SCHED_DAY		0
#define C7_SCHED_HOUR		17
#define C7_SCHED_MINUTE		0
#define C7_SCHED_SECOND		0

#define C8_CONTROL_NO		7
#define C8_LABEL			"CLEAR_DONE_TODAY"
#define	C8_TYPE				CLEAR_DONE
//#define C8_DESCR			"Work around - clear Done Today Flags just after midnight"
#define C8_INPUT			""
#define C8_OUTPUT			""
#define C8_ACT_COMMAND		0
#define C8_DEACT_COMMAND	0
#define C8_LOW_VAL			0
#define C8_HIGH_VAL			0
#define C8_SCHED_DAY		0
#define C8_SCHED_HOUR		0
#define C8_SCHED_MINUTE		1
#define C8_SCHED_SECOND		0


/************************************************
 Outputs
 ************************************************/
#define OUTPUT_COUNT			2

#define OUT1_OUTPUT_NO			0
#define OUT1_DEVICE_LABEL		"LIGHTS"
#define OUT1_DEVICE_NUM			0
//#define OUT1_DESCR				"Relay to Control Grow Lights"
#define OUT1_DIGITAL_PIN		30

#define OUT2_OUTPUT_NO			1
#define OUT2_DEVICE_LABEL		"WATER_HEATER"
#define OUT2_DEVICE_NUM			0
//#define OUT2_DESCR				"Relay to the water heater"
#define OUT2_DIGITAL_PIN		32




#endif /* FF_SYS_CONFIG_H_ */
