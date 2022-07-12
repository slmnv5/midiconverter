
# mimap5 - MIDI converter, counter and typing keyboard controller

This command line utility for linux is used to:

- #### Convert MIDI events using event matching and conversion rules saved in a text file.
As an example notes ON/OFF may be converted to control change (CC).

- #### Count MIDI events to produce new events.
The same MIDI note sent several times, less than 0.6 seconds apart, makes a series and generates new MIDI event. This is useful for midi controllers with few buttons as the number of possible MIDI commands increases many times.

- #### Convert typing keyboard into MIDI controller.
If MIDI count does not provide as many commands as you need, hook up USB typing keybioard for ultimate MIDI controller with about 700 commands if you use all keys and count them.


Application creates IN and OUT virtual MIDI ports, IN port messages are converted and sent to OUT port.
For details see [details.txt](details.txt) and for example see [rules.txt](rules.txt)

For using typing keyboard look agt [kbdmap.txt](kbdmap.txt) file. Press / release events are converted to MIDI note ON / OFF events that in turn may be counted to make new events.

I use this utility with [Pepelats audio looper on Raspberry Pi](https://github.com/slmnv5/pepelats.git) and the [rules.txt](rules.txt) is written for this. This allows me to use 4 note buttons and 2 expression pedals to make many different MIDI commands out of Irig BlueBoard controller.

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


