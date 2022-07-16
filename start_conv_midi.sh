#!/bin/bash
# This script starts MIDI converter mimap5 (github link below)

# Part of hardware MIDI port name - source of messages
HARDWARE_NAME="BlueBoard"
# MIDI port name that is source of converted messages
EXT_CONV="PedalCommands"

THIS_DIR=$(dirname "$0")
cd "$THIS_DIR" || exit 1
sudo killall mimap5
#wget -nc -O mimap5 https://github.com/slmnv5/mimap5/blob/master/mimap5?raw=true
chmod a+x mimap5

./mimap5 -r rules.txt  -n "$EXT_CONV" "$@" &
sleep 2
#aconnect -l

HARDWARE_OUT=$(aconnect -l | awk -v nm="$HARDWARE_NAME" '$0 ~ nm {print $2;exit}')
# connect using linux alsa command
CLIENT_IN=$(aconnect -l | awk -v nm="$EXT_CONV" '$0 ~ nm {print $2;exit}')
if aconnect -e "${HARDWARE_OUT}0" "${CLIENT_IN}0"; then
  echo "Connected MIDI ${HARDWARE_OUT}0 to ${EXT_CONV}1"
else
  echo "Failed connect MIDI ${HARDWARE_OUT}0 to ${EXT_CONV}1"
fi
