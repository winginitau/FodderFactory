'''
Created on 7 Jul. 2018

@author: brendan
'''

import serial
from datetime import datetime
from collections import deque
#from kivy.clock import Clock
from privatelib.ff_config import STATE_ON, STATE_OFF, ParsedMessage, \
                                 SERIAL_POLL_INTERVAL, MESSAGE_PARSE_INTERVAL, MESSAGE_BROKER_INTERVAL, \
                                 MESSAGE_FILENAME, MYSQL_POLL_INTERVAL, \
                                 INPUTS_LIST, OUTPUTS_LIST, ENERGY_LIST, RULES_TO_CATCH, \
                                 DB_WRITE_LOCAL, DB_WRITE_CLOUD, \
                                 DB_LOCAL_CONFIG, DB_CLOUD_CONFIG, \
                                 ui_inputs_values, ui_outputs_values, ui_energy_values, active_rules, \
                                 UI_UPDATE_FROM_MESSAGES, \
                                 UI_UPDATE_FROM_LOCAL_DB, UI_UPDATE_FROM_CLOUD_DB, \
                                 DB_CLOUD_QUEUE_MAX_LEN, \
                                 DB_LOCAL_QUEUE_MAX_LEN, \
                                 DB_CLOUD_QUEUE_TRIGGER_LEN, \
                                 DB_LOCAL_QUEUE_TRIGGER_LEN, \
                                 PARSED_MESSAGE_QUEUE_MAX_LEN, \
                                 SERIAL_MESSAGE_QUEUE_MAX_LEN, \
                                 DB_CLOUD_WORKER_INTERVAL, DB_LOCAL_WORKER_INTERVAL, \
                                 TEMPERATURE_SANITY_HIGH, TEMPERATURE_SANITY_LOW, \
                                 PRINT_MESSAGES

#from privatelib.global_vars import active_rules
from privatelib.db_funcs import db_add_log_entry, db_get_last_message_by_source, \
                                db_get_last_energy_message_by_string

from privatelib.app_msg_handler import AppMessageHandler

from threading import Thread
from time import sleep
#from kivy.clock import Clock



#msg_sys = MessageSystem();

    
    
     

