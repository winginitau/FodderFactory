'''
Created on 8 Jul. 2018

@author: brendan
'''

from kivy.uix.button import Button
from kivy.uix.screenmanager import SlideTransition, SwapTransition

from datetime import datetime, timedelta

from privatelib.palette import FF_WHITE_HG, FF_RED_LG, FF_GREEN_LG, FF_YELLOW_LG, \
                               FF_WHITE_LG, FF_WHITE, FF_BLUE_LG, FF_GREY_LG, FF_SLATE_LG

from privatelib.ff_config import FLOAT_INIT, \
                                 INPUTS_LIST, OUTPUTS_LIST, \
                                 STATE_ON, STATE_OFF, STATE_UNKNOWN, \
                                 ui_inputs_values, ui_outputs_values, ui_energy_values, \
                                 GetBackColorByTemp, GetForeColorByTemp, active_rules


from privatelib.global_vars import msg_sys, sm 

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
        #self.border = [self.size[1]+30,self.size[0]+30,self.size[1]+30,self.size[0]+30]
        #self.text = "\n[b]" + self.text + "[/b]"
        self.text = ""
    def set_name(self, name_str):
        self.name = name_str
        self.text = name_str    

class RuleButton(BasicButton):
    def __init__(self, **kwargs):
        super(RuleButton, self).__init__(**kwargs)
        self.background_color = FF_BLUE_LG
        self.color = FF_WHITE_HG
        self.text=""
        self.font_size = '22sp'
#self.valign = "bottom"
        #self.size_hint_x = None
        #self.size_hint_y = None
        #self.height = 120
        #self.width = 150
    def update(self, _dt):                   # dt passed in kivy callback - not used
        self.rule_count = 0
        self.text = ""
        if len(active_rules) == 0:
            self.background_color = FF_SLATE_LG
            self.color = FF_WHITE_HG
            self.text = "No Active Rules"
        else:
            self.background_color = FF_GREEN_LG
            self.color = FF_WHITE_HG
            self.text = active_rules[0]
        for n in range(1, len(active_rules)):
            self.text = self.text + "\n" + active_rules[n]
            
class NavigationButton(BasicButton):
    def __init__(self, **kwargs):
        super(NavigationButton, self).__init__(**kwargs)
        self.background_color = FF_BLUE_LG
        self.color = FF_WHITE_HG

class NavCloseToMainButton(NavigationButton):
    def __init__(self, **kwargs):
        super(NavCloseToMainButton, self).__init__(**kwargs)
    def on_press(self):
        sm.transition = SwapTransition()
        sm.current = "main"

class NavNextButton(NavigationButton):
    def __init__(self, **kwargs):
        super(NavNextButton, self).__init__(**kwargs)
    def on_press(self):
        sm.transition = SlideTransition(direction='left')
        #sm.transition.direction = 'left'
        sm.current = sm.next()
        #sm.switch_to(MainScreen())

class NavPrevButton(NavigationButton):
    def __init__(self, **kwargs):
        super(NavPrevButton, self).__init__(**kwargs)
    def on_press(self):
        sm.transition = SlideTransition(direction='right')
        sm.current = sm.previous()
        
class NavZoomButton(NavigationButton):
    def __init__(self, **kwargs):
        super(NavZoomButton, self).__init__(**kwargs)
    def register_on_press_callback(self, func, param):  
        self.on_press_callback = func
        self.on_press_param = param  
    def on_press(self):
        if self.on_press_callback:
            self.on_press_callback(self.on_press_param)
        else:
            print("Error No Callback defined")
        
