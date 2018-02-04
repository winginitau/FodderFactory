#!/bin/sh

# here the log file is defined

#LOGFILE = /var/log/fritzbox.log
LOGFILE=/root/fritzbox.log

# the IP address of the gateway is defined 
#myGw = "192.168.0.1"
# we cant rely on this

# Here the IP address of Fritzbox defined. If the VPN connection stands, then ping should work. 
myHost="xxx.xxx.xxx.xxx"

# Value -> how many pings to ensure connected
value=4 

########################## check local network

#count = $(ping $value $myGw | grep "received" | awk, {print $4} ')

#if [$ count -eq 4] then 
    # the coming echos are the info outputs in logfile 
#    echo "$ (date +% Y-% m-% d:% T): router with IP $myGW is reachable" | tee -a $LOGFILE 
#else 
#    echo "" | tee -a $LOGFILE 
#    echo "$(date +% Y-% m-% d:% T): Router with IP $myGw is not reachable" | tee -a $LOGFILE 
#    echo "$(date +% Y- m-% d:% T): disconnect VPN connection "| tee -a $LOGFILE  

# here the VPNC-Demon is stopped so it does not run in the background more 
#    /usr/sbin/vpnc-disconnect 

# oftentimes the wlan connections Broken. Here all network connections are restarted. 
#    echo "$(date +% Y-% m-% d:% T): Restart network connections" | tee -a $LOGFILE
##/etc/init.d/networking restart 
#    ifconfig wlan0 down
#    sleep 5 
#    ifconfig wlan0 up
## wait 20 seconds 
#    sleep 20 
# read from local ip address 
#    ipwlan = $(ifconfig wlan0 | grep "inet address" | cut -b 24-38) 
#    echo "$(date +% Y-% m-% d:% T): Network connections has been restarted. wlan0 IP Address: $ ipwlan "| tee -a $LOGFILE 
#    echo "$ (date +% Y-% m-% d:% T): Rebuild VPN connection, start script vpnc_fritzbox" | tee -a $LOGFILE 
# start from VPNC-Demon. Read out PID and VPN-IP address / usr / sbin / vpnc fritzbox.conf 
#    sleep 1 
#    pid = $(pidof vpnc) 
#    ipvpn = $(ifconfig tun0 | grep "inet address" | cut -b 24-38)
#    echo "$(date +% Y-% m-% d:% T): The VPN connection was successfully established. the VPN IP address is: $ipvpn. VPNC-Demon is active under id: $(pidof vpnc) "| tee -a $ LOGFILE 
#    echo "" | tee -a $ LOGFILE 
#fi

########################## check the destination network

while true
do

count=0

count=$(ping -c $value $myHost | grep "received" | awk '{print $4}')

if [ $count -eq 4 ] ;
then 
    # the coming echos are the info outputs in logfile 
#    echo "$(date +% Y-% m-% d:% T): Fritzbox with IP $myhost is reachable and VPN connection is" | tee -a $LOGFILE 
    echo "VPN is reachable"
else 
#    echo "" | tee -a $ LOGFILE 
#    echo "$ (date +% Y-% m-% d:% T): Fritzbox with IP $ myHost is not reachable" | tee -a $ LOGFILE 
#    echo "$ (date +% Y- m-% d:% T): disconnect VPN connection "| tee -a $LOGFILE 
    # here the VPNC Demon is stopped so that it does not run in the background more 
echo "Ping failed, unloading vpnc"
    /usr/sbin/vpnc-disconnect 
echo "Sleeping for 20"
    sleep 20 
    # start from VPNC-Demon. Read out PID and VPN-IP address 
echo "Starting vpnc"
    /usr/sbin/vpnc 
#    sleep 1 
#    pid=$(pidof vpnc) 
#    ipvpn=$(ifconfig tun0 | grep "inet address" | cut -b 24-38) 
#    echo "$ (date +% Y-% m-% d:% T): The VPN Connection was successfully established. the VPN IP address is: $ipvpn. VPNC-Demon is active under id: $(pidof vpnc) "| tee -a $LOGFILE 
#    echo "" | tee -a $ LOGFILE 
fi

sleep 10

done
