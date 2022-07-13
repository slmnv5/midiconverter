#ifndef MIDICONVERTER_H
#define MIDICONVERTER_H
#include "pch.hpp"

#include "RuleMapper.hpp"
#include "MidiClient.hpp"
#include "KbdPort.hpp"

class MidiConverter {
private:
    RuleMapper* rule_mapper;
    MidiClient* midi_client;
    KbdPort* kbd_port;

public:
    MidiConverter(RuleMapper* rm, MidiClient* mc, KbdPort* kp) {
        rule_mapper = rm;
        midi_client = mc;
        kbd_port = kp;
    }

    virtual ~MidiConverter() {
    }
    virtual string toString() const {
        return rule_mapper->toString();
    }
    virtual void process_one_event(snd_seq_event_t* event, MidiEvent& ev);

};

#endif
