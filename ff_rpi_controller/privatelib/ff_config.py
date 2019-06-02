'''
Created on 2 Jul. 2018

@author: brendan
'''

#from privatelib.msg_parser import MessageSystem

from privatelib.palette import \
FF_WHITE_HG, \
FF_RED_LG, \
FF_GREEN_LG, \
FF_COLD_BLUE_LG, \
FF_FREEZE_BLUE_LG, \
FF_YELLOW_LG
#FF_BLACK_LG, \
#FF_BLACK_MG, \
#FF_BLACK_HG, \
#FF_BLACK, \
#FF_WHITE_LG, \
#FF_WHITE, \
#FF_RED_HG, \
#FF_RED, \
#FF_GREEN_HG, \
#FF_GREEN, \
#FF_BLUE_LG, \
#FF_BLUE_HG, \
#FF_BLUE, \
#FF_COLD_BLUE_HG, \
#FF_COLD_BLUE, \
#FF_FREEZE_BLUE_HG, \
#FF_FREEZE_BLUE, \
#FF_YELLOW_HG, \
#FF_YELLOW, \
#FF_GREY_LG, \
#FF_GREY_HG, \
#FF_GREY, \
#FF_SAND_LG, \
#FF_SAND_HG, \
#FF_SAND, \
#FF_SLATE_LG, \
#FF_SLATE_HG, \
#FF_SLATE

from datetime import datetime, date, time 

#########################################################
# [Index, "SOURCE BLOCK", "Display Label", initial_graph_period_days]
#########################################################
INPUTS_LIST = (
    [0, "IN_TOP_TEMP", "Top", 1],
    [1, "IN_BOT_TEMP", "Bottom", 1],
    [2, "IN_OUT_TEMP", "Outside", 3],
    #[3, "IN_WATER_TEMP", "Water"],
    #[4, "IN_CAB_TEMP", "Cabinet"],
    [3, "IN_TANK_TEMP", "Tank", 10]
)

# [Index, "SOURCE BLOCK", "Display Label"]

OUTPUTS_LIST = (
    [0, "OUT_CIRC_FAN", "Circulation", 1],
    [1, "OUT_EXH_FAN", "Exhaust", 1],
    [2, "OUT_COLD_FAN", "Cooling", 1],
    [3, "OUT_LIGHTS", "Lights", 5], 
    [4, "OUT_WATER_TOP", "Top Spray", 1],
    [5, "OUT_WATER_BOT", "Bottom Spray", 1],
    [6, "OUT_COLD_WATER", "Cold Water", 1],
    #[1, "OUT_WATER_HEAT", "Heater"],
)

DB_INPUT_LOW_HIGH = (
    ["IN_TOP_TEMP", 18, 22],
    ["IN_BOT_TEMP", 18, 22],
    ["IN_OUT_TEMP", 18, 22],
    ["IN_WATER_TEMP", 18, 22],
    ["IN_CAB_TEMP", 18, 22],
    ["IN_TANK_TEMP", 18, 22]
)

ENERGY_LIST = (
    ["VE_DATA_SOC", "State of Charge", 10],
    ["VE_DATA_VOLTAGE", "Battery Voltage", 5], 
    ["VE_DATA_POWER", "Power Flow", 2],
    ["VE_DATA_CURRENT", "Battery Current", 2],
)

# Battery stats
# C10 Low 1.8v / cell
# C8 Low 1.75c / cell
#  never below 1.6 for more than 2 minutes
# Charge at 2.25 - 2.28 @ 25c 
DB_ENERGY_GRAPH_PARAMS = (
    # SOURCE, Y-LABEL, Y-LOW-LINE, Y-HIGH-LINE, Y-TICS
    ["VE_DATA_SOC", "% of Full Capacity", 50, 100, 10],
    ["VE_DATA_VOLTAGE", "Volts", 21.6, 27.36, 1], 
    ["VE_DATA_POWER", "Watts", 0, 0, 200],
    ["VE_DATA_CURRENT", "Amps", 0, 0, 5],        
)

RULES_TO_CATCH = (
    ["RL_LIGHTS", "Lights"],
    ["RL_COLD_FAN", "Cooling"], 
    ["RL_CIRC_FAN", "Circulate"], 
    ["RL_EXH_FAN", "Exhaust"],
    ["RL_COLD_WATER_TOP", "Cold Water Top"],
    ["RL_COLD_WATER_BOT", "Cold Water Bottom"],
    ["RL_WATER_TOP", "Water Top"],
    ["RL_WATER_BOT", "Water Bottom"],
    ["RL_BOOST", "Boost"],
    ["RL_TOP_BOOST", "Boost Top"],
    ["RL_BOT_BOOST", "Boost Bottom"],
)
    