class FFTempButton(BasicButton):
    def __init__(self, **kwargs):
        super(FFTempButton, self).__init__(**kwargs)
        self.temperature = 255.255
        #self.size_hint_max_y = 200
        self.source_index = 255
    def on_press(self):
        sm.transition = SwapTransition()
        #sm.transition = RiseInTransition()
        sm.current = self.name
        
    def set_source_by_index(self, idx):
        self.source_index = idx                    
            
    def set_source_by_disp_name(self, disp_name):
        for i, _src, disp in INPUTS_LIST:
            if disp == disp_name:
                self.source_index = i

    def set_source_by_source_name(self, source_name):
        for i, src, _disp in INPUTS_LIST:
            if src == source_name:
                self.source_index = i        
        
    def update(self, _dt):                   # dt passed in kivy callback - not used
        self.temperature = ui_inputs_values[self.source_index]
        if (self.temperature != FLOAT_INIT):
            #self.background_normal = ''
            self.background_color = GetBackColorByTemp(self.temperature, self.source_index)
            self.color = GetForeColorByTemp(self.temperature, self.source_index)
        else:
            self.background_color = FF_GREY_LG
            self.color = FF_WHITE_LG            
        self.text = self.name + "\n[b]" + str(self.temperature) + "[/b]"
        
class FFDeviceButton(BasicButton):
    def __init__(self, **kwargs):
        super(FFDeviceButton, self).__init__(**kwargs)
        self.FF_state = STATE_UNKNOWN

    def on_press(self):
        sm.transition = SwapTransition()
        sm.current = self.name

    def set_source_by_index(self, idx):
        self.source_index = idx                    
    
    def set_source_by_disp_name(self, disp_name):
        for i, _src, disp in OUTPUTS_LIST:
            if disp == disp_name:
                self.source_index = i

    def set_source_by_source_name(self, source_name):
        for i, src, _disp in OUTPUTS_LIST:
            if src == source_name:
                self.source_index = i

    def update(self, _dt):                   # dt passed in kivy callback - not used
        self.FF_state = ui_outputs_values[self.source_index]
        #self.background_color = GetBackColorByTemp(self.temperature)
        if (self.FF_state == STATE_UNKNOWN):
            self.state_str = "\nUnknown"
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

class FFEnergyButton(BasicButton):
    def __init__(self, **kwargs):
        super(FFEnergyButton, self).__init__(**kwargs)

    def on_press(self):
        sm.transition = SwapTransition()
        sm.current = "State of Charge"
    
    def update(self, _dt):                   # dt passed in kivy callback - not used      
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

class TimeButton(BasicButton):
    def __init__(self, **kwargs):
        super(TimeButton, self).__init__(**kwargs)
        self.background_color = FF_SLATE_LG
        self.color = FF_WHITE_HG
        self.text = "[b]--:--:--[/b]"
    def on_press(self):
        pass
        #BasicButton.on_press(self)
        #sm.transition = SwapTransition()
        #sm.current = 'system'                                
    def set_name(self, name_str):
        self.name = name_str
    def update(self, _dt):
        #self.currentDT = "{0:%H:%M:%S  %d-%m-%Y}".format(datetime.datetime.now())
        self.currentDT = "{0:%H:%M:%S}".format(datetime.now())
        #self.text = str(self.currentDT.hour) + ":" + \
        #            str(self.currentDT.minute) + ":" + \
        #            str(self.currentDT.second)
        #self.text = "[b]Time: " + str(self.currentDT) + " [/b]" 
        self.text = "[b]" + self.currentDT + "[/b]"          

class MessageIndicatorButton(BasicButton):
    def __init__(self, **kwargs):
        super(MessageIndicatorButton, self).__init__(**kwargs)
        self.background_color = FF_YELLOW_LG
        self.color = FF_WHITE_HG
        self.text = "[b]No Data Yet[/b]"
        self.nowDT = datetime.now()
        self.delta = timedelta()
    def set_name(self, name_str):
        self.name = name_str
    def on_press(self):
        pass
        #BasicButton.on_press(self)
        #sm.transition = SwapTransition()
        #sm.current = 'system'            
    def update(self, _dt):
        #global message_ever_received
        #global last_message_time
        #global msg_sys
        if (msg_sys.message_ever_received):
            self.nowDT = datetime.now()
            self.delta = self.nowDT - msg_sys.last_message_time
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

