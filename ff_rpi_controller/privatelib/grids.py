'''
Created on 9 Jul. 2018

@author: brendan
'''

from kivy.uix.gridlayout import GridLayout
from kivy.uix.label import Label
from kivy.lang import Builder
#from kivy.graphics.instructions import InstructionGroup

from privatelib.ff_config import INPUTS_LIST, OUTPUTS_LIST, CLOCK_UPDATE_INTERVAL,\
                                 UI_REFRESH_INTERVAL, GRAPH_UPDATE_INTERVAL, \
                                 DB_INPUT_LOW_HIGH, DB_ENERGY_GRAPH_PARAMS, \
                                 DB_LOCAL_CONFIG, DB_CLOUD_CONFIG, \
                                 GRAPH_UPDATE_FROM_LOCAL_DB, GRAPH_UPDATE_FROM_CLOUD_DB, \
                                 TEMPERATURE_SANITY_HIGH, TEMPERATURE_SANITY_LOW, \
                                 BACKGROUND_IMAGE
                                 
                                 
from kivy.clock import Clock
from graph import Graph
#from timeseries.TimeSeriesGraph import TimeSeriesGraph
from graph import MeshLinePlot
from kivy.graphics import Rectangle
from datetime import datetime, timedelta

import math

from privatelib.buttons import FFTempButton, FFDeviceButton, FFEnergyButton, \
                                TimeButton, MessageIndicatorButton, \
                                NavPrevButton, NavZoomButton, NavCloseToMainButton, \
                                NavNextButton, RuleButton
from privatelib.db_funcs import db_temperature_data ,db_device_data, \
                                db_energy_data, db_log_history, \
                                db_energy_data_new, db_temperature_data_new
                               
from privatelib.palette import FF_WHITE, FF_BLACK_MG, FF_WHITE_LG, FF_SLATE_HG, \
                               FF_RED, FF_GREEN, FF_BLUE, FF_SLATE_LG, FF_BLUE_LG
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.textinput import TextInput
                                


class MainTopGrid(GridLayout):
    def __init__(self, **kwargs):
        super(MainTopGrid, self).__init__(**kwargs)
        self.cols = 4
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
        for i, _l, d, _p in INPUTS_LIST:
            self.input_buttons.append(FFTempButton(markup=True))
            self.input_buttons[i].set_name(d)
            self.input_buttons[i].set_source_by_index(i)
            self.add_widget(self.input_buttons[i])
            Clock.schedule_interval(self.input_buttons[i].update, UI_REFRESH_INTERVAL)
        
        # add the device buttons
        for i, _l, d, _p in OUTPUTS_LIST:
            self.output_buttons.append(FFDeviceButton(markup=True))
            self.output_buttons[i].set_name(d)
            self.output_buttons[i].set_source_by_index(i)
            self.add_widget(self.output_buttons[i])
            Clock.schedule_interval(self.output_buttons[i].update, UI_REFRESH_INTERVAL)
        
        self.rule_button = RuleButton(markup=True)
        self.add_widget(self.rule_button)
        Clock.schedule_interval(self.rule_button.update, UI_REFRESH_INTERVAL)

class MainMiddleGrid(GridLayout):
    def __init__(self, **kwargs):
        super(MainMiddleGrid, self).__init__(**kwargs)
        self.cols = 1
        self.spacing = 15
        self.padding = [15, 0]
        
        self.energy_button = FFEnergyButton(markup=True)
        self.energy_button.set_name("Energy")
        self.add_widget(self.energy_button)
        
        Clock.schedule_interval(self.energy_button.update, UI_REFRESH_INTERVAL)

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
        
        Clock.schedule_interval(self.current_time.update, CLOCK_UPDATE_INTERVAL)
        Clock.schedule_interval(self.last_rx_status.update, UI_REFRESH_INTERVAL)

