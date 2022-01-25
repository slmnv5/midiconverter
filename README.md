

# MIDI message converter

This application is used for two tasks:

### Convert MIDI messages using rules from text files
Notes, control change (CC) or program change (PC) messages may be modified or converted to each other.
For details and exammple see [rules.txt](rules.txt)


### Count MIDI messages to produce new messages
With MIDI count MIDI notes sent less than 0.6 seconds apart make a single series and generate new MIDI message.
Along with long hold of the last note in series this technigue allows 6-7 different combinations.
This is useful for small midi controllers with few buttons as the niumber of possible commands increases many times.

Application creates two MIDI ports IN and OUT that are used to connect hardware or software MIDI ports.
Example of command line:

mimap5 -r rules.txt -v 

