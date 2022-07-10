#!/bin/bash
# This script starts MIDI converter mimap5 (github link below)

# Part of MIDI port name - source of messages
HARDWARE_NAME="BlueBoard"
# MIDI port name that is source of converted messages
EXT_CONV="PedalCommands"

cd_to_script_dir() {
  THIS_DIR=$(dirname "$0")
  cd "$THIS_DIR" || exit 1
}

check_if_running() {
  found=$(ps -ef | grep -v grep | grep mimap5)
  if [ -n "$found" ]; then
    echo "Exiting, this script is already running"
    exit 1
  fi
}

cd_to_script_dir
sudo killall mimap5

wget -nc -O mimap5 https://github.com/slmnv5/mimap5/blob/master/mimap5?raw=true
chmod a+x mimap5

# Start converter and create in and out virtual MIDI ports
sudo ./mimap5 -r rules.txt   -k kbdmap.txt  -n "$EXT_CONV" "$@"  &
time sleep 2

HARDWARE_OUT=""
for k in {1..10}; do
  echo "Waiting for MIDI port $HARDWARE_NAME"
  HARDWARE_OUT=$(aconnect -l | awk -v nm="$HARDWARE_NAME" '$0 ~ nm {print $2;exit}')
  if [ -z "$HARDWARE_OUT" ]; then
    sleep 5
  else
    break
  fi
done

CLIENT_IN=$(aconnect -l | awk -v nm="$EXT_CONV" '$0 ~ nm {print $2;exit}')
if aconnect -e "${HARDWARE_OUT}0" "${CLIENT_IN}1"; then
  echo "Connected MIDI ${HARDWARE_OUT}0 to ${EXT_CONV}1"
else
  echo "Failed connect MIDI ${HARDWARE_OUT}0 to ${EXT_CONV}1"
fi
