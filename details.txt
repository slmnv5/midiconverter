Rule is a single line in text file, extra spaces are removed  
Semicolon starts a comment that goes to the end of the line
Every incoming MIDI event goes trough the list of rules from top to bottom.

### Count rule
Count rule type has 2 parts. Example: n,0,12,=c
If MIDI note event matches first part, it is counted. New note is created, its velocity depends on count. 
Count stops after 0.6 seconds of inactivity or if another MIDI event interrupts count.
This rule adds number of notes in series, adds 5 if the last note was released with a delay and set velocity of note to that value.

#### Example of count rule:
n,0,12,=c; this counts note 12.
Double tap will create new note 12 with velocity = 2. Double tap and hold velocity = 2+5 = 7, etc.
Count rule does not send note OFF only note ON


### Conversion rule
Conversion rule has 3 parts separated by '='. Example: c,0,12,0:70=n,,12,77=p
If MIDI event matches first part, it is converted to match second part. 
The last part is rule type. Conversion rule types are: 's' - stop, 'p' - pass, 'o' - once 

- For rules Pass and Once converted event is passed to the remaining rules in the list.
- For Count and Stop rules processing stops if a match for the event is found.
- Once rule makes conversion only if event is different from the previous, thus for few identical events only first is converted, others are ignored.


#### Examples of conversion rule:

n,2,30:40,1:127=c,2,,,=p 
;note ON channel 2, note number 30:40, any velocity = convert to CC on channel 2, keep MIDI values the same = pass to next rule

c,,,120:127=n,15,,20,=s 
;CC on any channel (0:15), any control number (0:127), value 120:127 = convert to note ON channel 15, 
;keep same MIDI value for note number, velocity 20 = stop, do not scan remaining rules

;Less verbose rule description
n,,,1:5=n,0,0,0=s; all quiet notes silenced, stop scanning remaining rules  
n,,,101:127=n,2,,=p; all loud notes sent to channel #2, scan remaining rules using converted note 
a,,,=n,0,0,0=s; catch any event and convert to a zero note off, stop rule
a,,,=a,,,=s; any message passed as is, without changes
n,,,=n,,,=k; any note is killed
c,,,=c,2,,=s; any CC is moved to channel 2
n,1,,0=c,0,7,0=s; all note OFF on channel 1 convert to CC 7 value zero on channel 0 




### Command line parameters:
Usage: midiconverter -r <file> [options]
		-r <file> load file with rules, see rules.txt for details
		options:
		  -h displays this info
		  -n [name] optional MIDI client name
		  -v verbose output
		  -vv more verbose
		  -vvv even more verbose
