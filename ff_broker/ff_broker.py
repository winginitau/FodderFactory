
from serial import Serial
from collections import deque
from time import sleep
from enum import Enum
from threading import Thread
from datetime import datetime, timedelta


SERIAL_POLL_INTERVAL = 0.1      # Seconds between hardware serial polls pushing to serial queue
BROKER_POLL_INTERVAL = 0.05
SERIAL_TIMING_DELAY = 3.0
MESSAGE_PARSE_INTERVAL = 1.0    # clearning serial queue and pushing to parse queue and updating UI data set
MESSAGE_BROKER_INTERVAL = 1.0   # clearing parse queue and pushing to DB and file queues
DB_LOCAL_WORKER_INTERVAL = 5.0        # Seconds between write attempts to the DBs 
DB_CLOUD_WORKER_INTERVAL = 15.0
UI_REFRESH_INTERVAL = 1.0       # Seconds wait between updating display from UI data
GRAPH_UPDATE_INTERVAL = 120.0   # Seconds - to re-request graph data from DB and update
CLOCK_UPDATE_INTERVAL = 1.0     # Literally the clock display update

MYSQL_POLL_INTERVAL = 3.0        # If enabled, how often UI data set is refreshed from the DB

class InterfaceModes(Enum):
    CHAR = 0
    LINE = 1
    BINARY = 2
    
class Utils():
    def __init__(self, **kwargs):
        super(Utils, self).__init__(**kwargs)
    def timeout_start(self, timeout_period=5):
        self.time_start = datetime.now()
        self.time_period = timedelta(seconds = timeout_period)
    def timeout(self):
        if (datetime.now() - self.time_start) > self.time_period:
            return True 
        else: return False 
    
    
class Interface():
    def __init__(self, **kwargs):
        super(Interface, self).__init__(**kwargs)
        self.id = ""
        self.description = ""
        self.hanging_prompts = False;
        self.txQ = deque()
        self.rxQ = deque()
        self.rxQMax = 2000
        self.txQMax = 2000
        self.errorLog = deque()
        self.mode = InterfaceModes.LINE
        self.connected = False 
        self.input_array = bytearray()
        self.output_array = bytearray()
        self.byte_count = 0
    def changeMode(self, mode):
        self.mode = mode
        
