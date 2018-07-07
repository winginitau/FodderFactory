'''
Created on 7 Jul. 2018

@author: brendan
'''

import serial
from datetime import datetime, date, time
from collections import deque
from kivy.clock import Clock
from privatelib.ff_config import UINT16_INIT, FLOAT_INIT, SERIAL_POLL_INTERVAL, DATA_PARSE_INTERVAL, MESSAGE_FILENAME, \
                                 INPUTS_LIST, OUTPUTS_LIST, STATE_ON, STATE_OFF, \
                                 ui_inputs_values, ui_outputs_values, ui_energy_values
from privatelib.db_funcs import db_add_log_entry


class ParsedMessage():
    def __init__(self, **kwargs):
        super(ParsedMessage, self).__init__(**kwargs)
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

class MessageSystem():
    def __init__(self, **kwargs):
        super(MessageSystem, self).__init__(**kwargs)
        self.last_message_time = datetime.now()
        self.message_ever_received = False
        self.message_queue = deque()
        self.valid_config = False
        self.tcp = False
        self.serial = False    
        
    def configure(self, tcp=False, serial=False, serial_port='', serial_speed='', tcp_address='', tcp_port=''):
        if(tcp and serial):
            print("ERROR: One serial or tcp data connection can be configured - not both")
            exit()        
        if (tcp):
            if (tcp_address and tcp_port):
                self.tcp=True
                self.tcp_address=tcp_address
                self.tcp_port=tcp_port
            else:
                print("ERROR: Configure TCP called without valid address and port")
                self.tcp = False
        if(serial):
            if (serial_port and serial_speed):
                self.serial = True
                self.serial_port=serial_port
                self.serial_speed=serial_speed
                if self.serial_open(self.serial_port, self.serial_speed):
                    Clock.schedule_interval(self.serial_read_line, SERIAL_POLL_INTERVAL)
                    print("Serial poll clock added")
                else: print("Serial Port Not Opened - why??")
            else:
                print("ERROR Configure serial called without valid port and speed")
                self.serial = False
          
        if (not self.tcp) and (not self.serial):
            print("WARNING: No Message Source Configured - Live Data will Not be Displayed")

        Clock.schedule_interval(self.parse_and_update, DATA_PARSE_INTERVAL)
        print("Message parsing loop added on clock")            

    def end(self):
        if self.serial:
            self.serial_connection.close()
            
    def parse(self, raw_msg_string):
        self.parsed_message = ParsedMessage()
        self.parsed_message.valid = False
        self.good_length = False
        self.good_decode = False
        self.good_dt = False
    
        #print (raw_msg_string)
        if len(raw_msg_string) > 30:
            self.good_length = True
            try:
                self.string_list = raw_msg_string.decode().split(",")
                self.good_decode = True
            except:
                print("Bad message decode")
            
        if (self.good_decode):
            try:
                self.message_dt = datetime.strptime((self.string_list[0] + " " + self.string_list[1]), '%Y-%m-%d %H:%M:%S')
                self.good_dt = True
            except:
                self.good_dt = False
                print ("Bad date or time format in message")
                print (self.string_list[0] + " " + self.string_list[1])
                 
        if (self.good_length and self.good_decode and self.good_dt):
            if not("DEBUG" in self.string_list):
                self.parsed_message.dt = self.message_dt
                self.parsed_message.source_block_string = self.string_list[2]
                self.parsed_message.dest_block_string = self.string_list[3]
                self.parsed_message.msg_type_string = self.string_list[4]
                self.parsed_message.msg_string = self.string_list[5]
                self.parsed_message.int_val = int(self.string_list[6])
                self.parsed_message.float_val = float(self.string_list[7])
                self.parsed_message.valid = True
                self.parsed_message.time_rx = datetime.now()
                with open(MESSAGE_FILENAME, "a") as msg_log_file:
                    print(raw_msg_string, file=msg_log_file)
                print(self.parsed_message.dt, \
                      self.parsed_message.source_block_string, self.parsed_message.dest_block_string, \
                      self.parsed_message.msg_type_string, self.parsed_message.msg_string, \
                      self.parsed_message.int_val, self.parsed_message.float_val)
             
                db_add_log_entry(self.parsed_message)    
                
        return self.parsed_message


    def parse_and_update(self, _dt):
        self.parsed_message = ParsedMessage()
        while (len(self.message_queue) > 0):
            self.parsed_message = self.parse(self.message_queue.popleft())
            if (self.parsed_message.valid == True):
                for i, source, _disp in INPUTS_LIST:
                    if (self.parsed_message.source_block_string == source):
                        ui_inputs_values[i] = self.parsed_message.float_val
                for i, source, _disp in OUTPUTS_LIST:
                    if (self.parsed_message.source_block_string == source):
                        if (self.parsed_message.msg_type_string == "ACTIVATED"):        
                            ui_outputs_values[i] = STATE_ON
                        elif (self.parsed_message.msg_type_string == "DEACTIVATED"):        
                            ui_outputs_values[i] = STATE_OFF
                if (self.parsed_message.msg_string == "VE_DATA_SOC"):
                    ui_energy_values[0] = float(self.parsed_message.int_val) / 10
                if (self.parsed_message.msg_string == "VE_DATA_VOLTAGE"):
                    ui_energy_values[1] = float(self.parsed_message.int_val) / 1000
                if (self.parsed_message.msg_string == "VE_DATA_POWER"):
                    ui_energy_values[2] = float(self.parsed_message.int_val)
                if (self.parsed_message.msg_string == "VE_DATA_CURRENT"):
                    ui_energy_values[3] = float(self.parsed_message.int_val) / 1000
                self.last_message_time = datetime.now()
                self.message_ever_received = True

    def serial_open(self, port, speed):
        # Connect to serial port first
        try:
            print("Connecting to Serial Port " + port + " at " + str(speed))
            self.serial_connection = serial.Serial(port, speed)
            return True
        except Exception as e:
            print ("Failed to connect to serial port")
            print(e)
            self.serial = False
            exit()
            
    def serial_close(self):
        # Close serial communication
        print("Closing Serial Port Connection")
        self.serial_connection.close()

    def serial_read_line(self, _dt):
        if (self.serial_connection.inWaiting() > 0):
            try:
                self.message_queue.append(self.serial_connection.readline())
            except:
                print ("Error reading from serial port")
                self.serial = False
                exit()
