#!/bin/bash

echo "Make sure device is off"

BEFORE="/tmp/${PID}_B.xml"
AFTER="/tmp/${PID}_A.xml"

nmap -F 192.168.166.* -oX ${BEFORE} > /dev/null

echo "Turn device on"

nmap -F 192.168.166.* -oX ${AFTER} > /dev/null

ndiff ${BEFORE} ${AFTER}


#### FIXME make this loop and generate diffs instead of this
