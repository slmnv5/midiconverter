#include "pch.hpp"
#include "MidiEvent.hpp"
#include "MidiFilter.hpp"
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
			LOG(loglevel::WARN) << "Possible loss of MIDI events! " << result;
			continue;
		}

		if (!readMidiEvent(event, ev)) {
			LOG(loglevel::DEBUG) << "Unknown MIDI message sent as is";
			send_event(event);
		} else {
			LOG(loglevel::DEBUG) << "Processing known event: " << ev.toString();
			process_one_event(event, ev);
		}
	}
}

void MidiFilter::send_event(snd_seq_event_t *event) const {
	snd_seq_ev_set_subs(event);
	snd_seq_ev_set_direct(event);
	snd_seq_ev_set_source(event, outport);
	snd_seq_event_output_direct(seq_handle, event);
	snd_seq_free_event(event);

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
	if (!note_counter.need_count(ev)) {
		LOG(loglevel::DEBUG) << "Ignore non countable event: " << ev.toString();
		snd_seq_free_event(event);
		return;
	}

	auto moment = the_clock::now();
	millis delta = std::chrono::duration_cast<millis>(moment - last_moment);
	if (delta > millis_600) {
		LOG(loglevel::DEBUG) << "After long delay send event as is: "
				<< ev.toString();
		last_ev = ev;
		last_moment = moment;
		count_on = count_off = 0;
		send_event(event);
		return;
	}

	if (ev.evtype == MidiEvType::NOTEON)
		count_on++;
	else
		count_off++;

	LOG(loglevel::DEBUG) << "Changed count for event: " << ev.toString();

	thread t1(&MidiFilterCount::send_event_delayed, this, event, ev, count_on,
			count_off);

}
void MidiFilterCount::send_event_delayed(snd_seq_event_t *event, MidiEvent ev,
		int cnt_on, int cnt_off) {
	std::this_thread::sleep_for(millis_600);
	if (count_on != cnt_on || count_off != cnt_off) {
		// new note came, counts changed, keep waiting
		snd_seq_free_event(event);
		return;
	} else {
		ev.v1 = note_counter.convert_v1(ev.v1) + count_on
				+ (count_on > count_off ? 5 : 0);
		writeMidiEvent(event, ev);
		count_on = count_off = 0;
		LOG(loglevel::DEBUG) << "Sending event: " << ev.toString();
		send_event(event);
	}

}
