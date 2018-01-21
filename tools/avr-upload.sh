#!/bin/sh

#avrdude -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:ff_arduino.hex:i
avrdude -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:serial_copy_test.hex:i
screen /dev/ttyACM0 9600

