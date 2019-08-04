/************************************************
 ff_registry.h
 Fodder Factory Monitoring and Control

 (c) Brendan McLearie - bmclearie@mclearie.com

 Component Registry
************************************************/
#ifndef SRC_FF_REGISTRY_H_
#define SRC_FF_REGISTRY_H_

/************************************************
 Includes
************************************************/
#include <build_config.h>
#include <time.h>
#include <stdint.h>

/************************************************
 Public and Forward Data Structure Declarations
************************************************/

typedef struct FF_SYSTEM_SPECIFIC {		// 5 bytes
	uint8_t temp_scale;
	uint8_t language;
	uint8_t week_start;
	uint16_t start_delay;
} SystemSpecific;

typedef struct FF_INPUT_SPECIFIC {		// 16 bytes
	// Settings
	uint16_t interface;
	uint8_t if_num;
	uint16_t log_rate;
	uint16_t poll_rate;
	uint8_t data_units;
	uint8_t data_type;		// float, int
	// Run-time
	time_t last_polled;		// The time the block last read / polled its interface
	time_t last_logged; 	// The time the block last logged a message
} InputSpecific;

typedef struct FF_MONITOR_SPECIFIC {	// 16 bytes
	uint16_t input1;
	uint16_t input2;
	uint16_t input3;
	uint16_t input4;
	float act_val;
	float deact_val;
} MonitorSpecific;

typedef struct FF_SCHED_SPECIFIC {		// 18 bytes
	uint8_t days[7];
	time_t time_start;
	time_t time_end;
	TV_TYPE time_duration;
	TV_TYPE time_repeat;
} ScheduleSpecific;

typedef struct FF_RULE_SPECIFIC {		// 8 bytes
	uint16_t param1;
	uint16_t param2;
	uint16_t param3;
	uint16_t param_not;
} RuleSpecific;

typedef struct FF_CONTROLLER_SPECIFIC {	// 6 bytes
	uint16_t rule;
	uint16_t output;
	uint8_t act_cmd;
	uint8_t deact_cmd;
} ControllerSpecific;

typedef struct FF_OUTPUT_SPECIFIC {		// 3 bytes
	uint8_t interface;
	uint8_t if_num;
	uint8_t command;
} OutputSpecific;


typedef union BLOCK_SETTINGS {
	SystemSpecific sys;
	InputSpecific in;
	MonitorSpecific mon;
	ScheduleSpecific sch;
	RuleSpecific rl;
	ControllerSpecific con;
	OutputSpecific out;
} BlockSettings;

typedef struct BLOCK_NODE {						// 9 bytes common config
	// common configuration data
	// note: these types are locked to config file keys strings
	// see ff_string_consts.h
	struct BLOCK_NODE *next;
	uint8_t cat;
	uint16_t type;
	uint16_t id;
	//char block_label[MAX_LABEL_LENGTH];
	char *label;

	#ifndef EXCLUDE_DISPLAYNAME
		//char display_name[MAX_LABEL_LENGTH];
		char *display_name;
	#endif
	#ifndef EXCLUDE_DESCRIPTION
		//char description[MAX_DESCR_LENGTH];
		char *description;
	#endif

	// common operational, run-time data		// 11 bytes common operational
	// not tied to config, though must be initialised in AddNewBlock()
	uint8_t active;		// boolean flag - is the block activated or deactivated TODO consider renaming
	uint8_t bool_val;	// generic holder for a boolean value (representing some real world status)
	int32_t int_val;	// generic holder of an int value (representing some real world status)
	float f_val;		// generic holder of a float value (representing some real world status)
	//time_t last_update;	// The time the block was last updated / operated / polled etc
	//time_t last_logged; // The time the block last logged a message (if applicable)
	uint8_t status;		// Status flag (eg for MON cat to indicate if the last input read was valid)

	// union of block_type specific settings
	// also tied to config file key strings
	BlockSettings settings;
} BlockNode;

typedef struct INT_WIFI {
	char* wifi_ssid[16];
	char* wpa_psk[16];
	uint8_t static_address;
	uint8_t ip_static[4];
	uint8_t netmask_static[4];
	uint8_t gateway_staic[4];
} InterfaceWIFI;

typedef struct INT_ETHERNET {
	uint8_t static_address;
	uint8_t ip_static[4];
	uint8_t ip_netmask[4];
	uint8_t ip_gateway[4];
} InterfaceETHERNET;

typedef struct INT_PWM_IN {
	//"IF_PWM_IN"
} InterfacePWM_IN;

