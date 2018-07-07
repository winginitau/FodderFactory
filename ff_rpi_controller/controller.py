
'''
    Fodder Factory UI Controller
    
    For Raspberry Pi Running rasbian and RPI Touch Screen 
    
    Copyright Brendan McLearie 2018 
'''




import kivy
from kivy.uix.label import Label
#from kivy.uix.boxlayout import BoxLayout
#from kivy.uix.anchorlayout import AnchorLayout
from kivy.uix.gridlayout import GridLayout
#from _socket import TCP_CORK
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

#from datetime import datetime, date, time, timedelta



#from math import sin, cos

#kivy.garden.garden_system_dir = 'libs/garden'
#from kivy.garden.graph import Graph

from graph import Graph
from graph import MeshLinePlot
# MeshStemPlot, LinePlot, SmoothLinePlot, ContourPlot

#from kivy.base import runTouchApp

from privatelib.palette import *
from privatelib.ff_config import *
from privatelib.db_funcs import *

from privatelib.msg_parser import MessageSystem

##### Globals Variables###############

#message_queue = deque()    
#last_message_time = datetime.now()
#message_ever_received = False


     
#i_energy_soc = UINT16_INIT
#energy_soc = FLOAT_INIT
#energy_voltage = FLOAT_INIT
#energy_power = FLOAT_INIT
#energy_current = FLOAT_INIT


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
        
class FFDeviceButton(MainDisplayButton):
    def __init__(self, **kwargs):
        super(FFDeviceButton, self).__init__(**kwargs)
        self.FF_state = STATE_UNKNOWN

    def on_press(self):
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

class TimeButton(MainDisplayButton):
    def __init__(self, **kwargs):
        super(TimeButton, self).__init__(**kwargs)
        self.background_color = FF_SLATE_LG
        self.color = FF_WHITE_HG
        self.text = "[b]--:--:--[/b]"
                    
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
        
    def update(self, _dt):
        global message_ever_received
        global last_message_time
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
        for i, _l, d in INPUTS_LIST:
            self.input_buttons.append(FFTempButton(markup=True))
            self.input_buttons[i].set_name(d)
            self.input_buttons[i].set_source_by_index(i)
            self.add_widget(self.input_buttons[i])
            Clock.schedule_interval(self.input_buttons[i].update, DATA_PARSE_INTERVAL)
        
        # add the device buttons
        for i, _l, d in OUTPUTS_LIST:
            self.output_buttons.append(FFDeviceButton(markup=True))
            self.output_buttons[i].set_name(d)
            self.output_buttons[i].set_source_by_index(i)
            self.add_widget(self.output_buttons[i])
            Clock.schedule_interval(self.output_buttons[i].update, DATA_PARSE_INTERVAL)