class MainParentGrid(GridLayout):
    def __init__(self, **kwargs):
        super(MainParentGrid, self).__init__(**kwargs)
        self.cols = 1
            
        self.bind(size=self.update_size)
        self.temperature_section = MainTopGrid()
        self.energy_section = MainMiddleGrid()
        self.nav_section = MainBottomGrid()
        
        self.add_widget(self.temperature_section)
        self.add_widget(self.energy_section)
        self.add_widget(self.nav_section)
    def update_size(self, _w ,_h):
        #print (self.size)  
        with self.canvas.before:
            #self.rect = Rectangle(size=(800, 480), pos=self.pos, source='cows.png')
            self.rect = Rectangle(size=self.size, pos=self.pos, source=BACKGROUND_IMAGE)

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

def get_energy_graph_params(source_str):
    for src, ylabel, low_line, high_line, ytics in DB_ENERGY_GRAPH_PARAMS:
        if src == source_str:
            return ylabel, low_line, high_line, ytics 
    return 0,0,0,0


class GraphGrid(GridLayout):
    def __init__(self, **kwargs):
        super(GraphGrid, self).__init__(**kwargs)
        self.cols = 1
        self.spacing = 0
        self.padding = [10, 0, 10, 10]
        if GRAPH_UPDATE_FROM_LOCAL_DB and GRAPH_UPDATE_FROM_CLOUD_DB:
            print("ERROR: (GraphGrid) Multiple DBs configured for UI update")
            exit(1)
        if GRAPH_UPDATE_FROM_LOCAL_DB:
            self.db_source = DB_LOCAL_CONFIG
        elif GRAPH_UPDATE_FROM_CLOUD_DB:
            self.db_source = DB_CLOUD_CONFIG
        else:
            print("ERROR: (GraphGrid) __init__ called no valid DB sources")
            exit(1)
        #self.size_hint_min_y = 200
    def setup(self, disp_name="", data_source="", data_type="", \
              ylabel="", y_ticks_major=2, period_in_days=1):
        self.disp_name = disp_name
        self.data_source = data_source
        self.data_type = data_type
        self.ylabel = ylabel
        self.y_ticks_major = y_ticks_major
        self.period = timedelta(period_in_days)
        self.x_ticks_major = 3600
        self.x_offset = 0
        self.min_y = -1
        self.max_y = 1
    def adjust_period(self, days):
        self.period = self.period + timedelta(days=days)
        if self.period.days < 1:
            self.period = timedelta(days=1)
        self.update(0)
    def add_graph(self, draw_high_line=False, high_colour=FF_RED, \
                  draw_low_line=False, low_colour=FF_BLUE, day_night=False):
        self.draw_high_line = draw_high_line
        self.draw_low_line = draw_low_line
        self.draw_day_night = day_night
        if self.draw_high_line:
            self.high_line_values = []
        if self.draw_low_line:
            self.low_line_values = []
        if self.draw_day_night:
            self.day_night_values = []
        self.data_line_values = []
        #self.refresh_data()        
        self.graph = Graph(xlabel=self.disp_name, ylabel=self.ylabel, \
                           x_grid_label=False, \
                           x_grid=True, \
                           x_ticks_minor=0, \
                           x_ticks_major=self.x_ticks_major, \
                           xmin=0, xmax=(86400 + self.x_offset), \
                           y_grid_label=True, \
                           y_grid=True, \
                           y_ticks_major=self.y_ticks_major, \
                           ymin=self.min_y, ymax=self.max_y, \
                           padding=5, \
                           border_color = FF_WHITE_LG, \
                           background_color = FF_BLACK_MG, \
                           _with_stencilbuffer=False)
        self.add_widget(self.graph)
        if self.draw_high_line:
            self.plot_high = MeshLinePlot(color=high_colour)
            self.plot_high.points = self.high_line_values.copy()  
            self.graph.add_plot(self.plot_high)
        if self.draw_low_line:
            self.plot_low = MeshLinePlot(color=low_colour)
            self.plot_low.points = self.low_line_values.copy()  
            self.graph.add_plot(self.plot_low)
        if self.draw_day_night:
            self.day_night_plot = MeshLinePlot(color=FF_WHITE_LG)
            self.day_night_plot.points = self.day_night_values.copy()
            self.graph.add_plot(self.day_night_plot)
        self.plot_data = MeshLinePlot(color=FF_WHITE)        
        self.plot_data.points = self.data_line_values.copy()
        self.graph.add_plot(self.plot_data)
        #self.update(0)
        Clock.schedule_once(self.update, 0.2)
        self.update_event = Clock.schedule_interval(self.update, GRAPH_UPDATE_INTERVAL)    
    def destroy(self):
        Clock.unschedule(self.update_event)
    def update(self, _dt):        
        if self.period.days == 1:
            self.graph.x_ticks_major=3600   # 1 per 1 hours = 24 lines
            #for child in self.children:
            #    print(child._ticks_majorx)
            #    for i in range(0, len (child._ticks_majorx)):
            #        child._ticks_majorx[i] = float(i)
            #    print(child._ticks_majorx)
            #self.graph._update_labels()
                    
        elif self.period.days == 2:          
            self.graph.x_ticks_major=7200   # 1 per 1.5 hours = 36 lines
        elif self.period.days == 3:
            self.graph.x_ticks_major=10800  # 1 per 3 hours = 24 lines
        elif self.period.days == 4:
            self.graph.x_ticks_major=21600   # 1 per 4 hours = 32 lines
        elif (self.period.days >= 5) and (self.period.days <= 7):
            self.graph.x_ticks_major=43200
        elif (self.period.days > 7) and (self.period.days <= 30):
            self.graph.x_ticks_major=86400
        elif (self.period.days > 30) and (self.period.days <= 60):
            self.graph.x_ticks_major=216000
        elif (self.period.days > 60) and (self.period.days <= 90):
            self.graph.x_ticks_major=432000
        elif (self.period.days > 90) and (self.period.days <= 183):
            self.graph.x_ticks_major=2628000
        elif (self.period.days > 183) and (self.period.days <= 365):
            self.graph.x_ticks_major=2628000
        elif (self.period.days > 365) and (self.period.days <= 730):
            self.graph.x_ticks_major=2628000
        elif (self.period.days > 730):
            self.graph.x_ticks_major=31536000
        
        self.refresh_data()     
        self.graph.xlabel=(self.disp_name + " - " + str(self.period.days) + " Days")                
        self.graph.xmax=(self.period.total_seconds() + self.x_offset)

        self.graph.ymin=self.min_y
        self.graph.ymax=self.max_y
        if self.draw_high_line:
            self.plot_high.points = self.high_line_values.copy()  
        if self.draw_low_line:
            self.plot_low.points = self.low_line_values.copy()
        if self.draw_day_night:
            self.day_night_plot.points = self.day_night_values.copy()  
        self.plot_data.points = self.data_line_values.copy()        
        self.graph._redraw_all()   
            
