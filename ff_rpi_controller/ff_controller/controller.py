
'''
    Fodder Factory Controller
    
    Linux module and for Raspberry Pi
    
    Copyright Brendan McLearie 2018
    
'''

import serial
from collections import deque


import kivy
from kivy.uix.label import Label
kivy.require('1.10.0') # replace with your current kivy version !
from kivy.config import Config
from kivy.app import App
from kivy.uix.button import Button
#from kivy.uix.togglebutton import ToggleButton
#from kivy.uix.label import Label
from kivy.uix.gridlayout import GridLayout
#from kivy.uix.image import Image
#from kivy.uix.slider import Slider
from kivy.clock import Clock
from kivy.graphics import Color, Rectangle
#from kivy.properties import StringProperty, OptionProperty
from kivy.properties import NumericProperty
#BooleanProperty, ReferenceListProperty,

import datetime
 
#import copy

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
    
last_message_time = datetime.datetime.now()
message_ever_received = False
     
#i_energy_soc = UINT16_INIT
#energy_soc = FLOAT_INIT
#energy_voltage = FLOAT_INIT
#energy_power = FLOAT_INIT
#energy_current = FLOAT_INIT


# Global Functions

def GetBackColorByTemp(temp_val, src_index):
    if (src_index == 4):
        for [low, high, [r, g, b, a]] in CABINET_HEAT_BACK_PALLET:
            if ((temp_val > low) and (temp_val <= high)):
                #print (temp_val)
                #print(low, high, [r, g, b, a])
                return [r, g, b, a]
    else:
        for [low, high, [r, g, b, a]] in STD_HEAT_BACK_PALLET:
            if ((temp_val > low) and (temp_val <= high)):
                #print (temp_val)
                #print(low, high, [r, g, b, a])
                return [r, g, b, a]
    return [1, 1, 1, 1]

def GetForeColorByTemp(temp_val, src_index):
    if (src_index == 4):
        for [low, high, [r, g, b, a]] in CABINET_HEAT_FORE_PALLET:
            if ((temp_val > low) and (temp_val <= high)):
                #print (temp_val)
                #print(low, high, [r, g, b, a])
                return [r, g, b, a]
    else:
        for [low, high, [r, g, b, a]] in STD_HEAT_FORE_PALLET:
            if ((temp_val > low) and (temp_val <= high)):
                #print (temp_val)
                #print(low, high, [r, g, b, a])
                return [r, g, b, a]
    return [1, 1, 1, 1]


class ParsedMessage():
    def __init__(self):
        self.date_string = ""
        self.time_string = ""
        self.source_block_string = ""
        self.dest_block_string = ""
        self.msg_type_string = ""
        self.msg_string = ""
        self.int_val = UINT16_INIT
        self.float_val = FLOAT_INIT
        self.valid = False
        self.time_rx = datetime.datetime.now()
        
def parse_message_string(raw_msg_string):
#    print(raw_msg_string)
    parsed_message = ParsedMessage()
    parsed_message.valid = False
    good_length = False
    good_date = False
    good_decode = False
    
    print (raw_msg_string)

    if len(raw_msg_string) > 30:
        good_length = True
        try:
            string_list = raw_msg_string.decode().split(",")
            good_decode = True
        except:
            good_decode = False
        if (good_decode):
            try:
                datetime.datetime.strptime(string_list[0], '%Y-%m-%d')
                good_date = True
            except:
                good_date = False
                 
    if (good_length and good_decode and good_date):
        if not("DEBUG" in string_list):
            parsed_message.date_string = string_list[0]
            parsed_message.time_string = string_list[1]
            parsed_message.source_block_string = string_list[2]
            parsed_message.dest_block_string = string_list[3]
            parsed_message.msg_type_string = string_list[4]
            parsed_message.msg_string = string_list[5]
            parsed_message.int_val = int(string_list[6])
            parsed_message.float_val = float(string_list[7])
            parsed_message.valid = True
            parsed_message.time_rx = datetime.datetime.now()
            with open(MESSAGE_FILENAME, "a") as msg_log_file:
                print(raw_msg_string, file=msg_log_file)
                
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
            last_message_time = datetime.datetime.now()
            message_ever_received = True
            

def modem_read_line(dt):
        #print("********RadioModem method read_line called***********")
        if (radio_modem_port.inWaiting() > 0):
            message_line_buffer.append(radio_modem_port.readline())
#            print(message_line_buffer)


class FFButton(Button):
    def __init__(self, **kwargs):
        super(FFButton, self).__init__(**kwargs)
        #self.text_size = self.size
        self.halign = "center"
        self.valign = "middle"
        self.background_normal = ''
        #self.background_color = FF_GREY
        self.font_size = '28sp'
        #self.spacing = 10
        #self.padding = (10, 10)   
        #self.color = FF_WHITE

    def set_name(self, name_str):
        self.name = name_str
        self.text = name_str    
    def set_source_index(self, idx):
        self.source_index = idx
        
class FFTempButton(FFButton):
    def __init__(self, **kwargs):
        super(FFTempButton, self).__init__(**kwargs)
        self.temperature = 255.255
        #self.size_hint_max_y = 200

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
        
class FFDeviceButton(FFButton):
    def __init__(self, **kwargs):
        super(FFDeviceButton, self).__init__(**kwargs)
        self.FF_state = STATE_UNKNOWN
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

