'''
Created on 31 May 2019

@author: brendan
'''

from datetime import datetime, timedelta
from threading import Thread
from time import sleep


from privatelib.ff_config import ERROR_REPEAT_WINDOW


class AppMessageHandler():
    def __init__(self, **kwargs):
        super(AppMessageHandler, self).__init__(**kwargs)
        self.INFO = int(0)
        self.DATA = int(1)
        self.WARN = int(2)
        self.ERROR = int(3)
        self.EXCEPT = int(4)
        self.FATAL = int(5)
        self.MessageLevelStrings = ['INFO: ', 'DATA: ', 'WARN: ', 'ERROR: ', 'EXCEPTION: ', 'FATAL: ']
        self.register = {}
        self.repeat_window = timedelta(seconds=ERROR_REPEAT_WINDOW)
        self.janitor_thread = Thread(target=self.janitor, args=(0,))
        self.janitor_thread.setDaemon(True)
        self.janitor_thread.start()
        print(datetime.now().isoformat(' ')[:-7] + " INFO: AppMessageHandler._init_() janitor thread started")

    def appMessage(self, srcString, levelInt, errString):
        #print("Got Error String " + errString)
        #print(self.register)
        if errString in self.register:
            last_dt, repeat_count = self.register[errString]
            if datetime.now() > (last_dt + self.repeat_window):
                if repeat_count > 0:
                    print(errString)
                    print("--- repeated " + str(repeat_count) + " times")
                    self.register[errString] = (datetime.now(), 0)
                else:
                    print(datetime.now().isoformat(' ')[:-7] + ' ' + self.MessageLevelStrings[levelInt] + srcString + ' ' + errString)
                    self.register[errString] = (datetime.now(), 0)
            else:
                self.register[errString] = (last_dt, repeat_count + 1)
        else:
            self.register[errString] = (datetime.now(), 0)
            print (datetime.now().isoformat(' ')[:-7] + ' ' + self.MessageLevelStrings[levelInt] + srcString + ' ' + errString)
    
    def janitor(self, _dt):
        while True:
            for msg, details in self.register.items():
                last_seen = details[0]
                repeat_count = details[1]
                if repeat_count > 0:
                    if datetime.now() > (last_seen + self.repeat_window + (self.repeat_window / 2) ):
                        print(datetime.now().isoformat(' ')[:-7] + " WARN: AppMessageHandler.janitor() " + msg)
                        print(datetime.now().isoformat(' ')[:-7] + " WARN: AppMessageHandler.janitor() --- repeated " + str(repeat_count) + " times")
                        self.register[msg] = (datetime.now(), 0)
            sleep(ERROR_REPEAT_WINDOW)
                
            
if __name__ == '__main__':
    ''' Test code'''
    msg = AppMessageHandler()
    msg.appMessage("Test", msg.INFO, "Error 1")
    msg.appMessage("Test", msg.INFO, "Error 2")
    msg.appMessage("Test", msg.INFO, "Error 3")
    sleep(4)
    msg.appMessage("Test", msg.INFO, "Error 1")
    msg.appMessage("Test", msg.INFO, "Error 2")
    msg.appMessage("Test", msg.INFO, "Error 3")
    sleep(1)
    for i in range(20):
        msg.appMessage("Test", msg.INFO, "Error 1")
    msg.appMessage("Test", msg.INFO, "Error 2")
    sleep(10)
    msg.appMessage("Test", msg.INFO, "Error 1")
    msg.appMessage("Test", msg.INFO, "Error 2")
    msg.appMessage("Test", msg.INFO, "Error 3")
