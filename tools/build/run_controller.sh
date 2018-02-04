#!/bin/sh

while true
  do
    python3 /root/controller.py
    echo Controller exited..... restarting in 10
    sleep 10
  done



