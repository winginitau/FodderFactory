'''
Created on 7 Jul. 2018

@author: brendan
'''

import serial
from datetime import datetime, date, time
from collections import deque
#from kivy.clock import Clock
from privatelib.ff_config import UINT16_INIT, FLOAT_INIT, STATE_ON, STATE_OFF, ParsedMessage, \
                                 SERIAL_POLL_INTERVAL, DATA_PARSE_INTERVAL, \
                                 MESSAGE_FILENAME, MAX_MESSAGE_QUEUE, MYSQL_POLL_INTERVAL, \
                                 INPUTS_LIST, OUTPUTS_LIST, ENERGY_LIST, RULES_TO_CATCH, \
                                 DB_WRITE_LOCAL, DB_WRITE_CLOUD, \
                                 DB_LOCAL_CONFIG, DB_CLOUD_CONFIG, \
                                 ui_inputs_values, ui_outputs_values, ui_energy_values, active_rules, \
                                 UI_UPDATE_FROM_MESSAGES, \
                                 UI_UPDATE_FROM_LOCAL_DB, UI_UPDATE_FROM_CLOUD_DB, \
                                 DB_CLOUD_BUFFER_SIZE

#from privatelib.global_vars import active_rules
from privatelib.db_funcs import db_add_log_entry, db_get_last_message_by_source, \
                                db_get_last_energy_message_by_string
from threading import Thread
from time import sleep
from kivy.clock import Clock


#msg_sys = MessageSystem();


