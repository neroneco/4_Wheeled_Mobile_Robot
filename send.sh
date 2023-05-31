#! /bin/bash

if [ -z "$1" ]; then 
    IP='172.24.1.1'
else 
    IP=$1
fi

scp ./bin_arm/server ./bin_arm/wheels_control ./bin_arm/sensors root@$IP:~
