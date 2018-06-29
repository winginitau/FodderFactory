
'''
    Fodder Factory UI Controller
    
    For Raspberry Pi Running rasbian and RPI Touch Screen 
    
    Copyright Brendan McLearie 2018 
'''

import serial
from collections import deque

import kivy
from kivy.uix.label import Label
from kivy.uix.boxlayout import BoxLayout
#from kivy.uix.anchorlayout import AnchorLayout
from kivy.uix.gridlayout import GridLayout
kivy.require('1.10.0') # replace with your current kivy version !
from kivy.config import Config
from kivy.app import App
from kivy.uix.button import Button
from kivy.clock import Clock
from kivy.uix.screenmanager import ScreenManager, Screen
from kivy.graphics import Rectangle

#from kivy.uix.togglebutton import ToggleButton
#from kivy.uix.label import Label
#from kivy.uix.image import Image
#from kivy.uix.slider import Slider
#from kivy.properties import StringProperty, OptionProperty
#from kivy.properties import NumericProperty

#import datetime

from datetime import datetime, date, time, timedelta

from configparser import ConfigParser
 
#import mysql.connector
from mysql.connector import MySQLConnection, Error

from math import sin, cos

#kivy.garden.garden_system_dir = 'libs/garden'
#from kivy.garden.graph import Graph

from graph import Graph
from graph import MeshLinePlot, MeshStemPlot, LinePlot, SmoothLinePlot, ContourPlot

#from kivy.base import runTouchApp


# Constants

################################################### 
#Colours
###################################################
_FF_BLACK = [0, 0, 0]
_FF_WHITE = [255, 255, 255]
_FF_RED = [196, 3, 51]
_FF_GREEN = [0, 158, 107]
_FF_BLUE = [0, 135, 189]
_FF_COLD_BLUE = [153, 228, 255]
_FF_FREEZE_BLUE = [204, 241, 255]
_FF_YELLOW = [255, 211, 1]
_FF_GREY = [128, 127, 128]
_FF_SAND = [237, 235, 230]
_FF_SLATE = [69, 72, 90]

L_G = 0.2
H_G = 0.8

FF_BLACK = [x/255 for x in _FF_BLACK]
FF_BLACK_LG = [x/255 for x in _FF_BLACK]
FF_BLACK_HG = [x/255 for x in _FF_BLACK]
FF_BLACK_LG.append(L_G)
FF_BLACK_HG.append(H_G)
FF_BLACK.append(1.0)

FF_WHITE = [x/255 for x in _FF_WHITE]
FF_WHITE_LG = [x/255 for x in _FF_WHITE]
FF_WHITE_HG = [x/255 for x in _FF_WHITE]
FF_WHITE_LG.append(L_G)
FF_WHITE_HG.append(H_G)
FF_WHITE.append(1.0)

FF_RED = [x/255 for x in _FF_RED]
FF_RED_LG = [x/255 for x in _FF_RED]
FF_RED_HG = [x/255 for x in _FF_RED]
FF_RED_LG.append(L_G)
FF_RED_HG.append(H_G)
FF_RED.append(1.0)

FF_GREEN = [x/255 for x in _FF_GREEN]
FF_GREEN_LG = [x/255 for x in _FF_GREEN]
FF_GREEN_HG = [x/255 for x in _FF_GREEN]
FF_GREEN_LG.append(L_G)
FF_GREEN_HG.append(H_G)
FF_GREEN.append(1.0)

FF_BLUE = [x/255 for x in _FF_BLUE]
FF_BLUE_LG = [x/255 for x in _FF_BLUE]
FF_BLUE_HG = [x/255 for x in _FF_BLUE]
FF_BLUE_LG.append(L_G)
FF_BLUE_HG.append(H_G)
FF_BLUE.append(1.0)

FF_COLD_BLUE = [x/255 for x in _FF_COLD_BLUE]
FF_COLD_BLUE_LG = [x/255 for x in _FF_COLD_BLUE]
FF_COLD_BLUE_HG = [x/255 for x in _FF_COLD_BLUE]
FF_COLD_BLUE_LG.append(L_G)
FF_COLD_BLUE_HG.append(H_G)
FF_COLD_BLUE.append(1.0)

FF_FREEZE_BLUE = [x/255 for x in _FF_FREEZE_BLUE]
FF_FREEZE_BLUE_LG = [x/255 for x in _FF_FREEZE_BLUE]
FF_FREEZE_BLUE_HG = [x/255 for x in _FF_FREEZE_BLUE]
FF_FREEZE_BLUE_LG.append(L_G)
FF_FREEZE_BLUE_HG.append(H_G)
FF_FREEZE_BLUE.append(1.0)

FF_YELLOW = [x/255 for x in _FF_YELLOW]
FF_YELLOW_LG = [x/255 for x in _FF_YELLOW]
FF_YELLOW_HG = [x/255 for x in _FF_YELLOW]
FF_YELLOW_LG.append(L_G)
FF_YELLOW_HG.append(H_G)
FF_YELLOW.append(1.0)

FF_GREY = [x/255 for x in _FF_GREY]
FF_GREY_LG = [x/255 for x in _FF_GREY]
FF_GREY_HG = [x/255 for x in _FF_GREY]
FF_GREY_LG.append(L_G)
FF_GREY_HG.append(H_G)
FF_GREY.append(1.0)

FF_SAND = [x/255 for x in _FF_SAND]
FF_SAND_LG = [x/255 for x in _FF_SAND]
FF_SAND_HG = [x/255 for x in _FF_SAND]
FF_SAND_LG.append(L_G)
FF_SAND_HG.append(H_G)
FF_SAND.append(1.0)