########################################################
INT8_INIT = -127
UINT8_INIT = 255

UINT16_INIT = 65535
FLOAT_INIT = 255.255

TEMPERATURE_SANITY_HIGH = 79.0
TEMPERATURE_SANITY_LOW = -60.0

########################################################
#MESSAGE_FILENAME = "message_dev.log"
#BACKGROUND_IMAGE = 'cows.png'
BACKGROUND_IMAGE = 'night3.jpg'
MESSAGE_FILENAME = "message.log"
MODEM_SERIAL_PORT = "/dev/ttyV0"
MODEM_SERIAL_SPEED = 9600

SERIAL_POLL_INTERVAL = 0.1      # Seconds between hardware serial polls pushing to serial queue
MESSAGE_PARSE_INTERVAL = 1.0    # clearning serial queue and pushing to parse queue and updating UI data set
MESSAGE_BROKER_INTERVAL = 1.0   # clearing parse queue and pushing to DB and file queues
DB_LOCAL_WORKER_INTERVAL = 5.0        # Seconds between write attempts to the DBs 
DB_CLOUD_WORKER_INTERVAL = 15.0
UI_REFRESH_INTERVAL = 1.0       # Seconds wait between updating display from UI data
GRAPH_UPDATE_INTERVAL = 120.0   # Seconds - to re-request graph data from DB and update
CLOCK_UPDATE_INTERVAL = 1.0     # Literally the clock display update

MYSQL_POLL_INTERVAL = 3.0        # If enabled, how often UI data set is refreshed from the DB

INTERNET_PC = 0
RPI_CONTROLLER = 1
SERIAL_MONITOR = 2

GLOBAL_CONFIG = INTERNET_PC

if GLOBAL_CONFIG == INTERNET_PC:
    PROCESS_SERIAL_MESSAGES = False
    UI_UPDATE_FROM_MESSAGES = False
    UI_UPDATE_FROM_LOCAL_DB = False
    UI_UPDATE_FROM_CLOUD_DB = True
    GRAPH_UPDATE_FROM_LOCAL_DB = False
    GRAPH_UPDATE_FROM_CLOUD_DB = True
    DB_WRITE_LOCAL = False
    DB_WRITE_CLOUD = False
    PRINT_MESSAGES = True
    DB_CONFIG_FILE = '/home/brendan/controller_config.ini'

if GLOBAL_CONFIG == RPI_CONTROLLER:
    PROCESS_SERIAL_MESSAGES = True
    UI_UPDATE_FROM_MESSAGES = True
    UI_UPDATE_FROM_LOCAL_DB = False
    UI_UPDATE_FROM_CLOUD_DB = False
    GRAPH_UPDATE_FROM_LOCAL_DB = True
    GRAPH_UPDATE_FROM_CLOUD_DB = False
    DB_WRITE_LOCAL = True
    DB_WRITE_CLOUD = True
    PRINT_MESSAGES = False
    DB_CONFIG_FILE = '/root/controller_config.ini'
    
if GLOBAL_CONFIG == SERIAL_MONITOR:
    PROCESS_SERIAL_MESSAGES = True
    UI_UPDATE_FROM_MESSAGES = True
    UI_UPDATE_FROM_LOCAL_DB = False
    UI_UPDATE_FROM_CLOUD_DB = False
    GRAPH_UPDATE_FROM_LOCAL_DB = True
    GRAPH_UPDATE_FROM_CLOUD_DB = False
    DB_WRITE_LOCAL = True
    DB_WRITE_CLOUD = False
    PRINT_MESSAGES = True
    DB_CONFIG_FILE = '/home/brendan/controller_config.ini'


DB_LOCAL_CONFIG = 'mysql.local'
#DB_CLOUD_CONFIG = 'mysql.cloud'
DB_CLOUD_CONFIG = 'mysql.wusdutu'


DB_CLOUD_QUEUE_TRIGGER_LEN = 1
DB_CLOUD_QUEUE_MAX_LEN = 10000   # should be larger than local
DB_LOCAL_QUEUE_TRIGGER_LEN = 1
DB_LOCAL_QUEUE_MAX_LEN = 1000
PARSED_MESSAGE_QUEUE_MAX_LEN = 2000
SERIAL_MESSAGE_QUEUE_MAX_LEN = 2000