class MessageSystem():
    def __init__(self, **kwargs):
        super(MessageSystem, self).__init__(**kwargs)
        self.last_message_time = datetime.now()
        self.message_ever_received = False
        self.message_queue = deque()
        self.valid_config = False
        self.tcp = False
        self.serial = False  
        self.db_local_buffer = deque()
        self.db_cloud_buffer = deque()
         
        
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
                    #Clock.schedule_interval(self.serial_read_line, SERIAL_POLL_INTERVAL)
                    #print("Serial poll clock added")
                    self.serial_thread = Thread(target=self.serial_read_line, args=(0,))
                    self.serial_thread.setDaemon(True)
                    self.serial_thread.start()
                    print("Serial thread started")
                else: print("Serial Port Not Opened - why??")
            else:
                print("ERROR Configure serial called without valid port and speed")
                self.serial = False
          
        if (not self.tcp) and (not self.serial):
            print("WARNING: No Message Source Configured - Live Data will Not be Displayed")

        self.parse_thread = Thread(target=self.parse_and_update, args=(0,))
        self.parse_thread.setDaemon(True)
        self.parse_thread.start()
        print("Message parsing thread started")            

    def end(self):
        if self.serial:
            self.serial_close()
            #self.serial_connection.close()
    def setup_db_ui_source(self):
        self.ui_from_mysql_thread = Thread(target=self.get_db_ui_data, args=(0,))
        self.ui_from_mysql_thread.setDaemon(True)
        self.ui_from_mysql_thread.start()
    def get_db_ui_data(self, _dt):
        if UI_UPDATE_FROM_LOCAL_DB and UI_UPDATE_FROM_CLOUD_DB:
            print("ERROR: Multiple DBs configured for UI update")
            exit(1)
        if UI_UPDATE_FROM_LOCAL_DB:
            db_source = DB_LOCAL_CONFIG
        elif UI_UPDATE_FROM_CLOUD_DB:
            db_source = DB_CLOUD_CONFIG
        else:
            print("ERROR: Attempting to start DB UI thread with no valid sources")
            exit(1)
        print("DB UI polling thread starting")            
        while True:
            any_valid_query = False
            message = ParsedMessage()
            try:
                for i, source, _disp, _period in INPUTS_LIST: 
                    message.valid = False
                    message = db_get_last_message_by_source(source, message, 5, db=db_source)
                    if message.valid:
                        ui_inputs_values[i] = message.float_val
                        any_valid_query = True
                    else:
                        print("WARNING: DB query for: " + source + " Returned no valid messages")
    
                for i, source, _disp, _period in OUTPUTS_LIST:
                    message.valid = False
                    message = db_get_last_message_by_source(source, message, 24*60, db=db_source)
                    if message.valid:
                        if (message.msg_type_string == "ACTIVATED"):        
                            ui_outputs_values[i] = STATE_ON
                        elif (message.msg_type_string == "DEACTIVATED"):        
                            ui_outputs_values[i] = STATE_OFF
                        any_valid_query = True
                    else:
                        print("WARNING: DB query for: " + source + " Returned no valid messages")
    
                for source, _disp, _period in ENERGY_LIST:
                    message.valid = False
                    message = db_get_last_energy_message_by_string(source, message, 5, db=db_source)
                    if message.valid:
                        if (message.msg_string == "VE_DATA_SOC"):
                            ui_energy_values[0] = float(message.int_val) / 10
                        if (message.msg_string == "VE_DATA_VOLTAGE"):
                            ui_energy_values[1] = float(message.int_val) / 1000
                        if (message.msg_string == "VE_DATA_CURRENT"):
                            ui_energy_values[3] = float(message.int_val) / 1000
                        if (message.msg_string == "VE_DATA_POWER"):
                            ui_energy_values[2] = float(message.int_val)
                        any_valid_query = True
                    else:
                        print("WARNING: DB query for: " + source + " Returned no valid messages")
                
                for rule, disp in RULES_TO_CATCH:
                    message.valid = False
                    message = db_get_last_message_by_source(rule, message, 24*60, db=db_source)
                    if message.valid:
                        if (message.source_block_string == rule):
                            if (message.msg_type_string == "ACTIVATED"):
                                if not(disp in active_rules):
                                    active_rules.append(disp)
                            if (message.msg_type_string == "DEACTIVATED"):
                                try:
                                    active_rules.remove(disp)
                                except:
                                    pass
                                    #print("Attempted to Remove Non-Active Rule")
                        any_valid_query = True
                    else:
                        print("WARNING: DB query for: " + rule + " Returned no valid messages")

                if any_valid_query:                                
                    self.last_message_time = datetime.now()
                    self.message_ever_received = True
                    self.message_count = self.message_count + 1
                    if (self.message_count % 1000 == 0):
                        print("Message Count: " + str(self.message_count))
                else:
                    print("No valid message returned by db for for any UI queries")
            except:
                print("Error: Exception Raised Getting and Processing Mmessages from MySQL")
            sleep(MYSQL_POLL_INTERVAL)

            
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
                print("Raw Message: " + str(raw_msg_string))
            
        if (self.good_decode):
            try:
                self.message_dt = datetime.strptime((self.string_list[0] + " " + self.string_list[1]), '%Y-%m-%d %H:%M:%S')
                self.good_dt = True
            except:
                self.good_dt = False
                print ("Bad date or time format in message")
                print("Raw Message: " + str(raw_msg_string))
                #print (self.string_list[0] + " " + self.string_list[1])
                 
        if (self.good_length and self.good_decode and self.good_dt):
            if not("DEBUG" in self.string_list):
                try:
                    self.parsed_message.dt = self.message_dt
                    self.parsed_message.source_block_string = self.string_list[2]
                    self.parsed_message.dest_block_string = self.string_list[3]
                    self.parsed_message.msg_type_string = self.string_list[4]
                    self.parsed_message.msg_string = self.string_list[5]
                    self.parsed_message.int_val = int(self.string_list[6])
                    self.parsed_message.float_val = float(self.string_list[7])
                    self.parsed_message.valid = True
                    self.parsed_message.time_rx = datetime.now()
                except:
                    print("Error: Exception raised assigning decoded message to struct")
                    print("Raw Message: " + str(raw_msg_string))

                if self.parsed_message.valid:    
                    with open(MESSAGE_FILENAME, "a") as msg_log_file:
                        print(raw_msg_string, file=msg_log_file)
                    print(self.parsed_message.dt, \
                          self.parsed_message.source_block_string, self.parsed_message.dest_block_string, \
                          self.parsed_message.msg_type_string, self.parsed_message.msg_string, \
                          self.parsed_message.int_val, self.parsed_message.float_val)
                    if DB_WRITE_LOCAL:
                        self.db_local_buffer.append(self.parsed_message)
                        if len(self.db_local_buffer) > 0:
                            db_add_log_entry(self.db_local_buffer, db=DB_LOCAL_CONFIG)    
                    if DB_WRITE_CLOUD:
                        self.db_cloud_buffer.append(self.parsed_message)
                        if len(self.db_cloud_buffer) > DB_CLOUD_BUFFER_SIZE:
                            db_add_log_entry(self.db_cloud_buffer, db=DB_CLOUD_CONFIG)    
        return self.parsed_message


    def parse_and_update(self, _dt):
        self.message_count = 0
        while True:
            if (len(self.message_queue) > 0):
                #self.parsed_message = ParsedMessage()
                self.parsed_message = self.parse(self.message_queue.popleft())
                if UI_UPDATE_FROM_MESSAGES:
                    if (self.parsed_message.valid == True):
                        try:
                            for i, source, _disp, _period in INPUTS_LIST:
                                if (self.parsed_message.source_block_string == source):
                                    ui_inputs_values[i] = self.parsed_message.float_val
                            for i, source, _disp, _period in OUTPUTS_LIST:
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
                            
                            for rule, disp in RULES_TO_CATCH:
                                if (self.parsed_message.source_block_string == rule):
                                    if (self.parsed_message.msg_type_string == "ACTIVATED"):
                                        if not(disp in active_rules):
                                            active_rules.append(disp)
                                    if (self.parsed_message.msg_type_string == "DEACTIVATED"):
                                        try:
                                            active_rules.remove(disp)
                                        except:
                                            print("Attempted to Remove Non-Active Rule")
                                                            
                            
                            self.last_message_time = datetime.now()
                            self.message_ever_received = True
                            self.message_count = self.message_count + 1
                            if (self.message_count % 1000 == 0):
                                print("Message Count: " + str(self.message_count))
                        except:
                            print("Error: Exception parsing message after sucessful decode")
            else:
                sleep(DATA_PARSE_INTERVAL)
                
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
        while True:
            if (self.serial_connection.inWaiting() > 0):
                if (len(self.message_queue) < MAX_MESSAGE_QUEUE):
                    try:
                        self.message_queue.append(self.serial_connection.readline())
                    except:
                        print ("Error reading from serial port")
                        self.serial = False
                        exit()
            else:
                sleep(SERIAL_POLL_INTERVAL)