class MessageSystem():
    def __init__(self, **kwargs):
        super(MessageSystem, self).__init__(**kwargs)
        self.last_message_time = datetime.now()
        self.message_ever_received = False
        self.valid_config = False
        self.tcp = False
        self.serial = False  
        self.serial_message_queue = deque()
        self.parsed_message_queue = deque()
        self.db_local_queue = deque()
        self.db_cloud_queue = deque()
        self.msMsg = AppMessageHandler()
    def configure(self, tcp=False, serial=False, serial_port='', serial_speed='', tcp_address='', tcp_port=''):
        if(tcp and serial):
            #print("ERROR: One serial or tcp data connection can be configured - not both")
            self.msMsg.appMessage("MessageSystem.configure()", self.msMsg.FATAL, \
                            "One serial or tcp data connection can be configured - not both")
            exit()        
        if (tcp):
            if (tcp_address and tcp_port):
                self.tcp=True
                self.tcp_address=tcp_address
                self.tcp_port=tcp_port
            else:
                self.msMsg.appMessage("MessageSystem.configure()", self.msMsg.ERROR, \
                                         "Configure TCP called without valid address and port")
                self.tcp = False
        if(serial):
            if (serial_port and serial_speed):
                self.serial = True
                self.serial_port=serial_port
                self.serial_speed=serial_speed
                if self.serial_open(self.serial_port, self.serial_speed):
                    #Clock.schedule_interval(self.serial_read_line, SERIAL_POLL_INTERVAL)
                    #print("Serial poll clock added")
                    ''' Start the serial_thread '''
                    self.serial_thread = Thread(target=self.serial_read_line, args=(0,))
                    self.serial_thread.setDaemon(True)
                    self.serial_thread.start()
                    self.msMsg.appMessage("MessageSystem.configure()", self.msMsg.INFO, \
                                    "Serial thread started")
                else: self.msMsg.appMessage("MessageSystem.configure()", self.msMsg.ERROR, \
                                               "Serial Port Not Opened - why??")
            else:
                self.msMsg.appMessage("MessageSystem.configure()", self.msMsg.ERROR, \
                                         "ERROR Configure serial called without valid port and speed")
                self.serial = False
          
        if (not self.tcp) and (not self.serial):
            self.msMsg.appMessage("MessageSystem.configure()", self.msMsg.WARN, \
                                     "No Message Source Configured - Live Data will Not be Displayed")

        ''' Start the message parsing thread '''
        self.parse_thread = Thread(target=self.parse_worker, args=(0,))
        self.parse_thread.setDaemon(True)
        self.parse_thread.start()
        self.msMsg.appMessage("MessageSystem.configure()", self.msMsg.INFO, \
                                 "Message parsing thread started")  

        ''' Start the message broker thread '''
        self.broker_thread = Thread(target=self.message_broker, args=(0,))
        self.broker_thread.setDaemon(True)
        self.broker_thread.start()
        self.msMsg.appMessage("MessageSystem.configure()", self.msMsg.INFO, \
                                 "Message broker thread started")

        ''' Start the db_local worker thread '''
        self.db_local_thread = Thread(target=self.db_local_worker, args=(0,))
        self.db_local_thread.setDaemon(True)
        self.db_local_thread.start()
        self.msMsg.appMessage("MessageSystem.configure()", self.msMsg.INFO, \
                                 "db_local worker threat started")

        ''' Start the db_cloud worker thread '''
        self.db_cloud_thread = Thread(target=self.db_cloud_worker, args=(0,))
        self.db_cloud_thread.setDaemon(True)
        self.db_cloud_thread.start()
        self.msMsg.appMessage("MessageSystem.configure()", self.msMsg.INFO, \
                                 "db_cloud worker threat started")

    def end(self):
        if self.serial:
            self.msMsg.appMessage("MessageSystem.end()", self.msMsg.INFO, \
                                     "Closing serial data connection")
            self.serial_close()
            #self.serial_connection.close()
    def setup_db_ui_source(self):
        self.ui_from_mysql_thread = Thread(target=self.get_db_ui_data, args=(0,))
        self.ui_from_mysql_thread.setDaemon(True)
        self.ui_from_mysql_thread.start()
    def get_db_ui_data(self, _dt):
        if UI_UPDATE_FROM_LOCAL_DB and UI_UPDATE_FROM_CLOUD_DB:
            self.msMsg.appMessage("MessageSystem.get_db_ui_data()", self.msMsg.FATAL, \
                                     "Multiple DBs configured for UI update")
            exit(1)
        if UI_UPDATE_FROM_LOCAL_DB:
            db_source = DB_LOCAL_CONFIG
        elif UI_UPDATE_FROM_CLOUD_DB:
            db_source = DB_CLOUD_CONFIG
        else:
            self.msMsg.appMessage("MessageSystem.get_db_ui_data()", self.msMsg.FATAL, \
                                     "ERROR: Attempting to start DB UI thread with no valid sources")
            exit(1)
        self.msMsg.appMessage("MessageSystem.get_db_ui_data()", self.msMsg.INFO, \
                                 "DB UI polling thread starting")            
        self.message_count = 0        
        while True:
            if self.message_count > 65000:
                self.message_count = 0
                self.msMsg.appMessage("MessageSystem.get_db_ui_data()", self.msMsg.INFO, \
                                         "Message Count > 65000, Reset to 0")
            any_valid_query = False
            message = ParsedMessage()
            try:
                for i, source, _disp, _period in INPUTS_LIST: 
                    message.valid = False
                    message = db_get_last_message_by_source(source, message, 10, db=db_source)
                    if message.valid:
                        if message.float_val < TEMPERATURE_SANITY_HIGH \
                           and message.float_val > TEMPERATURE_SANITY_LOW: 
                            ui_inputs_values[i] = message.float_val
                            any_valid_query = True
                        else:
                            self.msMsg.appMessage("MessageSystem.get_db_ui_data()", self.msMsg.WARN, \
                                                     "Temperature value out of sanity range")
                    else:
                        self.msMsg.appMessage("MessageSystem.get_db_ui_data()", self.msMsg.WARN, \
                                                 "DB query for: " + source + " Returned no valid messages")
    
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
                        self.msMsg.appMessage("MessageSystem.get_db_ui_data()", self.msMsg.WARN, \
                                                 "DB query for: " + source + " Returned no valid messages")
    
                for source, _disp, _period in ENERGY_LIST:
                    message.valid = False
                    message = db_get_last_energy_message_by_string(source, message, 10, db=db_source)
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
                        self.msMsg.appMessage("MessageSystem.get_db_ui_data()", self.msMsg.WARN, \
                                                 "DB query for: " + source + " Returned no valid messages")
                
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
                        self.msMsg.appMessage("MessageSystem.get_db_ui_data()", self.msMsg.WARN, \
                                                 "DB query for: " + rule + " Returned no valid messages")

                if any_valid_query:                                
                    self.last_message_time = datetime.now()
                    self.message_ever_received = True
                    self.message_count = self.message_count + 1
                    if (self.message_count % 1000 == 0):
                        self.msMsg.appMessage("MessageSystem.get_db_ui_data()", self.msMsg.INFO, \
                                                 "Message Count: " + str(self.message_count))
                else:
                    self.msMsg.appMessage("MessageSystem.get_db_ui_data()", self.msMsg.WARN, \
                                             "No valid message returned by db for for any UI queries")
            except Exception as e:
                self.msMsg.appMessage("(MessageSystem.get_db_ui_data()", self.msMsg.EXCEPT, \
                                         "Exception Raised Getting and Processing msMsg from MySQL")
                print(e)
            sleep(MYSQL_POLL_INTERVAL)

    def parse(self, parsed_message, raw_msg_string):
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
                self.msMsg.appMessage("MessageSystem.parse()", self.msMsg.WARN, \
                                         "Bad message decode")
                self.msMsg.appMessage("MessageSystem.parse()", self.msMsg.WARN, \
                                         "Raw Message: " + str(raw_msg_string))
            
        if (good_decode):
            try:
                message_dt = datetime.strptime((string_list[0] + " " + string_list[1]), '%Y-%m-%d %H:%M:%S')
                good_dt = True
            except:
                self.good_dt = False
                self.msMsg.appMessage("MessageSystem.parse()", self.msMsg.WARN, \
                                         "Bad date or time format in message")
                self.msMsg.appMessage("MessageSystem.parse()", self.msMsg.WARN, \
                                         "Raw Message: " + str(raw_msg_string))
                #print (self.string_list[0] + " " + self.string_list[1])
                 
        if (good_length and good_decode and good_dt):
            if not("DEBUG" in string_list):
                try:
                    parsed_message.raw_message_string = raw_msg_string
                    parsed_message.dt = message_dt
                    parsed_message.source_block_string = string_list[2]
                    parsed_message.dest_block_string = string_list[3]
                    parsed_message.msg_type_string = string_list[4]
                    parsed_message.msg_string = string_list[5]
                    parsed_message.int_val = int(string_list[6])
                    parsed_message.float_val = float(string_list[7])
                    parsed_message.time_rx = datetime.now()
                    parsed_message.valid = True
                except:
                    self.msMsg.appMessage("MessageSystem.parse()", self.msMsg.EXCEPT, \
                                             "Exception raised assigning decoded message to struct")
                    self.msMsg.appMessage("MessageSystem.parse()", self.msMsg.EXCEPT, \
                                             "Raw Message: " + str(raw_msg_string))
        return parsed_message


    def update_UI_values(self, parsed_message):
        if (parsed_message.valid == True):
            try:
                for i, source, _disp, _period in INPUTS_LIST:
                    if (parsed_message.source_block_string == source):
                        ui_inputs_values[i] = parsed_message.float_val
                for i, source, _disp, _period in OUTPUTS_LIST:
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
                
                for rule, disp in RULES_TO_CATCH:
                    if (parsed_message.source_block_string == rule):
                        if (parsed_message.msg_type_string == "ACTIVATED"):
                            if not(disp in active_rules):
                                active_rules.append(disp)
                        if (parsed_message.msg_type_string == "DEACTIVATED"):
                            try:
                                active_rules.remove(disp)
                            except:
                                self.msMsg.appMessage("MessageSystem.update_UI_values()", self.msMsg.WARN, \
                                                         "Attempted to Remove Non-Active Rule")                
            except:
                self.msMsg.appMessage("MessageSystem.update_UI_values()", self.msMsg.EXCEPT, \
                                         "Exception parsing message after sucessful decode")


    def parse_worker(self, _dt):
        ''' Message Parsing Thread '''
        self.message_count = 0
        while True:
            while len(self.serial_message_queue) > 0:
                if len(self.parsed_message_queue) < PARSED_MESSAGE_QUEUE_MAX_LEN:
                    new_parsed_message = ParsedMessage()
                    new_parsed_message = self.parse(new_parsed_message, self.serial_message_queue.popleft())
                    if (new_parsed_message.valid == True):
                        ''' Add it to the parsed_message_queue '''
                        self.parsed_message_queue.append(new_parsed_message)
                    if UI_UPDATE_FROM_MESSAGES:
                        self.update_UI_values(new_parsed_message)
                    self.last_message_time = datetime.now()
                    self.message_ever_received = True
                    self.message_count = self.message_count + 1
                    if (self.message_count % 1000 == 0):
                        self.msMsg.appMessage("MessageSystem.parse_worker()", self.msMsg.INFO, \
                                                 "Message Count: " + str(self.message_count))
                else:
                    self.msMsg.appMessage("MessageSystem.parse_worker()", self.msMsg.WARN, \
                                             "Parsed Message Queue at MAX Length.")
            ''' Keep the thread throttled '''
            sleep(MESSAGE_PARSE_INTERVAL)

    def print_queue_stats(self, start=True):
        ''' For Debugging '''
        if start: label = "(queue_stats_sta)"
        else: label = "(queue_stats_end)"
        print(label + " SerialQ:" + str(len(self.serial_message_queue)) \
              + " ParsedQ:" + str(len(self.parsed_message_queue)) \
              + " DBLocalQ:" + str(len(self.db_local_queue)) \
              + " DBCloudQ:" + str(len(self.db_cloud_queue)) )
                
    def message_broker(self, _dt):
        ''' 
        Single input queue, multiple output queue broker.
        Presently hard coded to:
            if db_local_queue not full:
                Pull from: parsed_message_queue
                if configured:
                    Push to:   self.db_local_queue (Blocking - if full, stop pulling)
            if configured:
              Push to:   file MESSAGE_FILENAME (if db_local_queue not blocked, no exceptions handled)
            if configured and db_local_queue not blocked:
              Push to:   self.db_cloud_queue (if db_cloud_queue full, then drop push)
        '''
        while True:
            #self.print_queue_stats(start=True)
            while len(self.parsed_message_queue) > 0:
                if len(self.db_local_queue) < DB_LOCAL_QUEUE_MAX_LEN:
                    parsed_message = self.parsed_message_queue.popleft()          
                    if parsed_message.valid:    
                        if DB_WRITE_LOCAL:
                            self.db_local_queue.append(parsed_message)
                        with open(MESSAGE_FILENAME, "a") as msg_log_file:
                            print(parsed_message.raw_message_string, file=msg_log_file)
                        if PRINT_MESSAGES:
                            print(" --- ", \
                                  parsed_message.dt, \
                                  parsed_message.source_block_string, parsed_message.dest_block_string, \
                                  parsed_message.msg_type_string, parsed_message.msg_string, \
                                  parsed_message.int_val, parsed_message.float_val)
                        if DB_WRITE_CLOUD:
                            if len(self.db_cloud_queue) < DB_CLOUD_QUEUE_MAX_LEN:
                                self.db_cloud_queue.append(parsed_message)
                            else:
                                self.msMsg.appMessage("MessageSystem.message_broker()", self.msMsg.WARN, \
                                                         "db_cloud_queue overrun - length exceeded DB_CLOUD_QUEUE_MAX_LEN")
                else:
                    self.msMsg.appMessage("MessageSystem.message_broker()", self.msMsg.WARN, \
                                             "db_local_queue full. Blocking.")
            #self.print_queue_stats(start=False)
            sleep(MESSAGE_BROKER_INTERVAL)
            
    def db_cloud_worker(self, _dt):
        while True:            
            if len(self.db_cloud_queue) > DB_CLOUD_QUEUE_TRIGGER_LEN:
                try:
                    db_add_log_entry(self.db_cloud_queue, db=DB_CLOUD_CONFIG)
                except Exception as e:
                    self.msMsg.appMessage("MessageSystem.db_cloud_worker()", self.msMsg.EXCEPT, \
                                             "Exception calling (db_add_log_entry)")
                    self.msMsg.appMessage("MessageSystem.db_cloud_worker()", self.msMsg.EXCEPT, \
                                             "Exception: " + str(e))
            sleep(DB_CLOUD_WORKER_INTERVAL)    

    def db_local_worker(self, _dt):
        while True:
            if len(self.db_local_queue) > DB_LOCAL_QUEUE_TRIGGER_LEN:
                try:
                    db_add_log_entry(self.db_local_queue, db=DB_LOCAL_CONFIG)    
                except Exception as e:
                    self.msMsg.appMessage("MessageSystem.db_local_worker()", self.msMsg.EXCEPT, \
                                             "Exception calling (db_add_log_entry)")
                    self.msMsg.appMessage("MessageSystem.db_local_worker()", self.msMsg.EXCEPT, \
                                             "Exception: " + str(e))
            sleep(DB_LOCAL_WORKER_INTERVAL)
                            
    def serial_open(self, port, speed):
        # Connect to serial port first
        try:
            self.msMsg.appMessage("MessageSystem.serial_open()", self.msMsg.INFO, \
                                     "Connecting to Serial Port " + port + " at " + str(speed))
            self.serial_connection = serial.Serial(port, speed)
            return True
        except Exception as e:
            self.msMsg.appMessage("MessageSystem.serial_open()", self.msMsg.FATAL, \
                                     "Failed to connect to serial port")
            print(e)
            self.serial = False
            exit()
            
    def serial_close(self):
        # Close serial communication
        self.serial_connection.close()
        self.msMsg.appMessage("MessageSystem.serial_close()", self.msMsg.INFO, \
                                 "Closed Serial Port Connection")

    def serial_read_line(self, _dt):
        while True:
            if (self.serial_connection.inWaiting() > 0):
                try:
                    if (len(self.serial_message_queue) < SERIAL_MESSAGE_QUEUE_MAX_LEN):
                        message = self.serial_connection.readline()
                        self.serial_message_queue.append(message)
                    else:
                        self.msMsg.appMessage("MessageSystem.serial_read_line()", self.msMsg.WARN, \
                                                 "Serial Queue Full. UART may overrun.")
                except:
                    self.msMsg.appMessage("MessageSystem.serial_read_line()", self.msMsg.FATAL, \
                                             "Error reading from serial port")
                    self.serial = False
                    exit()
            else:
                sleep(SERIAL_POLL_INTERVAL)

