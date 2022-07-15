#!/bin/bash
# This script starts MIDI converter mimap5 (github link below)

# MIDI port name that is source of converted messages
EXT_CONV="PedalCommands"

THIS_DIR=$(dirname "$0")
cd "$THIS_DIR" || exit 1
sudo killall mimap5
chmod a+x mimap5

# Start converter and create in and out virtual MIDI ports using typing keyboard only
sudo ./mimap5 -r rules.txt -k kbdmap.txt "$@" &


