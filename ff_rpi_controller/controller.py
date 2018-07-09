
'''
    Fodder Factory UI Controller
    
    For Raspberry Pi Running rasbian and RPI Touch Screen 
    
    Copyright Brendan McLearie 2018 
'''

import kivy
from kivy.uix.label import Label
#from _socket import TCP_CORK
kivy.require('1.10.0') # replace with your current kivy version !
from kivy.config import Config
from kivy.app import App
#from kivy.clock import Clock
from kivy.uix.screenmanager import Screen

from privatelib.palette import FF_WHITE, FF_GREY_HG

from privatelib.ff_config import INPUTS_LIST, OUTPUTS_LIST, ENERGY_LIST, \
                                 MODEM_SERIAL_PORT, MODEM_SERIAL_SPEED                                
from privatelib.db_funcs import DBConnectTest
from privatelib.global_vars import msg_sys, sm
from privatelib.grids import MainParentGrid, DataParentGrid
                                

##### Globals Variables###############


# Global Functions


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

            
       
# Declare screens
class MainScreen(Screen):
    def __init__(self, **kwargs):
        super(MainScreen, self).__init__(**kwargs)
        self.screen_layout = MainParentGrid()
        self.add_widget(self.screen_layout)

class DataScreen(Screen):
    def __init__(self, **kwargs):
        super(DataScreen, self).__init__(**kwargs)
        #self.screen_layout = MainParentGrid()
    def setup(self, disp_name="", data_source="", data_type=""):
        self.disp_name = disp_name
        self.data_source = data_source
        self.data_type = data_type

        #self.screen_layout = DataParentGrid()
        #self.screen_layout.setup(disp_name=disp_name, data_source=data_source, \
        #                         data_type=data_type) 
        #self.add_widget(self.screen_layout)

    def on_enter(self, *args):
        Screen.on_enter(self, *args)       
        self.screen_layout = DataParentGrid()
        self.screen_layout.setup(disp_name=self.disp_name, data_source=self.data_source, \
                                 data_type=self.data_type) 
        self.add_widget(self.screen_layout)
        
    def on_leave(self, *args):
        Screen.on_leave(self, *args)
        self.remove_widget(self.screen_layout)


#sm = ScreenManager()
sm.add_widget(MainScreen(name='main'))
for i, source, disp in INPUTS_LIST:
    scr = DataScreen(name=disp)
    scr.setup(disp_name=disp, data_source=source, data_type="INPUT")
    sm.add_widget(scr)
for i, source, disp in OUTPUTS_LIST:
    scr = DataScreen(name=disp)
    scr.setup(disp_name=disp, data_source=source, data_type="OUTPUT")
    sm.add_widget(scr)
for source, disp in ENERGY_LIST:
    scr = DataScreen(name=disp)
    scr.setup(disp_name=disp, data_source=source, data_type="ENERGY")
    sm.add_widget(scr)
    
# Top Level App                                      
class MyApp(App):

    def build(self):
        Config.set('graphics', 'width', '800')
        Config.set('graphics', 'height', '480')
        Config.write()
        #return MainTopGrid()
        #return MainParentGrid()
        return sm   


#msg_sys = MessageSystem();

############################################################################
# Main
############################################################################
if __name__ == '__main__':
       
    DBConnectTest()

    # Set Up Data Processing
    msg_sys.configure(tcp=False, serial=True, \
                      serial_port=MODEM_SERIAL_PORT, serial_speed=MODEM_SERIAL_SPEED, \
                      tcp_address="192.168.5.253", tcp_port="3333")
    

    # Launch the app
    MyApp().run()
    #runTouchApp(sm)
    print("Closing data connection")
    msg_sys.end()
    
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
        speedSlider = Slider(orientation='vertical', min_y=1, max_y=30, value=speed)
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



    
