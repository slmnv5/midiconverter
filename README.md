
# midiconverter - MIDI converter and note counter

This command line utility for linux is used to:

### Convert MIDI events using event matching and conversion rules saved in a text file.
As an example notes may be converted to control changes (CC), specific notes may be sent to another channel, filtered out etc.

### Count MIDI notes to produce new notes.
The same MIDI note sent several times, less than 0.6 seconds apart, makes a series and generates new MIDI note. This is useful for midi controllers with few buttons (e.g. MIDI foot controllers) as the number of possible MIDI commands increases many times.

Application creates IN and OUT virtual MIDI ports, IN port messages are converted and sent to OUT port.
For details see [details.md](details.md) and for example see [rules.txt](rules.txt)

I use this utility with [Pepelats audio looper on Raspberry Pi](https://github.com/slmnv5/pepelats.git) and the [rules.txt](rules.txt) is written for this, only 6 buttons can make about 30 different MIDI notes.

### Installation
Install dependencies. RaspberryPi OS Lite was missing packages that I installed:
- sudo apt install libasound2-dev libasound2

There is make file to build application (some other targets available):
make clean app