class MainMiddleGrid(GridLayout):
    def __init__(self, **kwargs):
        super(MainMiddleGrid, self).__init__(**kwargs)
        self.cols = 1
        self.spacing = 15
        self.padding = [15, 0]
        
        self.energy_button = FFEnergyButton(markup=True)
        self.energy_button.set_name("Energy")
        self.add_widget(self.energy_button)
        
        Clock.schedule_interval(self.energy_button.update, DATA_PARSE_INTERVAL)

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
        
        Clock.schedule_interval(self.current_time.update, DATA_PARSE_INTERVAL)
        Clock.schedule_interval(self.last_rx_status.update, DATA_PARSE_INTERVAL)

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
                           ymin=self.min_y, ymax=self.max_y, \
                           padding=5, \
                           border_color = FF_WHITE_LG, \
                           background_color = FF_BLACK_MG, \
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
        
        Clock.schedule_interval(self.update, 5)

    def refresh_data(self):         
        self.db_result_list = []
        self.db_result_list = db_last24_temps_by_block_label(self.source, self.db_result_list)
        
        self.hot_line = []
        self.cold_line = []
        self.data_line = []
        
        nowDT = datetime.now()
        now_min = nowDT.minute
        now_sec = nowDT.second
        
        self.x_offset = (now_min * 60) + now_sec
        
        self.min_y = 80
        self.max_y = -80
        self.low_line, self.high_line = get_temp_graph_low_high(self.source)
        
        for t, v in self.db_result_list:
            if v > self.max_y:
                self.max_y = v
            if v < self.min_y:
                self.min_y = v
            t_mod = t + self.x_offset
            self.data_line.append((t_mod,v))      
            self.hot_line.append((t_mod,self.high_line)) 
            self.cold_line.append((t_mod,self.low_line))
                     
        self.min_y, self.max_y = calc_temp_graph_y_grid(self.min_y, self.max_y, 2)
        
        if self.min_y >= self.low_line: self.min_y = self.low_line - 1
        if self.max_y <= self.high_line: self.max_y = self.high_line + 1

    def update(self, _dt):
        #self.graph.remove_plot(self.plot)
        self.refresh_data()
        
        self.graph.xmax=(86400 + self.x_offset)
        self.graph.ymin=self.min_y
        self.graph.ymax=self.max_y
                        
        self.plot_hot.points = self.hot_line.copy()  
        self.plot_cold.points = self.cold_line.copy()  
        self.plot_data.points = self.data_line.copy()
        
        self.graph._redraw_all()   
              
class ZoomDeviceDataGrid(GridLayout):
    def __init__(self, **kwargs):
        super(ZoomDeviceDataGrid, self).__init__(**kwargs)
        self.cols = 1
        self.spacing = 0
        self.padding = [10, 0, 10, 10]


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
                           background_color = FF_BLACK_MG, \
                           _with_stencilbuffer=False)
        self.add_widget(self.graph)

        self.plot_data = MeshLinePlot(color=FF_WHITE)
        self.plot_data.points = self.data_line.copy()  
        
        self.graph.add_plot(self.plot_data)
        
        Clock.schedule_interval(self.update, 60)

    def refresh_data(self):        
        self.db_result_list = []
        self.db_result_list = db_last24_device_by_block_label(self.source, self.db_result_list)
        
        self.data_line = []
        
        nowDT = datetime.now()
        now_min = nowDT.minute
        now_sec = nowDT.second
        
        self.x_offset = (now_min * 60) + now_sec
                
        for t, v in self.db_result_list:
            t_mod = t + self.x_offset
            self.data_line.append((t_mod,v))      

    def update(self, _dt):
        #self.graph.remove_plot(self.plot)
        self.refresh_data()
        
        self.graph.xmax=(86400 + self.x_offset)                
        self.plot_data.points = self.data_line.copy()
        self.graph._redraw_all()   

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
                           background_color = FF_BLACK_MG, \
                           _with_stencilbuffer=False)
        self.add_widget(self.graph)

        self.plot_data = MeshLinePlot(color=FF_WHITE)
        self.plot_data.points = self.data_line.copy()  
        
        self.graph.add_plot(self.plot_data)
        
        Clock.schedule_interval(self.update, 60)

    def refresh_data(self):        
        
        self.db_result_list = []
        self.db_result_list = db_energy_data(energy_message = self.source, db_result_list = self.db_result_list) 
        
        self.data_line = []
        
        nowDT = datetime.now()
        now_min = nowDT.minute
        now_sec = nowDT.second
        
        self.x_offset = (now_min * 60) + now_sec
                
        for t, v in self.db_result_list:
            t_mod = t + self.x_offset
            self.data_line.append((t_mod,v))      

    def update(self, _dt):
        #self.graph.remove_plot(self.plot)
        self.refresh_data()
        
        self.graph.xmax=(self.period.total_seconds() + self.x_offset)                
        self.plot_data.points = self.data_line.copy()   
        self.graph._redraw_all()

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
        Clock.schedule_interval(self.zoom_button.update, DATA_PARSE_INTERVAL)
                                           
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
       
    DBConnectTest()

    # Set Up Data Processing
    msg_sys = MessageSystem();
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



    
