#!/bin/bash
# This script starts MIDI converter mimap5 (github link below)

# Part of hardware MIDI port name - source of messages
HARDWARE_NAME="BlueBoard"
# MIDI port name that is source of converted messages
EXT_CONV="PedalCommands"

cd_to_script_dir() {
  THIS_DIR=$(dirname "$0")
  cd "$THIS_DIR" || exit 1
}

cd_to_script_dir
sudo killall mimap5
sudo killall mimap_d

wget -nc -O mimap5 https://github.com/slmnv5/mimap5/blob/master/mimap5?raw=true
chmod a+x mimap5

./mimap_d -r rules.txt  -n "$EXT_CONV" "$@" &

# Wait for hardware to appear
HARDWARE_OUT=""
for k in {1..50}; do
  echo "Waiting for MIDI port $HARDWARE_NAME"
  HARDWARE_OUT=$(aconnect -l | awk -v nm="$HARDWARE_NAME" '$0 ~ nm {print $2;exit}')
  if [ -z "$HARDWARE_OUT" ]; then
    sleep 5
  else
    break
  fi
done

# connect using linux alsa command
CLIENT_IN=$(aconnect -l | awk -v nm="$EXT_CONV" '$0 ~ nm {print $2;exit}')
if aconnect -e "${HARDWARE_OUT}0" "${CLIENT_IN}1"; then
  echo "Connected MIDI ${HARDWARE_OUT}0 to ${EXT_CONV}1"
else
  echo "Failed connect MIDI ${HARDWARE_OUT}0 to ${EXT_CONV}1"
fi
