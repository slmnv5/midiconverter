#!/bin/bash

# prepare all
aconnect -x
killall aseqdump
killall -9 mimap_d
killall -9 vmpk
sleep 2

# start this app.
vmpk &
sleep 1
./mimap_d -c file_count.txt $@  &
sleep 2


CLIENT=$(aconnect -l | awk '/mimap_client/ {print $2}')
INPORT=$(aconnect -l | awk '/mimap_in_port/ {print $1}')
OUTPORT=$(aconnect -l | awk '/mimap_out_port/ {print $1}')
MIDIKEYS=$(aconnect -l | awk '/VMPK Output/ {print $2}')
echo aconnect ${MIDIKEYS}0 ${CLIENT}${INPORT} 
aconnect ${MIDIKEYS}0 ${CLIENT}${INPORT}  || exit 1

# see converted messages
sleep 1
echo start monitoring MIDI port
aseqdump -p ${CLIENT}${OUTPORT}




