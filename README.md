

## MIDI message counter and converter

This application is used for two tasks:

# Count MIDI messages to produce new messages
Common issue with MIDI controllers is a lack of sufficent number of available messages.
With MIDI count number of available messages (notes) may be increates many times because double push of a button will generate other note than single push. Long release of a button also affect the process. 
For details and exammple see [count.txt](count.txt)

# Convert MIDI messages using rules
Another issue when devices use different notes, CCs or channels may be solved with MIDI convertiing. Application can convert notes to control changes and vise versa. It can transpose notes,  change notes velocity, change MIDI channels and do many other things.
For details and exammple see [rules.txt](rules.txt)

