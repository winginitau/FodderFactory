'''
Created on 9 Jul. 2018

@author: brendan
'''

from kivy.uix.gridlayout import GridLayout
from privatelib.ff_config import INPUTS_LIST, OUTPUTS_LIST, CLOCK_UPDATE_INTERVAL,\
                                 DATA_PARSE_INTERVAL, GRAPH_UPDATE_INTERVAL, \
                                 DB_INPUT_LOW_HIGH, DB_ENERGY_GRAPH_PARAMS
from kivy.clock import Clock
from graph import Graph
from graph import MeshLinePlot
from kivy.graphics import Rectangle
from datetime import datetime, timedelta


from privatelib.buttons import FFTempButton, FFDeviceButton, FFEnergyButton, \
                                TimeButton, MessageIndicatorButton, \
                                NavPrevButton, NavZoomButton, NavCloseToMainButton, \
                                NavNextButton
from privatelib.db_funcs import db_temperature_data ,db_device_data, \
                                db_energy_data
                               
from privatelib.palette import \
FF_WHITE, \
FF_BLACK_MG, \
FF_WHITE_LG, \
FF_RED, \
FF_GREEN, \
FF_BLUE
#FF_GREY_HG, \
#FF_GREY_LG, \
#FF_BLUE_LG, \
#FF_SLATE_LG, \
#FF_WHITE_HG, \
#FF_RED_LG, \
#FF_GREEN_LG, \
#FF_YELLOW_LG, \
#FF_COLD_BLUE_LG, \
#FF_FREEZE_BLUE_LG, \
#FF_BLACK_LG, \
#FF_BLACK_HG, \
#FF_BLACK, \
#FF_RED_HG, \
#FF_GREEN_HG, \
#FF_BLUE_HG, \
#FF_COLD_BLUE_HG, \
#FF_COLD_BLUE, \
#FF_FREEZE_BLUE_HG, \
#FF_FREEZE_BLUE, \
#FF_YELLOW_HG, \
#FF_YELLOW, \
#FF_GREY, \
#FF_SAND_LG, \
#FF_SAND_HG, \
#FF_SAND, \
#FF_SLATE_HG, \
#FF_SLATE
                                


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
        
        Clock.schedule_interval(self.current_time.update, CLOCK_UPDATE_INTERVAL)
        Clock.schedule_interval(self.last_rx_status.update, DATA_PARSE_INTERVAL)

class MainParentGrid(GridLayout):
    def __init__(self, **kwargs):
        super(MainParentGrid, self).__init__(**kwargs)
        self.cols = 1
        with self.canvas.before:
            self.rect = Rectangle(size=(800, 480), pos=self.pos, source='cows.png')

        self.temperature_section = MainTopGrid()
        self.energy_section = MainMiddleGrid()
        self.nav_section = MainBottomGrid()
        
        self.add_widget(self.temperature_section)
        self.add_widget(self.energy_section)
        self.add_widget(self.nav_section)
        

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
    def add_graph(self, draw_high_line=False, high_colour=FF_RED, draw_low_line=False, low_colour=FF_BLUE):
        self.draw_high_line = draw_high_line
        self.draw_low_line = draw_low_line
        if self.draw_high_line:
            self.high_line_values = []
        if self.draw_low_line:
            self.low_line_values = []
        self.data_line_values = []
        self.refresh_data()        
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
        self.plot_data = MeshLinePlot(color=FF_WHITE)        
        self.plot_data.points = self.data_line_values.copy()
        self.graph.add_plot(self.plot_data)
        self.update(0)
        Clock.schedule_interval(self.update, GRAPH_UPDATE_INTERVAL)
    def update(self, _dt):
        self.refresh_data()     
        self.graph.xlabel=(self.disp_name + " - " + str(self.period.days) + " Days")                
        self.graph.xmax=(self.period.total_seconds() + self.x_offset)
        
        if self.period.days == 1:
            self.graph.x_ticks_major=3600   # 1 per 1 hours = 24 lines
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
        
        self.graph.ymin=self.min_y
        self.graph.ymax=self.max_y
        if self.draw_high_line:
            self.plot_high.points = self.high_line_values.copy()  
        if self.draw_low_line:
            self.plot_low.points = self.low_line_values.copy()  
        self.plot_data.points = self.data_line_values.copy()        
        self.graph._redraw_all()   
            
