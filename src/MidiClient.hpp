#ifndef MIDICLIENT_H
#define MIDICLIENT_H
#include "pch.hpp"

#include "RuleMapper.hpp"
#include "KbdPort.hpp"

using namespace std;



class MidiClient {
protected:
	int client = -1;
	int inport = -1;
	int outport = -1;
	snd_seq_t* seq_handle = nullptr;


public:
	MidiClient(const char* clientName) {
		open_alsa_connection(clientName);
	}
	virtual ~MidiClient() {
	}
	void process_events();
	virtual void process_one_event(snd_seq_event_t* event, MidiEvent& ev) {}
	void make_and_send(snd_seq_event_t*, const MidiEvent& ev) const;
	void send_event(snd_seq_event_t* event) const;
	void new_event_input(const MidiEvent& ev) const;

private:
	void open_alsa_connection(const char* clientName);
};

#endif
