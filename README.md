
# MIDI message converter

This application is used for two tasks:

#### Convert MIDI messages using rules from text files
Notes, control change (CC) or program change (PC) messages may be modified or converted to each other.

#### Count MIDI messages to produce new messages
MIDI notes sent less than 0.6 seconds apart belong to a series and generate new MIDI message.
With long hold of the last note in series this  allows 6-7 different combinations.
This is useful for small midi controllers with few buttons as the number of possible commands increases many times.

For details and example see [rules.txt](rules.txt)

## Installation
Install dependencies. On RaspberryPi OS Lite was only one missing package that I installed:
- sudo apt install libasound2-dev
Clone the project
- cd ~/; git clone https://github.com/slmnv5/mimap5
Build release version
- cd ~/mimap5; make mimap5

## Test if not working
Build and run unit tests
- make mimap_t

Build with debug options
- make mimap_d 

## Test and rules configuration
Application creates two MIDI ports IN and OUT that are used to connect hardware or software MIDI ports.
For examples of using it check file [start_test.sh](start_test.sh)


