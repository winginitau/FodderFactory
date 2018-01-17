
'''
    Fodder Factory Controller
    
    Linux module and for Raspberry Pi
    
    Copyright Brendan McLearie 2018
    
'''

import serial
from collections import deque


import kivy
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


# Constants

# Colours

_FF_RED = [196, 3, 51]
_FF_GREEN = [0, 158, 107]
_FF_BLUE = [0, 135, 189]
_FF_COLD_BLUE = [153, 228, 255]
_FF_FREEZE_BLUE = [204, 241, 255]
_FF_YELLOW = [255, 211, 1]
_FF_GREY = [128, 127, 128]
_FF_SAND = [237, 235, 230]
_FF_SLATE = [69, 72, 90]

FF_RED = [x/255 for x in _FF_RED]
FF_RED.append(1.0)

FF_GREEN = [x/255 for x in _FF_GREEN]
FF_GREEN.append(1.0)

FF_BLUE = [x/255 for x in _FF_BLUE]
FF_BLUE.append(1.0)

FF_COLD_BLUE = [x/255 for x in _FF_COLD_BLUE]
FF_COLD_BLUE.append(1.0)

FF_FREEZE_BLUE = [x/255 for x in _FF_FREEZE_BLUE]
FF_FREEZE_BLUE.append(1.0)

FF_YELLOW = [x/255 for x in _FF_YELLOW]
FF_YELLOW.append(1.0)

FF_GREY = [x/255 for x in _FF_GREY]
FF_GREY.append(1.0)

FF_SAND = [x/255 for x in _FF_SAND]
FF_SAND.append(1.0)

FF_SLATE = [x/255 for x in _FF_SLATE]
FF_SLATE.append(1.0)


# [Index, "SOURCE BLOCK", "Display Label"]

INPUTS_LIST = (
    [0, "IN_INSIDE_TOP_TEMP", "Inside Top"],
    [1, "IN_INSIDE_BOTTOM_TEMP", "Inside Bottom"],
    [2, "IN_OUTSIDE_TEMP", "Outside"],
    [3, "IN_WATER_TEMP", "Header Water"],
    [4, "IN_CABINET_TEMP", "Electrical Cabinet"],
    [5, "IN_LONGRUN_TEMP", "Main Water Tank"]
)

# [Index, "SOURCE BLOCK", "Display Label"]

OUTPUTS_LIST = (
    [0, "OUT_END_LIGHTS", "Lights"], 
    [1, "OUT_WATER_HEATER", "Water Heater"],
    [2, "OUT_FRESH_AIR_FAN", "Inlet Fan"],
    [3, "OUT_CIRCULATION_FAN", "Circulation Fan"],
    [4, "OUT_EXHAUST_FAN", "Exhaust Fan"],
    [5, "OUT_WATERING_SOLENOID_TOP", "Top Sprinklers"],
    [6, "OUT_WATERING_SOLENOID_BOTTOM", "Bottom Sprinklers"]
)

INT8_INIT = -127
UINT8_INIT = 255

UINT16_INIT = 65535
FLOAT_INIT = 255.255


MODEM_SERIAL_PORT = "/dev/ttyUSB0"
MODEM_SERIAL_SPEED = 57600

SERIAL_POLL_INTERVAL = 0.5          # Seconds
UI_UPDATE_INTERVAL = 1.0            # Seconds

HEAT_PALLET = (
    [-50, 15, FF_FREEZE_BLUE ],       # Freeze Blue
    [15, 20, FF_COLD_BLUE ],    # Cold Blue
    [20, 25, FF_GREEN ],     # green - good
    [25, 30, FF_YELLOW ],    # yellow - warm
    [30, 100, FF_RED ]     # red - hot
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
    STATE_UNKNOWN,
    STATE_UNKNOWN,
    STATE_UNKNOWN,
    STATE_UNKNOWN,
    STATE_UNKNOWN 
]

# Global Functions

def get_colour_by_temp(temp_val):
    for [low, high, [r, g, b, a]] in HEAT_PALLET:
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
        
def parse_message_string(raw_msg_string):
#    print(raw_msg_string)
    parsed_message = ParsedMessage()
    parsed_message.valid = False
    print (raw_msg_string)
    if len(raw_msg_string) > 30:                            #  possibly valid message not init junk
        string_list = raw_msg_string.decode().split(",")
        print (string_list)
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

    return parsed_message


def parse_and_update(dt):               # dt passed in kivy callback - not used
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