class TemperatureGraphGrid(GraphGrid):
    def __init__(self, **kwargs):
        super(TemperatureGraphGrid, self).__init__(**kwargs)
    
    def refresh_data(self):         
        nowDT = datetime.now()
        now_min = nowDT.minute
        now_sec = nowDT.second

        self.db_result_list = []
        self.db_result_list = db_temperature_data_new(block_label = self.data_source, \
                                                  endDT = nowDT, \
                                                  periodTD = self.period, \
                                                  db_result_list = self.db_result_list, \
                                                  db=self.db_source) 
        if self.draw_high_line:
            self.high_line_values = []
        if self.draw_low_line:
            self.low_line_values = []
        self.data_line_values = []
        
        self.x_offset = (now_min * 60) + now_sec
        
        self.min_y = 80
        self.max_y = -80
        self.low_line, self.high_line = get_temp_graph_low_high(self.data_source)
        
        for t, v in self.db_result_list:
            if v < TEMPERATURE_SANITY_HIGH and v > TEMPERATURE_SANITY_LOW:
                if v > self.max_y:
                    self.max_y = v
                if v < self.min_y:
                    self.min_y = v
                t_mod = t + self.x_offset
                self.data_line_values.append((t_mod,v))      
                self.high_line_values.append((t_mod,self.high_line)) 
                self.low_line_values.append((t_mod,self.low_line))
            else:
                print("(TemperatureGraphGrid.refreh_data) WARNING: Temperature value out of sanity range")            
        self.min_y, self.max_y = calc_temp_graph_y_grid(self.min_y, self.max_y, 2)
        if self.min_y >= self.low_line: self.min_y = self.low_line - 1
        if self.max_y <= self.high_line: self.max_y = self.high_line + 1
              
