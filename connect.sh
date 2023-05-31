#! /bin/bash

if [ -z "$1" ]; then 
    IP='172.24.1.1'
else 
    IP=$1
fi

ssh root@$IP