class TemperatureGraphGrid(GraphGrid):
    def __init__(self, **kwargs):
        super(TemperatureGraphGrid, self).__init__(**kwargs)
    
    def refresh_data(self):         
        self.db_result_list = []
        self.db_result_list = db_temperature_data(block_label = self.data_source, \
                                                  periodTD = self.period, \
                                                  db_result_list = self.db_result_list) 
        
        if self.draw_high_line:
            self.high_line_values = []
        if self.draw_low_line:
            self.low_line_values = []
        self.data_line_values = []

        nowDT = datetime.now()
        now_min = nowDT.minute
        now_sec = nowDT.second
        
        self.x_offset = (now_min * 60) + now_sec
        
        self.min_y = 80
        self.max_y = -80
        self.low_line, self.high_line = get_temp_graph_low_high(self.data_source)
        
        for t, v in self.db_result_list:
            if v > self.max_y:
                self.max_y = v
            if v < self.min_y:
                self.min_y = v
            t_mod = t + self.x_offset
            self.data_line_values.append((t_mod,v))      
            self.high_line_values.append((t_mod,self.high_line)) 
            self.low_line_values.append((t_mod,self.low_line))            
        self.min_y, self.max_y = calc_temp_graph_y_grid(self.min_y, self.max_y, 2)
        if self.min_y >= self.low_line: self.min_y = self.low_line - 1
        if self.max_y <= self.high_line: self.max_y = self.high_line + 1
              
class DeviceGraphGrid(GraphGrid):
    def __init__(self, **kwargs):
        super(DeviceGraphGrid, self).__init__(**kwargs)

    def refresh_data(self):        
        self.db_result_list = []
        self.db_result_list = db_device_data(block_label = self.data_source, \
                                             periodTD = self.period, \
                                             db_result_list = self.db_result_list)  
        self.data_line_values = []
        nowDT = datetime.now()
        now_min = nowDT.minute
        now_sec = nowDT.second
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
        self.db_result_list = []
        self.db_result_list = db_energy_data(energy_message = self.data_source, \
                                             periodTD = self.period, \
                                             db_result_list = self.db_result_list)         
        if self.draw_high_line:
            self.high_line_values = []
        if self.draw_low_line:
            self.low_line_values = []
        self.data_line_values = []
        
        nowDT = datetime.now()
        now_min = nowDT.minute
        now_sec = nowDT.second
        
        self.x_offset = (now_min * 60) + now_sec
        
        self.min_y = 30000
        self.max_y = -30000
        self.ylabel, self.low_line, self.high_line, \
            self.y_ticks_major = get_energy_graph_params(self.data_source)
                
        for t, v in self.db_result_list:
            if v > self.max_y:
                self.max_y = v
            if v < self.min_y:
                self.min_y = v
            t_mod = t + self.x_offset
            self.data_line_values.append((t_mod,v))      
            self.high_line_values.append((t_mod,self.high_line)) 
            self.low_line_values.append((t_mod,self.low_line))
                     
        #self.min_y, self.max_y = calc_temp_graph_y_grid(self.min_y, self.max_y, 2)
        
        if self.min_y >= self.low_line: self.min_y = self.low_line - (self.y_ticks_major / 2)
        if self.max_y <= self.high_line: self.max_y = self.high_line + (self.y_ticks_major / 2)
                     
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
     
        with self.canvas.before:
            self.rect = Rectangle(size=(800, 480), pos=self.pos, source='cows.png')

        #self.title_section = ZoomHeaderGrid()
        #self.data_type = data_type
    def setup(self, disp_name="", data_source="", data_type=""):
        if data_type == "INPUT":
            self.data_section = TemperatureGraphGrid()
            self.data_section.setup(disp_name=disp_name, data_source=data_source, \
                                    ylabel="Temperature", \
                                    y_ticks_major=2, \
                                    period_in_days=1)
            self.data_section.add_graph(draw_high_line=True, draw_low_line=True)
        
        elif data_type == "OUTPUT":
            self.data_section = DeviceGraphGrid()
            self.data_section.setup(disp_name=disp_name, data_source=data_source, \
                                    ylabel="OFF                                                    ON", \
                                    y_ticks_major=2, \
                                    period_in_days=1)
            self.data_section.add_graph()
        
        elif data_type == "ENERGY":
            self.data_section = EnergyGraphGrid()
            self.data_section.setup(disp_name=disp_name, data_source=data_source, \
                                    ylabel="Energy", period_in_days=5)
            self.data_section.add_graph(draw_high_line=True, high_colour=FF_GREEN, \
                                        draw_low_line=True, low_colour=FF_RED)
        
        self.nav_section = DataNavigationGrid()
        self.nav_section.setup(disp_name=disp_name, data_type=data_type, \
                               data_section=self.data_section)
        
        self.add_widget(self.nav_section)
        self.add_widget(self.data_section) 
