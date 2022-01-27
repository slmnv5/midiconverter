
# mimap5 - MIDI message converter

This command line utility for linux is used for two tasks:

#### Convert MIDI messages using rules from text files
Notes, control change (CC) or program change (PC) messages may be modified or converted to each other.

#### Count MIDI messages to produce new messages
The same MIDI note sent several times, less than 0.6 seconds apart, make a series and generate new MIDI message.
With long hold of the last note in series this allows at least 7 different combinations.
This is useful for small midi controllers with few buttons as the number of possible commands increases many times.

For details and example see [rules.txt](rules.txt)

I use this utility with [Pepelats audio looper on Rapberry Pi](https://github.com/slmnv5/pepelats.git) and the [rules.txt](rules.txt) is written for this.

## Installation
Install dependencies. On RaspberryPi OS Lite was missing package that I installed:
- sudo apt install libasound2-dev libasound2

Clone the project
- cd ~/; git clone https://github.com/slmnv5/mimap5

For Raspberry Pi ARM32 there is a release version in this repo, but if you need to build it:
- cd ~/mimap5; make clean mimap5



## Test if not working
Build and run unit tests
- make mimap_t

Build with debug options
- make mimap_d 

## Test your rules
Application creates two MIDI ports IN and OUT that are used to connect hardware or software MIDI ports.
For an examples of using it check file [start_test.sh](start_test.sh)
It helps to see how your rules are working with verbose logging option