class FFEnergyButton(FFButton):
    def __init__(self, **kwargs):
        super(FFEnergyButton, self).__init__(**kwargs)
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

class TimeButton(FFButton):
    def __init__(self, **kwargs):
        super(TimeButton, self).__init__(**kwargs)
        self.background_color = FF_SLATE_LG
        self.color = FF_WHITE_HG
        self.text = "[b]--:--:--[/b]"
                    
    def set_name(self, name_str):
        self.name = name_str
    def update(self, dt):
        #self.currentDT = "{0:%H:%M:%S  %d-%m-%Y}".format(datetime.datetime.now())
        self.currentDT = "{0:%H:%M:%S}".format(datetime.datetime.now())
        #self.text = str(self.currentDT.hour) + ":" + \
        #            str(self.currentDT.minute) + ":" + \
        #            str(self.currentDT.second)
        #self.text = "[b]Time: " + str(self.currentDT) + " [/b]" 
        self.text = "[b]" + self.currentDT + "[/b]"          

class UpdateIndicatorButton(FFButton):
    def __init__(self, **kwargs):
        super(UpdateIndicatorButton, self).__init__(**kwargs)
        self.background_color = FF_YELLOW_LG
        self.color = FF_WHITE_HG
        self.text = "[b]Lights on, but no one home[/b]"
        self.nowDT = datetime.datetime.now()
        self.delta = datetime.timedelta()
    def set_name(self, name_str):
        self.name = name_str
        
    def update(self, dt):
        global message_ever_received
        global last_message_time
        if (message_ever_received):
            self.nowDT = datetime.datetime.now()
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

            
class TopSection(GridLayout):
    def __init__(self, **kwargs):
        super(TopSection, self).__init__(**kwargs)
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
            self.input_buttons[i].set_source_index(i)
            self.add_widget(self.input_buttons[i])
            Clock.schedule_interval(self.input_buttons[i].update, UI_UPDATE_INTERVAL)
        
        # add the device buttons
        for i, l, d in OUTPUTS_LIST:
            self.output_buttons.append(FFDeviceButton(markup=True))
            self.output_buttons[i].set_name(d)
            self.output_buttons[i].set_source_index(i)
            self.add_widget(self.output_buttons[i])
            Clock.schedule_interval(self.output_buttons[i].update, UI_UPDATE_INTERVAL)

class MiddleSection(GridLayout):
    def __init__(self, **kwargs):
        super(MiddleSection, self).__init__(**kwargs)
        self.cols = 1
        self.spacing = 15
        self.padding = [15, 0]
        
        self.energy_button = FFEnergyButton(markup=True)
        self.energy_button.set_name("Energy")
        self.add_widget(self.energy_button)
        
        Clock.schedule_interval(self.energy_button.update, UI_UPDATE_INTERVAL)

class BottomSection(GridLayout):
    def __init__(self, **kwargs):
        super(BottomSection, self).__init__(**kwargs)
        self.cols = 3
        self.spacing = 15
        self.padding = [15, 15, 15, 0]
                
        self.current_time = TimeButton(markup=True)
        self.current_time.set_name("TimeNow")
        self.add_widget(self.current_time)
        
        self.last_rx_status = UpdateIndicatorButton(markup=True)
        self.last_rx_status.set_name("TimeRxStatus")
        self.add_widget(self.last_rx_status)
        
        Clock.schedule_interval(self.current_time.update, UI_UPDATE_INTERVAL)
        Clock.schedule_interval(self.last_rx_status.update, UI_UPDATE_INTERVAL)

class MainScreen(GridLayout):
    def __init__(self, **kwargs):
        super(MainScreen, self).__init__(**kwargs)
        self.cols = 1
        #self.spacing = 0
        #self.padding = 0
        #self.row_default_height = 30
        #self.height = 480
        #self.width = 800
        # Make the background sand:
        with self.canvas.before:
            #Color(237, 235, 230, 1)
            #self.rect = Rectangle(size=(800, 480), pos=self.pos)
            self.rect = Rectangle(size=(800, 480), pos=self.pos, source='cows.png')

        self.temperature_section = TopSection()
        self.energy_section = MiddleSection()
        self.status_section = BottomSection()
        
        self.add_widget(self.temperature_section)
        self.add_widget(self.energy_section)
        self.add_widget(self.status_section)
        
        #Clock.schedule_interval(radio_modem.read_line, SERIAL_POLL_INTERVAL)
        Clock.schedule_interval(parse_and_update, UI_UPDATE_INTERVAL)
        print("*************RADIO MODEM READ and PARSE CLOCKS ADDED ******")
        Clock.schedule_interval(modem_read_line, SERIAL_POLL_INTERVAL)
         
        
class MyApp(App):

    def build(self):
        Config.set('graphics', 'width', '800')
        Config.set('graphics', 'height', '480')
        Config.write()
        #return TopSection()
        return MainScreen()   

if __name__ == '__main__':
    # Connect to serial port first
    try:
        print("Connecting to Radio Modem")
        radio_modem_port = serial.Serial(MODEM_SERIAL_PORT, MODEM_SERIAL_SPEED)
    except:
        print ("Failed to connect")
        exit()
        

    # Launch the app
    MyApp().run()

    # Close serial communication
    print("Closing Radio Modem Connection")
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
        speedSlider = Slider(orientation='vertical', min=1, max=30, value=speed)
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



    