FF_SLATE = [x/255 for x in _FF_SLATE]
FF_SLATE_LG = [x/255 for x in _FF_SLATE]
FF_SLATE_HG = [x/255 for x in _FF_SLATE]
FF_SLATE_LG.append(L_G)
FF_SLATE_HG.append(H_G)
FF_SLATE.append(1.0)


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
    ["IN_CABINET_TEMP", 4, 50],
    ["IN_LONGRUN_TEMP", 4, 22]
)

ENERGY_LIST = (
    ["VE_DATA_SOC", "State of Charge"],
    ["VE_DATA_VOLTAGE", "Voltage"], 
    ["VE_DATA_POWER", "Power"],
    ["VE_DATA_CURRENT", "Current"],
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
UI_UPDATE_INTERVAL = 1.0            # Seconds

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

message_line_buffer = deque()

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
    
last_message_time = datetime.now()
message_ever_received = False
db_result_list = []

     
#i_energy_soc = UINT16_INIT
#energy_soc = FLOAT_INIT
#energy_voltage = FLOAT_INIT
#energy_power = FLOAT_INIT
#energy_current = FLOAT_INIT


# Global Functions

def iter_row(cursor, size=10):
    while True:
        rows = cursor.fetchmany(size)
        if not rows:
            break
        for row in rows:
            yield row

def db_energy_data(energy_message, endDT=datetime.now(), periodTD=timedelta(days=3)):    
    global db_result_list
    try:
        dbconfig = read_db_config()
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
 
        startDT = endDT - periodTD
        
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        endDTstr = endDT.strftime("%Y-%m-%d %H:%M:%S")
        
        query = "SELECT datetime, int_val from message_log " \
                "WHERE source = 'system' " \
                "AND msg_type = 'DATA' " \
                "AND message = %s " \
                "AND datetime BETWEEN %s and %s " \
                "ORDER BY datetime"
                #"and time <= '00:07:00'"
            
        #args = (block_label,)
        args = (energy_message, startDTstr, endDTstr)
        
        cursor.execute(query, args)

        db_result_list = []
        for row in iter_row(cursor, 10):
            #print (row)
            td = row[0] - startDT
            tds = td.total_seconds()
            tdsi = int(tds)
            if energy_message == "VE_DATA_VOLTAGE":
                db_result_list.append( (tdsi, row[1]/100, ) )
            elif energy_message == "VE_DATA_CURRENT":
                db_result_list.append( (tdsi, row[1]/100, ) )
            else:
                db_result_list.append( (tdsi, row[1], ) )
                                       
        #print(db_result_list)
 
    except Error as e:
        print(e)
 
    finally:
        cursor.close()
        conn.close()


def db_last24_device_by_block_label(block_label):
    global db_result_list
    try:
        dbconfig = read_db_config()
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
 
        period = timedelta(hours=24)
        nowDT = datetime.now()
        startDT = nowDT - period
        
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        nowDTstr = nowDT.strftime("%Y-%m-%d %H:%M:%S")
        
        query = "SELECT datetime, msg_type from message_log " \
                "WHERE source = %s " \
                "AND datetime BETWEEN %s and %s " \
                "ORDER BY datetime"
                #"and time <= '00:07:00'"
            
        #args = (block_label,)
        args = (block_label, startDTstr, nowDTstr)
        
        cursor.execute(query, args)

        db_result_list = []
        for row in iter_row(cursor, 10):
            #print (row)
            td = row[0] - startDT
            tds = td.total_seconds()
            tdsi = int(tds)
            if row[1] == "ACTIVATED":
                db_result_list.append( (tdsi, 0, ) )
                db_result_list.append( (tdsi, 1, ) )
            if row[1] == "DEACTIVATED":
                db_result_list.append( (tdsi, 1, ) )
                db_result_list.append( (tdsi, 0, ) )
                                       
        #print(db_result_list)
 
    except Error as e:
        print(e)
 
    finally:
        cursor.close()
        conn.close()


def db_last24_temps_by_block_label(block_label):
    
    global db_result_list
    try:
        dbconfig = read_db_config()
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
 
        period = timedelta(hours=24)
        nowDT = datetime.now()
        startDT = nowDT - period
        
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        nowDTstr = nowDT.strftime("%Y-%m-%d %H:%M:%S")
        
        query = "SELECT datetime, float_val from message_log " \
                "WHERE source = %s " \
                "AND datetime BETWEEN %s and %s " \
                "ORDER BY datetime"
                #"and time <= '00:07:00'"
            
        #args = (block_label,)
        args = (block_label, startDTstr, nowDTstr)
        
        cursor.execute(query, args)

        db_result_list = []
        for row in iter_row(cursor, 10):
            #print (row)
            td = row[0] - startDT
            tds = td.total_seconds()
            tdsi = int(tds)
            db_result_list.append( (tdsi, row[1]), )
        #print(db_result_list)
 
    except Error as e:
        print(e)
 
    finally:
        cursor.close()
        conn.close()
        
def db_add_log_entry(parsed_message):
    
    log_data = {
        'datetime': parsed_message.dt.strftime("%Y-%m-%d %H:%M:%S"),
        'date': parsed_message.dt.strftime("%Y-%m-%d"),
        'time': parsed_message.dt.strftime("%H:%M:%S"),
        'source': parsed_message.source_block_string,
        'destination': parsed_message.dest_block_string,
        'msg_type': parsed_message.msg_type_string,
        'message': parsed_message.msg_string,
        'int_val': parsed_message.int_val,
        'float_val': parsed_message.float_val,
        }

    query = "INSERT INTO message_log " \
            "(datetime, date, time, source, destination, msg_type, message, int_val, float_val) " \
            "VALUES" \
            "(%(datetime)s, %(date)s, %(time)s, %(source)s,%(destination)s, " \
            "%(msg_type)s, %(message)s, %(int_val)s, %(float_val)s)"

    try:
        db_config = read_db_config()
        conn = MySQLConnection(**db_config) 
        cursor = conn.cursor()
        cursor.execute(query, log_data)
        conn.commit()
    except Error as error:
        print(error)
 
    finally:
        cursor.close()
        conn.close()

def read_db_config(filename='controller_config.ini', section='mysql'):
    """ Read database configuration file and return a dictionary object
    :param filename: name of the configuration file
    :param section: section of database configuration
    :return: a dictionary of database parameters
    """
    # create parser and read ini configuration file
    parser = ConfigParser()
    parser.read(filename)
 
    # get section, default to mysql
    db = {}
    if parser.has_section(section):
        items = parser.items(section)
        for item in items:
            db[item[0]] = item[1]
    else:
        raise Exception('{0} not found in the {1} file'.format(section, filename))
 
    return db

def DBConnectTest():
    """ Connect to MySQL database """
 
    db_config = read_db_config()
 
    try:
        print('Connecting to MySQL database...')
        conn = MySQLConnection(**db_config)
 
        if conn.is_connected():
            print('Database Connection Tested OK')
        else:
            print('Database Connection Failed.')
            exit()
 
    except Error as error:
        print(error)
 
    finally:
        conn.close()
        #print('Connection closed.')


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
    def __init__(self):
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
        
def parse_message_string(raw_msg_string):
#    print(raw_msg_string)
    parsed_message = ParsedMessage()
    parsed_message.valid = False
    good_length = False
    good_decode = False
    good_dt = False
    
    #print (raw_msg_string)

    if len(raw_msg_string) > 30:
        good_length = True
        try:
            string_list = raw_msg_string.decode().split(",")
            good_decode = True
        except:
            print("Bad message decode")
            
        if (good_decode):
            try:
                message_dt = datetime.strptime((string_list[0] + " " + string_list[1]), '%Y-%m-%d %H:%M:%S')
                good_dt = True
            except:
                good_dt = False
                print ("Bad date or time format in message")
                print (string_list[0] + " " + string_list[1])
                 
    if (good_length and good_decode and good_dt):
        if not("DEBUG" in string_list):
            parsed_message.dt = message_dt
            parsed_message.source_block_string = string_list[2]
            parsed_message.dest_block_string = string_list[3]
            parsed_message.msg_type_string = string_list[4]
            parsed_message.msg_string = string_list[5]
            parsed_message.int_val = int(string_list[6])
            parsed_message.float_val = float(string_list[7])
            parsed_message.valid = True
            parsed_message.time_rx = datetime.now()
            with open(MESSAGE_FILENAME, "a") as msg_log_file:
                print(raw_msg_string, file=msg_log_file)
            print(parsed_message.dt, \
                  parsed_message.source_block_string, parsed_message.dest_block_string, \
                  parsed_message.msg_type_string, parsed_message.msg_string, \
                  parsed_message.int_val, parsed_message.float_val)
             
            db_add_log_entry(parsed_message)    
            #db_add_log_entry(message_date, message_time, \
            #             parsed_message.source_block_string, parsed_message.dest_block_string, \
            #             parsed_message.msg_type_string, parsed_message.msg_string, \
            #             parsed_message.int_val, parsed_message.float_val)
                
    return parsed_message


def parse_and_update(dt):
    global message_ever_received
    global last_message_time

#    print("*******Callback parse_and_update called by clock *****************")
    parsed_message = ParsedMessage()
    while (len(message_line_buffer) > 0):
        parsed_message = parse_message_string(message_line_buffer.popleft())
        if (parsed_message.valid == True):
            for i, source, disp in INPUTS_LIST:
                if (parsed_message.source_block_string == source):
                    ui_inputs_values[i] = parsed_message.float_val
            for i, source, disp in OUTPUTS_LIST:
                if (parsed_message.source_block_string == source):
                    if (parsed_message.msg_type_string == "ACTIVATED"):        
                        ui_outputs_values[i] = STATE_ON
                    elif (parsed_message.msg_type_string == "DEACTIVATED"):        
                        ui_outputs_values[i] = STATE_OFF
            if (parsed_message.msg_string == "VE_DATA_SOC"):
                ui_energy_values[0] = float(parsed_message.int_val) / 10
            if (parsed_message.msg_string == "VE_DATA_VOLTAGE"):
                ui_energy_values[1] = float(parsed_message.int_val) / 1000
            if (parsed_message.msg_string == "VE_DATA_POWER"):
                ui_energy_values[2] = float(parsed_message.int_val)
            if (parsed_message.msg_string == "VE_DATA_CURRENT"):
                ui_energy_values[3] = float(parsed_message.int_val) / 1000
            last_message_time = datetime.now()
            message_ever_received = True
            

def serial_read_line(dt):
        #print("********RadioModem method read_line called***********")
        if (radio_modem_port.inWaiting() > 0):
            message_line_buffer.append(radio_modem_port.readline())
#            print(message_line_buffer)

class BasicLabel(Label):
    def __init__(self, **kwargs):
        super(BasicLabel, self).__init__(**kwargs)
        #self.text_size = self.size
        self.halign = "center"
        self.valign = "middle"
        self.background_normal = ''
        #self.background_color = FF_GREY
        self.font_size = '28sp'
        #self.spacing = 10
        #self.padding = (10, 10)   
        #self.color = FF_WHITE
        self.name = ''
        self.text = self.name    
        
        self.background_color = FF_GREY_HG
        self.color = FF_WHITE
        self.text = "\n[b]" + self.text + "[/b]"
    def set_name(self, name_str):
        self.name = name_str
        self.text = name_str    


class BasicButton(Button):
    def __init__(self, **kwargs):
        super(BasicButton, self).__init__(**kwargs)
        #self.text_size = self.size
        self.halign = "center"
        self.valign = "middle"
        self.background_normal = ''
        #self.size_hint_max_y = 80
        #self.size_hint_y = 0.2
        #self.background_color = FF_GREY
        self.font_size = '28sp'
        #self.spacing = 10
        #self.padding = (10, 10)   
        #self.color = FF_WHITE
        self.name = ''
        self.text = self.name    
        
        self.background_color = FF_GREY_LG
        self.color = FF_WHITE_LG
        #self.text = "\n[b]" + self.text + "[/b]"
        self.text = ""
    def set_name(self, name_str):
        self.name = name_str
        self.text = name_str    

class NavigationButton(BasicButton):
    def __init__(self, **kwargs):
        super(NavigationButton, self).__init__(**kwargs)
        self.background_color = FF_BLUE_LG
        self.color = FF_WHITE_HG

class NavCloseToMainButton(NavigationButton):
    def __init__(self, **kwargs):
        super(NavCloseToMainButton, self).__init__(**kwargs)
    def on_press(self):
        sm.current = "main"

class NavNextButton(NavigationButton):
    def __init__(self, **kwargs):
        super(NavNextButton, self).__init__(**kwargs)
    def on_press(self):
        sm.current = sm.next()
        #sm.switch_to(MainScreen())

class NavPrevButton(NavigationButton):
    def __init__(self, **kwargs):
        super(NavPrevButton, self).__init__(**kwargs)
    def on_press(self):
        sm.current = sm.previous()
        
class MainDisplayButton(BasicButton):
    pass        

class FFTempButton(MainDisplayButton):
    def __init__(self, **kwargs):
        super(FFTempButton, self).__init__(**kwargs)
        self.temperature = 255.255
        #self.size_hint_max_y = 200
        self.source_index = 255
    def on_press(self):
        sm.current = self.name
        
    def set_source_by_index(self, idx):
        self.source_index = idx                    
            
    def set_source_by_disp_name(self, disp_name):
        for i, src, disp in INPUTS_LIST:
            if disp == disp_name:
                self.source_index = i

    def set_source_by_source_name(self, source_name):
        for i, src, disp in INPUTS_LIST:
            if src == source_name:
                self.source_index = i        
        
    def update(self, dt):                   # dt passed in kivy callback - not used
        self.temperature = ui_inputs_values[self.source_index]
        if (self.temperature != FLOAT_INIT):
            #self.background_normal = ''
            self.background_color = GetBackColorByTemp(self.temperature, self.source_index)
            self.color = GetForeColorByTemp(self.temperature, self.source_index)
        else:
            self.background_color = FF_GREY_LG
            self.color = FF_WHITE_LG            
        self.text = self.name + "\n[b]" + str(self.temperature) + "[/b]"
        
class FFDeviceButton(MainDisplayButton):
    def __init__(self, **kwargs):
        super(FFDeviceButton, self).__init__(**kwargs)
        self.FF_state = STATE_UNKNOWN

    def on_press(self):
        sm.current = self.name

    def set_source_by_index(self, idx):
        self.source_index = idx                    
    
    def set_source_by_disp_name(self, disp_name):
        for i, src, disp in OUTPUTS_LIST:
            if disp == disp_name:
                self.source_index = i

    def set_source_by_source_name(self, source_name):
        for i, src, disp in OUTPUTS_LIST:
            if src == source_name:
                self.source_index = i

    def update(self, dt):                   # dt passed in kivy callback - not used
        self.FF_state = ui_outputs_values[self.source_index]
        #self.background_color = GetBackColorByTemp(self.temperature)
        if (self.FF_state == STATE_UNKNOWN):
            self.state_str = "\nState Unknown"
            self.background_color = FF_GREY_LG
            self.color = FF_WHITE_LG            
        elif (self.FF_state == STATE_OFF):
            self.state_str = "\n[b]OFF[/b]"
            #self.background_normal = ""
            self.background_color = FF_SLATE_LG
            self.color = FF_WHITE_HG
        elif (self.FF_state == STATE_ON):
            self.state_str = "\n[b]ON[/b]"
            #self.background_normal = ""
            self.background_color = FF_GREEN_LG
            self.color = FF_WHITE_HG
        self.text = self.name + self.state_str

class FFEnergyButton(MainDisplayButton):
    def __init__(self, **kwargs):
        super(FFEnergyButton, self).__init__(**kwargs)

    def on_press(self):
        sm.current = "State of Charge"
    
    def update(self, dt):                   # dt passed in kivy callback - not used      
        self.soc = ui_energy_values[0]
        self.voltage = ui_energy_values[1]
        self.power = ui_energy_values[2]
        self.current = ui_energy_values[3]
        
        if (self.soc == 0):
            self.background_color = FF_GREY_LG
            self.color = FF_WHITE_LG
        elif (self.soc > 101):
            self.background_color = FF_GREY_LG
            self.color = FF_WHITE_LG
        elif (self.soc > 70):
            self.background_color = FF_GREEN_LG
            self.color = FF_WHITE_HG
        elif (self.soc > 60):
            self.background_color = FF_YELLOW_LG
            self.color = FF_WHITE
        elif (self.soc > 50):
            self.background_color = FF_RED_LG
            self.color = FF_WHITE
        
        self.text = "[b]Energy    "+"{:.1f}".format(self.soc)+" %     " + \
                    "{:.2f}".format(self.voltage)+" V     " + \
                    "{:.2f}".format(self.current)+" A     " + \
                    "{:.0f}".format(self.power) + " W [/b]"

class TimeButton(MainDisplayButton):
    def __init__(self, **kwargs):
        super(TimeButton, self).__init__(**kwargs)
        self.background_color = FF_SLATE_LG
        self.color = FF_WHITE_HG
        self.text = "[b]--:--:--[/b]"
                    
    def set_name(self, name_str):
        self.name = name_str
    def update(self, dt):
        #self.currentDT = "{0:%H:%M:%S  %d-%m-%Y}".format(datetime.datetime.now())
        self.currentDT = "{0:%H:%M:%S}".format(datetime.now())
        #self.text = str(self.currentDT.hour) + ":" + \
        #            str(self.currentDT.minute) + ":" + \
        #            str(self.currentDT.second)
        #self.text = "[b]Time: " + str(self.currentDT) + " [/b]" 
        self.text = "[b]" + self.currentDT + "[/b]"          

class MessageIndicatorButton(MainDisplayButton):
    def __init__(self, **kwargs):
        super(MessageIndicatorButton, self).__init__(**kwargs)
        self.background_color = FF_YELLOW_LG
        self.color = FF_WHITE_HG
        self.text = "[b]Lights on, but no one home[/b]"
        self.nowDT = datetime.now()
        self.delta = timedelta()
    def set_name(self, name_str):
        self.name = name_str
        
    def update(self, dt):
        global message_ever_received
        global last_message_time
        if (message_ever_received):
            self.nowDT = datetime.now()
            self.delta = self.nowDT - last_message_time
            self.seconds = self.delta.total_seconds()
            if (self.seconds > 3600):
                self.text = "[b]>60min Rooted[/b]"
                self.background_color = FF_RED_LG
            elif (self.seconds > 1800):
                self.text = "[b]>30min Really Broken[/b]"
                self.background_color = FF_RED_LG
            elif (self.seconds > 900):
                self.text = "[b]>15min Yep Broken[/b]"
                self.background_color = FF_RED_LG
            elif (self.seconds > 600):
                self.text = "[b]>10min Probably Broken[/b]"
                self.background_color = FF_RED_LG
            elif (self.seconds > 300):
                self.text = "[b]5 mins - Broken?[/b]"
                self.background_color = FF_YELLOW_LG
            elif (self.seconds > 210):
                self.text = "[b]A few minutes ago[/b]"
                self.background_color = FF_YELLOW_LG
            elif (self.seconds > 80):
                self.text = "[b]A couple of minutes ago[/b]"
                self.background_color = FF_GREEN_LG
            elif (self.seconds > 50):
                self.text = "[b]About a minute ago[/b]"
                self.background_color = FF_GREEN_LG
            elif (self.seconds > 30):
                self.text = "[b]Less than a minute ago[/b]"
                self.background_color = FF_GREEN_LG
            elif (self.seconds > 10):
                self.text = "[b]A few seconds ago[/b]"
                self.background_color = FF_GREEN_LG
            else: 
                self.text = "[b]Right Now[/b]"
                self.background_color = FF_GREEN_LG

            
class MainTopGrid(GridLayout):
    def __init__(self, **kwargs):
        super(MainTopGrid, self).__init__(**kwargs)
        self.cols = 3
        self.spacing = 15
        self.padding = 15
        #self.row_default_height = 30
        #self.size_hint_y = 400
        #self.height = 400
        #self.width = 800
        self.size_hint_min_y = 350
        self.input_buttons = []
        self.output_buttons = []
                
        # add the input buttons
        for i, l, d in INPUTS_LIST:
            self.input_buttons.append(FFTempButton(markup=True))
            self.input_buttons[i].set_name(d)
            self.input_buttons[i].set_source_by_index(i)
            self.add_widget(self.input_buttons[i])
            Clock.schedule_interval(self.input_buttons[i].update, UI_UPDATE_INTERVAL)
        
        # add the device buttons
        for i, l, d in OUTPUTS_LIST:
            self.output_buttons.append(FFDeviceButton(markup=True))
            self.output_buttons[i].set_name(d)
            self.output_buttons[i].set_source_by_index(i)
            self.add_widget(self.output_buttons[i])
            Clock.schedule_interval(self.output_buttons[i].update, UI_UPDATE_INTERVAL)

class MainMiddleGrid(GridLayout):
    def __init__(self, **kwargs):
        super(MainMiddleGrid, self).__init__(**kwargs)
        self.cols = 1
        self.spacing = 15
        self.padding = [15, 0]
        
        self.energy_button = FFEnergyButton(markup=True)
        self.energy_button.set_name("Energy")
        self.add_widget(self.energy_button)
        
        Clock.schedule_interval(self.energy_button.update, UI_UPDATE_INTERVAL)

class MainBottomGrid(GridLayout):
    def __init__(self, **kwargs):
        super(MainBottomGrid, self).__init__(**kwargs)
        self.cols = 3
        self.spacing = 15
        self.padding = [15, 15, 15, 0]
                
        self.current_time = TimeButton(markup=True)
        self.current_time.set_name("TimeNow")
        self.add_widget(self.current_time)
        
        self.last_rx_status = MessageIndicatorButton(markup=True)
        self.last_rx_status.set_name("TimeRxStatus")
        self.add_widget(self.last_rx_status)
        
        Clock.schedule_interval(self.current_time.update, UI_UPDATE_INTERVAL)
        Clock.schedule_interval(self.last_rx_status.update, UI_UPDATE_INTERVAL)

class MainParentGrid(GridLayout):
    def __init__(self, **kwargs):
        super(MainParentGrid, self).__init__(**kwargs)
        self.cols = 1
        with self.canvas.before:
            self.rect = Rectangle(size=(800, 480), pos=self.pos, source='cows.png')

        self.temperature_section = MainTopGrid()
        self.energy_section = MainMiddleGrid()
        self.status_section = MainBottomGrid()
        
        self.add_widget(self.temperature_section)
        self.add_widget(self.energy_section)
        self.add_widget(self.status_section)
        

#### NOT USED ##############
class ZoomHeaderGrid(GridLayout):
    def __init__(self, **kwargs):
        super(ZoomHeaderGrid, self).__init__(**kwargs)
        self.cols = 3
        self.rows = 1
        self.spacing = 5
        self.padding = [5, 5, 5, 0]
        self.anchor_y = 'top'

        self.size_hint_max_y = 80

def calc_temp_graph_y_grid(t_min, t_max, base):
    
    #start by rounding to the nearest multiple of base
    graph_min = int(base * round(float(t_min)/base))
    graph_max = int(base * round(float(t_max)/base))

    # add margins if necessary
    if t_max >= graph_max:
            graph_max = graph_max + base
    if t_min <= graph_min:
        graph_min = graph_min - base
        
    return graph_min, graph_max

def get_temp_graph_low_high(block_str):
    for b, l, h in DB_INPUT_LOW_HIGH:
        if b == block_str:
            return l, h 
    return 0,0

class ZoomTemperatureDataGrid(GridLayout):
    def __init__(self, **kwargs):
        super(ZoomTemperatureDataGrid, self).__init__(**kwargs)
        self.cols = 1
        self.spacing = 0
        self.padding = [10, 0, 10, 10]
        #self.size_hint_min_y = 200

        self.source = zoom_source
        self.type = zoom_type
        self.name = zoom_name

        self.refresh_data()
                
        self.graph = Graph(xlabel=(self.name + " - Last 24 Hours"), ylabel='Temperature', \
                           x_grid_label=True, \
                           x_grid=True, \
                           x_ticks_minor=0, \
                           x_ticks_major=3600, \
                           xmin=0, xmax=(86400 + self.x_offset), \
                           y_grid_label=True, \
                           y_grid=True, \
                           y_ticks_major=2, \
                           ymin=self.min_temp, ymax=self.max_temp, \
                           padding=5, \
                           border_color = FF_WHITE_LG, \
                           background_color = FF_BLACK_LG, \
                           _with_stencilbuffer=False)
        self.add_widget(self.graph)

        self.plot_hot = MeshLinePlot(color=FF_RED)
        self.plot_cold = MeshLinePlot(color=FF_BLUE)
        self.plot_data = MeshLinePlot(color=FF_WHITE)
        
        self.plot_hot.points = self.hot_line.copy()  
        self.plot_cold.points = self.cold_line.copy()  
        self.plot_data.points = self.data_line.copy()  
        
        self.graph.add_plot(self.plot_data)
        self.graph.add_plot(self.plot_hot)
        self.graph.add_plot(self.plot_cold)
        
        Clock.schedule_interval(self.update, 60)

    def refresh_data(self):        
        global db_result_list
        db_result_list = []
        db_last24_temps_by_block_label(self.source)
        
        self.hot_line = []
        self.cold_line = []
        self.data_line = []
        
        nowDT = datetime.now()
        now_min = nowDT.minute
        now_sec = nowDT.second
        
        self.x_offset = (now_min * 60) + now_sec
        
        self.min_temp = 80
        self.max_temp = -80
        self.low_line, self.high_line = get_temp_graph_low_high(self.source)
        
        for t, v in db_result_list:
            if v > self.max_temp:
                self.max_temp = v
            if v < self.min_temp:
                self.min_temp = v
            t_mod = t + self.x_offset
            self.data_line.append((t_mod,v))      
            self.hot_line.append((t_mod,self.high_line)) 
            self.cold_line.append((t_mod,self.low_line))
                     
        self.min_temp, self.max_temp = calc_temp_graph_y_grid(self.min_temp, self.max_temp, 2)
        
        if self.min_temp >= self.low_line: self.min_temp = self.low_line - 1
        if self.max_temp <= self.high_line: self.max_temp = self.high_line + 1

    def update(self, dt):
        #self.graph.remove_plot(self.plot)
        self.refresh_data()
        
        self.graph.xmax=(86400 + self.x_offset)
                
        self.plot_hot.points = self.hot_line.copy()  
        self.plot_cold.points = self.cold_line.copy()  
        self.plot_data.points = self.data_line.copy()   
              
class ZoomDeviceDataGrid(GridLayout):
    def __init__(self, **kwargs):
        super(ZoomDeviceDataGrid, self).__init__(**kwargs)
        self.cols = 1
        self.spacing = 0
        self.padding = [10, 0, 10, 10]
        #self.size_hint_min_y = 200

        self.source = zoom_source
        self.type = zoom_type
        self.name = zoom_name

        self.refresh_data()
                
        self.graph = Graph(xlabel=(self.name + " - Last 24 Hours"), ylabel='OFF                                                    ON', \
                           x_grid_label=True, \
                           x_grid=True, \
                           x_ticks_minor=0, \
                           x_ticks_major=3600, \
                           xmin=0, xmax=(86400 + self.x_offset), \
                           y_grid_label=True, \
                           y_grid=True, \
                           y_ticks_minor=0, \
                           y_ticks_major=0, \
                           ymin=-0.2, ymax=1.2, \
                           padding=5, \
                           border_color = FF_WHITE_LG, \
                           background_color = FF_BLACK_LG, \
                           _with_stencilbuffer=False)
        self.add_widget(self.graph)

        self.plot_data = MeshLinePlot(color=FF_WHITE)
        self.plot_data.points = self.data_line.copy()  
        
        self.graph.add_plot(self.plot_data)
        
        Clock.schedule_interval(self.update, 60)

    def refresh_data(self):        
        global db_result_list
        db_result_list = []
        db_last24_device_by_block_label(self.source)
        
        self.data_line = []
        
        nowDT = datetime.now()
        now_min = nowDT.minute
        now_sec = nowDT.second
        
        self.x_offset = (now_min * 60) + now_sec
                
        for t, v in db_result_list:
            t_mod = t + self.x_offset
            self.data_line.append((t_mod,v))      

    def update(self, dt):
        #self.graph.remove_plot(self.plot)
        self.refresh_data()
        
        self.graph.xmax=(86400 + self.x_offset)                
        self.plot_data.points = self.data_line.copy()   

class ZoomEnergyDataGrid(GridLayout):
    def __init__(self, **kwargs):
        super(ZoomEnergyDataGrid, self).__init__(**kwargs)
        self.cols = 1
        self.spacing = 0
        self.padding = [10, 0, 10, 10]
        #self.size_hint_min_y = 200

        self.source = zoom_source
        self.type = zoom_type
        self.name = zoom_name

        self.refresh_data()
                
        self.period = timedelta(days=3)
                
        self.graph = Graph(xlabel=(self.name + " - Last 3 Days"), ylabel='Energy', \
                           x_grid_label=True, \
                           x_grid=True, \
                           x_ticks_minor=0, \
                           x_ticks_major=7200, \
                           xmin=0, xmax=(self.period.total_seconds() + self.x_offset), \
                           y_grid_label=True, \
                           y_grid=True, \
                           y_ticks_minor=0, \
                           y_ticks_major=250, \
                           ymin=-500, ymax=1250, \
                           padding=5, \
                           border_color = FF_WHITE_LG, \
                           background_color = FF_BLACK_LG, \
                           _with_stencilbuffer=False)
        self.add_widget(self.graph)

        self.plot_data = MeshLinePlot(color=FF_WHITE)
        self.plot_data.points = self.data_line.copy()  
        
        self.graph.add_plot(self.plot_data)
        
        Clock.schedule_interval(self.update, 60)

    def refresh_data(self):        
        global db_result_list
        db_result_list = []
        db_energy_data(self.source) 
        
        self.data_line = []
        
        nowDT = datetime.now()
        now_min = nowDT.minute
        now_sec = nowDT.second
        
        self.x_offset = (now_min * 60) + now_sec
                
        for t, v in db_result_list:
            t_mod = t + self.x_offset
            self.data_line.append((t_mod,v))      

    def update(self, dt):
        #self.graph.remove_plot(self.plot)
        self.refresh_data()
        
        self.graph.xmax=(self.period.total_seconds() + self.x_offset)                
        self.plot_data.points = self.data_line.copy()   


class ZoomNavigationGrid(GridLayout):
    def __init__(self, **kwargs):
        super(ZoomNavigationGrid, self).__init__(**kwargs)
        self.cols = 4
        self.spacing = 10
        self.padding = [10, 10, 10, 10]
        self.size_hint_max_y = 80
        
        self.prev_button = NavPrevButton()
        self.prev_button.set_name('<< Previous')
        self.add_widget(self.prev_button)

        if zoom_type == 'INPUT':
            self.zoom_button = FFTempButton(markup=True)
            self.zoom_button.set_source_by_disp_name(zoom_name)
        elif zoom_type == 'OUTPUT':
            self.zoom_button = FFDeviceButton(markup=True)
            self.zoom_button.set_source_by_disp_name(zoom_name)
        elif zoom_type == "ENERGY":
            self.zoom_button = FFEnergyButton(markup=True)
            self.zoom_button.text = "ENERGY"
            
        self.zoom_button.set_name(zoom_name)
        self.add_widget(self.zoom_button)
        Clock.schedule_interval(self.zoom_button.update, UI_UPDATE_INTERVAL)
                                           
        self.close_button = NavCloseToMainButton()
        self.close_button.set_name('Close')
        self.add_widget(self.close_button)

        self.next_button = NavNextButton()
        self.next_button.set_name('Next >>')
        self.add_widget(self.next_button)

class ZoomParentGrid(GridLayout):
    def __init__(self, **kwargs):
        super(ZoomParentGrid, self).__init__(**kwargs)
        self.cols = 1
        #self.rows = 1
        self.spacing = 0
        self.padding = [0, 0, 0, 0]
     
        with self.canvas.before:
            self.rect = Rectangle(size=(800, 480), pos=self.pos, source='cows.png')

        #self.title_section = ZoomHeaderGrid()
        self.zoom_type = zoom_type
        if self.zoom_type == "INPUT":
            self.data_section = ZoomTemperatureDataGrid()
        elif self.zoom_type == "OUTPUT":
            self.data_section = ZoomDeviceDataGrid()
        elif self.zoom_type == "ENERGY":
            self.data_section = ZoomEnergyDataGrid()
        self.status_section = ZoomNavigationGrid()
        
        #self.add_widget(self.title_section)
        self.add_widget(self.status_section)
        self.add_widget(self.data_section) 
       
# Declare screens
class MainScreen(Screen):
    def __init__(self, **kwargs):
        super(MainScreen, self).__init__(**kwargs)
        self.screen_layout = MainParentGrid()
        self.add_widget(self.screen_layout)

class ZoomScreen(Screen):
    def __init__(self, **kwargs):
        super(ZoomScreen, self).__init__(**kwargs)
        #self.screen_layout = MainParentGrid()
        self.screen_layout = ZoomParentGrid()
        self.add_widget(self.screen_layout)


sm = ScreenManager()
sm.add_widget(MainScreen(name='main'))
for i, source, disp in INPUTS_LIST:
    zoom_name = disp
    zoom_source = source
    zoom_type = "INPUT"
    sm.add_widget(ZoomScreen(name=disp))
for i, source, disp in OUTPUTS_LIST:
    zoom_name = disp
    zoom_source = source
    zoom_type = "OUTPUT"
    sm.add_widget(ZoomScreen(name=disp))
for source, disp in ENERGY_LIST:
    zoom_name = disp
    zoom_source = source
    zoom_type = "ENERGY"
    sm.add_widget(ZoomScreen(name=disp))
        
         
# Top Level App                                      
class MyApp(App):

    def build(self):
        Config.set('graphics', 'width', '800')
        Config.set('graphics', 'height', '480')
        Config.write()
        #return MainTopGrid()
        #return MainParentGrid()
        return sm   

############################################################################
if __name__ == '__main__':
    # Connect to serial port first
    try:
        print("Connecting to Radio Modem")
        radio_modem_port = serial.Serial(MODEM_SERIAL_PORT, MODEM_SERIAL_SPEED)
    except:
        print ("Failed to connect to tty")
        exit()
        
    DBConnectTest()

    # Set Up Data Processing
    Clock.schedule_interval(parse_and_update, UI_UPDATE_INTERVAL)
    Clock.schedule_interval(serial_read_line, SERIAL_POLL_INTERVAL)
    print("*************SERIAL READ and PARSE CLOCKS ADDED ******")            

    # Launch the app
    MyApp().run()
    #runTouchApp(sm)

    # Close serial communication
    print("Closing Serial Port Connection")
    radio_modem_port.close()
    
    
#import RPi.GPIO as GPIO

#for now, use a global for blink speed (better implementation TBD):
#speed = 1.0

# Set up GPIO:
#beepPin = 17
#ledPin = 27
#buttonPin = 22
#flashLedPin = 10
#GPIO.setmode(GPIO.BCM)
#GPIO.setup(beepPin, GPIO.OUT)
#GPIO.output(beepPin, GPIO.LOW)
#GPIO.setup(ledPin, GPIO.OUT)
#GPIO.output(ledPin, GPIO.LOW)
#GPIO.setup(flashLedPin, GPIO.OUT)
#GPIO.output(flashLedPin, GPIO.LOW)
#GPIO.setup(buttonPin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Define some helper functions:

# This callback will be bound to the LED toggle and Beep button:
'''
def press_callback(obj):
    print("Button pressed,", obj.text)
    if obj.text == 'BEEP!':
        # turn on the beeper:
#        GPIO.output(beepPin, GPIO.HIGH)
        # schedule it to turn off:
        Clock.schedule_once(buzzer_off, .1)
    if obj.text == 'LED':
        if obj.state == "down":
            print ("button on")
#            GPIO.output(ledPin, GPIO.HIGH)
        else:
            print ("button off")
#            GPIO.output(ledPin, GPIO.LOW)

def buzzer_off(dt):
    pass
    
#    GPIO.output(beepPin, GPIO.LOW)

# Toggle the flashing LED according to the speed global
# This will need better implementation
def flash(dt):
    global speed
#    GPIO.output(flashLedPin, not GPIO.input(flashLedPin))
    Clock.schedule_once(flash, 1.0/speed)

# This is called when the slider is updated:
def update_speed(obj, value):
    global speed
    print("Updating speed to:" + str(obj.value))
    speed = obj.value

# Modify the Button Class to update according to GPIO input:
class InputButton(Button):
    def update(self, dt):
#        if GPIO.input(buttonPin) == True:
        if True:
            self.state = 'normal'
        else:
            self.state = 'down'            
'''


         
'''
class MyApp(App):

    def build(self):
        # Set up the layout:
        layout = GridLayout(cols=1, spacing=30, padding=30, row_default_height=30)

        # Make the background gray:
        with layout.canvas.before:
            Color(.2,.2,.2,1)
            self.rect = Rectangle(size=(800,600), pos=layout.pos)

        # Instantiate the first UI object (the GPIO input indicator):
        inputDisplay = InputButton(text="Input")

        # Schedule the update of the state of the GPIO input button:
        Clock.schedule_interval(inputDisplay.update, 1.0/10.0)

        # Create the rest of the UI objects (and bind them to callbacks, if necessary):
        outputControl = ToggleButton(text="LED")
        outputControl.bind(on_press=press_callback)
        
        beepButton = Button(text="BEEP!")
        beepButton.bind(on_press=press_callback)
        
        msgLabel = MessageLabel(text="INIT TEXT") 
        
        
        
        Clock.schedule_interval(msgLabel.update, serial_check_time)

        #wimg = Image(source='logo.png')
        speedSlider = Slider(orientation='vertical', min_temp=1, max_temp=30, value=speed)
        speedSlider.bind(on_touch_down=update_speed, on_touch_move=update_speed)

        # Add the UI elements to the layout:
        #layout.add_widget(wimg)
        layout.add_widget(msgLabel)
        layout.add_widget(inputDisplay)
        layout.add_widget(outputControl)
        layout.add_widget(beepButton)
        layout.add_widget(speedSlider)

        # Start flashing the LED
        Clock.schedule_once(flash, 1.0/speed)

        return layout

'''

'''
    def timer(self):
        # Get data from serial port
        value = arduino.readline()

        # Draw the circle according to the data
        self.centered_circle.circle = (self.center_x, self.center_y,value)

        # More about drawing in Kivy here: http://kivy.org/docs/api-kivy.graphics.html
'''



    