def modem_read_line(dt):
        #print("********RadioModem method read_line called***********")
        if (radio_modem_port.inWaiting() > 0):
            message_line_buffer.append(radio_modem_port.readline())
#            print(message_line_buffer)


'''        
class RadioModem():
    def __init__(self, serial_port):
        self.port = serial_port
        print("********RadioModem __init__ called***********")
    def read_line(self, dt):                # dt passed in kivy callback - not used
        print("********RadioModem method read_line called***********")
        if (self.port.inWaiting() > 0):
            message_line_buffer.append(self.port.readline())
            print(message_line_buffer)
'''

class FFButton(Button):
    def __init__(self, **kwargs):
        super(FFButton, self).__init__(**kwargs)
        #self.text_size = self.size
        self.halign = "center"
        self.valign = "middle"
        self.background_normal = ''
        self.background_color = FF_GREY
    def set_name(self, name_str):
        self.name = name_str
        self.text = name_str    
    def set_source_index(self, idx):
        self.source_index = idx
        
class FFTempButton(FFButton):
    def __init__(self, **kwargs):
        super(FFTempButton, self).__init__(**kwargs)
        self.temperature = 255.255
    def update(self, dt):                   # dt passed in kivy callback - not used
        self.temperature = ui_inputs_values[self.source_index]
        if (self.temperature != FLOAT_INIT):
            #self.background_normal = ''
            self.background_color = get_colour_by_temp(self.temperature)
        else:
            self.background_color = FF_GREY            
        self.text = self.name + "\n\n[b]" + str(self.temperature) + "[/b]"
        #self.text.color = [1, 0, 0, 1]
        
class FFDeviceButton(FFButton):
    def __init__(self, **kwargs):
        super(FFDeviceButton, self).__init__(**kwargs)
        self.FF_state = STATE_UNKNOWN
    def update(self, dt):                   # dt passed in kivy callback - not used
        self.FF_state = ui_outputs_values[self.source_index]
        #self.background_color = get_colour_by_temp(self.temperature)
        if (self.FF_state == STATE_UNKNOWN):
            self.state_str = "State Unknown"
        elif (self.FF_state == STATE_OFF):
            self.state_str = "OFF"
            #self.background_normal = ""
            self.background_color = FF_SLATE
        elif (self.FF_state == STATE_ON):
            self.state_str = "ON"
            #self.background_normal = ""
            self.background_color = FF_GREEN
        self.text = self.name + "\n\n[b]" + self.state_str + "[/b]"
        #self.text.color = [1, 0, 0, 1]
#        print("*****FFDeviceButton update method called by clock *************")

class FFMainScreen(GridLayout):
    def __init__(self, **kwargs):
        super(FFMainScreen, self).__init__(**kwargs)
        self.cols = 3
        self.spacing = 10
        self.padding = 30
        #self.row_default_height = 30
        self.height = 480
        self.width = 800
        
        self.input_buttons = []
        self.output_buttons = []
        
#        radio_modem = RadioModem(radio_modem_port)
#        print("*************RADIO MODEM INSTANTIATED****************")
 
        # Make the background sand:
        with self.canvas.before:
            Color(237, 235, 230, 1)
            self.rect = Rectangle(size=(800, 480), pos=self.pos)
        
        for i, l, d in INPUTS_LIST:
            self.input_buttons.append(FFTempButton(markup=True))
            self.input_buttons[i].set_name(d)
            self.input_buttons[i].set_source_index(i)
            self.add_widget(self.input_buttons[i])
            Clock.schedule_interval(self.input_buttons[i].update, UI_UPDATE_INTERVAL)
        
        for i, l, d in OUTPUTS_LIST:
            self.output_buttons.append(FFDeviceButton(markup=True))
            self.output_buttons[i].set_name(d)
            self.output_buttons[i].set_source_index(i)
            self.add_widget(self.output_buttons[i])
            Clock.schedule_interval(self.output_buttons[i].update, UI_UPDATE_INTERVAL)
        
        #Clock.schedule_interval(radio_modem.read_line, SERIAL_POLL_INTERVAL)
        Clock.schedule_interval(parse_and_update, UI_UPDATE_INTERVAL)
        print("*************RADIO MODEM READ and PARSE CLOCKS ADDED ******")
        Clock.schedule_interval(modem_read_line, SERIAL_POLL_INTERVAL)

class MyApp(App):

    def build(self):
        Config.set('graphics', 'width', '800')
        Config.set('graphics', 'height', '480')
        Config.write()
        return FFMainScreen()
           

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



    
