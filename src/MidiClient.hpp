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
	void take_in(const MidiEvent& ev) const {
		send_new_event(ev, inport);
	}
	void send_out(const MidiEvent& ev) const {
		send_new_event(ev, outport);
	}
protected:
	virtual void process_one_event(snd_seq_event_t* event, MidiEvent& ev) {}
	void send_event(snd_seq_event_t* event, int port) const;
private:
	void send_new_event(const MidiEvent& ev, int port) const;
	void open_alsa_connection(const char* clientName);
};
//=============== class that maps in event to out events ============================

class MidiConverter : public MidiClient {
private:
	RuleMapper rule_mapper;
public:
	MidiConverter(const string& ruleFile, MidiClient& mc) :
		MidiClient(mc), rule_mapper(ruleFile, *this) {
	}
	virtual ~MidiConverter() {
	}
	virtual string toString() const {
		return rule_mapper.toString();
	}
	virtual void process_one_event(snd_seq_event_t* event, MidiEvent& ev);

};

#endif
