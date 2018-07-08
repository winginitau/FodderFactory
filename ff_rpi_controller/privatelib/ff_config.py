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


#########################################################
# [Index, "SOURCE BLOCK", "Display Label"]
#########################################################
INPUTS_LIST = (
    [0, "IN_INSIDE_TOP_TEMP", "Top"],
    [1, "IN_INSIDE_BOTTOM_TEMP", "Bottom"],
    [2, "IN_OUTSIDE_TEMP", "Outside"],
    [3, "IN_WATER_TEMP", "Water"],
    [4, "IN_CABINET_TEMP", "Cabinet"],
    [5, "IN_LONGRUN_TEMP", "Main Tank"]
)

# [Index, "SOURCE BLOCK", "Display Label"]

OUTPUTS_LIST = (
    [0, "OUT_END_LIGHTS", "Lights"], 
    [1, "OUT_WATER_HEATER", "Heater"],
#    [2, "OUT_FRESH_AIR_FAN", "Inlet Fan"],
    [2, "OUT_CIRCULATION_FAN", "Circulation"],
    [3, "OUT_EXHAUST_FAN", "Exhaust"],
    [4, "OUT_WATERING_SOLENOID_TOP", "Top Spray"],
    [5, "OUT_WATERING_SOLENOID_BOTTOM", "Bottom Spray"]
)

DB_INPUT_LOW_HIGH = (
    ["IN_INSIDE_TOP_TEMP", 18, 22],
    ["IN_INSIDE_BOTTOM_TEMP", 18, 22],
    ["IN_OUTSIDE_TEMP", 18, 22],
    ["IN_WATER_TEMP", 18, 22],
    ["IN_CABINET_TEMP", 18, 22],
    ["IN_LONGRUN_TEMP", 18, 22]
)

ENERGY_LIST = (
    ["VE_DATA_SOC", "State of Charge"],
    ["VE_DATA_VOLTAGE", "Battery Voltage"], 
    ["VE_DATA_POWER", "Power Flow"],
    ["VE_DATA_CURRENT", "Battery Current"],
)

DB_ENERGY_GRAPH_PARAMS = (
    # SOURCE, Y-LABEL, Y-LOW-LINE, Y-HIGH-LINE, Y-TICS
    ["VE_DATA_SOC", "% of Full Capacity", 50, 100, 10],
    ["VE_DATA_VOLTAGE", "Volts", 23.4, 28, 1], 
    ["VE_DATA_POWER", "Watts", 0, 0, 200],
    ["VE_DATA_CURRENT", "Amps", 0, 0, 5],        
)
########################################################
INT8_INIT = -127
UINT8_INIT = 255

UINT16_INIT = 65535
FLOAT_INIT = 255.255

########################################################
#MESSAGE_FILENAME = "message_dev.log"
MESSAGE_FILENAME = "message.log"
MODEM_SERIAL_PORT = "/dev/ttyV0"
MODEM_SERIAL_SPEED = 9600

SERIAL_POLL_INTERVAL = 0.1          # Seconds
DATA_PARSE_INTERVAL = 1.0          # Seconds
GRAPH_UPDATE_INTERVAL = 5.0         #seconds

DB_WRITE_DATA = True

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
#    STATE_UNKNOWN,
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