class LinuxSerialPort(Interface):
    def __init__(self, **kwargs):
        super(LinuxSerialPort, self).__init__(**kwargs)
        self.lastSerialWriteDT = datetime.now()
        self.lastSerialReadDT = datetime.now()
    def config(self, device="/dev/null", speed=9600):
        self.device = device
        self.speed = speed
    def connect(self):
        self.connection = Serial(self.device, self.speed)
        #sleep(0.3)
        #self.connection.open()
        if self.connection.is_open: 
            self.connected = True
            print("LinuxSerialPort Open")
            return True 
    def close(self):
        self.connection.close()
        self.connected = False
    def timeSinceLastRXTX(self):
        if self.lastSerialWriteDT > self.lastSerialReadDT:
            self.idle_time = datetime.now() - self.lastSerialWriteDT
        else:
            self.idle_time = datetime.now() -self.lastSerialReadDT
        return self.idle_time.total_seconds()
    def timeSinceLastTX(self):
        self.idle_tx = datetime.now() - self.lastSerialWriteDT
        return
    def readAvailableToQueue(self):
        self.TDSinceLastWrite = datetime.now() - self.lastSerialWriteDT
        if self.TDSinceLastWrite.seconds > 2:
            sleep(.5)
            if (self.connection.inWaiting() > 0):
                try:
                    if (len(self.rxQ) < self.rxQMax):
                            self.input_string = self.connection.readline(timeout=1)
                            self.lastSerialReadDT = datetime.now() 
                            self.input_string = self.input_string.decode()
                            self.input_string = self.input_string.rstrip('\n\r')
                            self.rxQ.append(self.input_string)
                            sleep(0.5)
                        
                    else:
                        self.errorLog.append(self.name, "Serial RX Queue Full. UART may overrun.")
                except:
                    self.errorLog.append("Error reading from serial port")

    def writeQueueToSerial(self):
        if (len(self.txQ) >0):
            try:
                self.TDSinceLastRead = datetime.now() - self.lastSerialReadDT
                self.TDSinceLastWrite = datetime.now() - self.lastSerialWriteDT
                #print (TD.microseconds)
                if (self.TDSinceLastRead.seconds >  2) and (self.TDSinceLastRead.seconds > 2):
                    line = self.txQ.popleft().rstrip('\n\r')
                    line = line + '\n'
                
                    print ("Sending: ", line)
                    #print(input_string.encode())
                    self.connection.write(line.encode())
                    self.lastSerialWriteDT = datetime.now()
                    sleep(0.5)
                    #self.connection.flush()
            except:
                self.errorLog.append("Error writing to serial port")
            if (len(self.txQ) > self.txQMax):
                self.errorLog.append(self.name, "Serial TX Queue Full. Connection Problem?")

    def run(self):
        ''' as a thread'''
        while True:
            if self.connected:
                while self.connection.in_waiting > 0:
                    #print("connection.in_waiting > 0")
                    self.read_char = self.connection.read(size=1)
                    self.byte_count += 1
                    #print(self.read_char)
                    self.lastSerialReadDT = datetime.now()
                    if self.read_char == b'\n':
                        self.byte_count = 0
                        self.input_string = self.input_array.decode()
                        self.input_string = self.input_string.strip('\n\r')
                        #print(self.input_string)
                        if not self.input_string == "":
                            self.rxQ.append(self.input_string)
                        self.input_array = bytearray()
                        self.input_string = "" 
                    else:
                        self.input_array += self.read_char
                    self.reading = False 
                else:
                    if self.hanging_prompts:
                        ''' nothing in waiting - has it been a while? '''
                        if self.timeSinceLastRXTX() > 2:
                            '''still bytes in the input_array? '''
                            if self.byte_count > 0: 
                                ''' Assume an un terminated prompt'''
                                self.byte_count = 0
                                self.input_string = self.input_array.decode()
                                self.input_string = self.input_string.strip('\n\r')
                                #print(self.input_string)
                                self.rxQ.append(self.input_string)
                                self.input_array = bytearray() 
                                self.input_string = ""
                                #print(self.input_array)
                '''Outgoing'''
                sleep(0.100)
                while len(self.txQ) > 0:
                    self.output_string = self.txQ.popleft()
                    self.output_string.strip('\n\r')
                    self.output_string += '\n'
                    self.output_array = self.output_string.encode()
                    self.connection.flush()
                    #sleep(0.3)
                    #print("Writing Line to Serial Port")
                    self.connection.write(self.output_array)
                    self.connection.flush()
                    self.lastSerialWriteDT = datetime.now()
                    sleep(0.100)
            #sleep(SERIAL_POLL_INTERVAL)        
    
class Adaptor():
    def __init__(self, **kwargs):
        super(Adaptor, self).__init__(**kwargs)
        self.brokerToInterfaceQ = deque()
        self.interfaceToBrokerQ = deque()
        self.last_line_sent = ""
    def config(self, interfaceObj, logQ, timeout=5):
        #self.name = name
        self.interface = interfaceObj
        self.interfaceRxQ = interfaceObj.rxQ
        self.interfaceTxQ = interfaceObj.txQ
        self.log = logQ
        self.timeout_period = timedelta(seconds=timeout)
    def connectInterface(self):
        self.interface.connect()
    def interfaceIsConnected(self):
        return self.interface.connected
    def sendNextLineToInterface(self):
        if len(self.brokerToInterfaceQ) > 0:
            self.last_line_sent = self.brokerToInterfaceQ.popleft()
            self.interfaceTxQ.append(self.last_line_sent)
    def lineAvailableFromInterface(self):
        return (len(self.interfaceRxQ) > 0)
    def appendForSendingToInterface(self, line):
        self.brokerToInterfaceQ.append(line)
    def getLineForBroker(self):
        return self.interfaceToBrokerQ.popleft()
    def appendForBrokerCollection(self, line):
        self.interfaceToBrokerQ.append(line)
    def lineAvailableForBroker(self):
        return (len(self.interfaceToBrokerQ) > 0)
    def getLineFromInterface(self):
        return self.interfaceRxQ.popleft()
    def timeSinceLastInterfaceRXTX(self):
        return self.interface.timeSinceLastRXTX()
    
        
