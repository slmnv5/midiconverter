#!/bin/bash

# prepare all
aconnect -x
killall -9 aseqdump
killall -9 mimap_d
killall -9 vmpk
sleep 2

# start this app.
./mimap_d -c count.txt -n counter $@  &
sleep 2
vmpk &
sleep 2

CLIENT=$(aconnect -l | awk '/counter/ {print $2;exit}')
INPORT=$(aconnect -l | awk '/counter_in/ {print $1;exit}')
OUTPORT=$(aconnect -l | awk '/counter_out/ {print $1;exit}')
MIDIKEYS=$(aconnect -l | awk '/VMPK Output/ {print $2;exit}')
echo aconnect ${MIDIKEYS}0 ${CLIENT}${INPORT} 
aconnect ${MIDIKEYS}0 ${CLIENT}${INPORT}  || exit 1

# see converted messages
sleep 1
echo start monitoring MIDI port
aseqdump -p ${CLIENT}${OUTPORT} &





