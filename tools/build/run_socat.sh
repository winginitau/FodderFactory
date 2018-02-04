#!/bin/sh

while true
  do

    socat pty,link=/dev/ttyV0,raw,echo=0,waitslave tcp:192.168.10.2:3333

#    socat -v pty,link=/dev/ttyV0,raw,echo=0,waitslave tcp:192.168.10.2:3333 2>/root/socat.dump

#    socat pty,link=/dev/ttyV0,raw,echo=0,waitslave SYSTEM:'tee /root/ttyV0.log | socat - "tcp:192.168.10.2:3333" | tee /root/ttyV0_1.log'
    
    #socat /dev/ttyS0,raw,echo=0 SYSTEM:'tee input.txt | socat - "PTY,link=/tmp/ttyV0,raw,echo=0,waitslave" | tee output.txt'

    echo socat exited...... restarting in 10
    sleep 10
  done


