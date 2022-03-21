
# mimap5 - MIDI converter

This command line utility for linux is used for:

- #### Convert MIDI events using rules in a text files
As an example notes ON/OFF may be converted to control change (CC).

- #### Count MIDI events to produce new events
The same MIDI note sent several times, less than 0.6 seconds apart, makes a series and generates new MIDI event.
This is useful for midi controllers with few buttons as the number of possible MIDI commands increases many times.

For details see [details.txt](details.txt) and for example see [rules.txt](rules.txt)

I use this utility with [Pepelats audio looper on Raspberry Pi](https://github.com/slmnv5/pepelats.git) and the [rules.txt](rules.txt) is written for this.

## Installation
Install dependencies. On RaspberryPi OS Lite was missing package that I installed:
- sudo apt install libasound2-dev libasound2

Clone the project
- cd ~/; git clone https://github.com/slmnv5/mimap5

For Raspberry Pi ARM32 there is a release version in this repo, but if you need to build it:
- cd ~/mimap5; make clean mimap5



## Tests
Build and run unit tests
- make mimap_t

Build with debug options
- make mimap_d 

## Test your rules
Application creates two MIDI ports IN and OUT that are used to connect hardware or software MIDI ports.
For an examples of using it check file [start_test.sh](start_test.sh)
It helps to see how your rules are working with verbose logging option