class DeviceGraphGrid(GraphGrid):
    def __init__(self, **kwargs):
        super(DeviceGraphGrid, self).__init__(**kwargs)

    def refresh_data(self):        
        nowDT = datetime.now()
        now_min = nowDT.minute
        now_sec = nowDT.second

        self.db_result_list = []
        self.db_result_list = db_device_data(block_label = self.data_source, \
                                             endDT = nowDT, \
                                             periodTD = self.period, \
                                             db_result_list = self.db_result_list, \
                                             db=self.db_source)  
        self.data_line_values = []
        self.x_offset = (now_min * 60) + now_sec
        self.min_y = -0.2
        self.max_y = 1.2
        for t, v in self.db_result_list:
            t_mod = t + self.x_offset
            self.data_line_values.append((t_mod,v))      
            
class EnergyGraphGrid(GraphGrid):
    def __init__(self, **kwargs):
        super(EnergyGraphGrid, self).__init__(**kwargs)

    def refresh_data(self):
        nowDT = datetime.now()
        now_min = nowDT.minute
        now_sec = nowDT.second
                    
        self.db_result_list = []
        self.db_result_list = db_energy_data(energy_message = self.data_source, \
                                             endDT = nowDT, \
                                             periodTD = self.period, \
                                             db_result_list = self.db_result_list, \
                                             db=self.db_source)         
        if self.draw_high_line:
            self.high_line_values = []
        if self.draw_low_line:
            self.low_line_values = []
        if self.draw_day_night:
            self.day_night_values = []
        self.data_line_values = []
        
        
        self.x_offset = (now_min * 60) + now_sec
        
        self.min_y = 30000
        self.max_y = -30000
        self.ylabel, self.low_line, self.high_line, \
            self.y_ticks_major = get_energy_graph_params(self.data_source)
                
        for t_index, v, _dt in self.db_result_list:
            if v > self.max_y:
                self.max_y = math.ceil(v)
            if v < self.min_y:
                self.min_y = math.floor(v)
            t_mod = t_index + self.x_offset
            self.data_line_values.append((t_mod,v))      
            self.high_line_values.append((t_mod,self.high_line)) 
            self.low_line_values.append((t_mod,self.low_line))
        
        if self.min_y >= self.low_line: self.min_y = self.low_line - (self.y_ticks_major / 2)
        if self.max_y <= self.high_line: self.max_y = self.high_line + (self.y_ticks_major / 2)
        
        if self.draw_day_night:
            last_point_was_daylight = False
            last_daylight_t_mod = 0
            draw_up = True
            for t_index, _v, dt in self.db_result_list:
                if (dt.hour >= 6) and (dt.hour < 18):
                    t_mod = t_index + self.x_offset
                    if last_point_was_daylight:
                        #step = int(self.graph.x_ticks_major * 24 / 800)
                        #day_light_pixel_width = 800 / self.period / 2
                        total_period_in_seconds = self.period.total_seconds()
                        seconds_in_one_pixel = total_period_in_seconds / 730
                        step =int(seconds_in_one_pixel)
                         
                        for t in range(last_daylight_t_mod, t_mod, step):
                            if draw_up:
                                self.day_night_values.append((t, self.min_y))
                                self.day_night_values.append((t, self.max_y))
                                #self.day_night_values.append((t, self.min_y))
                                draw_up = False
                            else:
                                self.day_night_values.append((t, self.max_y))
                                self.day_night_values.append((t, self.min_y))
                                #self.day_night_values.append((t, self.min_y))
                                draw_up = True
                    #self.day_night_values.append((t_mod, self.min_y))
                    #self.day_night_values.append((t_mod, self.max_y))
                    #self.day_night_values.append((t_mod, self.min_y))
                    last_point_was_daylight = True
                    last_daylight_t_mod = t_mod
                else: 
                    last_point_was_daylight = False
                    draw_up = True   

                     
