#!/bin/bash

# prepare all
aconnect -x
killall -9 mimap_d
sleep 2

MIDIKEYS=$(aconnect -l | awk '/BlueBoard/ {print $2;exit}')
if [ -z "$MIDIKEYS" ]; then 
	MIDIKEYS=$(aconnect -l | awk '/VMPK Output/ {print $2;exit}')
	if [ -z "$MIDIKEYS" ]; then 
		vmpk &
		sleep 3
		MIDIKEYS=$(aconnect -l | awk '/VMPK Output/ {print $2;exit}')
		if [ -z "$MIDIKEYS" ]; then
			echo "Could not find or start MIDIKEYS for input"
			exit 1
		fi
	fi
fi

# start this app.
./mimap_d -r rules.txt -n counter $@  &
sleep 2

CLIENT=$(aconnect -l | awk '/counter/ {print $2;exit}')
INPORT=$(aconnect -l | awk '/counter_in/ {print $1;exit}')
OUTPORT=$(aconnect -l | awk '/counter_out/ {print $1;exit}')

echo aconnect ${MIDIKEYS}0 ${CLIENT}${INPORT} 
aconnect ${MIDIKEYS}0 ${CLIENT}${INPORT}  || exit 1

# see converted messages
sleep 1
echo start monitoring MIDI port
aseqdump -p ${CLIENT}${OUTPORT} &





