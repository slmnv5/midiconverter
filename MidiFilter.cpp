#include "pch.hpp"
#include "MidiEvent.hpp"
#include "MidiFilter.hpp"
#include <alsa/asoundlib.h>
#include "log.hpp"

using namespace std;

void MidiFilter::open_alsa_connection() {
	const string inPortName = clientName + "_in";
	const string outPortName = clientName + "_out";

	if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0)
		throw MidiAppError("Error opening ALSA seq_handle");

	snd_seq_set_client_name(seq_handle, clientName.c_str());
	client = snd_seq_client_id(seq_handle);

	inport = snd_seq_create_simple_port(seq_handle, inPortName.c_str(),
	SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
	SND_SEQ_PORT_TYPE_APPLICATION);
	if (inport < 0)
		throw MidiAppError("Error creating seq_handle IN port");

	outport = snd_seq_create_simple_port(seq_handle, outPortName.c_str(),
	SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
	SND_SEQ_PORT_TYPE_APPLICATION);
	if (outport < 0)
		throw MidiAppError("Error creating seq_handle OUT port");

	cout << "MIDI ports created: IN=" << client << ":" << inport << "   OUT="
			<< client << ":" << outport << endl;
}

void MidiFilter::process_events(long count) {
	snd_seq_event_t *event = nullptr;
	MidiEvent ev;
	long k = 0;
	while (k++ < count) {
		int result = snd_seq_event_input(seq_handle, &event);
		if (result < 0) {
			LOG(LogLvl::WARN) << "Possible loss of MIDI events! " << result;
			continue;
		}

		if (!readMidiEvent(event, ev)) {
			LOG(LogLvl::DEBUG) << "Unknown MIDI message sent as is, type: "
					<< to_string(event->type);
			send_event(event);
		} else {
			process_one_event(event, ev);
		}
	}
}

void MidiFilter::send_event(snd_seq_event_t *event) const {
	snd_seq_ev_set_direct(event);

// either send to 64:0
// snd_seq_ev_set_dest(event, 64, 0);
// or send to subscribers of source port
	snd_seq_ev_set_subs(event);

	snd_seq_ev_set_source(event, outport);
	snd_seq_event_output_direct(seq_handle, event);
}

//===============================================================
void MidiFilterRule::process_one_event(snd_seq_event_t *event, MidiEvent &ev) {
	if (rule_mapper.applyRules(ev)) {
		LOG(LogLvl::DEBUG) << "Writing event: " << ev.toString();
		writeMidiEvent(event, ev);
	}
	LOG(LogLvl::DEBUG) << "Sending event: " << ev.toString();
	send_event(event);
}

//===============================

void MidiFilterCount::process_one_event(snd_seq_event_t *event, MidiEvent &ev) {
	snd_seq_free_event(event);
	if (!note_counter.is_countable(ev)) {
		LOG(LogLvl::DEBUG) << "Ignore non countable event: "
				<< ev.toString();
	} else if (ev.isCc() && note_counter.convert_cc_note(ev)) {
		process_note(ev);
	} else if (ev.isNote()) {
		process_note(ev);
	}
}

void MidiFilterCount::process_note(MidiEvent &ev) {
	if (!similar_and_fast(ev)) {
		if (ev.isNoteOn()) {
			LOG(LogLvl::DEBUG)
					<< "New note, reset count and sent note ON/OFF: "
					<< ev.toString();
			last_ev = ev;
			count_on = count_off = 0;
			send_on_off(ev);
		}
	}

	if (ev.isNoteOn()) {
		thread(&MidiFilterCount::send_event_delayed, this, ev, count_on++).detach();
	} else {
		count_off++;
	}

}

bool MidiFilterCount::similar_and_fast(const MidiEvent &ev) {
// true if event is similar to latest note and came fast
	time_pt now = the_clock::now();
	millis delta = std::chrono::duration_cast<millis>(now - last_moment);
	last_moment = now;
	return last_ev.is_similar(ev) && delta < millis_600;
}

void MidiFilterCount::send_event_delayed(MidiEvent ev, int cnt_on) {
	std::this_thread::sleep_for(millis_600);
	if (count_on != cnt_on) {
		// new note came, count on changed, keep waiting
		LOG(LogLvl::DEBUG) << "Delayed check note: " << ev.toString()
				<< ", on: " << count_on << ", off:" << count_off << ", on1: "
				<< cnt_on;
		return;
	}
	ev.v1 = note_counter.convert_v1(ev.v1) + count_on
			+ (count_on > count_off ? 5 : 0);
	assert(ev.isNoteOn() && ev.isValid());
	count_on = count_off = 0;
	send_new(ev);
}

void MidiFilterCount::send_new(const MidiEvent &ev) const {
	snd_seq_event_t *event = new snd_seq_event_t();
	snd_seq_ev_clear(event);
	if (!writeMidiEvent(event, ev)) {
		snd_seq_free_event(event);
		LOG(LogLvl::ERROR) << "Failed to write event: " << ev.toString();
		return;
	};
}

void MidiFilterCount::send_on_off(const MidiEvent &ev) const {
	MidiEvent e1(ev);
	assert(e1.isNoteOn());
	send_new(e1);
	e1.evtype = MidiEventType::NOTEOFF;
	send_new(e1);
}