typedef struct INT_PWM_OUT {
	//"IF_PWM_OUT"
} InterfacePWM_OUT;

typedef struct INT_PPM_IN {
	//"IF_PPM_IN"
} InterfacePPM_IN;

typedef struct INT_PPM_OUT {
	//"IF_PPM_OUT"
}InterfacePPM_OUT;

typedef struct INT_DS1820B {
	uint8_t bus_pin;
	uint8_t dallas_address[8];
	uint8_t in_progress;
} InterfaceDS1820B;

typedef struct INT_ONEWIRE_BUS {
	uint8_t bus_pin;
	uint8_t device_count;
} InterfaceONEWIRE_BUS;

typedef struct INT_DIG_PIN_IN {
	//"IF_DIG_PIN_IN"
} InterfaceDIG_PIN_IN;

typedef struct INT_DIG_PIN_OUT {
	//"IF_DIG_PIN_OUT"
} InterfaceDIG_PIN_OUT;

typedef struct INT_VED_VOLTAGE {
	//"IF_VED_VOLTAGE"
} InterfaceVED_VOLTAGE;

typedef struct INT_VED_CURRENT {
	//"IF_VED_CURRENT"
} InterfaceVED_CURRENT;

typedef struct INT_VED_POWER {
	//"IF_VED_POWER"
} InterfaceVED_POWER;

typedef struct INT_VED_SOC {
	//"IF_VED_SOC"
} InterfaceVED_SOC;

typedef struct INT_HW_SERIAL {
	uint8_t port_num;
	uint8_t speed;
} InterfaceHW_SERIAL;

typedef struct INTERFACE_NODE {
	uint16_t id;
	char* label;
	uint8_t	type;
	#ifndef	EXCLUDE_DISPLAYNAME
	char* display_name;
	#endif
	#ifndef EXCLUDE_DESCRIPTION
	char* description;
	#endif
	void* IFSettings;
	uint8_t status;
	struct INTERFACE_NODE *next;
} InterfaceNode;


typedef struct UI_DATA_SET {
	float inside_current;
	float inside_min;
	time_t inside_min_dt;
	float inside_max;
	time_t inside_max_dt;

	float outside_current;
	float outside_min;
	time_t outside_min_dt;
	float outside_max;
	time_t outside_max_dt;

	float water_current;
	float water_min;
	time_t water_min_dt;
	float water_max;
	time_t water_max_dt;

	//TODO not being used - delete?
	//uint8_t light_flag;				//current state of lights
	//uint8_t water_heater_flag;		//current state of water heater
} UIDataSet;

/************************************************
 Function Prototypes
************************************************/

// Main running routines
void BlockDispatcher(void(*f)(BlockNode*));
void InterfaceDispatcher(void (*func)(InterfaceNode*));
void Operate(BlockNode *b);
void SetupInterface(InterfaceNode* b);
void Setup(BlockNode *b);

// Low Level Block List Operations
uint8_t GetBlockCatIdxByBlockCatName(char* name);
InterfaceNode* GetInterfaceByID(uint16_t id);
uint8_t SetCommand(uint16_t block_id, uint8_t cmd_msg);
uint8_t IsActive(uint16_t block_id);
uint8_t GetBVal(uint16_t block_id);
float GetFVal(uint16_t block_id);
uint16_t GetBlockIDByLabel(const char* label, bool report_fail);
char const* GetBlockLabelString(uint16_t block_id);
void SetBlockLabelString(uint8_t block_type, int idx, const char* label);
BlockNode* GetLastBlockAddr(void);
BlockNode* GetBlockListHead(void);
InterfaceNode* GetInterfaceListHead(void);
BlockNode* GetBlockByID(BlockNode *list_node, uint16_t block_id);
BlockNode* GetBlockByLabel(const char *block_label);
InterfaceNode* GetInterfaceNodeByLabel(InterfaceNode *head, const char* label);
InterfaceNode* GetInterfaceByLabel(const char* label);
uint8_t DeleteBlockByID(uint16_t bid);
uint8_t DeleteBlockByLabel(const char *label);
BlockNode* AddBlock(uint8_t block_cat, const char *block_label);
InterfaceNode* AddInterface(uint8_t if_type, const char *label);
//BlockNode* AddBlockNode(BlockNode** head_ref, uint8_t block_cat, const char *block_label);
char* UpdateBlockLabel(BlockNode* b, const char * block_label);
#ifndef EXCLUDE_DISPLAYNAME
char* UpdateDisplayName(BlockNode* b, const char * display_name);
#endif
#ifndef EXCLUDE_DESCRIPTION
char* UpdateDescription(BlockNode* b, const char * description);
#endif
uint16_t GetInterfaceWithMatchingDallas(uint8_t addr[8]);
void DropBlockList(void);

