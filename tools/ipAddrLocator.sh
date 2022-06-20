#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Must provide network address range"
	exit 1
fi

echo "Scanning for new devices in address range:" ${1}

OLD="/tmp/$$_A.xml"
NEW="/tmp/$$_B.xml"

nmap -F ${1} -oX ${OLD} > /dev/null

while true; do
	nmap -F ${1} -oX ${NEW} > /dev/null
	ndiff ${OLD} ${NEW} | egrep -B1 ".Host is up"
	mv ${NEW} ${OLD}
	echo "----"
done
