#ifndef MIDICONVERTER_H
#define MIDICONVERTER_H

#include "pch.hpp"
#include "lib/utils.hpp"
#include "MidiClient.hpp"
#include <alsa/asoundlib.h>
#include "MidiEvent.hpp"
#include "RuleMapper.hpp"
#include "MidiClient.hpp"
#include "MidiConverter.hpp"



class MidiConverter {
private:
    RuleMapper* rule_mapper;

public:
    MidiConverter(RuleMapper* rm) :
        rule_mapper(rm) {}
    virtual ~MidiConverter() {
    }

    void process_events();
    void process_one_event(MidiEvent& ev);

};

#endif
