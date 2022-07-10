#!/bin/bash
# This script starts MIDI converter mimap5 (github link below)

# MIDI port name that is source of converted messages
EXT_CONV="PedalCommands"

cd_to_script_dir() {
  THIS_DIR=$(dirname "$0")
  cd "$THIS_DIR" || exit 1
}

cd_to_script_dir
sudo killall mimap5

wget -nc -O mimap5 https://github.com/slmnv5/mimap5/blob/master/mimap5?raw=true
chmod a+x mimap5

# Start converter and create in and out virtual MIDI ports using typing keyboard only
sudo ./mimap5 -r rules.txt -k kbdmap.txt "$@" &


