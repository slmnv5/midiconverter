#include "pch.hpp"
#include "MidiEvent.hpp"
#include "RuleMapper.hpp"
#include "MidiClient.hpp"
#include "MidiConverter.hpp"

#include <alsa/asoundlib.h>
#include "log.hpp"

void MidiConverter::process_events() {
    MidiEvent ev;
    int result;
    while (true) {
        if (kbd_port != nullptr) {
            result = kbd_port->get_input_event(ev);
        }
        else {
            result = midi_client->get_input_event(ev);
        }
        if (result < 0) {
            continue;
        }
        process_one_event(ev);
    }
}



void MidiConverter::process_one_event(MidiEvent& ev) {
    snd_seq_event_t event;
    if (rule_mapper->applyRules(ev)) {
        LOG(LogLvl::INFO) << "Send mapped event: " << ev.toString();
        midi_client->make_and_send(&event, ev);
    }
}