class FodderArduino(Adaptor, Utils):
    def __init__(self, **kwargs):
        super(FodderArduino, self).__init__(**kwargs)
        self.response = ""
        self.itchActivated = False 
        self.response_list = []
        self.command = "RunLinemode"
    def setup(self):
        self.interface.config("/dev/ttyACM0", 9600)
    def mode_command(self, cmd="RunLinemode"):
        self.command = cmd
    def run(self):
        ''' As a thread'''
        while True:
            if self.command == "RunLinemode":
                if self.interfaceIsConnected():
                    if self.lineAvailableFromInterface():
                        #print("lineAvailableFromInterface")
                        self.appendForBrokerCollection(self.getLineFromInterface())
                    if len(self.brokerToInterfaceQ) > 0:
                        self.sendNextLineToInterface()
            elif self.command == "ActivateItch":
                self.appendForBrokerCollection("Recieved ActivateItch Command")
                self.activateItch()
                self.appendForBrokerCollection("Returning to RunLinemode")
                self.command = "RunLinemode"
            elif self.command == "ConfigureDevice":
                self.appendForBrokerCollection("Recieved ConfigureDevice Command")
                self.processConfigCommands()
                self.appendForBrokerCollection("Returning to RunLinemode After process config commands")
                self.command = "RunLinemode"
                
    def activateItch(self):
        if self.interfaceIsConnected():
            while self.timeSinceLastInterfaceRXTX() < 3: 
                sleep(1)
                print (self.timeSinceLastInterfaceRXTX())
            self.brokerToInterfaceQ.append("%%%")
            self.sendNextLineToInterface()
            while True:
                ''' add timeout! '''
                if self.lineAvailableFromInterface():
                    self.response = self.getLineFromInterface()
                    #print (self.response)
                    self.appendForBrokerCollection(self.response)
                if self.response == "OK":
                    #print ("BINGO!")
                    self.appendForBrokerCollection("ITCH Text CCC mode detected by adaoptor")
                    self.itchActivated = True 
                    return True
    def processConfigCommands(self):
        if not self.itchActivated:
            #self.log.append("Adaptor:FodderArduino:processConfigCommands itch is not activated on interface device")
            print("Adaptor:FodderArduino:processConfigCommands itch is not activated on interface device")
            return 
        #check for commands in queue
        self.timeout_start()
        while True:
            if len(self.brokerToInterfaceQ) > 0:
                break
            if self.timeout():
                #self.log.append("Adaptor:FodderArduino:processConfigCommands timed out waiting for commands")
                print("Adaptor:FodderArduino:processConfigCommands timed out waiting for commands")
                return 
        while len(self.brokerToInterfaceQ) > 0:
            self.sendNextLineToInterface()
            self.response_list.append(self.last_line_sent)
            self.timeout_start()
            while not self.timeout():
                if self.lineAvailableFromInterface():
                    self.response = self.getLineFromInterface()
                    self.appendForBrokerCollection(self.response)
                    if self.response in self.response_list:
                        #print("got echo")
                        # echo of send now reieved back
                        # look for command prompt return
                        # ignoring anything else
                        self.tries = 100
                        while self.tries > 0:
                            if self.lineAvailableFromInterface():
                                self.tries -= 1
                                self.response = self.getLineFromInterface()
                                #print("Got a line, sending it")
                                self.appendForBrokerCollection(self.response)
                                if self.response == "OK":
                                    #print("got prompt again")
                                    break
                        else:
                            self.appendForBrokerCollection("Adaptor:FodderArduino:processConfigCommands: "
                                                           "100 reponses without command prompt return")
                        self.response_list.clear()
                        #print("breaking timeout loop")
                        break
                    else:
                        break    
                else: #line not yet avail
                    pass
            else: 
                self.appendForBrokerCollection("Adaptor:FodderArduino:processConfigCommands: "
                                               "Timed out waiting for reponse from command send")
            sleep(0.300)