class DataNavigationGrid(GridLayout):
    def __init__(self, **kwargs):
        super(DataNavigationGrid, self).__init__(**kwargs)
        self.cols = 5
        self.spacing = 10
        self.padding = [10, 10, 10, 10]
        self.size_hint_max_y = 80
        
    def setup(self, disp_name="", data_type="", data_source="", data_section=()):
        self.disp_name = disp_name
        self.data_type = data_type
        self.data_source = data_source
        self.data_section = data_section
        
        self.prev_button = NavPrevButton()
        self.prev_button.set_name('<< Prev')
        self.add_widget(self.prev_button)

        self.zoom_in_button = NavZoomButton()
        self.zoom_in_button.set_name("Zoom(+)")
        self.zoom_in_button.register_on_press_callback(self.data_section.adjust_period, -1)
        self.add_widget(self.zoom_in_button)

        self.zoom_out_button = NavZoomButton()
        self.zoom_out_button.set_name("Zoom(-)")
        self.zoom_out_button.register_on_press_callback(self.data_section.adjust_period, 1)
        self.add_widget(self.zoom_out_button)
        
        self.close_button = NavCloseToMainButton()
        self.close_button.set_name('Close')
        self.add_widget(self.close_button)

        self.next_button = NavNextButton()
        self.next_button.set_name('Next >>')
        self.add_widget(self.next_button)

                                           
class DataParentGrid(GridLayout):
    def __init__(self, **kwargs):
        super(DataParentGrid, self).__init__(**kwargs)
        self.cols = 1
        #self.rows = 1
        self.spacing = 0
        self.padding = [0, 0, 0, 0]
     
        self.bind(size=self.update_size)
    def update_size(self, _w, _h):
        with self.canvas.before:
            #self.rect = Rectangle(size=(800, 480), pos=self.pos, source='cows.png')
            self.rect = Rectangle(size=self.size, pos=self.pos, source=BACKGROUND_IMAGE)

        #self.title_section = ZoomHeaderGrid()
        #self.data_type = data_type
    def setup(self, disp_name="", data_source="", data_type="", init_period=1):
        if data_type == "INPUT":
            self.data_section = TemperatureGraphGrid()
            self.data_section.setup(disp_name=disp_name, data_source=data_source, \
                                    ylabel="Temperature", \
                                    y_ticks_major=2, \
                                    period_in_days=init_period)
            self.data_section.add_graph(draw_high_line=True, draw_low_line=True)
        
        elif data_type == "OUTPUT":
            self.data_section = DeviceGraphGrid()
            self.data_section.setup(disp_name=disp_name, data_source=data_source, \
                                    ylabel="OFF                                                    ON", \
                                    y_ticks_major=2, \
                                    period_in_days=init_period)
            self.data_section.add_graph()
        
        elif data_type == "ENERGY":
            self.data_section = EnergyGraphGrid()
            
            ylabel, _low_line, _high_line, \
                y_ticks_major = get_energy_graph_params(data_source)

            self.data_section.setup(disp_name=disp_name, data_source=data_source, \
                                    ylabel=ylabel, \
                                    y_ticks_major=y_ticks_major, \
                                    period_in_days=init_period)
            self.data_section.add_graph(draw_high_line=True, high_colour=FF_GREEN, \
                                        draw_low_line=True, low_colour=FF_RED)
        
        self.nav_section = DataNavigationGrid()
        self.nav_section.setup(disp_name=disp_name, data_type=data_type, \
                               data_section=self.data_section)
        
        self.add_widget(self.nav_section)
        self.add_widget(self.data_section) 



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
        
        self.background_color = FF_SLATE_LG
        self.color = FF_WHITE
        self.text = "\n[b]" + self.text + "[/b]"
    def set_name(self, name_str):
        self.name = name_str
        self.text = name_str    