DEBUG_WIDGET = False
ERROR_REPEAT_WINDOW = 3

#########################################################

STD_HEAT_BACK_PALLET = (
    [-50.00, 11.99, FF_FREEZE_BLUE_LG ],       # Freeze Blue
    [11.99, 17.99, FF_COLD_BLUE_LG ],    # Cold Blue
    [17.99, 22.00, FF_GREEN_LG ],     # green - good
    [22.00, 25.00, FF_YELLOW_LG ],    # yellow - warm
    [25.00, 100.00, FF_RED_LG ]     # red - hot
)
    
CABINET_HEAT_BACK_PALLET = (
    [-50, -4, FF_FREEZE_BLUE_LG ],       # Freeze Blue
    [-4, 5, FF_COLD_BLUE_LG ],    # Cold Blue
    [5, 50, FF_GREEN_LG ],     # green - good
    [50, 60, FF_YELLOW_LG ],    # yellow - warm
    [60, 100, FF_RED_LG ]     # red - hot
)

STD_HEAT_FORE_PALLET = (
    [-50, 12, FF_WHITE_HG ],       # Freeze Blue
    [12, 19, FF_WHITE_HG ],    # Cold Blue
    [19, 25, FF_WHITE_HG ],     # green - good
    [25, 30, FF_WHITE_HG ],    # yellow - warm
    [30, 100, FF_WHITE_HG ]     # red - hot
)
    
CABINET_HEAT_FORE_PALLET = (
    [-50, -4, FF_WHITE_HG ],       # Freeze Blue
    [-4, 5, FF_WHITE_HG ],    # Cold Blue
    [5, 50, FF_WHITE_HG ],     # green - good
    [50, 60, FF_WHITE_HG ],    # yellow - warm
    [60, 100, FF_WHITE_HG ]     # red - hot
)
    
STATE_UNKNOWN = 2
STATE_ON = 1
STATE_OFF = 0

# Global Variables

active_rules = []

ui_inputs_values = [
    FLOAT_INIT, 
    FLOAT_INIT, 
    FLOAT_INIT, 
    FLOAT_INIT, 
    FLOAT_INIT, 
    FLOAT_INIT 
]

ui_outputs_values = [ 
    STATE_UNKNOWN,
    STATE_UNKNOWN,
    STATE_UNKNOWN,
    STATE_UNKNOWN,
    STATE_UNKNOWN,
    STATE_UNKNOWN,
    STATE_UNKNOWN 
]

ui_energy_values = [
    FLOAT_INIT,
    FLOAT_INIT,
    FLOAT_INIT,
    FLOAT_INIT
]


def GetBackColorByTemp(temp_val, src_index):
    if (src_index == 4):
        for [low, high, [r, g, b, a]] in CABINET_HEAT_BACK_PALLET:
            if ((temp_val > low) and (temp_val <= high)):
                #print (temp_val)
                #print(low_line, high_line, [r, g, b, a])
                return [r, g, b, a]
    else:
        for [low, high, [r, g, b, a]] in STD_HEAT_BACK_PALLET:
            if ((temp_val > low) and (temp_val <= high)):
                #print (temp_val)
                #print(low_line, high_line, [r, g, b, a])
                return [r, g, b, a]
    return [1, 1, 1, 1]

def GetForeColorByTemp(temp_val, src_index):
    if (src_index == 4):
        for [low, high, [r, g, b, a]] in CABINET_HEAT_FORE_PALLET:
            if ((temp_val > low) and (temp_val <= high)):
                #print (temp_val)
                #print(low_line, high_line, [r, g, b, a])
                return [r, g, b, a]
    else:
        for [low, high, [r, g, b, a]] in STD_HEAT_FORE_PALLET:
            if ((temp_val > low) and (temp_val <= high)):
                #print (temp_val)
                #print(low_line, high_line, [r, g, b, a])
                return [r, g, b, a]
    return [1, 1, 1, 1]

class ParsedMessage():
    def __init__(self, **kwargs):
        super(ParsedMessage, self).__init__(**kwargs)
        self.raw_message_string = ""
        self.date = date.today()
        self.time = time()
        self.dt = datetime(1980,1,1)
        self.source_block_string = ""
        self.dest_block_string = ""
        self.msg_type_string = ""
        self.msg_string = ""
        self.int_val = UINT16_INIT
        self.float_val = FLOAT_INIT
        self.valid = False
        self.time_rx = datetime.now()


