#include "pch.hpp"
#include "MidiEvent.hpp"
#include "RuleMapper.hpp"
#include "MidiClient.hpp"
#include "MidiConverter.hpp"

#include <alsa/asoundlib.h>
#include "log.hpp"


void MidiConverter::process_one_event(snd_seq_event_t* event, MidiEvent& ev) {
    if (rule_mapper->applyRules(ev)) {
        LOG(LogLvl::INFO) << "Send mapped event: " << ev.toString();
        midi_client->make_and_send(event, ev);
    }
}