class TextDisplayLabelBox(Label):
    def __init__(self, **kwargs):
        super(TextDisplayLabelBox, self).__init__(**kwargs)
        #self.text_size = self.size
        self.halign = "left"
        self.valign = "bottom"
        self.background_normal = ''
        #self.background_color = FF_GREY
        self.font_size = '14sp'
        #self.spacing = 10
        #self.padding = (10, 10)   
        #self.color = FF_WHITE
        self.name = ''
        self.text = self.name    
        
        self.background_color = FF_SLATE_LG
        self.color = FF_WHITE
        #self.text = "\n[b]" + self.text + "[/b]"
    def set_name(self, name_str):
        self.name = name_str
        self.text = name_str    



Builder.load_string('''
<SystemParentGrid>:    
    BoxLayout:
        orientation: "horizontal"
        height: 480
        width: 800
        spacing: 0
        padding: [0, 0, 0, 0]
        canvas.before:
            #source: 'cows.png'
            Color: 
                rgba: 0,0,0,1
            Rectangle:
                size: 800, 480
        ScrollView:
            MessageLogBox:
                size_hint: None, None
                size: self.texture_size 
        GridLayout:
            cols: 1
            NavCloseToMainButton:
                set_name: 'Close'
                text: 'Close'
                size_hint_y: 0.2
            ListTextField:    
            ScrollView:
                MessageLogBox:
                    size_hint: None, None
                    size: self.texture_size 
<ListTextField>:
    #halign = "left"
    #self.valign = "bottom"
    #self.background_normal = ''
    #self.background_color = FF_GREY
    font_size: '14sp'
    #spacing: 3
    #self.padding = (10, 10)   
    #self.color = FF_WHITE        
    #background_color: FF_SLATE_LG
    #color: FF_WHITE
    text: "ListTextField"
    
    
        
''')

'''
<Controller>:
    label_wid: my_custom_label

    BoxLayout:
        orientation: 'vertical'
        padding: 20

        Button:
            text: 'My controller info is: ' + root.info
            on_press: root.do_action()

        Label:
            id: my_custom_label
            text: 'My label before button press'
'''

class SystemParentGrid(BoxLayout):
    def __init__(self, **kwargs):
        super(SystemParentGrid, self).__init__(**kwargs)

        #def build(self):
        #    return self
        

        #self.message_log = TextInput(size_hint = (1, None))
        #self.message_log = TextDisplayLabelBox()
class ListTextField(Label):
    def __init__(self, **kwargs):
        super(ListTextField, self).__init__(**kwargs)
        self.background_color = FF_BLUE_LG
        self.color = FF_WHITE
        self.canvas.before.clear()
        #self.canvas.before:
        #self.canvas.before.Color(FF_BLUE_LG)
        #self.canvas.before.Rectangle(pos=self.pos, size=self.size)
        #self.background_color = FF_BLUE_LG
        #canvas_back = InstructionGroup()        
        #canvas_back.add(Color(0, 0, 1, 0.2))
        #canvas_back.add(Rectangle(pos=self.pos, size=(100, 100)))        
        #with self.canvas:
        #self.canvas.add(canvas_back)
        #    Rectangle(pos=self.pos, size=self.size)
            

class MessageLogBox(TextDisplayLabelBox):
    def __init__(self, **kwargs):
        super(MessageLogBox, self).__init__(**kwargs)
        self.log_result = []
        # add DB sources checks
        self.log_result = db_log_history(db_result_list=self.log_result)
        self.text = "TBA"
        self.lines = []
        #for row in self.log_result:
        #    self.text = self.text + "\n" + row
        
        #self.add_widget(self.message_log)

        
        

    