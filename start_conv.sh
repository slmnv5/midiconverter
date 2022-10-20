#!/bin/bash
# This script starts MIDI converter mimap5 (github link below)

# Part of hardware MIDI port name - source of messages
HARDWARE_NAME="BlueBoard"
# MIDI port name that is source of converted messages
EXT_CONV="PedalCommands"

THIS_DIR=$(dirname "$0")
cd "$THIS_DIR" || exit 1
sudo killall mimap5
sleep 1
#wget -nc -O mimap5 https://github.com/slmnv5/mimap5/blob/master/mimap5?raw=true
chmod a+x mimap5

./mimap5 -r rules.txt -i "$HARDWARE_NAME" -n "$EXT_CONV" "$@" &

