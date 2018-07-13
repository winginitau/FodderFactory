'''
Created on 8 Jul. 2018

@author: brendan
'''

from privatelib.msg_parser import MessageSystem
from kivy.uix.screenmanager import ScreenManager, SwapTransition



#########################################################################
# Global Vars
#########################################################################

msg_sys = MessageSystem();
sm = ScreenManager(transition=SwapTransition())