// itch integration functions (called by user_code.cpp)
uint8_t RegLookupBlockLabel(const char* lookup_string);
void RegShowBlocks(void(*Callback)(const char *));
void RegShowSystem(void(*Callback)(const char *));
void RegShowBlockByID(uint16_t, void(*Callback)(const char *));
void RegShowBlockByLabel(const char* block_label, void(*Callback)(const char *));
void RegIFOneWireScanBID(int16_t param1_int, void(*Callback)(const char*));
void RegIFOneWireScanLabel(char* BLOCK_LABEL, void(*Callback)(const char*));
void RegIFOneWireAssignBID(int16_t param1_int, int16_t param2_int, void(*Callback)(const char*));
void RegIFDS1820BRead(int16_t param1_int, void(*Callback)(const char*));
void RegIFDS1820BTest(int16_t param1_int, void(*Callback)(const char*));
void RegSetCommandOnBlockLabel(const char* block_label, uint16_t command, void(*Callback)(const char *));
void RegSetCommandOnBlockID(uint16_t id, uint16_t command, void(*Callback)(const char*));
void RegShowTime(void(*Callback)(const char*));
void RegSetTime(const char* time_str, void(*Callback)(const char*));
void RegShowDate(void(*Callback)(const char*));
void RegSetDate(const char* date_str, void(*Callback)(const char*));

void RegConfigClear(void(*Callback)(const char*));
void RegConfigLoad(void(*Callback)(const char*));
void RegConfigLoadBinary(void(*Callback)(const char*));
void RegConfigLoadINI(void(*Callback)(const char*));
void RegConfigSave(void(*Callback)(const char*));
void RegConfigShow(void(*Callback)(const char*));
void RegConfigSaveBinary(void(*Callback)(const char*));

void RegConfigInterface(const char* param1_string, uint16_t IF_CONFIG, const char* param2_string, void(*Callback)(const char*));
void RegConfigBlockSystem(const char* param1_string, uint16_t SYS_CONFIG, const char* param2_string, void(*Callback)(const char*));
void RegConfigBlockInput(const char* param1_string, uint16_t IN_CONFIG, const char* param2_string, void(*Callback)(const char*));
void RegConfigBlockMonitor(const char* param1_string, uint16_t MON_CONFIG, const char* param2_string, void(*Callback)(const char*));
void RegConfigBlockSchedule(const char* param1_string, uint16_t SCH_CONFIG, const char* param2_string, void(*Callback)(const char*));
void RegConfigBlockRule(const char* param1_string, uint16_t RL_CONFIG, const char* param2_string, void(*Callback)(const char*));
void RegConfigBlockController(const char* param1_string, uint16_t CON_CONFIG, const char* param2_string, void(*Callback)(const char*));
void RegConfigBlockOutput(const char* param1_string, uint16_t OUT_CONFIG, const char* param2_string, void(*Callback)(const char*));

void RegInitSetupAll(void(*Callback)(const char*));
void RegInitSetupBID(uint16_t block_id, void(*Callback)(const char*));
void RegInitValidateAll(void(*Callback)(const char*));
void RegInitValidateBID(uint16_t block_id, void(*Callback)(const char*));
void RegInitDisableAll(void(*Callback)(const char*));
void RegInitDisableBID(uint16_t block_id, void(*Callback)(const char*));

void RegAdminDisableBID(uint16_t block_id, void(*Callback)(const char*));
void RegAdminEnableBID(uint16_t block_id, void(*Callback)(const char*));
void RegConfigDeleteBID(uint16_t block_id, void(*Callback)(const char*));
void RegConfigDeleteBlockLabel(const char* BLOCK_LABEL, void(*Callback)(const char*));
void RegAdminCmdOnBID(uint16_t block_id, void(*Callback)(const char*));
void RegAdminCmdOffBID(uint16_t block_id, void(*Callback)(const char*));

void RegSystemReboot(void(*Callback)(const char*));

// Other misc functions
void UpdateStateRegister(uint16_t source, uint8_t msg_type, uint8_t msg_str, int i_val, float f_val);
void InitStateRegister(void);

#ifdef ARDUINO_LCD
UIDataSet* GetUIDataSet(void);
#endif //ARDUINO_LCD

#endif /* SRC_FF_REGISTRY_H_ */
