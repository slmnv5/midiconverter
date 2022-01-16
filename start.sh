#!/bin/bash

# prepare all
aconnect -x
killall -9 mimap
killall -9 vmpk
sleep 1

# start this app.
./mimap -c count.txt $@  &
sleep 1

# start vmpk keyboard for testing
vmpk &

sleep 1

CLIENT=$(aconnect -l | awk '/mimap_client/ {print $2}')
INPORT=$(aconnect -l | awk '/mimap_in_port/ {print $1}')
OUTPORT=$(aconnect -l | awk '/mimap_out_port/ {print $1}')
MIDIKEYS=$(aconnect -l | awk '/VMPK Output/ {print $2}')
echo aconnect ${MIDIKEYS}0 ${CLIENT}${INPORT} 
aconnect ${MIDIKEYS}0 ${CLIENT}${INPORT} 

# see converted messages
sleep 1

aseqdump -p ${CLIENT}${OUTPORT}




