#!/bin/bash
# This script starts MIDI converter midiconverter (github link below)

# Part of hardware MIDI port name - source of messages
HARDWARE_NAME="BlueBoard"
# MIDI port name that is source of converted messages
EXT_CONV="PedalCommands"

THIS_DIR=$(dirname "$0")
cd "$THIS_DIR" || exit 1
sudo killall midiconverter
sleep 1
#wget -nc -O midiconverter https://github.com/slmnv5/midiconverter/blob/master/midiconverter?raw=true
chmod a+x midiconverter

./midiconverter -r rules.txt -i "$HARDWARE_NAME" -n "$EXT_CONV" "$@" &