class Broker():
    def __init__(self, **kwargs):
        super(Broker, self).__init__(**kwargs)
    def attachAdaptor(self, adaptorObj):
        self.adaptor = adaptorObj
    def attachBus(self, busObj):
        self.bus = busObj 
    def run(self):
        # poll the adaptor - will perhaps wake the arduino will a DTR 
        self.adaptor.appendForSendingToInterface("\n")
        while True:
            while len(self.bus) > 0:
                self.adaptor.appendForSendingToInterface(self.bus.popleft())
            while self.adaptor.lineAvailableForBroker():
                print(self.adaptor.getLineForBroker())
            sleep(BROKER_POLL_INTERVAL)
 
class System():    
    def __init__(self, **kwargs):
        super(System, self).__init__(**kwargs)
        self.mainBus = deque()
        self.log_queue = deque()
    def config(self):
        ''' generalise later'''
         
        self.serialToBox = LinuxSerialPort()
        self.serialToBox.config("/dev/ttyACM0", 9600)

        self.FFAdaptor = FodderArduino();
        self.FFAdaptor.config(self.serialToBox, self.log_queue)
        self.FFAdaptor.connectInterface()    # Will init serial and connect
        
        self.FFBroker = Broker()
        self.FFBroker.attachAdaptor(self.FFAdaptor)
        self.FFBroker.attachBus(self.mainBus)

        ''' Start threads '''
        self.interface_thread = Thread(target=self.serialToBox.run)
        self.interface_thread.setDaemon(True)
        self.interface_thread.start()
        print("Interface thread started")
        
        self.adaptor_thread = Thread(target=self.FFAdaptor.run)
        self.adaptor_thread.setDaemon(True)
        self.adaptor_thread.start()
        print("Adaptor thread started")
        
        self.broker_thread = Thread(target=self.FFBroker.run)
        self.broker_thread.setDaemon(True)
        self.broker_thread.start()
        print("Broker thread started")
          
    def report(self, reportStr):
        print(reportStr)    
        
    def run(self):
        ''' Main loop, turn into thread later'''
            #self.serialToBox.run()
            #self.FFBroker.run()
        sleep(20)
        self.FFAdaptor.mode_command("ActivateItch")
        sleep(10)
        self.command_retry = 3
        self.command_finished = False 
        while (self.command_retry > 0) and (not self.command_finished):
            if self.FFAdaptor.itchActivated:
                self.mainBus.append("init disable all")  #suppress messages
                #self.mainBus.append("config reset")
                #self.cf = open("/home/brendan/git/FodderFactory/ff_simulator/CONFIG.TXT", "r")
                self.cf = open("CONFIG.TXT", "r")
                self.cf_lines = self.cf.readlines();
                for self.input_string in self.cf_lines:
                    self.line_clean = self.input_string.rstrip('\n\r')
                    #print(self.line_clean)
                    self.mainBus.append(self.line_clean)
                self.mainBus.append("config save")
                #self.mainBus.append("init val all")
                #self.mainBus.append("init set all")
                self.mainBus.append("\x04")
                print("Commands Queued to mainBus")
                self.FFAdaptor.mode_command("ConfigureDevice")
                self.command_finished = True
                break 
            else:
                print("Error sending commands. ITCH Not activated. Retrying in 20 seconds")
                self.command_retry -= 1
                sleep(20)
        else:
            print("Error waiting for itch. Giving up")
            exit(0)
        while True:
            print("System.Run Sleeping")
            sleep(30)
                               

def main():
    app = System()
    app.config()
    app.run()
         
        
if __name__ == '__main__':
    main()
    
    '''
    Need: 
        
        Interfaces:
            Serial
            File
            TCP Socket
            Database
        Adaptors - Know the protocal to communicate with their counter part
            ff_device
            ff_controller
            Other broker
        Queues:
            Arbitary contents
            Communicate between Adaptors and broker threads
        Bus:
            A central queue through which all traffic flows 
        Broker:
            A thread assigned to handle traffic between the Bus and a number of queues
        System:
            Read a config
            Set up Interfaces
            Setup Adaptors and associate them with an interface
            Connect in and outbound queues to the adaptors and the bus
            Assign a broker to look after a queue or queues
            Launch it all and monitor
            Add more brokers / queues if bottlenecking
            Source quench and rate limiting
        '''
    
    pass