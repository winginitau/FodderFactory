'''
Created on 1 Jul. 2018

@author: Brendan McLearie
'''

###################################################
# IMPORT TEMPLATE - Note Last line, No Comma
###################################################

#from privatelib.palette import \
#FF_WHITE, \
#FF_GREY_HG, \
#FF_GREEN, \
#FF_BLACK_MG, \
#FF_WHITE_LG, \
#FF_RED, \
#FF_BLUE, \
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



################################################### 
#Colours
###################################################
_FF_BLACK = [0, 0, 0]
_FF_WHITE = [255, 255, 255]
_FF_RED = [196, 3, 51]
_FF_GREEN = [0, 158, 107]
_FF_BLUE = [0, 135, 189]
_FF_COLD_BLUE = [153, 228, 255]
_FF_FREEZE_BLUE = [204, 241, 255]
_FF_YELLOW = [255, 211, 1]
_FF_GREY = [128, 127, 128]
_FF_SAND = [237, 235, 230]
_FF_SLATE = [69, 72, 90]

L_G = 0.4
M_G = 0.6
H_G = 0.8

FF_BLACK = [x/255 for x in _FF_BLACK]
FF_BLACK_LG = [x/255 for x in _FF_BLACK]
FF_BLACK_MG = [x/255 for x in _FF_BLACK]
FF_BLACK_HG = [x/255 for x in _FF_BLACK]
FF_BLACK_LG.append(L_G)
FF_BLACK_MG.append(M_G)
FF_BLACK_HG.append(H_G)
FF_BLACK.append(1.0)

FF_WHITE = [x/255 for x in _FF_WHITE]
FF_WHITE_LG = [x/255 for x in _FF_WHITE]
FF_WHITE_HG = [x/255 for x in _FF_WHITE]
FF_WHITE_LG.append(L_G)
FF_WHITE_HG.append(H_G)
FF_WHITE.append(1.0)

FF_RED = [x/255 for x in _FF_RED]
FF_RED_LG = [x/255 for x in _FF_RED]
FF_RED_HG = [x/255 for x in _FF_RED]
FF_RED_LG.append(L_G)
FF_RED_HG.append(H_G)
FF_RED.append(1.0)

FF_GREEN = [x/255 for x in _FF_GREEN]
FF_GREEN_LG = [x/255 for x in _FF_GREEN]
FF_GREEN_HG = [x/255 for x in _FF_GREEN]
FF_GREEN_LG.append(L_G)
FF_GREEN_HG.append(H_G)
FF_GREEN.append(1.0)

FF_BLUE = [x/255 for x in _FF_BLUE]
FF_BLUE_LG = [x/255 for x in _FF_BLUE]
FF_BLUE_HG = [x/255 for x in _FF_BLUE]
FF_BLUE_LG.append(L_G)
FF_BLUE_HG.append(H_G)
FF_BLUE.append(1.0)

FF_COLD_BLUE = [x/255 for x in _FF_COLD_BLUE]
FF_COLD_BLUE_LG = [x/255 for x in _FF_COLD_BLUE]
FF_COLD_BLUE_HG = [x/255 for x in _FF_COLD_BLUE]
FF_COLD_BLUE_LG.append(L_G)
FF_COLD_BLUE_HG.append(H_G)
FF_COLD_BLUE.append(1.0)

FF_FREEZE_BLUE = [x/255 for x in _FF_FREEZE_BLUE]
FF_FREEZE_BLUE_LG = [x/255 for x in _FF_FREEZE_BLUE]
FF_FREEZE_BLUE_HG = [x/255 for x in _FF_FREEZE_BLUE]
FF_FREEZE_BLUE_LG.append(L_G)
FF_FREEZE_BLUE_HG.append(H_G)
FF_FREEZE_BLUE.append(1.0)

FF_YELLOW = [x/255 for x in _FF_YELLOW]
FF_YELLOW_LG = [x/255 for x in _FF_YELLOW]
FF_YELLOW_HG = [x/255 for x in _FF_YELLOW]
FF_YELLOW_LG.append(L_G)
FF_YELLOW_HG.append(H_G)
FF_YELLOW.append(1.0)

FF_GREY = [x/255 for x in _FF_GREY]
FF_GREY_LG = [x/255 for x in _FF_GREY]
FF_GREY_HG = [x/255 for x in _FF_GREY]
FF_GREY_LG.append(L_G)
FF_GREY_HG.append(H_G)
FF_GREY.append(1.0)

FF_SAND = [x/255 for x in _FF_SAND]
FF_SAND_LG = [x/255 for x in _FF_SAND]
FF_SAND_HG = [x/255 for x in _FF_SAND]
FF_SAND_LG.append(L_G)
FF_SAND_HG.append(H_G)
FF_SAND.append(1.0)

FF_SLATE = [x/255 for x in _FF_SLATE]
FF_SLATE_LG = [x/255 for x in _FF_SLATE]
FF_SLATE_HG = [x/255 for x in _FF_SLATE]
FF_SLATE_LG.append(L_G)
FF_SLATE_HG.append(H_G)
FF_SLATE.append(1.0)



