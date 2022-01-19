#include "pch.hpp"
#include "MidiEvent.hpp"
#include "MidiFilter.hpp"
#include <alsa/asoundlib.h>
#include "log.hpp"

using namespace std;

void MidiFilter::open_alsa_connection() {
	const char *clentName = "mimap_client";
	const char *inPortName = "mimap_in_port";
	const char *outPortName = "mimap_out_port";

	if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0)
		throw MidiAppError("Error opening ALSA seq_handle");

	snd_seq_set_client_name(seq_handle, clentName);
	client = snd_seq_client_id(seq_handle);

	inport = snd_seq_create_simple_port(seq_handle, inPortName,
	SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE,
	SND_SEQ_PORT_TYPE_APPLICATION);
	if (inport < 0)
		throw MidiAppError("Error creating seq_handle IN port");

	outport = snd_seq_create_simple_port(seq_handle, outPortName,
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
			LOG(loglevel::WARN) << "Possible loss of MIDI events! "
					<< result;
			continue;
		}

		if (!readMidiEvent(event, ev)) {
			LOG(loglevel::DEBUG)
					<< "Unknown MIDI message sent as is, type: "
					<< to_string(event->type);
			send_event(event);
		} else {
			LOG(loglevel::DEBUG) << "Processing known event: "
					<< ev.toString();
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
		LOG(loglevel::DEBUG) << "Writing event: " << ev.toString();
		writeMidiEvent(event, ev);
	}
	LOG(loglevel::DEBUG) << "Sending event: " << ev.toString();
	send_event(event);
}

//===============================

void MidiFilterCount::process_one_event(snd_seq_event_t *event, MidiEvent &ev) {
	if (!note_counter.is_countable_note(ev)) {
		LOG(loglevel::DEBUG) << "Ignore non countable event: "
				<< ev.toString();
		snd_seq_free_event(event);
		return;
	}
	bool is_on = ev.evtype == MidiEvType::NOTEON;
	if (not_similar_or_delayed(ev)) {
		if (is_on) {
			LOG(loglevel::DEBUG)
					<< "New note, reset count and sent note ON as is: "
					<< ev.toString();
			last_ev = ev;
			count_on = 1;
			count_off = 0;
		}
		send_event(event);
	}
	if (!is_on && count_on == 1) {
		send_event(event);
	}
	snd_seq_ev_clear(event);

	if (is_on) {
		count_on++;
		LOG(loglevel::DEBUG) << "Changed ON count for note: "
				<< ev.toString();
		thread t1(&MidiFilterCount::send_event_delayed, this, ev, count_on,
				count_off);
		t1.detach();
	} else {
		count_off++;
	}
}

bool MidiFilterCount::not_similar_or_delayed(const MidiEvent &ev) {
	// true if different from the latest note or note came too late
	time_pt now = the_clock::now();
	millis delta = std::chrono::duration_cast<millis>(now - last_moment);
	last_moment = now;
	return !last_ev.is_similar(ev) || delta > millis_600;
}

void MidiFilterCount::send_event_delayed(MidiEvent ev, int cnt_on,
		int cnt_off) {
	std::this_thread::sleep_for(millis_600);
	if (count_on != cnt_on || count_off != cnt_off) {
// new note came, counts changed, keep waiting
		return;
	}
	ev.v1 = note_counter.convert_v1(ev.v1) + count_on
			+ (count_on > count_off ? 5 : 0);
	count_on = count_off = 0;
	snd_seq_event_t *event = new snd_seq_event_t();
	snd_seq_ev_clear(event);

	if (!writeMidiEvent(event, ev)) {
		snd_seq_free_event(event);
		return;
	}
	send_event(event);
}
